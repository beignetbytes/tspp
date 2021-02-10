#pragma once
#ifndef tspp_comp_extens_hpp
#define tspp_comp_extens_hpp

#include <utility>
#include <cmath>

#include <type_traits>
#include <utility>
#include <map>



namespace tspp {
	namespace compare_extensions {

		template <typename>
		struct is_pair : std::false_type
		{ };

		template <typename T, typename U>
		struct is_pair<std::pair<T, U>> : std::true_type
		{ };

		template <typename>
		struct is_double_pair : std::false_type
		{ };

		template <>
		struct is_double_pair<std::pair<double, double>> : std::true_type
		{ };


		template<typename LHS, typename RHS>
		using use_floating_point_compare = std::conjunction<std::is_floating_point<LHS>, std::is_floating_point<RHS>>;

		template<typename LHS, typename RHS>
		using use_paired_floatingpoint_compare = std::conjunction<is_double_pair<LHS>, is_double_pair<RHS>>;

		template<typename LHS, typename RHS>
		using use_special_case_compare = std::disjunction<use_floating_point_compare<LHS,RHS>, use_paired_floatingpoint_compare<LHS, RHS>>;

	
		template<typename LHS, typename RHS>
		typename std::enable_if<use_floating_point_compare<LHS,RHS>::value, bool>::type flex_compare(const LHS& lhs, const RHS& rhs) { //::value
			auto diff = std::abs(lhs - rhs);
			decltype(diff) epsilon = 0.0000001;
			return  (diff < epsilon) || (std::isnan(lhs) && std::isnan(rhs));
		};

		template<typename LHS, typename RHS>
		typename std::enable_if<!use_special_case_compare<LHS,RHS>::value, bool>::type flex_compare(const LHS& lhs, const RHS& rhs) {
			return lhs == rhs;
		};

		template<typename LHS, typename RHS>
		typename std::enable_if<use_paired_floatingpoint_compare<LHS,RHS>::value, bool>::type flex_compare(const LHS& lhs, const RHS& rhs) {
			bool first = tspp::compare_extensions::flex_compare<double, double>(lhs.first, rhs.first);
			bool second = tspp::compare_extensions::flex_compare<double, double>(lhs.second, rhs.second);
			return first && second;
		};

	}
}


#endif