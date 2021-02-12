#pragma once
#ifndef printutils_hpp
#define printutils_hpp

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include <iomanip>
#include <tspp/time_series.hpp>

namespace std {
	template <typename t1, typename t2>
	std::string to_string(const std::pair<t1, t2> &s) {
		return "(" + std::to_string(s.first) + "," + std::to_string(s.second) + ")";
	};
} // namespace std

namespace tspp_tests_utilities {
	class helper {
	  private:
		template <typename Clock_T>
		static std::string chrono_to_str(const std::chrono::time_point<Clock_T> &time) {

			typename Clock_T::duration since_epoch = time.time_since_epoch();
			std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
			auto sys_tp = std::chrono::system_clock::time_point(s);
			time_t curr_time = std::chrono::system_clock::to_time_t(sys_tp);
			char sRep[100];
			// struct tm buf;
			std::strftime(sRep, sizeof(sRep), "%Y-%m-%d %H:%M:%S", std::gmtime(&curr_time));

			since_epoch -= s;
			std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
			std::ostringstream stream;
			stream << sRep << "." << std::setfill('0') << std::setw(3) << milli.count();
			return stream.str();
		};

	  public:
		static void PRINT_ERROR_MSG(const std::string &str) { WRITE_MSG("ERROR! |>" + str); };
		static void PRINT_TEST_MESSAGE(const std::string &str) { WRITE_MSG("MSG! |>" + str); };
		template <typename Clock_T>
		static void PRINT_TEST_MESSAGE(const std::chrono::time_point<Clock_T> &tp) {
			WRITE_MSG("MSG! |>" + chrono_to_str(tp));
		};
		static void WRITE_MSG(const std::string &str) { std::cout << str << "\n"; };
		static void WRITE_ERR(const std::string &str) { std::cerr << str << "\n"; };

		template <class T>
		static void print_ts(const tspp::time_series<T, std::chrono::high_resolution_clock::time_point, tspp::default_vec_t> &ts) {
			if (ts.size() > 0) {
				using _iterator = typename tspp::time_series<T, std::chrono::high_resolution_clock::time_point, tspp::default_vec_t>::const_iterator;
				_iterator il = ts.cbegin();
				while (il != ts.cend()) {
					std::string datestr = chrono_to_str((*il).first);
					helper::PRINT_TEST_MESSAGE(datestr + "|" + std::to_string((*il).second));
					++il;
				}
			}
		};

		template <class T>
		static void reverse_print_ts(const tspp::time_series<T, std::chrono::high_resolution_clock::time_point, tspp::default_vec_t> &ts) {
			if (ts.size() > 0) {
				using _iterator = typename tspp::time_series<T, std::chrono::high_resolution_clock::time_point, tspp::default_vec_t>::const_reverse_iterator;
				_iterator il = ts.crbegin();
				while (il != ts.crend()) {
					std::string datestr = chrono_to_str((*il).first);
					helper::PRINT_TEST_MESSAGE(datestr + "|" + std::to_string((*il).second));
					++il;
				}
			}
		};
	};
}; // namespace tspp_tests_utilities

#endif