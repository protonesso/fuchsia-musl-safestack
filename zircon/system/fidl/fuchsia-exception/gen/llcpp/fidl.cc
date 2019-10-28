// WARNING: This file is machine generated by fidlgen.

#include <fuchsia/exception/llcpp/fidl.h>
#include <memory>

namespace llcpp {

namespace fuchsia {
namespace exception {

::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::ProcessLimbo_ReleaseProcess_Result() {
  tag_ = Tag::Invalid;
}

::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::~ProcessLimbo_ReleaseProcess_Result() {
  Destroy();
}

void ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::Destroy() {
  switch (which()) {
  case Tag::kResponse:
    response_.~ProcessLimbo_ReleaseProcess_Response();
    break;
  default:
    break;
  }
  tag_ = Tag::Invalid;
}

void ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::MoveImpl_(ProcessLimbo_ReleaseProcess_Result&& other) {
  switch (other.which()) {
  case Tag::kResponse:
    mutable_response() = std::move(other.mutable_response());
    break;
  case Tag::kErr:
    mutable_err() = std::move(other.mutable_err());
    break;
  default:
    break;
  }
  other.Destroy();
}

void ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::SizeAndOffsetAssertionHelper() {
  static_assert(offsetof(::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result, response_) == 4);
  static_assert(offsetof(::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result, err_) == 4);
  static_assert(sizeof(::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result) == ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::PrimarySize);
}


::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Response& ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::mutable_response() {
  if (which() != Tag::kResponse) {
    Destroy();
    new (&response_) ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Response;
  }
  tag_ = Tag::kResponse;
  return response_;
}

int32_t& ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result::mutable_err() {
  if (which() != Tag::kErr) {
    Destroy();
    new (&err_) int32_t;
  }
  tag_ = Tag::kErr;
  return err_;
}


namespace {

[[maybe_unused]]
constexpr uint64_t kHandler_OnException_Ordinal = 0x7ec50e5a00000000lu;
[[maybe_unused]]
constexpr uint64_t kHandler_OnException_GenOrdinal = 0x5de09d36098de59elu;
extern "C" const fidl_type_t fuchsia_exception_HandlerOnExceptionRequestTable;
extern "C" const fidl_type_t fuchsia_exception_HandlerOnExceptionResponseTable;
extern "C" const fidl_type_t v1_fuchsia_exception_HandlerOnExceptionResponseTable;

}  // namespace
template <>
Handler::ResultOf::OnException_Impl<Handler::OnExceptionResponse>::OnException_Impl(zx::unowned_channel _client_end, ::zx::exception exception, ::llcpp::fuchsia::exception::ExceptionInfo info) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<OnExceptionRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, OnExceptionRequest::PrimarySize);
  auto& _request = *reinterpret_cast<OnExceptionRequest*>(_write_bytes);
  _request.exception = std::move(exception);
  _request.info = std::move(info);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(OnExceptionRequest));
  ::fidl::DecodedMessage<OnExceptionRequest> _decoded_request(std::move(_request_bytes));
  Super::SetResult(
      Handler::InPlace::OnException(std::move(_client_end), std::move(_decoded_request), Super::response_buffer()));
}

Handler::ResultOf::OnException Handler::SyncClient::OnException(::zx::exception exception, ::llcpp::fuchsia::exception::ExceptionInfo info) {
  return ResultOf::OnException(zx::unowned_channel(this->channel_), std::move(exception), std::move(info));
}

Handler::ResultOf::OnException Handler::Call::OnException(zx::unowned_channel _client_end, ::zx::exception exception, ::llcpp::fuchsia::exception::ExceptionInfo info) {
  return ResultOf::OnException(std::move(_client_end), std::move(exception), std::move(info));
}

template <>
Handler::UnownedResultOf::OnException_Impl<Handler::OnExceptionResponse>::OnException_Impl(zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, ::zx::exception exception, ::llcpp::fuchsia::exception::ExceptionInfo info, ::fidl::BytePart _response_buffer) {
  if (_request_buffer.capacity() < OnExceptionRequest::PrimarySize) {
    Super::SetFailure(::fidl::DecodeResult<OnExceptionResponse>(ZX_ERR_BUFFER_TOO_SMALL, ::fidl::internal::kErrorRequestBufferTooSmall));
    return;
  }
  memset(_request_buffer.data(), 0, OnExceptionRequest::PrimarySize);
  auto& _request = *reinterpret_cast<OnExceptionRequest*>(_request_buffer.data());
  _request.exception = std::move(exception);
  _request.info = std::move(info);
  _request_buffer.set_actual(sizeof(OnExceptionRequest));
  ::fidl::DecodedMessage<OnExceptionRequest> _decoded_request(std::move(_request_buffer));
  Super::SetResult(
      Handler::InPlace::OnException(std::move(_client_end), std::move(_decoded_request), std::move(_response_buffer)));
}

