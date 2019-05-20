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

#ifndef ASYLO_IDENTITY_ADDITIONAL_AUTHENTICATED_DATA_GENERATOR_H_
#define ASYLO_IDENTITY_ADDITIONAL_AUTHENTICATED_DATA_GENERATOR_H_

#include <memory>
#include <string>

#include "absl/strings/string_view.h"
#include "asylo/crypto/util/bytes.h"
#include "asylo/util/statusor.h"

namespace asylo {

extern constexpr int kAdditionalAuthenticatedDataUuidSize = 16;
extern constexpr int kAdditionalAuthenticatedDataPurposeSize = 16;

// Generates additional authenticated data for use in security protocols that
// use enclave assertions. The produced additional authenticated data is 64
// bytes, where the relevant values are encoded as: [0..31] = SHA256(data),
// [32..47] = purpose, [48..63] = UUID.
class AdditionalAuthenticatedDataGenerator {
 public:
  // Creates a generator that uses |uuid| and |purpose|.
  AdditionalAuthenticatedDataGenerator(
      UnsafeBytes<kAdditionalAuthenticatedDataUuidSize> uuid,
      UnsafeBytes<kAdditionalAuthenticatedDataPurposeSize> purpose);

  // Creates a generator that generates AAD suitable for use in assertions for
  // Get PCE Info from Intel's PCE.
  static StatusOr<std::unique_ptr<AdditionalAuthenticatedDataGenerator>>
  CreateGetPceInfoAadGenerator();

  // Creates a generator that generates AAD suitable for use in assertions for
  // PCE Sign Report from Intel's PCE.
  static StatusOr<std::unique_ptr<AdditionalAuthenticatedDataGenerator>>
  CreatePceSignReportAadGenerator();

  // Creates a generator that generates AAD suitable for use in assertions for
  // Enclave Key Exchange Protocol.
  static StatusOr<std::unique_ptr<AdditionalAuthenticatedDataGenerator>>
  CreateEkepAadGenerator();

  // Generates a formatted additional authenticated data value using |data|.
  // Returns a non-OK Status if AAD generation fails.
  StatusOr<std::string> Generate(absl::string_view data);

 private:
  // Defines the UUID used in AAD generated by this object.
  UnsafeBytes<kAdditionalAuthenticatedDataUuidSize> uuid_;

  // Defines the purpose of the AAD generated by this object.
  UnsafeBytes<kAdditionalAuthenticatedDataPurposeSize> purpose_;
};

}  // namespace asylo

#endif  // ASYLO_IDENTITY_ADDITIONAL_AUTHENTICATED_DATA_GENERATOR_H_
