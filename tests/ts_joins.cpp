#include "print_utils.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>

#include <tspp/common/chrono_extensions.hpp>
#include <tspp/time_series.hpp>

#include <chrono>

TEST(TsppTests4, TsJoinTests) {

	auto epoch = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(0));
	auto ep1 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(1));
	auto ep2 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(2));
	auto ep3 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(3));
	auto ep4 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(4));

	tspp::time_series<double> ts3;
	ts3.insert_end(std::make_pair(epoch, 0.0));
	ts3.insert_end(std::make_pair(ep1, 1.0));
	ts3.insert_end(std::make_pair(ep2, 2.0));
	ts3.insert_end(std::make_pair(ep3, 3.0));
	ts3.insert_end(std::make_pair(ep4, 4.0));

	// test joins

	auto pair_func = [](double l, double r) -> std::pair<double, double> { return std::make_pair(l, r); };

	tspp::time_series<double> ts_other;
	ts_other.insert_end(std::make_pair(ep2, 2.0));
	ts_other.insert_end(std::make_pair(ep4, 4.0));

	// test inner joins

	auto ts_inner_joined = ts3.cross_apply_inner<std::pair<double, double>>(ts_other, pair_func);
	tspp::time_series<std::pair<double, double>> ts_inner_joined_exp;
	ts_inner_joined_exp.insert_end(std::make_pair(ep2, std::make_pair(2.0, 2.0)));
	ts_inner_joined_exp.insert_end(std::make_pair(ep4, std::make_pair(4.0, 4.0)));

	EXPECT_EQ(ts_inner_joined, ts_inner_joined_exp);

	// test left joins
	constexpr double _nan = std::numeric_limits<double>::quiet_NaN();
	auto ts_left_joined = ts3.cross_apply_left<std::pair<double, double>>(ts_other, _nan, pair_func);
	tspp::time_series<std::pair<double, double>> ts_left_joined_exp;
	ts_left_joined_exp.insert_end(std::make_pair(epoch, std::make_pair(0.0, _nan)));
	ts_left_joined_exp.insert_end(std::make_pair(ep1, std::make_pair(1.0, _nan)));
	ts_left_joined_exp.insert_end(std::make_pair(ep2, std::make_pair(2.0, 2.0)));
	ts_left_joined_exp.insert_end(std::make_pair(ep3, std::make_pair(3.0, _nan)));
	ts_left_joined_exp.insert_end(std::make_pair(ep4, std::make_pair(4.0, 4.0)));

	EXPECT_EQ(ts_left_joined, ts_left_joined_exp);
};