Handler::UnownedResultOf::OnException Handler::SyncClient::OnException(::fidl::BytePart _request_buffer, ::zx::exception exception, ::llcpp::fuchsia::exception::ExceptionInfo info, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::OnException(zx::unowned_channel(this->channel_), std::move(_request_buffer), std::move(exception), std::move(info), std::move(_response_buffer));
}

Handler::UnownedResultOf::OnException Handler::Call::OnException(zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, ::zx::exception exception, ::llcpp::fuchsia::exception::ExceptionInfo info, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::OnException(std::move(_client_end), std::move(_request_buffer), std::move(exception), std::move(info), std::move(_response_buffer));
}

::fidl::DecodeResult<Handler::OnExceptionResponse> Handler::InPlace::OnException(zx::unowned_channel _client_end, ::fidl::DecodedMessage<OnExceptionRequest> params, ::fidl::BytePart response_buffer) {
  Handler::SetTransactionHeaderFor::OnExceptionRequest(params);
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::DecodeResult<Handler::OnExceptionResponse>::FromFailure(
        std::move(_encode_request_result));
  }
  auto _call_result = ::fidl::Call<OnExceptionRequest, OnExceptionResponse>(
    std::move(_client_end), std::move(_encode_request_result.message), std::move(response_buffer));
  if (_call_result.status != ZX_OK) {
    return ::fidl::DecodeResult<Handler::OnExceptionResponse>::FromFailure(
        std::move(_call_result));
  }
  return ::fidl::Decode(std::move(_call_result.message));
}


bool Handler::TryDispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
  if (msg->num_bytes < sizeof(fidl_message_header_t)) {
    zx_handle_close_many(msg->handles, msg->num_handles);
    txn->Close(ZX_ERR_INVALID_ARGS);
    return true;
  }
  fidl_message_header_t* hdr = reinterpret_cast<fidl_message_header_t*>(msg->bytes);
  switch (hdr->ordinal) {
    case kHandler_OnException_Ordinal:
    case kHandler_OnException_GenOrdinal:
    {
      auto result = ::fidl::DecodeAs<OnExceptionRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      auto message = result.message.message();
      impl->OnException(std::move(message->exception), std::move(message->info),
          Interface::OnExceptionCompleter::Sync(txn));
      return true;
    }
    default: {
      return false;
    }
  }
}

bool Handler::Dispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
  bool found = TryDispatch(impl, msg, txn);
  if (!found) {
    zx_handle_close_many(msg->handles, msg->num_handles);
    txn->Close(ZX_ERR_NOT_SUPPORTED);
  }
  return found;
}


void Handler::Interface::OnExceptionCompleterBase::Reply() {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<OnExceptionResponse, ::fidl::MessageDirection::kSending>();
  FIDL_ALIGNDECL uint8_t _write_bytes[_kWriteAllocSize] = {};
  auto& _response = *reinterpret_cast<OnExceptionResponse*>(_write_bytes);
  Handler::SetTransactionHeaderFor::OnExceptionResponse(
      ::fidl::DecodedMessage<OnExceptionResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              OnExceptionResponse::PrimarySize,
              OnExceptionResponse::PrimarySize)));
  ::fidl::BytePart _response_bytes(_write_bytes, _kWriteAllocSize, sizeof(OnExceptionResponse));
  CompleterBase::SendReply(::fidl::DecodedMessage<OnExceptionResponse>(std::move(_response_bytes)));
}



void Handler::SetTransactionHeaderFor::OnExceptionRequest(const ::fidl::DecodedMessage<Handler::OnExceptionRequest>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kHandler_OnException_Ordinal);
}
void Handler::SetTransactionHeaderFor::OnExceptionResponse(const ::fidl::DecodedMessage<Handler::OnExceptionResponse>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kHandler_OnException_Ordinal);
}

::llcpp::fuchsia::exception::ProcessExceptionMetadata::Builder ProcessExceptionMetadata::Build() {
  return ProcessExceptionMetadata::Builder();
}

auto ::llcpp::fuchsia::exception::ProcessExceptionMetadata::Builder::set_info(::llcpp::fuchsia::exception::ExceptionInfo* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[1 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 1) {
    max_ordinal_ = 1;
  }
  return std::move(*this);
}

auto ::llcpp::fuchsia::exception::ProcessExceptionMetadata::Builder::set_process(::zx::process* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[2 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 2) {
    max_ordinal_ = 2;
  }
  return std::move(*this);
}

auto ::llcpp::fuchsia::exception::ProcessExceptionMetadata::Builder::set_thread(::zx::thread* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[3 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 3) {
    max_ordinal_ = 3;
  }
  return std::move(*this);
}

::llcpp::fuchsia::exception::ProcessException::Builder ProcessException::Build() {
  return ProcessException::Builder();
}

