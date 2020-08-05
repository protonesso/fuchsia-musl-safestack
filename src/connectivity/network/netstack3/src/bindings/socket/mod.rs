// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//! Socket features exposed by netstack3.

pub(crate) mod udp;

use std::convert::{TryFrom, TryInto};
use std::num::NonZeroU16;

use byteorder::{NativeEndian, NetworkEndian};
use fidl::endpoints::{ClientEnd, RequestStream};
use fidl_fuchsia_posix::Errno;
use fidl_fuchsia_posix_socket as psocket;
use fuchsia_async as fasync;
use fuchsia_zircon as zx;
use futures::{TryFutureExt, TryStreamExt};
use log::debug;
use net_types::ip::{Ip, IpAddress, IpVersion, Ipv4, Ipv4Addr, Ipv6, Ipv6Addr};
use net_types::{SpecifiedAddr, Witness};
use netstack3_core::{
    LocalAddressError, NetstackError, RemoteAddressError, SocketError, UdpSendError,
};
use zerocopy::{AsBytes, FromBytes, LayoutVerified, Unaligned, U16, U32};

use super::{context::InnerValue, StackContext};

// Socket constants defined in FDIO in
// `//sdk/lib/fdio/private-socket.h`
// TODO(brunodalbo) Come back to this, see if we can have those definitions in a
// public header from FDIO somehow so we don't need to redefine.
const ZXSIO_SIGNAL_INCOMING: zx::Signals = zx::Signals::USER_0;
const ZXSIO_SIGNAL_OUTGOING: zx::Signals = zx::Signals::USER_1;

/// Supported transport protocols.
#[derive(Debug)]
pub enum TransProto {
    Udp,
    Tcp,
}

impl TryFrom<psocket::DatagramSocketProtocol> for TransProto {
    type Error = Errno;

    fn try_from(value: psocket::DatagramSocketProtocol) -> Result<Self, Self::Error> {
        match value {
            psocket::DatagramSocketProtocol::Udp => Ok(TransProto::Udp),
            psocket::DatagramSocketProtocol::IcmpEcho => Err(Errno::Eprotonosupport),
        }
    }
}

impl TryFrom<psocket::StreamSocketProtocol> for TransProto {
    type Error = Errno;

    fn try_from(value: psocket::StreamSocketProtocol) -> Result<Self, Self::Error> {
        match value {
            psocket::StreamSocketProtocol::Tcp => Ok(TransProto::Tcp),
        }
    }
}

/// Common properties for socket workers.
#[derive(Debug)]
struct SocketWorkerProperties {}

pub(crate) trait SocketStackDispatcher:
    InnerValue<udp::UdpSocketCollection>
    + netstack3_core::UdpEventDispatcher<Ipv4>
    + netstack3_core::UdpEventDispatcher<Ipv6>
{
}
impl<T> SocketStackDispatcher for T where
    T: InnerValue<udp::UdpSocketCollection>
        + netstack3_core::UdpEventDispatcher<Ipv4>
        + netstack3_core::UdpEventDispatcher<Ipv6>
{
}

pub(crate) trait SocketStackContext:
    StackContext + udp::UdpStackContext<Ipv4> + udp::UdpStackContext<Ipv6>
where
    <Self as StackContext>::Dispatcher: SocketStackDispatcher,
{
}
impl<T> SocketStackContext for T
where
    T: StackContext + udp::UdpStackContext<Ipv4> + udp::UdpStackContext<Ipv6>,
    T::Dispatcher: SocketStackDispatcher,
{
}

pub(crate) struct SocketProviderWorker<C> {
    ctx: C,
}

