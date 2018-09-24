// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <inttypes.h>

#include <fbl/array.h>
#include <fbl/unique_ptr.h>
#include <tee-client-api/tee-client-types.h>
#include <zircon/assert.h>

#include "optee-smc.h"
#include "shared-memory.h"
#include "util.h"

namespace optee {

// OP-TEE Messages
//
// The majority of data exchange with OP-TEE occurs via OP-TEE messages. These are used in
// conjunction with the OP-TEE SMC Call with Arg function. When that SMC function is invoked,
// OP-TEE will expect a physical pointer to an OP-TEE message to be passed in arguments a1 and a2.
//
// Each message is made up of a header and a variable number of parameters. The relevant fields of
// a message can depend on the command and the context, so these helper classes aim to reduce the
// possibilities of invariant access. For example, in some instances, a field might be an input and
// in others, it might be an output.

struct MessageHeader {
    uint32_t command;
    uint32_t app_function;
    uint32_t session_id;
    uint32_t cancel_id;

    uint32_t unused;
    uint32_t return_code;
    uint32_t return_origin;
    uint32_t num_params;
};

struct MessageParam {
    enum AttributeType : uint64_t {
        kAttributeTypeNone = 0x0,
        kAttributeTypeValueInput = 0x1,
        kAttributeTypeValueOutput = 0x2,
        kAttributeTypeValueInOut = 0x3,
        kAttributeTypeRegMemInput = 0x5,
        kAttributeTypeRegMemOutput = 0x6,
        kAttributeTypeRegMemInOut = 0x7,
        kAttributeTypeTempMemInput = 0x9,
        kAttributeTypeTempMemOutput = 0xa,
        kAttributeTypeTempMemInOut = 0xb,

        kAttributeTypeMeta = 0x100,
        kAttributeTypeFragment = 0x200,
    };

    struct TemporaryMemory {
        uint64_t buffer;
        uint64_t size;
        uint64_t shared_memory_reference;
    };

    struct RegisteredMemory {
        uint64_t offset;
        uint64_t size;
        uint64_t shared_memory_reference;
    };

    union Value {
        struct {
            uint64_t a;
            uint64_t b;
            uint64_t c;
        } generic;
        TEEC_UUID uuid_big_endian;
        struct {
            SharedMemoryType memory_type;
            uint64_t memory_size;
        } allocate_memory_specs;
        struct {
            SharedMemoryType memory_type;
            uint64_t memory_id;
        } free_memory_specs;
    };

    uint64_t attribute;
    union {
        TemporaryMemory temporary_memory;
        RegisteredMemory registered_memory;
        Value value;
    } payload;
};

// MessageParamList
//
// MessageParamList is a non-owning view of the parameters in a Message. It is only valid within
// the lifetime of the Message.
class MessageParamList {
public:
    constexpr MessageParamList()
        : params_(nullptr), count_(0U) {}

    MessageParamList(MessageParam* params, size_t count)
        : params_(params), count_(count) {}

    size_t size() const { return count_; }
    MessageParam* get() const { return params_; }

    MessageParam& operator[](size_t i) const {
        ZX_DEBUG_ASSERT(i < count_);
        return params_[i];
    }

    MessageParam* begin() const {
        return params_;
    }
    MessageParam* end() const {
        return &params_[count_];
    }

private:
    MessageParam* params_;
    size_t count_;
};

template <typename PtrType>
class MessageBase {
    static_assert(fbl::is_same<PtrType, SharedMemory*>::value ||
                      fbl::is_same<PtrType, fbl::unique_ptr<SharedMemory>>::value,
                  "Template type of MessageBase must be a pointer (raw or smart) to SharedMemory!");

public:
    using SharedMemoryPtr = PtrType;

    zx_paddr_t paddr() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing uninitialized OP-TEE message");
        return memory_->paddr();
    }

    // TODO(godtamit): Move this to protected once all usages of it outside are removed
    // TODO(rjascani): Change this to return a reference to make ownership rules clearer
    MessageParamList params() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing uninitialized OP-TEE message");
        return MessageParamList(reinterpret_cast<MessageParam*>(header() + 1),
                                header()->num_params);
    }

    // Returns whether the message is valid. This must be true to access any class-specific field.
    bool is_valid() const { return memory_ != nullptr; }

protected:
    static constexpr size_t CalculateSize(size_t num_params) {
        return sizeof(MessageHeader) + (sizeof(MessageParam) * num_params);
    }

    // MessageBase
    //
    // Move constructor for MessageBase.
    MessageBase(MessageBase&& msg)
        : memory_(fbl::move(msg.memory_)) {
        msg.memory_ = nullptr;
    }

    // Move-only, so explicitly delete copy constructor and copy assignment operator for clarity
    MessageBase(const MessageBase&) = delete;
    MessageBase& operator=(const MessageBase&) = delete;

    explicit MessageBase()
        : memory_(nullptr) {}

    explicit MessageBase(SharedMemoryPtr memory)
        : memory_(fbl::move(memory)) {}

    MessageHeader* header() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing uninitialized OP-TEE message");
        return reinterpret_cast<MessageHeader*>(memory_->vaddr());
    }

    SharedMemoryPtr memory_;
};