auto ::llcpp::fuchsia::exception::ProcessException::Builder::set_exception(::zx::exception* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[1 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 1) {
    max_ordinal_ = 1;
  }
  return std::move(*this);
}

auto ::llcpp::fuchsia::exception::ProcessException::Builder::set_info(::llcpp::fuchsia::exception::ExceptionInfo* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[2 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 2) {
    max_ordinal_ = 2;
  }
  return std::move(*this);
}

auto ::llcpp::fuchsia::exception::ProcessException::Builder::set_process(::zx::process* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[3 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 3) {
    max_ordinal_ = 3;
  }
  return std::move(*this);
}

auto ::llcpp::fuchsia::exception::ProcessException::Builder::set_thread(::zx::thread* elem) -> Builder&& {
  ZX_ASSERT(elem);
  envelopes_[4 - 1].data = static_cast<void*>(elem);
  if (max_ordinal_ < 4) {
    max_ordinal_ = 4;
  }
  return std::move(*this);
}

::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::ProcessLimbo_RetrieveException_Result() {
  tag_ = Tag::Invalid;
}

::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::~ProcessLimbo_RetrieveException_Result() {
  Destroy();
}

void ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::Destroy() {
  switch (which()) {
  case Tag::kResponse:
    response_.~ProcessLimbo_RetrieveException_Response();
    break;
  default:
    break;
  }
  tag_ = Tag::Invalid;
}

void ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::MoveImpl_(ProcessLimbo_RetrieveException_Result&& other) {
  switch (other.which()) {
  case Tag::kResponse:
    mutable_response() = std::move(other.mutable_response());
    break;
  case Tag::kErr:
    mutable_err() = std::move(other.mutable_err());
    break;
  default:
    break;
  }
  other.Destroy();
}

void ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::SizeAndOffsetAssertionHelper() {
  static_assert(offsetof(::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result, response_) == 8);
  static_assert(offsetof(::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result, err_) == 8);
  static_assert(sizeof(::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result) == ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::PrimarySize);
}


::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Response& ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::mutable_response() {
  if (which() != Tag::kResponse) {
    Destroy();
    new (&response_) ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Response;
  }
  tag_ = Tag::kResponse;
  return response_;
}

int32_t& ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result::mutable_err() {
  if (which() != Tag::kErr) {
    Destroy();
    new (&err_) int32_t;
  }
  tag_ = Tag::kErr;
  return err_;
}


namespace {

[[maybe_unused]]
constexpr uint64_t kProcessLimbo_ListProcessesWaitingOnException_Ordinal = 0x7bde19d00000000lu;
[[maybe_unused]]
constexpr uint64_t kProcessLimbo_ListProcessesWaitingOnException_GenOrdinal = 0x43e1864bf356ef50lu;
extern "C" const fidl_type_t fuchsia_exception_ProcessLimboListProcessesWaitingOnExceptionRequestTable;
extern "C" const fidl_type_t fuchsia_exception_ProcessLimboListProcessesWaitingOnExceptionResponseTable;
extern "C" const fidl_type_t v1_fuchsia_exception_ProcessLimboListProcessesWaitingOnExceptionResponseTable;
[[maybe_unused]]
constexpr uint64_t kProcessLimbo_RetrieveException_Ordinal = 0x47ad466f00000000lu;
[[maybe_unused]]
constexpr uint64_t kProcessLimbo_RetrieveException_GenOrdinal = 0x6ee2df26d6e0444blu;
extern "C" const fidl_type_t fuchsia_exception_ProcessLimboRetrieveExceptionRequestTable;
extern "C" const fidl_type_t fuchsia_exception_ProcessLimboRetrieveExceptionResponseTable;
extern "C" const fidl_type_t v1_fuchsia_exception_ProcessLimboRetrieveExceptionResponseTable;
[[maybe_unused]]
constexpr uint64_t kProcessLimbo_ReleaseProcess_Ordinal = 0x7adea12100000000lu;
[[maybe_unused]]
constexpr uint64_t kProcessLimbo_ReleaseProcess_GenOrdinal = 0x76911553fea22b87lu;
extern "C" const fidl_type_t fuchsia_exception_ProcessLimboReleaseProcessRequestTable;
extern "C" const fidl_type_t fuchsia_exception_ProcessLimboReleaseProcessResponseTable;
extern "C" const fidl_type_t v1_fuchsia_exception_ProcessLimboReleaseProcessResponseTable;

}  // namespace
template <>
ProcessLimbo::ResultOf::ListProcessesWaitingOnException_Impl<ProcessLimbo::ListProcessesWaitingOnExceptionResponse>::ListProcessesWaitingOnException_Impl(zx::unowned_channel _client_end) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<ListProcessesWaitingOnExceptionRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, ListProcessesWaitingOnExceptionRequest::PrimarySize);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(ListProcessesWaitingOnExceptionRequest));
  ::fidl::DecodedMessage<ListProcessesWaitingOnExceptionRequest> _decoded_request(std::move(_request_bytes));
  Super::SetResult(
      ProcessLimbo::InPlace::ListProcessesWaitingOnException(std::move(_client_end), Super::response_buffer()));
}

