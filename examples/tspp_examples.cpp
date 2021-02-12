// time_seriescpp.cpp : Defines the entry point for the application.
//

#include "tspp_examples.hpp"

#include "utilities.hpp"
#include <tspp/common/chrono_extensions.hpp>
#include <tspp/common/time_series_errors.hpp>
#include <tspp/time_series.hpp>

#include "parquet_tests.hpp"
#include <chrono>
#include <iostream>
#include <numeric>
#include <string>

struct test_t {
	float val;
	int bla;
	std::string meow;

	std::string to_string() const { return "val = " + std::to_string(val) + ", bla= " + std::to_string(bla) + ", meow= " + meow; }
};

void tspp_tests::test_lib() {
	auto x1 = [](double x) -> double { return x * 2.0; };
	tspp::time_series<double> time_series;
	auto epoch = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(0));
	auto ep1 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(1));
	auto ep2 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(2));
	auto ep3 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(3));
	auto ep4 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(4));

	auto ep5 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(5));
	auto ep6 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(6));
	auto ep7 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(7));
	auto ep8 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(8));
	auto ep9 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(9));
	auto ep10 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(10));

	time_series.insert_end(std::make_pair(ep1, 12.0));
	time_series.insert_end(std::make_pair(ep3, 15.0));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("first following ep2, should be 15");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_following(ep2)));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("first prior to ep2, should 12");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_prior(ep2)));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("at or first following ep3, should be 15");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_following(ep3)));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("at or first prior to ep1, should 12");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_prior(ep1)));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("at or first following epoch, should be 12, epoch is not in series");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_following(epoch)));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("at or first prior ep4, should be 15, ep4 is not in series");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_prior(ep4)));

	try {

		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_following(ep4)));
	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("date that is past the last date of the series properly threw");
	}

	try {

		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at_or_first_prior(epoch)));
	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("date that is before the first date of the series properly threw");
	}

	// scraphelpers::helper::PRINT_TEST_MESSAGE(std::to_string(time_series.at(ep1)));
	auto moo = time_series.map<double>(x1);
	tspp_examples_utilities::helper::print_ts(moo);
	auto subs = time_series.between(epoch, ep1);
	tspp_examples_utilities::helper::print_ts(subs);
	try {
		auto subs2 = time_series.between(ep1, epoch);
		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("this should have thrown");
	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::WRITE_MSG("invalid thing properly threw exception =>" + (std::string(ex.what())));
	}

	tspp::time_series<std::string> time_series_string;
	time_series_string.insert_end(std::make_pair(epoch, std::string("bawkawww")));
	tspp::time_series<std::string> time_series_string2 = time_series_string;
	try {

		tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_string.at(ep1));

	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::WRITE_MSG("invalid thing properly threw exception =>" + (std::string(ex.what())));
	}

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("asking try_at, does it exist?");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(std::to_string(time_series_string.try_at(ep1).has_value()));

	time_series_string2.insert_end(std::make_pair(ep1, std::string("moo")));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("original says");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_string.at(epoch));
	// scraphelpers::helper::PRINT_TEST_MESSAGE(time_series_string.at(ep1));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("copy says");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_string2.at(epoch));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_string2.at(ep1));
	time_series_string2.at(epoch) = "im not a chicken im a cow";
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("after mod copy says");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_string2.at(epoch));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("after mod orig says");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_string.at(epoch));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("Testing custom structs");
	tspp::time_series<test_t> time_series_test_t;
	struct test_t t1 {
		12.f, 2, "meow"
	};
	struct test_t t2 {
		15.f, 3, "moo"
	};
	time_series_test_t.insert_end(std::make_pair(epoch, t1));
	time_series_test_t.insert_end(std::make_pair(ep1, t2));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_test_t.at(epoch).to_string());
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(time_series_test_t.at(ep1).to_string());
	auto x_test_t1 = [&](const test_t &x) -> double { return x.val * 2.0; };
	auto moo2 = time_series_test_t.map<double>(x_test_t1);

	tspp_examples_utilities::helper::print_ts(moo2);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("same as above but reverse iter");
	tspp_examples_utilities::helper::reverse_print_ts(moo2);

	// TESTS WRITTEN UNTIL HERE
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test rolling");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input_______________");
	tspp::time_series<double> ts2;

	ts2.insert_end(std::make_pair(epoch, 1.0));
	ts2.insert_end(std::make_pair(ep1, 1.0));
	ts2.insert_end(std::make_pair(ep2, 1.0));
	ts2.insert_end(std::make_pair(ep3, 1.0));
	ts2.insert_end(std::make_pair(ep4, 1.0));
	tspp_examples_utilities::helper::print_ts(ts2);
	std::function<double(std::deque<double>)> xarrfunc = [](std::deque<double> buff) { return std::accumulate(buff.begin(), buff.end(), 0.0); };
	tspp::time_series<double> rolling_sum = ts2.apply_rolling<double>(xarrfunc, 2);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output_______________");
	tspp_examples_utilities::helper::print_ts(rolling_sum);
	tspp::time_series<double> rolling_sum2 = ts2.apply_rolling<double>(xarrfunc, ts2.size());
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Window == TS.size()_______________");
	tspp_examples_utilities::helper::print_ts(rolling_sum2);
	tspp::time_series<double> rolling_sum3 = ts2.apply_rolling<double>(xarrfunc, ts2.size() + 1);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Window > TS.size()_______________");
	tspp_examples_utilities::helper::print_ts(rolling_sum3);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test shift");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input_______________");
	tspp::time_series<double> ts3;
	ts3.insert_end(std::make_pair(epoch, 0.0));
	ts3.insert_end(std::make_pair(ep1, 1.0));
	ts3.insert_end(std::make_pair(ep2, 2.0));
	ts3.insert_end(std::make_pair(ep3, 3.0));
	ts3.insert_end(std::make_pair(ep4, 4.0));
	tspp_examples_utilities::helper::print_ts(ts3);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Lag (-1 Shift)_______________");
	tspp::time_series<double> tslag = ts3.shift(-1);
	tspp_examples_utilities::helper::print_ts(tslag);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output fwd (1 Shift)_______________");
	tspp::time_series<double> tsfwd = ts3.shift(1);
	tspp_examples_utilities::helper::print_ts(tsfwd);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Lag 2 (-2 Shift)_______________");
	tspp::time_series<double> tslag2 = ts3.shift(-2);
	tspp_examples_utilities::helper::print_ts(tslag2);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Lag (OOB Shift)_______________");
	try {
		tspp_examples_utilities::helper::print_ts(ts3.shift(999));
	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::WRITE_MSG("invalid thing properly threw exception =>" + (std::string(ex.what())));
	}

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test skip");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input_______________");
	tspp_examples_utilities::helper::print_ts(ts3);
	auto diff_func = [](double fin, double init) -> double { return fin - init; };
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Change (1 skip)_______________");
	tspp::time_series<double> tsskip = ts3.skip<double>(1, diff_func);
	tspp_examples_utilities::helper::print_ts(tsskip);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Change (2 skip)_______________");
	tspp::time_series<double> tsskip2 = ts3.skip<double>(2, diff_func);
	tspp_examples_utilities::helper::print_ts(tsskip2);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Change (OOB Skip)_______________");
	try {
		tspp_examples_utilities::helper::print_ts(ts3.skip<double>(999, diff_func));
	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::WRITE_MSG("invalid thing properly threw exception =>" + (std::string(ex.what())));
	}

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test skip on index");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input_______________");
	tspp_examples_utilities::helper::print_ts(ts3);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Change (1s skip)_______________");
	auto index_func1s = [](tspp::time_series<double>::key_type key) -> tspp::time_series<double>::key_type { return key - std::chrono::seconds(1); };
	tspp::time_series<double> tsskip_idx = ts3.skip_on_index<double>(index_func1s, diff_func);
	tspp_examples_utilities::helper::print_ts(tsskip_idx);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Change (2s skip)_______________");
	auto index_func2s = [](tspp::time_series<double>::key_type key) -> tspp::time_series<double>::key_type { return key - std::chrono::seconds(2); };
	tspp::time_series<double> tsskip_idx2 = ts3.skip_on_index<double>(index_func2s, diff_func);
	tspp_examples_utilities::helper::print_ts(tsskip_idx2);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output Change (1d Skip)_______________");
	auto index_func1d = [](tspp::time_series<double>::key_type key) -> tspp::time_series<double>::key_type { return key - std::chrono::hours(24); };
	tspp::time_series<double> tsskip_idx3 = ts3.skip_on_index<double>(index_func1d, diff_func);
	time_series_REQUIRE((tsskip_idx3.empty()), "TS is not empty");

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test joins");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Inputs_______________");

	auto pair_func = [](double l, double r) -> std::pair<double, double> { return std::make_pair(l, r); };

	tspp::time_series<double> ts_other;
	ts_other.insert_end(std::make_pair(ep2, 2.0));
	ts_other.insert_end(std::make_pair(ep4, 4.0));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Inputs (Left)_______________");
	tspp_examples_utilities::helper::print_ts(ts3);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Inputs (Right)_______________");
	tspp_examples_utilities::helper::print_ts(ts_other);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test inner joins");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output_______________");
	auto ts_inner_joined = ts3.cross_apply_inner<std::pair<double, double>>(ts_other, pair_func);
	tspp_examples_utilities::helper::print_ts(ts_inner_joined);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test left joins");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output_______________");
	auto ts_left_joined = ts3.cross_apply_left<std::pair<double, double>>(ts_other, std::numeric_limits<double>::quiet_NaN(), pair_func);
	tspp_examples_utilities::helper::print_ts(ts_left_joined);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test chrono extensions");

	auto newtime =
		tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_up_to_nearest_duration(ep1, std::chrono::minutes(5));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("should be 1970-01-01 00:05:00.000");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(newtime);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("should be 1970-01-01 00:00:00.000");
	auto newtime2 =
		tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_down_to_nearest_duration(ep1, std::chrono::minutes(5));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(newtime2);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("should be 1970-01-01 00:00:00.000");
	auto newtime3 =
		tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_nearest_to_nearest_duration(ep1, std::chrono::minutes(5));
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE(newtime3);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test resample groupby");

	tspp::time_series<double> ts_forgrp;

	auto epm1 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(1));
	auto epm2 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(2));
	auto epm3 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(3));
	auto epm16 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(16));

	ts_forgrp.insert_end(std::make_pair(epm1, 2.0));
	ts_forgrp.insert_end(std::make_pair(epm2, 2.0));
	ts_forgrp.insert_end(std::make_pair(epm3, 5.0));
	ts_forgrp.insert_end(std::make_pair(epm16, 99.0));

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input_______________");
	tspp_examples_utilities::helper::print_ts(ts_forgrp);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output_______________");
	auto tsres = ts_forgrp.resample_and_agg<double>(
		[](const std::chrono::high_resolution_clock::time_point &t) {
			return tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_up_to_nearest_duration(
				t, std::chrono::minutes(15));
		},
		[](const std::vector<double> &buff) { return *(buff.end() - 1); });
	tspp_examples_utilities::helper::print_ts(tsres);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("test mergeasof");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Inputs_______________");
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input (left)_______________");
	tspp::time_series<double> ts_mal;

	ts_mal.insert_end(std::make_pair(ep1, 1.0));
	ts_mal.insert_end(std::make_pair(ep2, 1.0));
	ts_mal.insert_end(std::make_pair(ep3, 1.0));
	ts_mal.insert_end(std::make_pair(ep4, 1.0));
	ts_mal.insert_end(std::make_pair(ep5, 1.0));
	ts_mal.insert_end(std::make_pair(ep6, 1.0));
	ts_mal.insert_end(std::make_pair(ep7, 1.0));
	ts_mal.insert_end(std::make_pair(ep8, 1.0));
	ts_mal.insert_end(std::make_pair(ep9, 1.0));
	ts_mal.insert_end(std::make_pair(ep10, 1.0));

	tspp_examples_utilities::helper::print_ts(ts_mal);
	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input (right)_______________");
	tspp::time_series<double> ts_mar;
	ts_mar.insert_end(std::make_pair(ep2, 1.0));
	ts_mar.insert_end(std::make_pair(ep4, 2.0));
	ts_mar.insert_end(std::make_pair(ep5, 3.0));
	ts_mar.insert_end(std::make_pair(ep7, 4.0));
	ts_mar.insert_end(std::make_pair(ep8, 5.0));
	ts_mar.insert_end(std::make_pair(ep10, 6.0));
	tspp_examples_utilities::helper::print_ts(ts_mar);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output (default, no asof merge behavior)_______________");

	auto ts_ma_nm = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar, std::numeric_limits<double>::quiet_NaN(), std::nullopt, pair_func,
																	   tspp::enums::merge_asof_mode::no_roll);
	tspp_examples_utilities::helper::print_ts(ts_ma_nm);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output (merge as of prior 1s)_______________");
	using merge_itert = typename tspp::time_series<double>::const_iterator;
	auto comp_func = tspp::chrono_extensions::merge_helpers<std::chrono::high_resolution_clock::time_point>::merge_asof_prior<merge_itert, merge_itert>(
		std::chrono::seconds(1));
	auto ts_ma_p = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar, std::numeric_limits<double>::quiet_NaN(), comp_func, pair_func,
																	  tspp::enums::merge_asof_mode::roll_prior);
	tspp_examples_utilities::helper::print_ts(ts_ma_p);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output (merge as of prior 2s)_______________");
	auto comp_func2 = tspp::chrono_extensions::merge_helpers<std::chrono::high_resolution_clock::time_point>::merge_asof_prior<merge_itert, merge_itert>(
		std::chrono::seconds(2));
	auto ts_ma_p2 = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar, std::numeric_limits<double>::quiet_NaN(), comp_func2, pair_func,
																	   tspp::enums::merge_asof_mode::roll_prior);
	tspp_examples_utilities::helper::print_ts(ts_ma_p2);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Input (right)_______________");
	tspp::time_series<double> ts_mar2;
	ts_mar2.insert_end(std::make_pair(ep2, 1.0));
	ts_mar2.insert_end(std::make_pair(ep5, 2.0));
	ts_mar2.insert_end(std::make_pair(ep6, 3.0));
	ts_mar2.insert_end(std::make_pair(ep8, 4.0));
	ts_mar2.insert_end(std::make_pair(ep10, 5.0));
	tspp_examples_utilities::helper::print_ts(ts_mar2);

	tspp_examples_utilities::helper::PRINT_TEST_MESSAGE("_____________Output (merge as of fwd 1s)_______________");
	using merge_itert = typename tspp::time_series<double>::const_iterator;
	auto comp_funcfwd = tspp::chrono_extensions::merge_helpers<std::chrono::high_resolution_clock::time_point>::merge_asof_fwd<merge_itert, merge_itert>(
		std::chrono::seconds(1));
	auto ts_ma_f = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar2, std::numeric_limits<double>::quiet_NaN(), comp_funcfwd, pair_func,
																	  tspp::enums::merge_asof_mode::roll_following);
	tspp_examples_utilities::helper::print_ts(ts_ma_f);
};

int main() {
	try {
		tspp_tests::test_lib();
		parquet_io_tests::test_file_load_large_apply();
		parquet_io_tests::test_file_load_large_resampleandagg();
		parquet_io_tests::test_file_load_large_index();
		parquet_io_tests::test_file_load_large_load();
		parquet_io_tests::test_file_load_small();
		parquet_io_tests::test_write_file();
		std::cout << "done"
				  << "\n";
		return 0;
	} catch (const std::exception &ex) {
		tspp_examples_utilities::helper::WRITE_MSG(ex.what());
		return 1;
	}
}
