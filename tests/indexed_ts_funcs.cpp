#include "test_utils.hpp"
#include <gtest/gtest.h>

#include <tspp/time_series.hpp>

#include <chrono>
#include <numeric>

TEST(TsppTests3, TsIndexedFuncTests) {

	// test rolling
	tspp::time_series<double> ts2;
	auto epoch = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(0));
	auto ep1 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(1));
	auto ep2 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(2));
	auto ep3 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(3));
	auto ep4 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(4));

	ts2.insert_end(std::make_pair(epoch, 1.0));
	ts2.insert_end(std::make_pair(ep1, 1.0));
	ts2.insert_end(std::make_pair(ep2, 1.0));
	ts2.insert_end(std::make_pair(ep3, 1.0));
	ts2.insert_end(std::make_pair(ep4, 1.0));

	auto xarrfunc = [](std::deque<double> buff) { return std::accumulate(buff.begin(), buff.end(), 0.0); };
	tspp::time_series<double> rolling_sum = ts2.apply_rolling<double>(xarrfunc, 2);
	tspp::time_series<double> rolling_sum_exp;
	rolling_sum_exp.insert_end(std::make_pair(ep1, 2.0));
	rolling_sum_exp.insert_end(std::make_pair(ep2, 2.0));
	rolling_sum_exp.insert_end(std::make_pair(ep3, 2.0));
	rolling_sum_exp.insert_end(std::make_pair(ep4, 2.0));

	EXPECT_EQ(rolling_sum, rolling_sum_exp);

	tspp::time_series<double> rolling_sum2 = ts2.apply_rolling<double>(xarrfunc, ts2.size());
	tspp::time_series<double> rolling_sum2_exp;
	rolling_sum2_exp.insert_end(std::make_pair(ep4, 5.0));
	EXPECT_EQ(rolling_sum2, rolling_sum2_exp);
	tspp::time_series<double> rolling_sum3 = ts2.apply_rolling<double>(xarrfunc, ts2.size() + 1);

	EXPECT_TRUE(rolling_sum3.empty());

	// test shift
	tspp::time_series<double> ts3;
	ts3.insert_end(std::make_pair(epoch, 0.0));
	ts3.insert_end(std::make_pair(ep1, 1.0));
	ts3.insert_end(std::make_pair(ep2, 2.0));
	ts3.insert_end(std::make_pair(ep3, 3.0));
	ts3.insert_end(std::make_pair(ep4, 4.0));

	tspp::time_series<double> tslag = ts3.shift(-1);
	tspp::time_series<double> tslag_exp;
	tslag_exp.insert_end(std::make_pair(ep1, 0.0));
	tslag_exp.insert_end(std::make_pair(ep2, 1.0));
	tslag_exp.insert_end(std::make_pair(ep3, 2.0));
	tslag_exp.insert_end(std::make_pair(ep4, 3.0));

	EXPECT_EQ(tslag, tslag_exp);

	tspp::time_series<double> tsfwd = ts3.shift(1);
	tspp::time_series<double> tsfwd_exp;
	tsfwd_exp.insert_end(std::make_pair(epoch, 1.0));
	tsfwd_exp.insert_end(std::make_pair(ep1, 2.0));
	tsfwd_exp.insert_end(std::make_pair(ep2, 3.0));
	tsfwd_exp.insert_end(std::make_pair(ep3, 4.0));

	EXPECT_EQ(tsfwd, tsfwd_exp);

	tspp::time_series<double> tslag2 = ts3.shift(-2);
	tspp::time_series<double> tslag2_exp;
	tslag2_exp.insert_end(std::make_pair(ep2, 0.0));
	tslag2_exp.insert_end(std::make_pair(ep3, 1.0));
	tslag2_exp.insert_end(std::make_pair(ep4, 2.0));

	EXPECT_EQ(tslag2, tslag2_exp);

	auto fail_oobshift = [&]() { ts3.shift(999); };
	ASSERT_THROW(fail_oobshift);

	// test skip

	auto diff_func = [](double fin, double init) -> double { return fin - init; };
	tspp::time_series<double> tsskip = ts3.skip<double>(1, diff_func);
	tspp::time_series<double> tsskip_exp;
	tsskip_exp.insert_end(std::make_pair(ep1, 1.0));
	tsskip_exp.insert_end(std::make_pair(ep2, 1.0));
	tsskip_exp.insert_end(std::make_pair(ep3, 1.0));
	tsskip_exp.insert_end(std::make_pair(ep4, 1.0));

	EXPECT_EQ(tsskip, tsskip_exp);

	tspp::time_series<double> tsskip2 = ts3.skip<double>(2, diff_func);
	tspp::time_series<double> tsskip2_exp;
	tsskip2_exp.insert_end(std::make_pair(ep2, 2.0));
	tsskip2_exp.insert_end(std::make_pair(ep3, 2.0));
	tsskip2_exp.insert_end(std::make_pair(ep4, 2.0));

	EXPECT_EQ(tsskip2, tsskip2_exp);

	auto fail_oobskip = [&]() { ts3.skip<double>(999, diff_func); };

	ASSERT_THROW(fail_oobskip);

	// test skip on index
	auto index_func1s = [](tspp::time_series<double>::key_type key) -> tspp::time_series<double>::key_type { return key - std::chrono::seconds(1); };
	tspp::time_series<double> tsskip_idx = ts3.skip_on_index<double>(index_func1s, diff_func);
	tspp::time_series<double> tsskip_idx_exp;
	tsskip_idx_exp.insert_end(std::make_pair(ep1, 1.0));
	tsskip_idx_exp.insert_end(std::make_pair(ep2, 1.0));
	tsskip_idx_exp.insert_end(std::make_pair(ep3, 1.0));
	tsskip_idx_exp.insert_end(std::make_pair(ep4, 1.0));

	EXPECT_EQ(tsskip_idx, tsskip_idx_exp);

	auto index_func2s = [](tspp::time_series<double>::key_type key) -> tspp::time_series<double>::key_type { return key - std::chrono::seconds(2); };
	tspp::time_series<double> tsskip_idx2 = ts3.skip_on_index<double>(index_func2s, diff_func);
	tspp::time_series<double> tsskip_idx2_exp;
	tsskip_idx2_exp.insert_end(std::make_pair(ep2, 2.0));
	tsskip_idx2_exp.insert_end(std::make_pair(ep3, 2.0));
	tsskip_idx2_exp.insert_end(std::make_pair(ep4, 2.0));

	EXPECT_EQ(tsskip_idx2, tsskip_idx2_exp);

	auto index_func1d = [](tspp::time_series<double>::key_type key) -> tspp::time_series<double>::key_type { return key - std::chrono::hours(24); };
	tspp::time_series<double> tsskip_idx3 = ts3.skip_on_index<double>(index_func1d, diff_func);
	EXPECT_TRUE(tsskip_idx3.empty());
}