ProcessLimbo::ResultOf::ListProcessesWaitingOnException ProcessLimbo::SyncClient::ListProcessesWaitingOnException() {
  return ResultOf::ListProcessesWaitingOnException(zx::unowned_channel(this->channel_));
}

ProcessLimbo::ResultOf::ListProcessesWaitingOnException ProcessLimbo::Call::ListProcessesWaitingOnException(zx::unowned_channel _client_end) {
  return ResultOf::ListProcessesWaitingOnException(std::move(_client_end));
}

template <>
ProcessLimbo::UnownedResultOf::ListProcessesWaitingOnException_Impl<ProcessLimbo::ListProcessesWaitingOnExceptionResponse>::ListProcessesWaitingOnException_Impl(zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer) {
  FIDL_ALIGNDECL uint8_t _write_bytes[sizeof(ListProcessesWaitingOnExceptionRequest)] = {};
  ::fidl::BytePart _request_buffer(_write_bytes, sizeof(_write_bytes));
  memset(_request_buffer.data(), 0, ListProcessesWaitingOnExceptionRequest::PrimarySize);
  _request_buffer.set_actual(sizeof(ListProcessesWaitingOnExceptionRequest));
  ::fidl::DecodedMessage<ListProcessesWaitingOnExceptionRequest> _decoded_request(std::move(_request_buffer));
  Super::SetResult(
      ProcessLimbo::InPlace::ListProcessesWaitingOnException(std::move(_client_end), std::move(_response_buffer)));
}

ProcessLimbo::UnownedResultOf::ListProcessesWaitingOnException ProcessLimbo::SyncClient::ListProcessesWaitingOnException(::fidl::BytePart _response_buffer) {
  return UnownedResultOf::ListProcessesWaitingOnException(zx::unowned_channel(this->channel_), std::move(_response_buffer));
}

ProcessLimbo::UnownedResultOf::ListProcessesWaitingOnException ProcessLimbo::Call::ListProcessesWaitingOnException(zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::ListProcessesWaitingOnException(std::move(_client_end), std::move(_response_buffer));
}

::fidl::DecodeResult<ProcessLimbo::ListProcessesWaitingOnExceptionResponse> ProcessLimbo::InPlace::ListProcessesWaitingOnException(zx::unowned_channel _client_end, ::fidl::BytePart response_buffer) {
  constexpr uint32_t _write_num_bytes = sizeof(ListProcessesWaitingOnExceptionRequest);
  ::fidl::internal::AlignedBuffer<_write_num_bytes> _write_bytes;
  ::fidl::BytePart _request_buffer = _write_bytes.view();
  _request_buffer.set_actual(_write_num_bytes);
  ::fidl::DecodedMessage<ListProcessesWaitingOnExceptionRequest> params(std::move(_request_buffer));
  ProcessLimbo::SetTransactionHeaderFor::ListProcessesWaitingOnExceptionRequest(params);
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ProcessLimbo::ListProcessesWaitingOnExceptionResponse>::FromFailure(
        std::move(_encode_request_result));
  }
  auto _call_result = ::fidl::Call<ListProcessesWaitingOnExceptionRequest, ListProcessesWaitingOnExceptionResponse>(
    std::move(_client_end), std::move(_encode_request_result.message), std::move(response_buffer));
  if (_call_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ProcessLimbo::ListProcessesWaitingOnExceptionResponse>::FromFailure(
        std::move(_call_result));
  }
  return ::fidl::Decode(std::move(_call_result.message));
}

template <>
ProcessLimbo::ResultOf::RetrieveException_Impl<ProcessLimbo::RetrieveExceptionResponse>::RetrieveException_Impl(zx::unowned_channel _client_end, uint64_t process_koid) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<RetrieveExceptionRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, RetrieveExceptionRequest::PrimarySize);
  auto& _request = *reinterpret_cast<RetrieveExceptionRequest*>(_write_bytes);
  _request.process_koid = std::move(process_koid);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(RetrieveExceptionRequest));
  ::fidl::DecodedMessage<RetrieveExceptionRequest> _decoded_request(std::move(_request_bytes));
  Super::SetResult(
      ProcessLimbo::InPlace::RetrieveException(std::move(_client_end), std::move(_decoded_request), Super::response_buffer()));
}

ProcessLimbo::ResultOf::RetrieveException ProcessLimbo::SyncClient::RetrieveException(uint64_t process_koid) {
  return ResultOf::RetrieveException(zx::unowned_channel(this->channel_), std::move(process_koid));
}

