/*
 *
 * Copyright 2019 Asylo authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <memory>

#include "asylo/enclave.pb.h"
#include "asylo/grpc/auth/enclave_channel_credentials.h"
#include "asylo/grpc/auth/sgx_local_credentials_options.h"
#include "asylo/identity/sgx/code_identity_util.h"
#include "asylo/identity/sgx/identity_key_management_structs.h"
#include "asylo/identity/sgx/platform_provisioning.h"
#include "asylo/identity/sgx/remote_assertion.pb.h"
#include "asylo/identity/sgx/remote_assertion_generator_test_util_enclave.pb.h"
#include "asylo/identity/sgx/sgx_remote_assertion_generator_client.h"
#include "asylo/trusted_application.h"
#include "asylo/util/status_macros.h"
#include "include/grpcpp/grpcpp.h"

namespace asylo {
namespace sgx {
namespace {

Status GetRemoteAssertion(const GetRemoteAssertionInput &input,
                          GetRemoteAssertionOutput *output) {
  if (!input.has_server_address()) {
    return Status(error::GoogleError::INVALID_ARGUMENT,
                  "EnclaveConfig is missing server_address extension");
  }
  const std::string &server_address = input.server_address();
  LOG(INFO) << "Server address: " << server_address;

  // Make an RPC to the RemoteAssertionGenerator
  std::shared_ptr<::grpc::ChannelCredentials> channel_credentials =
      EnclaveChannelCredentials(BidirectionalSgxLocalCredentialsOptions());
  std::shared_ptr<::grpc::Channel> channel =
      ::grpc::CreateChannel(server_address, channel_credentials);

  SgxRemoteAssertionGeneratorClient client(channel);
  ASYLO_ASSIGN_OR_RETURN(*output->mutable_assertion(),
                         client.GenerateSgxRemoteAssertion("My User Data"));

  return Status::OkStatus();
}

Status GetTargetInfo(GetTargetInfoOutput *output) {
  AlignedTargetinfoPtr targetinfo;
  SetTargetinfoFromSelfIdentity(targetinfo.get());
  output->mutable_target_info_proto()->set_value(
      ConvertTrivialObjectToBinaryString(*targetinfo));
  return Status::OkStatus();
}

Status VerifyReport(const VerifyReportInput &input) {
  AlignedReportPtr report;
  ASYLO_ASSIGN_OR_RETURN(
      *report, ConvertReportProtoToHardwareReport(input.report_proto()));
  return VerifyHardwareReport(*report);
}

}  // namespace

class RemoteAssertionGeneratorTestUtilEnclave final
    : public TrustedApplication {
 public:
  Status Run(const EnclaveInput &input, EnclaveOutput *output) override {
    if (!input.HasExtension(
            remote_assertion_generator_test_util_enclave_input)) {
      return Status(error::GoogleError::INVALID_ARGUMENT,
                    "EnclaveInput is missing "
                    "RemoteAssertionGeneratorTestUtilEnclaveInput extension");
    }

    const RemoteAssertionGeneratorTestUtilEnclaveInput &enclave_input =
        input.GetExtension(remote_assertion_generator_test_util_enclave_input);
    RemoteAssertionGeneratorTestUtilEnclaveOutput *enclave_output =
        output->MutableExtension(
            remote_assertion_generator_test_util_enclave_output);

    switch (enclave_input.input_case()) {
      case RemoteAssertionGeneratorTestUtilEnclaveInput::
          kGetRemoteAssertionInput:
        return GetRemoteAssertion(
            enclave_input.get_remote_assertion_input(),
            enclave_output->mutable_get_remote_assertion_output());
      case RemoteAssertionGeneratorTestUtilEnclaveInput::kGetTargetInfoInput:
        return GetTargetInfo(enclave_output->mutable_get_target_info_output());
      case RemoteAssertionGeneratorTestUtilEnclaveInput::kVerifyReportInput:
        return VerifyReport(enclave_input.verify_report_input());
      default:
        return Status(error::GoogleError::INVALID_ARGUMENT,
                      "RemoteAssertionGeneratorTestUtilEnclaveInput not set");
    }
  }
};

}  // namespace sgx

TrustedApplication *BuildTrustedApplication() {
  return new sgx::RemoteAssertionGeneratorTestUtilEnclave();
}

}  // namespace asylo