impl<C> SocketProviderWorker<C>
where
    C: SocketStackContext,
    C::Dispatcher: SocketStackDispatcher,
{
    pub(crate) fn spawn(ctx: C, mut rs: psocket::ProviderRequestStream) {
        fasync::Task::spawn(
            async move {
                let worker = SocketProviderWorker { ctx };
                while let Some(req) = rs.try_next().await? {
                    worker.handle_fidl_socket_provider_request(req).await;
                }
                Ok(())
            }
            .unwrap_or_else(|e: anyhow::Error| {
                debug!("SocketProviderWorker finished with error {:?}", e)
            }),
        )
        .detach()
    }

    /// Spawns a socket worker for the given `transport`.
    ///
    /// The created worker will serve the appropriate FIDL protocol over `channel`
    /// and send [`Event::SocketEvent`] variants to the `event_loop`'s main mpsc
    /// channel.
    fn spawn_worker(
        &self,
        net_proto: IpVersion,
        transport: TransProto,
        channel: fasync::Channel,
        properties: SocketWorkerProperties,
    ) -> Result<(), Errno> {
        match transport {
            TransProto::Udp => udp::spawn_worker(
                net_proto,
                self.ctx.clone(),
                psocket::DatagramSocketRequestStream::from_channel(channel),
                properties,
            ),
            _ => Err(Errno::Eafnosupport),
        }
    }

    /// Handles a `fuchsia.posix.socket.Provider` FIDL request in `req`.
    async fn handle_fidl_socket_provider_request(&self, req: psocket::ProviderRequest) {
        match req {
            psocket::ProviderRequest::InterfaceIndexToName { index: _, responder } => {
                // TODO(https://fxbug.dev/48969): implement this method.
                responder_send!(responder, &mut Err(zx::Status::NOT_FOUND.into_raw()));
            }
            psocket::ProviderRequest::InterfaceNameToIndex { name: _, responder } => {
                // TODO(https://fxbug.dev/48969): implement this method.
                responder_send!(responder, &mut Err(zx::Status::NOT_FOUND.into_raw()));
            }
            psocket::ProviderRequest::Socket2 { domain: _, type_: _, protocol: _, responder } => {
                // NB: Netstack3 is not load-bearing enough to justify
                // maintaining two implementations.
                responder.control_handle().shutdown_with_epitaph(zx::Status::NOT_SUPPORTED);
            }
            psocket::ProviderRequest::StreamSocket { domain: _, proto: _, responder } => {
                responder_send!(responder, &mut Err(Errno::Eprotonosupport));
            }
            psocket::ProviderRequest::DatagramSocket { domain, proto, responder } => {
                responder_send!(
                    responder,
                    &mut self.socket::<psocket::DatagramSocketMarker, _>(domain, proto)
                );
            }
            psocket::ProviderRequest::GetInterfaceAddresses { responder } => {
                // TODO(https://fxbug.dev/54162): implement this method.
                responder_send!(responder, &mut std::iter::empty());
            }
        }
    }

    fn socket<T, P: TryInto<TransProto, Error = Errno>>(
        &self,
        domain: psocket::Domain,
        proto: P,
    ) -> Result<ClientEnd<T>, Errno> {
        let net_proto = match domain {
            psocket::Domain::Ipv4 => IpVersion::V4,
            psocket::Domain::Ipv6 => IpVersion::V6,
        };

        let (server, client) = zx::Channel::create().map_err(|_: zx::Status| Errno::Enobufs)?;
        self.spawn_worker(
            net_proto,
            proto.try_into()?,
            // we can safely unwrap here because we just created
            // this channel above.
            fasync::Channel::from_channel(server).unwrap(),
            SocketWorkerProperties {},
        )
        .map(|()| ClientEnd::<T>::new(client))
    }
}