ProcessLimbo::ResultOf::RetrieveException ProcessLimbo::Call::RetrieveException(zx::unowned_channel _client_end, uint64_t process_koid) {
  return ResultOf::RetrieveException(std::move(_client_end), std::move(process_koid));
}

template <>
ProcessLimbo::UnownedResultOf::RetrieveException_Impl<ProcessLimbo::RetrieveExceptionResponse>::RetrieveException_Impl(zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint64_t process_koid, ::fidl::BytePart _response_buffer) {
  if (_request_buffer.capacity() < RetrieveExceptionRequest::PrimarySize) {
    Super::SetFailure(::fidl::DecodeResult<RetrieveExceptionResponse>(ZX_ERR_BUFFER_TOO_SMALL, ::fidl::internal::kErrorRequestBufferTooSmall));
    return;
  }
  memset(_request_buffer.data(), 0, RetrieveExceptionRequest::PrimarySize);
  auto& _request = *reinterpret_cast<RetrieveExceptionRequest*>(_request_buffer.data());
  _request.process_koid = std::move(process_koid);
  _request_buffer.set_actual(sizeof(RetrieveExceptionRequest));
  ::fidl::DecodedMessage<RetrieveExceptionRequest> _decoded_request(std::move(_request_buffer));
  Super::SetResult(
      ProcessLimbo::InPlace::RetrieveException(std::move(_client_end), std::move(_decoded_request), std::move(_response_buffer)));
}

ProcessLimbo::UnownedResultOf::RetrieveException ProcessLimbo::SyncClient::RetrieveException(::fidl::BytePart _request_buffer, uint64_t process_koid, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::RetrieveException(zx::unowned_channel(this->channel_), std::move(_request_buffer), std::move(process_koid), std::move(_response_buffer));
}

ProcessLimbo::UnownedResultOf::RetrieveException ProcessLimbo::Call::RetrieveException(zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint64_t process_koid, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::RetrieveException(std::move(_client_end), std::move(_request_buffer), std::move(process_koid), std::move(_response_buffer));
}

::fidl::DecodeResult<ProcessLimbo::RetrieveExceptionResponse> ProcessLimbo::InPlace::RetrieveException(zx::unowned_channel _client_end, ::fidl::DecodedMessage<RetrieveExceptionRequest> params, ::fidl::BytePart response_buffer) {
  ProcessLimbo::SetTransactionHeaderFor::RetrieveExceptionRequest(params);
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ProcessLimbo::RetrieveExceptionResponse>::FromFailure(
        std::move(_encode_request_result));
  }
  auto _call_result = ::fidl::Call<RetrieveExceptionRequest, RetrieveExceptionResponse>(
    std::move(_client_end), std::move(_encode_request_result.message), std::move(response_buffer));
  if (_call_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ProcessLimbo::RetrieveExceptionResponse>::FromFailure(
        std::move(_call_result));
  }
  return ::fidl::Decode(std::move(_call_result.message));
}

template <>
ProcessLimbo::ResultOf::ReleaseProcess_Impl<ProcessLimbo::ReleaseProcessResponse>::ReleaseProcess_Impl(zx::unowned_channel _client_end, uint64_t process_koid) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<ReleaseProcessRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, ReleaseProcessRequest::PrimarySize);
  auto& _request = *reinterpret_cast<ReleaseProcessRequest*>(_write_bytes);
  _request.process_koid = std::move(process_koid);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(ReleaseProcessRequest));
  ::fidl::DecodedMessage<ReleaseProcessRequest> _decoded_request(std::move(_request_bytes));
  Super::SetResult(
      ProcessLimbo::InPlace::ReleaseProcess(std::move(_client_end), std::move(_decoded_request), Super::response_buffer()));
}

ProcessLimbo::ResultOf::ReleaseProcess ProcessLimbo::SyncClient::ReleaseProcess(uint64_t process_koid) {
  return ResultOf::ReleaseProcess(zx::unowned_channel(this->channel_), std::move(process_koid));
}

ProcessLimbo::ResultOf::ReleaseProcess ProcessLimbo::Call::ReleaseProcess(zx::unowned_channel _client_end, uint64_t process_koid) {
  return ResultOf::ReleaseProcess(std::move(_client_end), std::move(process_koid));
}

template <>
ProcessLimbo::UnownedResultOf::ReleaseProcess_Impl<ProcessLimbo::ReleaseProcessResponse>::ReleaseProcess_Impl(zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint64_t process_koid, ::fidl::BytePart _response_buffer) {
  if (_request_buffer.capacity() < ReleaseProcessRequest::PrimarySize) {
    Super::SetFailure(::fidl::DecodeResult<ReleaseProcessResponse>(ZX_ERR_BUFFER_TOO_SMALL, ::fidl::internal::kErrorRequestBufferTooSmall));
    return;
  }
  memset(_request_buffer.data(), 0, ReleaseProcessRequest::PrimarySize);
  auto& _request = *reinterpret_cast<ReleaseProcessRequest*>(_request_buffer.data());
  _request.process_koid = std::move(process_koid);
  _request_buffer.set_actual(sizeof(ReleaseProcessRequest));
  ::fidl::DecodedMessage<ReleaseProcessRequest> _decoded_request(std::move(_request_buffer));
  Super::SetResult(
      ProcessLimbo::InPlace::ReleaseProcess(std::move(_client_end), std::move(_decoded_request), std::move(_response_buffer)));
}

