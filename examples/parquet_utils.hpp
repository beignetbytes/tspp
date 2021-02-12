#pragma once
#ifndef parquet_utils_hpp
#define parquet_utils_hpp

#include <arrow/api.h>
#include <tspp/time_series.hpp>
//#include <cstdint>
//#include <iostream>
//#include <vector>
#include <string>

class parquet_utils {

  public:
	static arrow::Status time_series_from_file(const std::string &file_name, std::shared_ptr<tspp::time_series<double>> ts);
	static void time_series_to_file(const std::string &file_name, std::shared_ptr<tspp::time_series<double>> ts);
};
#endif