// Message
//
// A normal message from the rich world (REE).
class Message : public MessageBase<fbl::unique_ptr<SharedMemory>> {
public:
    enum Command : uint32_t {
        kOpenSession = 0,
        kInvokeCommand = 1,
        kCloseSession = 2,
        kCancel = 3,
        kRegisterSharedMemory = 4,
        kUnregisterSharedMemory = 5,
    };

    // Message
    //
    // Move constructor for Message. Uses the default implicit implementation.
    Message(Message&&) = default;

    // Move-only, so explicitly delete copy constructor and copy assignment operator for clarity
    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;

protected:
    using MessageBase::MessageBase; // inherit constructors
};

// OpenSessionMessage
//
// This OP-TEE message is used to start a session between a client app and trusted app.
class OpenSessionMessage : public Message {
public:
    explicit OpenSessionMessage(SharedMemoryManager::DriverMemoryPool* pool,
                                const UuidView& trusted_app,
                                const UuidView& client_app,
                                uint32_t client_login,
                                uint32_t cancel_id,
                                const fbl::Array<MessageParam>& params);

    // Outputs
    uint32_t session_id() const { return header()->session_id; }
    uint32_t return_code() const { return header()->return_code; }
    uint32_t return_origin() const { return header()->return_origin; }

protected:
    using Message::header; // make header() protected

    static constexpr size_t kNumFixedOpenSessionParams = 2;
};

// RpcMessage
//
// A message originating from the trusted world (TEE) specifying the details of a RPC request.
class RpcMessage : public MessageBase<SharedMemory*> {
public:
    enum Command : uint32_t {
        kLoadTa = 0,
        kAccessReplayProtectedMemoryBlock = 1,
        kAccessFileSystem = 2,
        kGetTime = 3,
        kWaitQueue = 4,
        kSuspend = 5,
        kAllocateMemory = 6,
        kFreeMemory = 7,
        kAccessSqlFileSystem = 8,
        kLoadGprof = 9,
        kPerformSocketIo = 10
    };

    // RpcMessage
    //
    // Move constructor for RpcMessage.
    RpcMessage(RpcMessage&& rpc_msg)
        : MessageBase(fbl::move(rpc_msg)),
          is_valid_(fbl::move(rpc_msg.is_valid_)) {
        rpc_msg.is_valid_ = false;
    }

    // Move-only, so explicitly delete copy constructor and copy assignment operator for clarity
    RpcMessage(const RpcMessage&) = delete;
    RpcMessage& operator=(const RpcMessage&) = delete;

    // RpcMessage
    //
    // Constructs an instance of an RpcMessage from a backing SharedMemory object.
    //
    // Parameters:
    //  * memory:   A pointer to the SharedMemory object backing the RpcMessage. This pointer must
    //              be non-null and valid.
    explicit RpcMessage(SharedMemory* memory)
        : MessageBase(memory), is_valid_(TryInitializeMembers()) {}

    uint32_t command() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        return header()->command;
    }

    void set_return_origin(uint32_t return_origin) {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        header()->return_origin = return_origin;
    }

    void set_return_code(uint32_t return_code) {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        header()->return_code = return_code;
    }

    // Returns whether the message is a valid RpcMessage. This must be true to access any
    // class-specific field.
    bool is_valid() const { return is_valid_; }

protected:
    bool is_valid_;

private:
    bool TryInitializeMembers();
};

// LoadTaRpcMessage
//
// A RpcMessage that should be interpreted with the command of loading a trusted application.
// A RpcMessage can be converted into a LoadTaRpcMessage via a constructor.
class LoadTaRpcMessage : public RpcMessage {
public:
    // LoadTaRpcMessage
    //
    // Move constructor for LoadTaRpcMessage. Uses the default implicit implementation.
    LoadTaRpcMessage(LoadTaRpcMessage&& load_ta_msg) = default;

    // LoadTaRpcMessage
    //
    // Constructs a LoadTaRpcMessage from a moved-in RpcMessage.
    explicit LoadTaRpcMessage(RpcMessage&& rpc_message)
        : RpcMessage(fbl::move(rpc_message)) {
        ZX_DEBUG_ASSERT(is_valid()); // The RPC message passed in should've been valid
        ZX_DEBUG_ASSERT(command() == RpcMessage::Command::kLoadTa);

        is_valid_ = is_valid_ && TryInitializeMembers();
    }

    const TEEC_UUID& ta_uuid() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        return ta_uuid_;
    }

    uint64_t memory_reference_id() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        return mem_id_;
    }

    uint64_t memory_reference_size() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        return mem_size_;
    }

    uint64_t memory_reference_offset() const {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        return mem_offset_;
    }

    void set_output_ta_size(uint64_t ta_size) {
        ZX_DEBUG_ASSERT_MSG(is_valid(), "Accessing invalid OP-TEE RPC message");
        *out_ta_size_ = ta_size;
    }

protected:
    static constexpr size_t kNumParams = 2;
    static constexpr size_t kUuidParamIndex = 0;
    static constexpr size_t kMemoryReferenceParamIndex = 1;

    TEEC_UUID ta_uuid_;
    uint64_t mem_id_;
    uint64_t mem_size_;
    size_t mem_offset_;
    uint64_t* out_ta_size_;

private:
    bool TryInitializeMembers();
};

} // namespace optee