ProcessLimbo::UnownedResultOf::ReleaseProcess ProcessLimbo::SyncClient::ReleaseProcess(::fidl::BytePart _request_buffer, uint64_t process_koid, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::ReleaseProcess(zx::unowned_channel(this->channel_), std::move(_request_buffer), std::move(process_koid), std::move(_response_buffer));
}

ProcessLimbo::UnownedResultOf::ReleaseProcess ProcessLimbo::Call::ReleaseProcess(zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint64_t process_koid, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::ReleaseProcess(std::move(_client_end), std::move(_request_buffer), std::move(process_koid), std::move(_response_buffer));
}

::fidl::DecodeResult<ProcessLimbo::ReleaseProcessResponse> ProcessLimbo::InPlace::ReleaseProcess(zx::unowned_channel _client_end, ::fidl::DecodedMessage<ReleaseProcessRequest> params, ::fidl::BytePart response_buffer) {
  ProcessLimbo::SetTransactionHeaderFor::ReleaseProcessRequest(params);
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ProcessLimbo::ReleaseProcessResponse>::FromFailure(
        std::move(_encode_request_result));
  }
  auto _call_result = ::fidl::Call<ReleaseProcessRequest, ReleaseProcessResponse>(
    std::move(_client_end), std::move(_encode_request_result.message), std::move(response_buffer));
  if (_call_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ProcessLimbo::ReleaseProcessResponse>::FromFailure(
        std::move(_call_result));
  }
  return ::fidl::Decode(std::move(_call_result.message));
}


bool ProcessLimbo::TryDispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
  if (msg->num_bytes < sizeof(fidl_message_header_t)) {
    zx_handle_close_many(msg->handles, msg->num_handles);
    txn->Close(ZX_ERR_INVALID_ARGS);
    return true;
  }
  fidl_message_header_t* hdr = reinterpret_cast<fidl_message_header_t*>(msg->bytes);
  switch (hdr->ordinal) {
    case kProcessLimbo_ListProcessesWaitingOnException_Ordinal:
    case kProcessLimbo_ListProcessesWaitingOnException_GenOrdinal:
    {
      auto result = ::fidl::DecodeAs<ListProcessesWaitingOnExceptionRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      impl->ListProcessesWaitingOnException(
          Interface::ListProcessesWaitingOnExceptionCompleter::Sync(txn));
      return true;
    }
    case kProcessLimbo_RetrieveException_Ordinal:
    case kProcessLimbo_RetrieveException_GenOrdinal:
    {
      auto result = ::fidl::DecodeAs<RetrieveExceptionRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      auto message = result.message.message();
      impl->RetrieveException(std::move(message->process_koid),
          Interface::RetrieveExceptionCompleter::Sync(txn));
      return true;
    }
    case kProcessLimbo_ReleaseProcess_Ordinal:
    case kProcessLimbo_ReleaseProcess_GenOrdinal:
    {
      auto result = ::fidl::DecodeAs<ReleaseProcessRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      auto message = result.message.message();
      impl->ReleaseProcess(std::move(message->process_koid),
          Interface::ReleaseProcessCompleter::Sync(txn));
      return true;
    }
    default: {
      return false;
    }
  }
}

bool ProcessLimbo::Dispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
  bool found = TryDispatch(impl, msg, txn);
  if (!found) {
    zx_handle_close_many(msg->handles, msg->num_handles);
    txn->Close(ZX_ERR_NOT_SUPPORTED);
  }
  return found;
}


void ProcessLimbo::Interface::ListProcessesWaitingOnExceptionCompleterBase::Reply(::fidl::VectorView<::llcpp::fuchsia::exception::ProcessExceptionMetadata> exception_list) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<ListProcessesWaitingOnExceptionResponse, ::fidl::MessageDirection::kSending>();
  std::unique_ptr<uint8_t[]> _write_bytes_unique_ptr(new uint8_t[_kWriteAllocSize]);
  uint8_t* _write_bytes = _write_bytes_unique_ptr.get();
  ListProcessesWaitingOnExceptionResponse _response = {};
  ProcessLimbo::SetTransactionHeaderFor::ListProcessesWaitingOnExceptionResponse(
      ::fidl::DecodedMessage<ListProcessesWaitingOnExceptionResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              ListProcessesWaitingOnExceptionResponse::PrimarySize,
              ListProcessesWaitingOnExceptionResponse::PrimarySize)));
  _response.exception_list = std::move(exception_list);
  auto _linearize_result = ::fidl::Linearize(&_response, ::fidl::BytePart(_write_bytes,
                                                                          _kWriteAllocSize));
  if (_linearize_result.status != ZX_OK) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  CompleterBase::SendReply(std::move(_linearize_result.message));
}

