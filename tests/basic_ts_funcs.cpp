#include <gtest/gtest.h>
#include "test_utils.hpp"

#include <tspp/time_series.hpp>

#include <chrono>

struct test_t {
    float val;
    int bla;
    std::string meow;

    bool operator==(const test_t& other) const {
        return std::abs(val - other.val) < 0.00001  && bla == other.bla && meow == other.meow;
    };

    std::string to_string() const {
        return "val = " + std::to_string(val) + ", bla= " + std::to_string(bla) + ", meow= " + meow;
    };
};

TEST(TsppTests2, TsBasicFuncTests) {


    auto x1 = [](double x) -> double { return x * 2.0; };
    tspp::time_series<double> time_series;
    auto epoch = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(0));
    auto ep1 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(1));

    auto ep3 = std::chrono::high_resolution_clock::time_point(std::chrono::seconds(3));


    time_series.insert_end(std::make_pair(ep1, 12.0));
    time_series.insert_end(std::make_pair(ep3, 15.0));
    auto time_seriesx2 = time_series.map<double>(x1);
    tspp::time_series<double> time_series_exp;
    time_series_exp.insert_end(std::make_pair(ep1, 24.0));
    time_series_exp.insert_end(std::make_pair(ep3, 30.0));
    EXPECT_EQ(time_seriesx2, time_series_exp);

    auto subs = time_series.between(epoch, ep1);
    tspp::time_series<double> sub_exp;
    sub_exp.insert_end(std::make_pair(ep1, 12.0));
    EXPECT_EQ(subs, sub_exp);

    auto fail_wrong_ord_between = [&]() {time_series.between(ep1, epoch); };
    ASSERT_THROW(fail_wrong_ord_between);



    tspp::time_series<test_t> time_series_test_t;
    struct test_t t1 { 12.f, 2, "meow" };
    struct test_t t2 { 15.f, 3, "moo" };
    time_series_test_t.insert_end(std::make_pair(epoch, t1));
    time_series_test_t.insert_end(std::make_pair(ep1, t2));

    tspp::time_series<test_t> time_series_test_t_exp;
    struct test_t t1e { 12.f, 2, "meow" };
    struct test_t t2e { 15.f, 3, "moo" };
    time_series_test_t_exp.insert_end(std::make_pair(epoch, t1e));
    time_series_test_t_exp.insert_end(std::make_pair(ep1, t2e));


    bool same_ts = time_series_test_t ==  time_series_test_t_exp;
    EXPECT_TRUE(same_ts);
    EXPECT_EQ(time_series_test_t.at(epoch).to_string(), "val = 12.000000, bla= 2, meow= meow");
    EXPECT_EQ(time_series_test_t.at(ep1).to_string(), "val = 15.000000, bla= 3, meow= moo");

    auto x_test_t1 = [](const test_t& x) -> double { return x.val * 2.0; };
    auto moo2 = time_series_test_t.map<double>(x_test_t1);
        
    tspp::time_series<double> time_series_test_t_mod_exp;
    time_series_test_t_mod_exp.insert_end(std::make_pair(epoch, 24.0));
    time_series_test_t_mod_exp.insert_end(std::make_pair(ep1, 30.0));
    bool tmod = moo2 == time_series_test_t_mod_exp;
    EXPECT_TRUE(tmod);
};