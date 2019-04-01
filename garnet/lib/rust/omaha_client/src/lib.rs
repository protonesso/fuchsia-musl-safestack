// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//! Crate omaha_client
//! This is for Omaha client binaries written in Rust.

#![feature(async_await, await_macro, futures_api)]

pub mod common;
pub mod configuration;
pub mod http_request;
pub mod protocol;
pub mod requests;