/// A trait generalizing the data structures passed as arguments to POSIX socket
/// calls.
///
/// `SockAddr` implementers are typically passed to POSIX socket calls as a blob
/// of bytes. It represents a type that can be parsed from a C API `struct
/// sockaddr`, expressed as a stream of bytes.
pub(crate) trait SockAddr:
    std::fmt::Debug + Sized + AsBytes + FromBytes + Unaligned
{
    /// The concrete address type for this `SockAddr`.
    type AddrType: IpAddress;
    /// The socket family (`AF_INET` or `AF_INET6`) that the implementer
    /// accepts.
    const FAMILY: u16;

    /// Creates a new `SockAddr`.
    ///
    /// Implementations must set their family field to `Self::FAMILY`.
    fn new(addr: Self::AddrType, port: u16) -> Self;

    /// Creates a new `SockAddr` with the specified `addr` and `port` serialized
    /// directly into a `Vec`.
    fn new_vec(addr: Self::AddrType, port: u16) -> Vec<u8> {
        let mut v = Vec::new();
        v.resize(std::mem::size_of::<Self>(), 0);
        let mut b = LayoutVerified::<_, Self>::new(&mut v[..]).unwrap();
        b.set_family(Self::FAMILY);
        b.set_addr(addr.bytes());
        b.set_port(port);
        v
    }

    /// Gets this `SockAddr`'s address.
    fn addr(&self) -> Self::AddrType;

    /// Set this [`SockAddr`]'s address.
    ///
    /// # Panics
    ///
    /// Panics id `addr` does not have the correct length for this
    /// [`SockAddr`] implementation.
    fn set_addr(&mut self, addr: &[u8]);

    /// Gets this `SockAddr`'s port.
    fn port(&self) -> u16;

    /// Set this [`SockAddr`]'s port.
    fn set_port(&mut self, port: u16);

    /// Gets this `SockAddr`'s family.
    fn family(&self) -> u16;

    /// Set this [`SockAddr`]'s family.
    fn set_family(&mut self, family: u16);

    /// Gets a `SpecifiedAddr` witness type for this `SockAddr`'s address.
    fn get_specified_addr(&self) -> Option<SpecifiedAddr<Self::AddrType>> {
        SpecifiedAddr::<Self::AddrType>::new(self.addr())
    }

    /// Attempts to create an inline `LayoutVerified` version of this `SockAddr`
    /// from a slice of bytes.
    fn parse(bytes: &[u8]) -> Option<LayoutVerified<&[u8], Self>> {
        LayoutVerified::new_from_prefix(bytes).map(|(x, _)| x)
    }

    /// Gets a `NonZeroU16` witness type for this `SockAddr`'s port.
    fn get_specified_port(&self) -> Option<NonZeroU16> {
        NonZeroU16::new(self.port())
    }
}

/// POSIX representation of an IPv6 socket address.
///
/// `SockAddr6` is equivalent to `struct sockaddr_in6` in the POSIX C API.
#[derive(AsBytes, FromBytes, Unaligned, Debug)]
#[repr(C)]
pub(crate) struct SockAddr6 {
    family: U16<NativeEndian>,
    port: U16<NetworkEndian>,
    flow_info: U32<NativeEndian>,
    addr: [u8; 16],
    scope_id: U32<NetworkEndian>,
}

impl SockAddr for SockAddr6 {
    type AddrType = Ipv6Addr;
    const FAMILY: u16 = libc::AF_INET6 as u16;

    /// Creates a new `SockAddr6`.
    fn new(addr: Self::AddrType, port: u16) -> Self {
        SockAddr6 {
            family: U16::new(Self::FAMILY),
            port: U16::new(port),
            flow_info: U32::ZERO,
            addr: addr.ipv6_bytes(),
            scope_id: U32::ZERO,
        }
    }

    fn addr(&self) -> Ipv6Addr {
        Ipv6Addr::new(self.addr)
    }

    fn set_addr(&mut self, addr: &[u8]) {
        self.addr.copy_from_slice(addr)
    }

    fn port(&self) -> u16 {
        self.port.get()
    }

    fn set_port(&mut self, port: u16) {
        self.port.set(port)
    }

    fn family(&self) -> u16 {
        self.family.get()
    }

    fn set_family(&mut self, family: u16) {
        self.family.set(family)
    }
}

/// POSIX representation of an IPv4 socket address.
///
/// `SockAddr` is equivalent to `struct sockaddr_in` in the POSIX C API.
#[derive(AsBytes, FromBytes, Unaligned, Debug)]
#[repr(C)]
pub(crate) struct SockAddr4 {
    family: U16<NativeEndian>,
    port: U16<NetworkEndian>,
    addr: [u8; 4],
}

impl SockAddr for SockAddr4 {
    type AddrType = Ipv4Addr;
    const FAMILY: u16 = libc::AF_INET as u16;

    /// Creates a new `SockAddr4`.
    fn new(addr: Self::AddrType, port: u16) -> Self {
        SockAddr4 { family: U16::new(Self::FAMILY), port: U16::new(port), addr: addr.ipv4_bytes() }
    }

    fn addr(&self) -> Ipv4Addr {
        Ipv4Addr::new(self.addr)
    }

    fn set_addr(&mut self, addr: &[u8]) {
        self.addr.copy_from_slice(addr)
    }

    fn port(&self) -> u16 {
        self.port.get()
    }

