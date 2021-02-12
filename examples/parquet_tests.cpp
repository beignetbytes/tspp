#include "parquet_tests.hpp"
#include "parquet_utils.hpp"
#include "utilities.hpp"
#include <string>

#include <tspp/common/chrono_extensions.hpp>
#include <tspp/time_series.hpp>

#include "bencher.hpp"

void parquet_io_tests::test_file_load_large_apply() {

	std::string file_name = R"(/mnt/c/dev/testdata_linux/large.parquet)";

	std::shared_ptr<tspp::time_series<double>> ts = std::make_shared<tspp::time_series<double>>();
	auto pass = parquet_utils::time_series_from_file(file_name, ts);

	auto x1 = [&](double x) { return x * 2.0; };

	if (pass.OK().ok()) {

		tspp_examples_utilities::BENCHMARK_WORKLOAD("Large TS Apply Internal", 100, [&]() {
			tspp::time_series<double> tsx2 = ts->map<double>(x1);
			return tsx2.size();
		});
	} else {
		tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading source file");
	}
};

namespace std {
	template <typename clock, typename duration>
	struct hash<std::chrono::time_point<clock, duration>> {
		using argument_type = std::chrono::time_point<clock, duration>;
		using result_type = std::size_t;
		using rep_type = typename argument_type::rep;
		result_type operator()(argument_type const &s) const {
			return std::hash<rep_type>{}(s.time_since_epoch().count()); // make this aware of the duration bit
		}
	};
} // namespace std

void parquet_io_tests::test_file_load_large_index() {

	std::string file_name = R"(/mnt/c/dev/testdata_linux/large.parquet)";

	std::shared_ptr<tspp::time_series<double>> ts = std::make_shared<tspp::time_series<double>>();
	auto pass = parquet_utils::time_series_from_file(file_name, ts);

	tspp::time_series<double> tsx2 = *ts;
	auto all_dates = tsx2.dates();
	std::vector<tspp::time_series<double>::key_type> dates;
	std::copy(all_dates.begin(), all_dates.end(), std::back_inserter(dates));

	if (pass.OK().ok()) {

		tspp_examples_utilities::BENCHMARK_WORKLOAD("Large TS lookup Internal", 100, [&]() {
			for (auto d : dates) {
				auto lookup = tsx2.at(d);
			}
			return 1;
		});
	} else {
		tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading source file");
	}

	std::unordered_map<tspp::time_series<double>::key_type, double> basemap;
	for (auto it = tsx2.begin(); it != tsx2.end(); ++it) {
		basemap.insert((*it));
	}

	if (pass.OK().ok()) {
		tspp_examples_utilities::BENCHMARK_WORKLOAD("Large lookup vs hashmap", 100, [&]() {
			for (auto d : dates) {
				auto lookup = basemap.at(d);
			}
			return 1;
		});
	} else {
		tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading source file");
	}
};

void parquet_io_tests::test_file_load_large_resampleandagg() {
	std::string file_name = R"(/mnt/c/dev/testdata_linux/large.parquet)";
	std::shared_ptr<tspp::time_series<double>> ts = std::make_shared<tspp::time_series<double>>();
	auto pass = parquet_utils::time_series_from_file(file_name, ts);

	if (pass.OK().ok()) {
		tspp::time_series<double> tsx2 = *ts;
		tspp_examples_utilities::BENCHMARK_WORKLOAD("Bar Data round up", 100, [&]() {
			auto tsres = tsx2.resample_and_agg<double>(
				[](const std::chrono::high_resolution_clock::time_point &t) {
					return tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_up_to_nearest_duration(
						t, std::chrono::minutes(15));
				},
				[](const std::vector<double> &buff) { return *(buff.end() - 1); });
			return tsres.size();
		});
	} else {
		tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading source file");
	}
};

void parquet_io_tests::test_file_load_large_load() {

	std::string file_name = R"(/mnt/c/dev/testdata_linux/large.parquet)";

	tspp_examples_utilities::BENCHMARK_WORKLOAD("Read Large File", 100, [&]() {
		std::shared_ptr<tspp::time_series<double>> ts = std::make_shared<tspp::time_series<double>>();
		auto pass = parquet_utils::time_series_from_file(file_name, ts);
		if (!pass.OK().ok()) {
			tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading");
		}
		return ts->size();
	});
};

void parquet_io_tests::test_write_file() {

	std::string file_src = R"(/mnt/c/dev/testdata_linux/large.parquet)";
	std::string file_dest = R"(/mnt/c/dev/testdata_linux/large_rt.parquet)";

	std::shared_ptr<tspp::time_series<double>> ts = std::make_shared<tspp::time_series<double>>();
	auto pass = parquet_utils::time_series_from_file(file_src, ts);

	if (pass.OK().ok()) {

		tspp_examples_utilities::BENCHMARK_WORKLOAD("Large TS Write", 10, [&]() {
			parquet_utils::time_series_to_file(file_dest, ts);
			return 1;
		});
	} else {
		tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading source file");
	}
}

void parquet_io_tests::test_file_load_small() {

	std::string file_name = R"(/mnt/c/dev/testdata_linux/small.parquet)";

	std::shared_ptr<tspp::time_series<double>> ts = std::make_shared<tspp::time_series<double>>();

	auto pass = parquet_utils::time_series_from_file(file_name, ts);
	if (pass.OK().ok()) {
		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Parquet Deserialize Internal_______________");
		tspp::time_series<double> tsv = *ts;
		tspp_examples_utilities::helper::print_ts(tsv);
	} else {
		tspp_examples_utilities::helper::WRITE_MSG("error encountered while reading source file");
	}
};