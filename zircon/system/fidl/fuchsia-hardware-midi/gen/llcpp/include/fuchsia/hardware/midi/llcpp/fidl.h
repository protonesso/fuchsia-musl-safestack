// WARNING: This file is machine generated by fidlgen.

#pragma once

#include <lib/fidl/internal.h>
#include <lib/fidl/txn_header.h>
#include <lib/fidl/llcpp/array.h>
#include <lib/fidl/llcpp/coding.h>
#include <lib/fidl/llcpp/connect_service.h>
#include <lib/fidl/llcpp/service_handler_interface.h>
#include <lib/fidl/llcpp/string_view.h>
#include <lib/fidl/llcpp/sync_call.h>
#include <lib/fidl/llcpp/traits.h>
#include <lib/fidl/llcpp/transaction.h>
#include <lib/fidl/llcpp/vector_view.h>
#include <lib/fit/function.h>
#include <lib/zx/channel.h>
#include <zircon/fidl.h>

namespace llcpp {

namespace fuchsia {
namespace hardware {
namespace midi {

struct Info;
class Device;

extern "C" const fidl_type_t fuchsia_hardware_midi_InfoTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_midi_InfoTable;

// Describes what type of MIDI device an implementation of Device represents
struct Info {
  static constexpr const fidl_type_t* Type = &fuchsia_hardware_midi_InfoTable;
  static constexpr const fidl_type_t* AltType = &v1_fuchsia_hardware_midi_InfoTable;
  static constexpr uint32_t MaxNumHandles = 0;
  static constexpr uint32_t PrimarySize = 2;
  [[maybe_unused]]
  static constexpr uint32_t MaxOutOfLine = 0;
  static constexpr uint32_t AltPrimarySize = 2;
  [[maybe_unused]]
  static constexpr uint32_t AltMaxOutOfLine = 0;

  // Whether or not this device is a MIDI sink
  bool is_sink = {};

  // Whether or not this device is a MIDI source
  bool is_source = {};
};

extern "C" const fidl_type_t fuchsia_hardware_midi_DeviceGetInfoRequestTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_midi_DeviceGetInfoRequestTable;
extern "C" const fidl_type_t fuchsia_hardware_midi_DeviceGetInfoResponseTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_midi_DeviceGetInfoResponseTable;

class Device final {
  Device() = delete;
 public:

  struct GetInfoResponse final {
    FIDL_ALIGNDECL
    fidl_message_header_t _hdr;
    ::llcpp::fuchsia::hardware::midi::Info info;

    static constexpr const fidl_type_t* Type = &fuchsia_hardware_midi_DeviceGetInfoResponseTable;
    static constexpr const fidl_type_t* AltType = &v1_fuchsia_hardware_midi_DeviceGetInfoResponseTable;
    static constexpr uint32_t MaxNumHandles = 0;
    static constexpr uint32_t PrimarySize = 24;
    static constexpr uint32_t MaxOutOfLine = 0;
    static constexpr uint32_t AltPrimarySize = 24;
    static constexpr uint32_t AltMaxOutOfLine = 0;
    static constexpr bool HasFlexibleEnvelope = false;
    static constexpr bool ContainsUnion = false;
    static constexpr ::fidl::internal::TransactionalMessageKind MessageKind =
        ::fidl::internal::TransactionalMessageKind::kResponse;
  };
  using GetInfoRequest = ::fidl::AnyZeroArgMessage;


  // Collection of return types of FIDL calls in this interface.
  class ResultOf final {
    ResultOf() = delete;
   private:
    template <typename ResponseType>
    class GetInfo_Impl final : private ::fidl::internal::OwnedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::OwnedSyncCallBase<ResponseType>;
     public:
      GetInfo_Impl(zx::unowned_channel _client_end);
      ~GetInfo_Impl() = default;
      GetInfo_Impl(GetInfo_Impl&& other) = default;
      GetInfo_Impl& operator=(GetInfo_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };

   public:
    using GetInfo = GetInfo_Impl<GetInfoResponse>;
  };

  // Collection of return types of FIDL calls in this interface,
  // when the caller-allocate flavor or in-place call is used.
  class UnownedResultOf final {
    UnownedResultOf() = delete;
   private:
    template <typename ResponseType>
    class GetInfo_Impl final : private ::fidl::internal::UnownedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::UnownedSyncCallBase<ResponseType>;
     public:
      GetInfo_Impl(zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer);
      ~GetInfo_Impl() = default;
      GetInfo_Impl(GetInfo_Impl&& other) = default;
      GetInfo_Impl& operator=(GetInfo_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };

   public:
    using GetInfo = GetInfo_Impl<GetInfoResponse>;
  };

  class SyncClient final {
   public:
    explicit SyncClient(::zx::channel channel) : channel_(std::move(channel)) {}
    ~SyncClient() = default;
    SyncClient(SyncClient&&) = default;
    SyncClient& operator=(SyncClient&&) = default;