    fn set_port(&mut self, port: u16) {
        self.port.set(port)
    }

    fn family(&self) -> u16 {
        self.family.get()
    }

    fn set_family(&mut self, family: u16) {
        self.family.set(family)
    }
}

/// Backing storage for `SockAddr` when used in headers for datagram POSIX
/// sockets.
///
/// Defined in C code in `sys/socket.h`.
// NOTE(brunodalbo) this struct is expected to be short-lived. Upcoming changes
// to the POSIX FIDL API will render this obsolete.
#[derive(AsBytes, FromBytes, Unaligned)]
#[repr(C)]
struct SockAddrStorage {
    family: U16<NativeEndian>,
    storage: [u8; 128 - 2],
}

// We need to implement Default manually because [u8; 128-2] does not provide a
// `Default` impl
impl Default for SockAddrStorage {
    fn default() -> Self {
        Self { family: Default::default(), storage: [0u8; 128 - 2] }
    }
}

/// Extension trait that associates a [`SockAddr`] implementation to an IP
/// version. We provide implementations for [`Ipv4`] and [`Ipv6`].
pub(crate) trait IpSockAddrExt: Ip {
    type SocketAddress: SockAddr<AddrType = Self::Addr>;
}

impl IpSockAddrExt for Ipv4 {
    type SocketAddress = SockAddr4;
}

impl IpSockAddrExt for Ipv6 {
    type SocketAddress = SockAddr6;
}

#[cfg(test)]
mod testutil {
    use net_types::ip::{AddrSubnetEither, IpAddr};

    use super::*;

    /// A struct that holds parameters to create a `SockAddr` value that can be used in tests.
    ///
    /// Used as a parameter by [`TestSockAddr::create_for_test`].
    pub(crate) struct SockAddrTestOptions {
        /// The port in the created `SockAddr`
        pub(crate) port: u16,
        /// Whether to set a bad socket family value (an unsupported one).
        pub(crate) bad_family: bool,
        // Whether to set the unspecified address.
        pub(crate) bad_address: bool,
    }

    /// A trait that exposes common test behavior to implementers of
    /// [`SockAddr`].
    pub(crate) trait TestSockAddr: SockAddr {
        /// The local address used for tests.
        const LOCAL_ADDR: Self::AddrType;
        /// The remote address used for tests.
        const REMOTE_ADDR: Self::AddrType;
        /// An alternate remote address used for tests.
        const REMOTE_ADDR_2: Self::AddrType;
        /// An non-local address which is unreachable, used for tests.
        const UNREACHABLE_ADDR: Self::AddrType;

        /// The default subnet prefix used for tests.
        const DEFAULT_PREFIX: u8;

        /// Get the [`psocket::Domain`] for this `SockAddr`'s family.
        fn domain() -> psocket::Domain {
            match i32::from(Self::FAMILY) {
                libc::AF_INET => psocket::Domain::Ipv4,
                libc::AF_INET6 => psocket::Domain::Ipv6,
                _ => panic!("Unrecognized socket family {}", Self::FAMILY),
            }
        }

        /// Creates the serialized representation of this [`SockAddr`] with the
        /// informed `options`.
        fn create_for_test(options: SockAddrTestOptions) -> Vec<u8> {
            let mut v = Vec::new();
            v.resize(std::mem::size_of::<Self>(), 0);
            let mut sockaddr = LayoutVerified::<_, Self>::new(&mut v[..]).unwrap();
            sockaddr.set_options(&options);
            v
        }

        /// Creates a [`SockAddr`] with the appropriate family with the given
        /// `addr` and `port`.
        fn create(addr: Self::AddrType, port: u16) -> Vec<u8> {
            let mut v = Vec::new();
            v.resize(std::mem::size_of::<Self>(), 0);
            let mut sockaddr = LayoutVerified::<_, Self>::new(&mut v[..]).unwrap();
            sockaddr.set_family(Self::FAMILY);
            sockaddr.set_port(port);
            sockaddr.set_addr(addr.bytes());
            v
        }

        /// Sets the options in `options` to this [`SockAddr`].
        fn set_options(&mut self, options: &SockAddrTestOptions) {
            self.set_family(if options.bad_family { Self::bad_family() } else { Self::FAMILY });
            self.set_port(options.port);
            if !options.bad_address {
                self.set_addr(Self::REMOTE_ADDR.bytes());
            }
        }

