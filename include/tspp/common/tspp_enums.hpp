#pragma once
#ifndef tspp_enums_hpp
#define tspp_enums_hpp
namespace tspp {
	namespace enums {
		enum merge_asof_mode { roll_prior = 1, roll_following = 2, no_roll = 3 };
		enum ordering { equal = 1, greater_than = 2, less_than = 3 };
	}; // namespace enums
};	   // namespace tspp
#endif
