/* Copyright 2019 The MLPerf Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef MLPERF_MLPERF_DRIVER_H_
#define MLPERF_MLPERF_DRIVER_H_

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "android/cpp/backend.h"
#include "android/cpp/dataset.h"
#include "loadgen/system_under_test.h"

namespace mlperf {
namespace mobile {

// MlperfDriver implements functions to run a mlperf test.
class MlperfDriver : public ::mlperf::SystemUnderTest {
 public:
  MlperfDriver(std::unique_ptr<Dataset> dataset,
               std::unique_ptr<Backend> backend, const std::string& scenario,
               int batch)
      : dataset_(std::move(dataset)),
        backend_(std::move(backend)),
        scenario_(scenario),
        batch_(batch) {}

  ~MlperfDriver() override {}

  // Runs MLPerf tests.
  void RunMLPerfTest(const std::string& mode, int min_query_count,
                     int min_duration, const std::string& output_dir);

  // A human-readable string for loggin purposes.
  const std::string& Name() const override { return backend_->Name(); }

  // Run N samples generated by loadgen. This function blocks until completion.
  void IssueQuery(const std::vector<::mlperf::QuerySample>& samples) override;

  // Flush the staged queries immediately.
  void FlushQueries() override { backend_->FlushQueries(); }

  // Called by loadgen to show us the recorded latencies.
  void ReportLatencyResults(
      const std::vector<::mlperf::QuerySampleLatency>& latencies_ns) final {
    latencies_ns_.insert(latencies_ns_.end(), latencies_ns.begin(),
                         latencies_ns.end());
  }

  // Calculates the 90 percentile latency from reported latencies.
  float ComputeLatency() {
    if (latencies_ns_.empty()) {
      return 0.0f;
    }
    if (scenario_ == "Offline") {
      return 1000.0f * GetNumSamples() / GetDurationMs();
    }
    std::sort(latencies_ns_.begin(), latencies_ns_.end());
    return static_cast<float>(latencies_ns_[latencies_ns_.size() * 0.9]) / 1e6;
  }

  // Forms a string to report 90 percentile latency in ms.
  std::string ComputeLatencyString() {
    if (latencies_ns_.empty()) {
      return std::string("N/A");
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << ComputeLatency();
    if (scenario_ == "Offline") {
      stream << " qps";
    } else {
      stream << " ms";
    }
    return stream.str();
  }

  // Asks the dataset to calculate the accuracy.
  float ComputeAccuracy() { return dataset_->ComputeAccuracy(); }

  // Forms a string to report the accuracy.
  std::string ComputeAccuracyString() {
    return dataset_->ComputeAccuracyString();
  }

  // Return the number of samples reported.
  size_t GetNumSamples() { return latencies_ns_.size(); }

  // Return the duration reported in ms.
  float GetDurationMs() {
    if (scenario_ == "Offline" && !latencies_ns_.empty()) {
      // In offline mode, all reported latencies are the total latency.
      return static_cast<float>(latencies_ns_[0]) / 1e6;
    }

    float result = 0.0f;
    for (int64_t latency : latencies_ns_) {
      result += static_cast<float>(latency) / 1e6;
    }
    return result;
  }

 private:
  std::unique_ptr<Dataset> dataset_;
  std::unique_ptr<Backend> backend_;
  // Reported latency from MLPerf.
  std::vector<int64_t> latencies_ns_;
  // Offline or SingleStream scenario.
  std::string scenario_;
  int batch_;
};

}  // namespace mobile
}  // namespace mlperf
#endif  // MLPERF_MLPERF_DRIVER_H_