void ProcessLimbo::Interface::ListProcessesWaitingOnExceptionCompleterBase::Reply(::fidl::BytePart _buffer, ::fidl::VectorView<::llcpp::fuchsia::exception::ProcessExceptionMetadata> exception_list) {
  if (_buffer.capacity() < ListProcessesWaitingOnExceptionResponse::PrimarySize) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  ListProcessesWaitingOnExceptionResponse _response = {};
  ProcessLimbo::SetTransactionHeaderFor::ListProcessesWaitingOnExceptionResponse(
      ::fidl::DecodedMessage<ListProcessesWaitingOnExceptionResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              ListProcessesWaitingOnExceptionResponse::PrimarySize,
              ListProcessesWaitingOnExceptionResponse::PrimarySize)));
  _response.exception_list = std::move(exception_list);
  auto _linearize_result = ::fidl::Linearize(&_response, std::move(_buffer));
  if (_linearize_result.status != ZX_OK) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  CompleterBase::SendReply(std::move(_linearize_result.message));
}

void ProcessLimbo::Interface::ListProcessesWaitingOnExceptionCompleterBase::Reply(::fidl::DecodedMessage<ListProcessesWaitingOnExceptionResponse> params) {
  ProcessLimbo::SetTransactionHeaderFor::ListProcessesWaitingOnExceptionResponse(params);
  CompleterBase::SendReply(std::move(params));
}


void ProcessLimbo::Interface::RetrieveExceptionCompleterBase::Reply(::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result result) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<RetrieveExceptionResponse, ::fidl::MessageDirection::kSending>();
  std::unique_ptr<uint8_t[]> _write_bytes_unique_ptr(new uint8_t[_kWriteAllocSize]);
  uint8_t* _write_bytes = _write_bytes_unique_ptr.get();
  RetrieveExceptionResponse _response = {};
  ProcessLimbo::SetTransactionHeaderFor::RetrieveExceptionResponse(
      ::fidl::DecodedMessage<RetrieveExceptionResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              RetrieveExceptionResponse::PrimarySize,
              RetrieveExceptionResponse::PrimarySize)));
  _response.result = std::move(result);
  auto _linearize_result = ::fidl::Linearize(&_response, ::fidl::BytePart(_write_bytes,
                                                                          _kWriteAllocSize));
  if (_linearize_result.status != ZX_OK) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  CompleterBase::SendReply(std::move(_linearize_result.message));
}
void ProcessLimbo::Interface::RetrieveExceptionCompleterBase::ReplySuccess(::llcpp::fuchsia::exception::ProcessException process_exception) {
  ProcessLimbo_RetrieveException_Response response;
  response.process_exception = std::move(process_exception);

  Reply(ProcessLimbo_RetrieveException_Result::WithResponse(std::move(response)));
}
void ProcessLimbo::Interface::RetrieveExceptionCompleterBase::ReplyError(int32_t error) {
  Reply(ProcessLimbo_RetrieveException_Result::WithErr(std::move(error)));
}

void ProcessLimbo::Interface::RetrieveExceptionCompleterBase::Reply(::fidl::BytePart _buffer, ::llcpp::fuchsia::exception::ProcessLimbo_RetrieveException_Result result) {
  if (_buffer.capacity() < RetrieveExceptionResponse::PrimarySize) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  RetrieveExceptionResponse _response = {};
  ProcessLimbo::SetTransactionHeaderFor::RetrieveExceptionResponse(
      ::fidl::DecodedMessage<RetrieveExceptionResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              RetrieveExceptionResponse::PrimarySize,
              RetrieveExceptionResponse::PrimarySize)));
  _response.result = std::move(result);
  auto _linearize_result = ::fidl::Linearize(&_response, std::move(_buffer));
  if (_linearize_result.status != ZX_OK) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  CompleterBase::SendReply(std::move(_linearize_result.message));
}
void ProcessLimbo::Interface::RetrieveExceptionCompleterBase::ReplySuccess(::fidl::BytePart _buffer, ::llcpp::fuchsia::exception::ProcessException process_exception) {
  ProcessLimbo_RetrieveException_Response response;
  response.process_exception = std::move(process_exception);

  Reply(std::move(_buffer), ProcessLimbo_RetrieveException_Result::WithResponse(std::move(response)));
}

void ProcessLimbo::Interface::RetrieveExceptionCompleterBase::Reply(::fidl::DecodedMessage<RetrieveExceptionResponse> params) {
  ProcessLimbo::SetTransactionHeaderFor::RetrieveExceptionResponse(params);
  CompleterBase::SendReply(std::move(params));
}


