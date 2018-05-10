// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "garnet/bin/guest/mgr/guest_holder.h"

namespace guestmgr {

GuestHolder::GuestHolder(
    uint32_t cid, std::string label,
    std::unique_ptr<VsockEndpoint> socket_endpoint,
    component::Services services,
    component::ApplicationControllerPtr application_controller)
    : cid_(cid),
      label_(std::move(label)),
      socket_endpoint_(std::move(socket_endpoint)),
      guest_services_(std::move(services)),
      guest_app_controller_(std::move(application_controller)) {
  guest_services_.ConnectToService(guest_controller_.NewRequest());
}

void GuestHolder::AddBinding(
    fidl::InterfaceRequest<guest::GuestController> request) {
  bindings_.AddBinding(guest_controller_.get(), std::move(request));
}

}  // namespace guestmgr
