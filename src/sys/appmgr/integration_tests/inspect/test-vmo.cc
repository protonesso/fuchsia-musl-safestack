// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fuchsia/inspect/cpp/fidl.h>
#include <fuchsia/io/cpp/fidl.h>
#include <lib/async/default.h>
#include <lib/fdio/directory.h>
#include <lib/fdio/fd.h>
#include <lib/fdio/fdio.h>
#include <lib/inspect/cpp/reader.h>
#include <lib/inspect/testing/cpp/inspect.h>
#include <lib/sys/cpp/testing/test_with_environment.h>
#include <zircon/device/vfs.h>

#include <gmock/gmock.h>

#include "src/lib/files/glob.h"
#include "src/lib/fxl/strings/substitute.h"

namespace component {
namespace {

using ::fxl::Substitute;
using sys::testing::EnclosingEnvironment;
using ::testing::UnorderedElementsAre;
using namespace inspect::testing;

constexpr char kTestComponent[] =
    "fuchsia-pkg://fuchsia.com/appmgr_inspect_integration_tests#meta/"
    "inspect_vmo_test_app.cmx";
constexpr char kTestProcessName[] = "inspect_vmo_test_app.cmx";

class InspectTest : public sys::testing::TestWithEnvironment {
 protected:
  InspectTest() {
    fuchsia::sys::LaunchInfo launch_info;
    launch_info.url = kTestComponent;

    environment_ = CreateNewEnclosingEnvironment("test", CreateServices());
    environment_->CreateComponent(std::move(launch_info), controller_.NewRequest());
    bool ready = false;
    controller_.events().OnDirectoryReady = [&ready] { ready = true; };
    RunLoopUntil([&ready] { return ready; });
    if (!ready) {
      printf("The output directory is not ready\n");
    }
  }
  ~InspectTest() { CheckShutdown(); }

  void CheckShutdown() {
    controller_->Kill();
    bool done = false;
    controller_.events().OnTerminated = [&done](int64_t code,
                                                fuchsia::sys::TerminationReason reason) {
      ASSERT_EQ(fuchsia::sys::TerminationReason::EXITED, reason);
      done = true;
    };
    RunLoopUntil([&done] { return done; });
  }

  // Open the root vmo from the Inspect Tree.
  // Returns ZX_OK on success.
  zx_status_t GetInspectVmo(zx::vmo* out_vmo) {
    files::Glob glob(
        Substitute("/hub/r/test/*/c/$0/*/out/diagnostics/fuchsia.inspect.Tree", kTestProcessName));
    if (glob.size() == 0) {
      return ZX_ERR_NOT_FOUND;
    }

    fuchsia::inspect::TreeSyncPtr ptr;
    zx_status_t status;
    status = fdio_service_connect(std::string(*glob.begin()).c_str(),
                                  ptr.NewRequest().TakeChannel().release());
    if (status != ZX_OK) {
      return status;
    }

    fuchsia::inspect::TreeContent content;
    status = ptr->GetContent(&content);
    if (status != ZX_OK) {
      return status;
    }

    *out_vmo = std::move(content.mutable_buffer()->vmo);
    return ZX_OK;
  }

 private:
  std::unique_ptr<EnclosingEnvironment> environment_;
  fuchsia::sys::ComponentControllerPtr controller_;
};

TEST_F(InspectTest, ReadHierarchy) {
  zx::vmo vmo;
  ASSERT_EQ(ZX_OK, GetInspectVmo(&vmo));

  auto hierarchy = inspect::ReadFromVmo(std::move(vmo)).take_value();

  EXPECT_THAT(
      hierarchy,
      AllOf(
          NodeMatches(NameMatches("root")),
          ChildrenMatch(UnorderedElementsAre(
              AllOf(NodeMatches(AllOf(NameMatches("t1"),
                                      PropertyList(UnorderedElementsAre(
                                          StringIs("version", "1.0"),
                                          ByteVectorIs("frame", std::vector<uint8_t>({0, 0, 0})),
                                          IntIs("value", -10), BoolIs("active", true))))),
                    ChildrenMatch(UnorderedElementsAre(
                        NodeMatches(AllOf(NameMatches("item-0x0"),
                                          PropertyList(UnorderedElementsAre(IntIs("value", 10))))),
                        NodeMatches(AllOf(NameMatches("item-0x1"),
                                          PropertyList(UnorderedElementsAre(IntIs("value", 100)))))

                            ))),
              AllOf(NodeMatches(AllOf(NameMatches("t2"),
                                      PropertyList(UnorderedElementsAre(
                                          StringIs("version", "1.0"),
                                          ByteVectorIs("frame", std::vector<uint8_t>({0, 0, 0})),
                                          IntIs("value", -10), BoolIs("active", true))))),
                    ChildrenMatch(UnorderedElementsAre(
                        NodeMatches(AllOf(NameMatches("item-0x2"),
                                          PropertyList(UnorderedElementsAre(IntIs("value", 4)))))))

                        )))));
}

}  // namespace
}  // namespace component
