#include <gtest/gtest.h>
#include "test_utils.hpp"

#include <tspp/time_series.hpp>
#include <tspp/common/chrono_extensions.hpp>

#include <chrono>

TEST(TsppTests, ChronoExtenTests) {


    //test chrono extensions

    auto ep1 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(1));

    //should be 1970-01-01 00:05:00.000
    auto newtime = tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_up_to_nearest_duration(ep1, std::chrono::minutes(5));
    EXPECT_EQ(newtime, std::chrono::high_resolution_clock::time_point(std::chrono::minutes(5)));

    //should be 1970-01-01 00:00:00.000
    auto newtime2 = tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_down_to_nearest_duration(ep1, std::chrono::minutes(5));
    EXPECT_EQ(newtime2, std::chrono::high_resolution_clock::time_point(std::chrono::minutes(0)));

    //should be 1970-01-01 00:00:00.000
    auto newtime3 = tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_nearest_to_nearest_duration(ep1, std::chrono::minutes(5));
    EXPECT_EQ(newtime3, std::chrono::high_resolution_clock::time_point(std::chrono::minutes(0)));


    //test resample groupby
    tspp::time_series<double> ts_forgrp;

    auto epm1 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(1));
    auto epm2 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(2));
    auto epm3 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(3));
    auto epm16 = std::chrono::high_resolution_clock::time_point(std::chrono::minutes(16));

    ts_forgrp.insert_end(std::make_pair(epm1, 2.0));
    ts_forgrp.insert_end(std::make_pair(epm2, 2.0));
    ts_forgrp.insert_end(std::make_pair(epm3, 5.0));
    ts_forgrp.insert_end(std::make_pair(epm16, 99.0));

    auto tsres = ts_forgrp.resample_and_agg<double>(
        [](const std::chrono::high_resolution_clock::time_point& t) { return tspp::chrono_extensions::time_rounder<std::chrono::high_resolution_clock::time_point>::round_up_to_nearest_duration(t, std::chrono::minutes(15)); },
        [](const std::vector<double>& buff) {return *(buff.end() - 1); }
    );

    //MSG!| > _____________Output_______________
    //    MSG!| > 1970 - 01 - 01 00:15 : 00.000 | 5.000000
    //    MSG!| > 1970 - 01 - 01 00:30 : 00.000 | 99.000000

    tspp::time_series<double> tsres_exp;
    tsres_exp.insert_end(std::make_pair(std::chrono::high_resolution_clock::time_point(std::chrono::minutes(15)), 5.0));
    tsres_exp.insert_end(std::make_pair(std::chrono::high_resolution_clock::time_point(std::chrono::minutes(30)), 99.0));

    EXPECT_EQ(tsres, tsres_exp);

};