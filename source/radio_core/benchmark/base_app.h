// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for benchmarking applications.
//
// Takes care of implementing typical benchmark pipeline, allowing actual
// benchmark application to only focus on algorithmic side without worrying of
// parsing command line, or clocking execution time.

#pragma once

#include <chrono>
#include <cstdlib>
#include <iostream>

#include <argparse/argparse.hpp>

namespace radio_core::benchmark {

class Benchmark {
 public:
  Benchmark() = default;
  ~Benchmark() = default;

  // Disallow copy.
  Benchmark(const Benchmark& other) = delete;
  Benchmark(Benchmark&& other) noexcept = delete;
  auto operator=(const Benchmark& other) -> Benchmark& = delete;
  auto operator=(Benchmark&& other) -> Benchmark& = delete;

  // Entry point to the benchmark application.
  //
  // Implements default pipeline of initialization, logic run, and
  // deinitialization.
  //
  // Returns an application exit code.
  virtual auto Run(int argc, char** argv) -> int {
    argparse::ArgumentParser parser(GetBenchmarkName(),
                                    GetBenchmarkVersion(),
                                    argparse::default_arguments::help);
    // clang-format off
    std::cout <<
      "**********************************************************************"
      << std::endl
      << "** " << GetBenchmarkName() << " benchmark" << std::endl <<
      "**********************************************************************"
      << std::endl;
    // clang-format on

    ConfigureBaseParser(parser);
    ConfigureParser(parser);

    try {
      parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
      std::cerr << err.what() << std::endl;
      std::cerr << std::endl << parser;
      return EXIT_FAILURE;
    }

    if (!HandleBaseArguments(parser) || !HandleArguments(parser)) {
      return EXIT_SUCCESS;
    }

    Initialize();

    const TimePoint time_start{Now()};
    const int num_iterations = GetNumIterations();
    for (int i = 0; i < num_iterations; ++i) {
      Iteration();
    }

    std::chrono::duration<double, std::milli> execution_time_ms{Now() -
                                                                time_start};

    std::cout << std::endl;
    std::cout << "Statistics" << std::endl;
    std::cout << "==========" << std::endl;
    std::cout << "Wall time               : " << execution_time_ms.count()
              << " ms" << std::endl;
    std::cout << "Avg. time per iteration : "
              << execution_time_ms.count() / GetNumIterations() << " ms"
              << std::endl;

    Finalize();

    return 0;
  }

 private:
  // THe number of iterations the benchmark code will be executed.
  int num_iterations_{32768};

  // Configure parser with arguments needed for the base application to
  // function. These arguments are common for all benchmark applications.
  void ConfigureBaseParser(argparse::ArgumentParser& parser) {
    parser.add_argument("--num-iterations")
        .default_value(32768)
        .help("The number of iterations to run the benchmark code")
        .scan<'i', int>();
  }

  // Handle arguments for the base application needs.
  virtual auto HandleBaseArguments(argparse::ArgumentParser& parser) -> bool {
    num_iterations_ = parser.get<int>("--num-iterations");
    return true;
  }

 protected:
  // Configure command line argument parser.
  // This function is run in the beginning of the main pipeline execution in
  // run().
  virtual void ConfigureParser(argparse::ArgumentParser& /*parser*/) {}

  // Handle parsed command line arguments.
  // This function is called after configureParser() but before initialize().
  //
  // If an unsupported configuration is requested via the parser then the
  // function should return false.
  virtual auto HandleArguments(argparse::ArgumentParser& /*parser*/) -> bool {
    return true;
  }

  virtual auto GetBenchmarkName() -> std::string { return "Benchmark"; }
  virtual auto GetBenchmarkVersion() -> std::string { return "0.1"; }

  virtual auto GetNumIterations() -> int { return num_iterations_; }

  // Initialize benchmark.
  // For example, prepare data to be processed.
  // This step is not included into the timing.
  virtual void Initialize() {}

  // One iteration of the calculation.
  virtual void Iteration() = 0;

  // Finalize benchmark.
  // For example, assess the evaluation result, or perform other steps needed
  // to ensure the calculation is not fully optimized out.
  virtual void Finalize() {}

 private:
  using TimePoint = std::chrono::high_resolution_clock::time_point;

  static inline auto Now() -> TimePoint {
    return std::chrono::high_resolution_clock::now();
  }
};

}  // namespace radio_core::benchmark