        /// Gets the local address and prefix configured for the test
        /// [`SockAddr`].
        fn config_addr_subnet() -> AddrSubnetEither {
            AddrSubnetEither::new(IpAddr::from(Self::LOCAL_ADDR), Self::DEFAULT_PREFIX).unwrap()
        }

        /// Gets the remote address and prefix to use for the test [`SockAddr`].
        fn config_addr_subnet_remote() -> AddrSubnetEither {
            AddrSubnetEither::new(IpAddr::from(Self::REMOTE_ADDR), Self::DEFAULT_PREFIX).unwrap()
        }

        /// Returns a bad socket family value (one that will cause errors when
        /// used with this [`SockAddr`].
        fn bad_family() -> u16 {
            match Self::FAMILY as i32 {
                libc::AF_INET => libc::AF_INET6 as u16,
                libc::AF_INET6 => libc::AF_INET as u16,
                _ => unreachable!(),
            }
        }
    }

    impl TestSockAddr for SockAddr6 {
        const LOCAL_ADDR: Ipv6Addr =
            Ipv6Addr::new([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 168, 0, 1]);
        const REMOTE_ADDR: Ipv6Addr =
            Ipv6Addr::new([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 168, 0, 2]);
        const REMOTE_ADDR_2: Ipv6Addr =
            Ipv6Addr::new([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 168, 0, 3]);
        const UNREACHABLE_ADDR: Ipv6Addr =
            Ipv6Addr::new([0, 0, 0, 0, 0, 0, 0, 42, 0, 0, 0, 0, 192, 168, 0, 1]);
        const DEFAULT_PREFIX: u8 = 64;
    }

    impl TestSockAddr for SockAddr4 {
        const LOCAL_ADDR: Ipv4Addr = Ipv4Addr::new([192, 168, 0, 1]);
        const REMOTE_ADDR: Ipv4Addr = Ipv4Addr::new([192, 168, 0, 2]);
        const REMOTE_ADDR_2: Ipv4Addr = Ipv4Addr::new([192, 168, 0, 3]);
        const UNREACHABLE_ADDR: Ipv4Addr = Ipv4Addr::new([192, 168, 42, 1]);
        const DEFAULT_PREFIX: u8 = 24;
    }
}

/// Trait expressing the conversion of error types into
/// [`fidl_fuchsia_posix::Errno`] errors for the POSIX-lite wrappers.
trait IntoErrno {
    /// Returns the most equivalent POSIX error code for `self`.
    fn into_errno(self) -> Errno;
}

impl IntoErrno for LocalAddressError {
    fn into_errno(self) -> Errno {
        match self {
            LocalAddressError::CannotBindToAddress
            | LocalAddressError::FailedToAllocateLocalPort => Errno::Eaddrnotavail,
            LocalAddressError::AddressMismatch => Errno::Einval,
            LocalAddressError::AddressInUse => Errno::Eaddrinuse,
        }
    }
}

impl IntoErrno for RemoteAddressError {
    fn into_errno(self) -> Errno {
        match self {
            RemoteAddressError::NoRoute => Errno::Enetunreach,
        }
    }
}

impl IntoErrno for SocketError {
    fn into_errno(self) -> Errno {
        match self {
            SocketError::Remote(e) => e.into_errno(),
            SocketError::Local(e) => e.into_errno(),
        }
    }
}

impl IntoErrno for UdpSendError {
    fn into_errno(self) -> Errno {
        match self {
            UdpSendError::Unknown => Errno::Eio,
            UdpSendError::Local(l) => l.into_errno(),
            UdpSendError::Remote(r) => r.into_errno(),
        }
    }
}

impl IntoErrno for NetstackError {
    fn into_errno(self) -> Errno {
        match self {
            NetstackError::Parse(_) => Errno::Einval,
            NetstackError::Exists => Errno::Ealready,
            NetstackError::NotFound => Errno::Efault,
            NetstackError::SendUdp(s) => s.into_errno(),
            NetstackError::Connect(c) => c.into_errno(),
            NetstackError::NoRoute => Errno::Ehostunreach,
            NetstackError::Mtu => Errno::Emsgsize,
        }
    }
}
