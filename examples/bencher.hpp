#pragma once
#ifndef bencher_hpp
#define bencher_hpp

#include <chrono>
#include <ctime>
#include <string>

#include "utilities.hpp"

namespace tspp_examples_utilities {

	template <typename WORKLOAD>
	static void BENCHMARK_WORKLOAD(const std::string &workload_name, int64_t nrep, const WORKLOAD &workload) {
		clock_t start = std::clock();
		std::size_t zz = 0;
		for (int64_t i = 0; i < nrep; i++) {
			zz += workload();
		}
		double duration = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);
		double percall = duration / static_cast<double>(nrep);
		std::string msg3 = "| TotalPoints: " + std::to_string(zz) + " points";
		std::string msg2 = "| PerCall: " + std::to_string(percall) + "s";
		std::string msg = "| Total: " + std::to_string(duration) + "s";
		helper::WRITE_MSG("BENCHMARK! (n=" + std::to_string(nrep) + ") |>" + workload_name + msg + msg2 + msg3);
	};
} // namespace tspp_examples_utilities

#endif