#include <gtest/gtest.h>
#include "test_utils.hpp"

#include <tspp/time_series.hpp>

#include <chrono>

TEST(TsppTests, PointTests) {

    tspp::time_series<double> time_series;
    auto epoch = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(0));
    auto ep1 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(1));
    auto ep2 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(2));
    auto ep3 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(3));
    auto ep4 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(4));

    time_series.insert_end(std::make_pair(ep1, 12.0));
    time_series.insert_end(std::make_pair(ep3, 15.0));

    auto fail_oob_before_first = [&]() {time_series.at_or_first_prior(epoch);  };
    ASSERT_THROW(fail_oob_before_first);

    auto fail_oob_later_than_last = [&]() {time_series.at_or_first_following(ep4); };
    ASSERT_THROW(fail_oob_later_than_last);


    EXPECT_EQ(time_series.at_or_first_following(ep2), 15.0);
    EXPECT_EQ(time_series.at_or_first_prior(ep2), 12.0);

    EXPECT_EQ(time_series.at_or_first_following(ep3), 15.0);
    EXPECT_EQ(time_series.at_or_first_prior(ep1), 12.0);

    EXPECT_EQ(time_series.at_or_first_following(epoch), 12.0);
    EXPECT_EQ(time_series.at_or_first_prior(ep4), 15.0);




    tspp::time_series<std::string> time_series_string;
    time_series_string.insert_end(std::make_pair(epoch, std::string("bawkawww")));
    tspp::time_series<std::string> time_series_string2 = time_series_string;

    auto fail_dne = [&]() { time_series_string.at(ep1);  };
    ASSERT_THROW(fail_dne);

    EXPECT_FALSE(time_series_string.try_at(ep1).has_value());

    time_series_string2.insert_end(std::make_pair(ep1, std::string("moo")));

    EXPECT_NE(time_series_string, time_series_string2);

    EXPECT_EQ(time_series_string2.at(epoch), "bawkawww");
    time_series_string2.at(epoch) = "im not a chicken im a cow";
    EXPECT_EQ(time_series_string2.at(epoch), "im not a chicken im a cow");
    EXPECT_EQ(time_series_string.at(epoch), "bawkawww");

}