void ProcessLimbo::Interface::ReleaseProcessCompleterBase::Reply(::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result result) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<ReleaseProcessResponse, ::fidl::MessageDirection::kSending>();
  FIDL_ALIGNDECL uint8_t _write_bytes[_kWriteAllocSize] = {};
  auto& _response = *reinterpret_cast<ReleaseProcessResponse*>(_write_bytes);
  ProcessLimbo::SetTransactionHeaderFor::ReleaseProcessResponse(
      ::fidl::DecodedMessage<ReleaseProcessResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              ReleaseProcessResponse::PrimarySize,
              ReleaseProcessResponse::PrimarySize)));
  _response.result = std::move(result);
  ::fidl::BytePart _response_bytes(_write_bytes, _kWriteAllocSize, sizeof(ReleaseProcessResponse));
  CompleterBase::SendReply(::fidl::DecodedMessage<ReleaseProcessResponse>(std::move(_response_bytes)));
}
void ProcessLimbo::Interface::ReleaseProcessCompleterBase::ReplySuccess() {
  ProcessLimbo_ReleaseProcess_Response response;

  Reply(ProcessLimbo_ReleaseProcess_Result::WithResponse(std::move(response)));
}
void ProcessLimbo::Interface::ReleaseProcessCompleterBase::ReplyError(int32_t error) {
  Reply(ProcessLimbo_ReleaseProcess_Result::WithErr(std::move(error)));
}

void ProcessLimbo::Interface::ReleaseProcessCompleterBase::Reply(::fidl::BytePart _buffer, ::llcpp::fuchsia::exception::ProcessLimbo_ReleaseProcess_Result result) {
  if (_buffer.capacity() < ReleaseProcessResponse::PrimarySize) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  auto& _response = *reinterpret_cast<ReleaseProcessResponse*>(_buffer.data());
  ProcessLimbo::SetTransactionHeaderFor::ReleaseProcessResponse(
      ::fidl::DecodedMessage<ReleaseProcessResponse>(
          ::fidl::BytePart(reinterpret_cast<uint8_t*>(&_response),
              ReleaseProcessResponse::PrimarySize,
              ReleaseProcessResponse::PrimarySize)));
  _response.result = std::move(result);
  _buffer.set_actual(sizeof(ReleaseProcessResponse));
  CompleterBase::SendReply(::fidl::DecodedMessage<ReleaseProcessResponse>(std::move(_buffer)));
}
void ProcessLimbo::Interface::ReleaseProcessCompleterBase::ReplySuccess(::fidl::BytePart _buffer) {
  ProcessLimbo_ReleaseProcess_Response response;

  Reply(std::move(_buffer), ProcessLimbo_ReleaseProcess_Result::WithResponse(std::move(response)));
}

void ProcessLimbo::Interface::ReleaseProcessCompleterBase::Reply(::fidl::DecodedMessage<ReleaseProcessResponse> params) {
  ProcessLimbo::SetTransactionHeaderFor::ReleaseProcessResponse(params);
  CompleterBase::SendReply(std::move(params));
}



void ProcessLimbo::SetTransactionHeaderFor::ListProcessesWaitingOnExceptionRequest(const ::fidl::DecodedMessage<ProcessLimbo::ListProcessesWaitingOnExceptionRequest>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kProcessLimbo_ListProcessesWaitingOnException_Ordinal);
}
void ProcessLimbo::SetTransactionHeaderFor::ListProcessesWaitingOnExceptionResponse(const ::fidl::DecodedMessage<ProcessLimbo::ListProcessesWaitingOnExceptionResponse>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kProcessLimbo_ListProcessesWaitingOnException_Ordinal);
}

void ProcessLimbo::SetTransactionHeaderFor::RetrieveExceptionRequest(const ::fidl::DecodedMessage<ProcessLimbo::RetrieveExceptionRequest>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kProcessLimbo_RetrieveException_Ordinal);
}
void ProcessLimbo::SetTransactionHeaderFor::RetrieveExceptionResponse(const ::fidl::DecodedMessage<ProcessLimbo::RetrieveExceptionResponse>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kProcessLimbo_RetrieveException_Ordinal);
}

void ProcessLimbo::SetTransactionHeaderFor::ReleaseProcessRequest(const ::fidl::DecodedMessage<ProcessLimbo::ReleaseProcessRequest>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kProcessLimbo_ReleaseProcess_Ordinal);
}
void ProcessLimbo::SetTransactionHeaderFor::ReleaseProcessResponse(const ::fidl::DecodedMessage<ProcessLimbo::ReleaseProcessResponse>& _msg) {
  fidl_init_txn_header(&_msg.message()->_hdr, 0, kProcessLimbo_ReleaseProcess_Ordinal);
}

}  // namespace exception
}  // namespace fuchsia
}  // namespace llcpp