TEST(TsppTests4, TsChronoJoinTests) {

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

	// test mergeasof
	auto pair_func = [](double l, double r) -> std::pair<double, double> { return std::make_pair(l, r); };
	constexpr double _nan = std::numeric_limits<double>::quiet_NaN();
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

	tspp::time_series<double> ts_mar;
	ts_mar.insert_end(std::make_pair(ep2, 1.0));
	ts_mar.insert_end(std::make_pair(ep4, 2.0));
	ts_mar.insert_end(std::make_pair(ep5, 3.0));
	ts_mar.insert_end(std::make_pair(ep7, 4.0));
	ts_mar.insert_end(std::make_pair(ep8, 5.0));
	ts_mar.insert_end(std::make_pair(ep10, 6.0));

	//_____________Output (default, no asof merge behavior)_______________

	auto ts_ma_nm = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar, _nan, std::nullopt, pair_func, tspp::enums::merge_asof_mode::no_roll);
	tspp::time_series<std::pair<double, double>> ts_ma_nm_exp;
	ts_ma_nm_exp.insert_end(std::make_pair(ep1, std::make_pair(1.0, _nan)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep2, std::make_pair(1.0, 1.0)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep3, std::make_pair(1.0, _nan)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep4, std::make_pair(1.0, 2.0)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep5, std::make_pair(1.0, 3.0)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep6, std::make_pair(1.0, _nan)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep7, std::make_pair(1.0, 4.0)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep8, std::make_pair(1.0, 5.0)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep9, std::make_pair(1.0, _nan)));
	ts_ma_nm_exp.insert_end(std::make_pair(ep10, std::make_pair(1.0, 6.0)));

	EXPECT_EQ(ts_ma_nm, ts_ma_nm_exp);

	//_____________Output (merge as of prior 1s)_______________
	using merge_itert = typename tspp::time_series<double>::const_iterator;
	auto comp_func = tspp::chrono_extensions::merge_helpers<std::chrono::high_resolution_clock::time_point>::merge_asof_prior<merge_itert, merge_itert>(
		std::chrono::seconds(1));
	auto ts_ma_p = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar, _nan, comp_func, pair_func, tspp::enums::merge_asof_mode::roll_prior);

	tspp::time_series<std::pair<double, double>> ts_ma_p_exp;
	ts_ma_p_exp.insert_end(std::make_pair(ep1, std::make_pair(1.0, _nan)));
	ts_ma_p_exp.insert_end(std::make_pair(ep2, std::make_pair(1.0, 1.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep3, std::make_pair(1.0, 1.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep4, std::make_pair(1.0, 2.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep5, std::make_pair(1.0, 3.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep6, std::make_pair(1.0, 3.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep7, std::make_pair(1.0, 4.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep8, std::make_pair(1.0, 5.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep9, std::make_pair(1.0, 5.0)));
	ts_ma_p_exp.insert_end(std::make_pair(ep10, std::make_pair(1.0, 6.0)));

	EXPECT_EQ(ts_ma_p, ts_ma_p_exp);

	//_____________Output (merge as of prior 2s)_______________

	auto comp_func2 = tspp::chrono_extensions::merge_helpers<std::chrono::high_resolution_clock::time_point>::merge_asof_prior<merge_itert, merge_itert>(
		std::chrono::seconds(2));
	auto ts_ma_p2 = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar, _nan, comp_func2, pair_func, tspp::enums::merge_asof_mode::roll_prior);
	tspp::time_series<std::pair<double, double>> ts_ma_p2_exp;
	ts_ma_p2_exp.insert_end(std::make_pair(ep1, std::make_pair(1.0, _nan)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep2, std::make_pair(1.0, 1.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep3, std::make_pair(1.0, 1.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep4, std::make_pair(1.0, 2.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep5, std::make_pair(1.0, 3.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep6, std::make_pair(1.0, 3.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep7, std::make_pair(1.0, 4.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep8, std::make_pair(1.0, 5.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep9, std::make_pair(1.0, 5.0)));
	ts_ma_p2_exp.insert_end(std::make_pair(ep10, std::make_pair(1.0, 6.0)));

	EXPECT_EQ(ts_ma_p2, ts_ma_p2_exp);

	tspp::time_series<double> ts_mar2;
	ts_mar2.insert_end(std::make_pair(ep2, 1.0));
	ts_mar2.insert_end(std::make_pair(ep5, 2.0));
	ts_mar2.insert_end(std::make_pair(ep6, 3.0));
	ts_mar2.insert_end(std::make_pair(ep8, 4.0));
	ts_mar2.insert_end(std::make_pair(ep10, 5.0));

	//_____________Output (merge as of fwd 1s)_______________

	auto comp_funcfwd = tspp::chrono_extensions::merge_helpers<std::chrono::high_resolution_clock::time_point>::merge_asof_fwd<merge_itert, merge_itert>(
		std::chrono::seconds(1));
	auto ts_ma_f = ts_mal.merge_apply_asof<std::pair<double, double>>(ts_mar2, _nan, comp_funcfwd, pair_func, tspp::enums::merge_asof_mode::roll_following);

	tspp::time_series<std::pair<double, double>> ts_ma_f_exp;
	ts_ma_f_exp.insert_end(std::make_pair(ep1, std::make_pair(1.0, 1.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep2, std::make_pair(1.0, 1.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep3, std::make_pair(1.0, _nan)));
	ts_ma_f_exp.insert_end(std::make_pair(ep4, std::make_pair(1.0, 2.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep5, std::make_pair(1.0, 2.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep6, std::make_pair(1.0, 3.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep7, std::make_pair(1.0, 4.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep8, std::make_pair(1.0, 4.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep9, std::make_pair(1.0, 5.0)));
	ts_ma_f_exp.insert_end(std::make_pair(ep10, std::make_pair(1.0, 5.0)));

	EXPECT_EQ(ts_ma_f, ts_ma_f_exp);
};