#pragma once
#ifndef tspp_chrono_extens_hpp
#define tspp_chrono_extens_hpp

#include <algorithm>
#include <chrono>
#include <functional>

#include <tspp/common/tspp_enums.hpp>

namespace tspp {
	namespace chrono_extensions {

		class gen_helpers {
		  public:
			template <class iterleft, class iterright, class duration_type, class func_in>
			static std::function<std::pair<tspp::enums::ordering, iterright>(iterleft, iterright, iterright)> merge_asof_frontend(const duration_type &duration,
																																  func_in func) {
				return [=](iterleft left, iterright right, iterright right_other) -> std::pair<tspp::enums::ordering, iterright> {
					return func(left, right, right_other, duration);
				};
			};
		};

		template <class time_type = std::chrono::high_resolution_clock::time_point, class time_arty_type = std::chrono::milliseconds>
		class merge_helpers {
		  private:
			template <class iterleft, class iterright, class duration_type>
			static std::pair<tspp::enums::ordering, iterright> merge_asof_prior_impl(iterleft itl, iterright itr, iterright itr_prior,
																					 const duration_type &duration) {
				time_arty_type diff = std::chrono::duration_cast<time_arty_type>(((*itl).first - (*itr_prior).first));
				time_arty_type zerodur(0);
				bool are_unequal = (*itl).first != (*itr).first;
				if (diff < zerodur && are_unequal) {
					return std::make_pair(tspp::enums::ordering::less_than, itr);
				} else if ((diff > duration && are_unequal)) {
					return std::make_pair(tspp::enums::ordering::greater_than, itr);
				} else if ((diff <= duration && are_unequal)) {
					return std::make_pair(tspp::enums::ordering::equal, itr_prior);
				} else {
					return std::make_pair(tspp::enums::ordering::equal, itr);
				}
			};
			template <class iterleft, class iterright, class duration_type>
			static std::pair<tspp::enums::ordering, iterright> merge_asof_fwd_impl(iterleft itl, iterright itr, iterright itr_peek,
																				   const duration_type &duration) {
				time_arty_type diff1 = std::chrono::duration_cast<time_arty_type>(((*itr_peek).first - (*itl).first));
				time_arty_type diff2 = std::chrono::duration_cast<time_arty_type>(((*itr).first - (*itl).first));
				time_arty_type zerodur(0);
				time_arty_type diff = time_arty_type(std::min(diff1.count(), std::max(diff2.count(), zerodur.count())));
				bool are_unequal = (*itl).first != (*itr).first;
				iterright itr_chosen = diff == diff2 ? itr : itr_peek;
				if (diff < zerodur && are_unequal) {
					return std::make_pair(tspp::enums::ordering::greater_than, itr);
				} else if ((diff > duration && are_unequal)) {
					return std::make_pair(tspp::enums::ordering::less_than, itr);
				} else if ((diff <= duration && are_unequal)) {
					return std::make_pair(tspp::enums::ordering::equal, itr_chosen);
				} else {
					return std::make_pair(tspp::enums::ordering::equal, itr);
				}
			};

		  public:
			template <class iterleft, class iterright, class duration_type>
			static auto merge_asof_prior(const duration_type &look_back) {
				return gen_helpers::merge_asof_frontend<iterleft, iterright, duration_type>(
					look_back, merge_helpers<time_type, time_arty_type>::merge_asof_prior_impl<iterleft, iterright, duration_type>);
			};

			template <class iterleft, class iterright, class duration_type>
			static std::function<std::pair<tspp::enums::ordering, iterright>(iterleft, iterright, iterright)> merge_asof_fwd(const duration_type &look_fwd) {
				return gen_helpers::merge_asof_frontend<iterleft, iterright, duration_type>(
					look_fwd, merge_helpers<time_type, time_arty_type>::merge_asof_fwd_impl<iterleft, iterright, duration_type>);
			};
		};

		template <class time_type = std::chrono::high_resolution_clock::time_point, class time_arty_type = std::chrono::milliseconds>
		class time_rounder {
		  public:
			template <class duration_type>
			static time_type round_up_to_nearest_duration(const time_type &time_in, const duration_type &duration) {

				time_arty_type time_since_epoch = std::chrono::duration_cast<time_arty_type>(time_in.time_since_epoch());
				time_arty_type dur = std::chrono::duration_cast<time_arty_type>(duration);
				auto mod_ticks = time_since_epoch.count() % dur.count();
				decltype(mod_ticks) zero = 0;
				auto delta = time_arty_type(std::max(dur.count() - mod_ticks, zero));
				return time_in + delta;
			};

			template <class duration_type>
			static time_type round_down_to_nearest_duration(const time_type &time_in, const duration_type &duration) {

				time_arty_type time_since_epoch = std::chrono::duration_cast<time_arty_type>(time_in.time_since_epoch());
				time_arty_type dur = std::chrono::duration_cast<time_arty_type>(duration);
				auto mod_ticks = time_since_epoch.count() % dur.count();
				auto delta = time_arty_type(mod_ticks);
				return time_in - delta;
			};

			template <class duration_type>
			static time_type round_nearest_to_nearest_duration(const time_type &time_in, const duration_type &duration) {

				time_arty_type time_since_epoch = std::chrono::duration_cast<time_arty_type>(time_in.time_since_epoch());
				time_arty_type dur = std::chrono::duration_cast<time_arty_type>(duration);
				auto mod_ticks = time_since_epoch.count() % dur.count();
				time_arty_type offset(0);
				if (mod_ticks > (dur.count() / 2)) {
					offset = time_arty_type(dur.count());
				}
				auto delta = time_arty_type(mod_ticks);
				return time_in + offset - delta;
			};
		};

	}; // namespace chrono_extensions
};	   // namespace tspp

#endif