    const ::zx::channel& channel() const { return channel_; }

    ::zx::channel* mutable_channel() { return &channel_; }

    // Get information about the type of MIDI device
    // Allocates 40 bytes of message buffer on the stack. No heap allocation necessary.
    ResultOf::GetInfo GetInfo();

    // Get information about the type of MIDI device
    // Caller provides the backing storage for FIDL message via request and response buffers.
    UnownedResultOf::GetInfo GetInfo(::fidl::BytePart _response_buffer);

   private:
    ::zx::channel channel_;
  };

  // Methods to make a sync FIDL call directly on an unowned channel, avoiding setting up a client.
  class Call final {
    Call() = delete;
   public:

    // Get information about the type of MIDI device
    // Allocates 40 bytes of message buffer on the stack. No heap allocation necessary.
    static ResultOf::GetInfo GetInfo(zx::unowned_channel _client_end);

    // Get information about the type of MIDI device
    // Caller provides the backing storage for FIDL message via request and response buffers.
    static UnownedResultOf::GetInfo GetInfo(zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer);

  };

  // Messages are encoded and decoded in-place when these methods are used.
  // Additionally, requests must be already laid-out according to the FIDL wire-format.
  class InPlace final {
    InPlace() = delete;
   public:

    // Get information about the type of MIDI device
    static ::fidl::DecodeResult<GetInfoResponse> GetInfo(zx::unowned_channel _client_end, ::fidl::BytePart response_buffer);

  };

  // Pure-virtual interface to be implemented by a server.
  class Interface {
   public:
    Interface() = default;
    virtual ~Interface() = default;
    using _Outer = Device;
    using _Base = ::fidl::CompleterBase;

    class GetInfoCompleterBase : public _Base {
     public:
      void Reply(::llcpp::fuchsia::hardware::midi::Info info);
      void Reply(::fidl::BytePart _buffer, ::llcpp::fuchsia::hardware::midi::Info info);
      void Reply(::fidl::DecodedMessage<GetInfoResponse> params);

     protected:
      using ::fidl::CompleterBase::CompleterBase;
    };

    using GetInfoCompleter = ::fidl::Completer<GetInfoCompleterBase>;

    virtual void GetInfo(GetInfoCompleter::Sync _completer) = 0;

  };

  // Attempts to dispatch the incoming message to a handler function in the server implementation.
  // If there is no matching handler, it returns false, leaving the message and transaction intact.
  // In all other cases, it consumes the message and returns true.
  // It is possible to chain multiple TryDispatch functions in this manner.
  static bool TryDispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn);

  // Dispatches the incoming message to one of the handlers functions in the interface.
  // If there is no matching handler, it closes all the handles in |msg| and closes the channel with
  // a |ZX_ERR_NOT_SUPPORTED| epitaph, before returning false. The message should then be discarded.
  static bool Dispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn);

  // Same as |Dispatch|, but takes a |void*| instead of |Interface*|. Only used with |fidl::Bind|
  // to reduce template expansion.
  // Do not call this method manually. Use |Dispatch| instead.
  static bool TypeErasedDispatch(void* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
    return Dispatch(static_cast<Interface*>(impl), msg, txn);
  }


  // Helper functions to fill in the transaction header in a |DecodedMessage<TransactionalMessage>|.
  class SetTransactionHeaderFor final {
    SetTransactionHeaderFor() = delete;
   public:
    static void GetInfoRequest(const ::fidl::DecodedMessage<Device::GetInfoRequest>& _msg);
    static void GetInfoResponse(const ::fidl::DecodedMessage<Device::GetInfoResponse>& _msg);
  };
};

}  // namespace midi
}  // namespace hardware
}  // namespace fuchsia
}  // namespace llcpp

namespace fidl {

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::midi::Info> : public std::true_type {};
static_assert(std::is_standard_layout_v<::llcpp::fuchsia::hardware::midi::Info>);
static_assert(offsetof(::llcpp::fuchsia::hardware::midi::Info, is_sink) == 0);
static_assert(offsetof(::llcpp::fuchsia::hardware::midi::Info, is_source) == 1);
static_assert(sizeof(::llcpp::fuchsia::hardware::midi::Info) == ::llcpp::fuchsia::hardware::midi::Info::PrimarySize);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::midi::Device::GetInfoResponse> : public std::true_type {};
template <>
struct IsFidlMessage<::llcpp::fuchsia::hardware::midi::Device::GetInfoResponse> : public std::true_type {};
static_assert(sizeof(::llcpp::fuchsia::hardware::midi::Device::GetInfoResponse)
    == ::llcpp::fuchsia::hardware::midi::Device::GetInfoResponse::PrimarySize);
static_assert(offsetof(::llcpp::fuchsia::hardware::midi::Device::GetInfoResponse, info) == 16);

}  // namespace fidl
