#pragma once
#ifndef scrapnative_parquet_tests_hpp
#define scrapnative_parquet_tests_hpp
class parquet_io_tests {
public:
    static void test_file_load_large_apply();
    static void test_file_load_large_index();
    static void test_file_load_large_resampleandagg();
    static void test_file_load_large_load();
    static void test_file_load_small();
    static void test_write_file();

};
#endif