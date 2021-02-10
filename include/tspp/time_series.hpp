#pragma once
#ifndef time_series_hpp
#define time_series_hpp

#include <tspp/common/compare_extensions.hpp>
#include <tspp/common/iterator_extensions.hpp>
#include <tspp/common/time_series_allocator.hpp>
#include <tspp/common/time_series_errors.hpp>
#include <tspp/common/tspp_enums.hpp>

#include <deque>
#include <map>
#include <vector>
#include <algorithm>

#include <functional>
#include <optional>

#include <sstream>
#include <chrono>
#include <string>

namespace tspp {


    template<class t_value>
    using default_vec_t = typename std::vector<t_value>;

    template <class T,
              class dt_type = std::chrono::high_resolution_clock::time_point,
              template<class> class backing_class = default_vec_t>
    class time_series {
      public:
        using key_type = dt_type;
        using value_type = T;
        template<class t_value>
        using container_type = backing_class<t_value>;

        using size_type = typename container_type<key_type>::size_type;
        using difference_type = typename container_type<key_type>::difference_type;

      private:
        container_type<key_type> keys_; 
        container_type<value_type> values_; 
        
      public:
        /*! Default constructor */
          time_series() {

          };
        /*! Default destructor */
        ~time_series() {

        };

        template <class DateIterator, class ValueIterator>
        time_series(DateIterator dBegin, DateIterator dEnd, ValueIterator vBegin) {
            while (dBegin != dEnd) {
                insert_end(*(dBegin++), *(vBegin++));
            }
        };

        time_series(container_type<key_type> keys, container_type<value_type> values) {
            keys_ = keys;
            values_ = values;
        };

        /*! copy constructor */
        time_series(const time_series<T, dt_type>& src) : time_series(src.keys_, src.values_) {
            
        };

        /*! move constructor */
        time_series(time_series<T, dt_type>&& src) {
            values_ = std::move(src.values_);
            keys_ = std::move(src.keys_);
        };

        /*! copy assign */
        time_series<T, dt_type>& operator=(const time_series<T, dt_type>& src) {
            if (this != &src) {
                values_ = src.values_;
                keys_ = src.keys_;
            }
            return *this;
        };
        
        /*! move assign */
        time_series<T, dt_type>& operator=(time_series<T, dt_type>&& src){
            if (this != &src) {
                values_ = std::move(src.values_);
                keys_ = std::move(src.keys_);
            }
            return *this;
        };

        /// first date
        dt_type first_date() const {
            time_series_REQUIRE(!values_.empty(), "empty timeseries");
            return keys_.begin();
        };

        /// last date
        dt_type last_date() const {
            time_series_REQUIRE(!values_.empty(), "empty timeseries");
            return keys_.rbegin();
        };
        //! size
        size_type size() const {
            return keys_.size();
        };

        /// returns whether the series contains any data
        bool empty() const {
            return keys_.empty() && values_.empty();
        };



        using const_value_iterator = typename container_type<value_type>::const_iterator;
        using const_key_iterator = typename container_type<dt_type>::const_iterator;

        using value_iterator = typename backing_class<value_type>::iterator;
        using key_iterator = typename backing_class<dt_type>::iterator;

        using const_reverse_value_iterator = typename container_type<value_type>::const_reverse_iterator;
        using const_reverse_key_iterator = typename container_type<dt_type>::const_reverse_iterator;

        using reverse_value_iterator = typename backing_class<value_type>::reverse_iterator;
        using reverse_key_iterator = typename backing_class<dt_type>::reverse_iterator;

        using value_iterator_category = typename const_value_iterator::iterator_category;
        using key_iterator_category = typename const_key_iterator::iterator_category;

        using iterator = typename tspp::iterator_extensions::iterator_pair<key_iterator, value_iterator>;
        using const_iterator = typename tspp::iterator_extensions::iterator_pair<const_key_iterator, const_value_iterator>;


        template<class other_value_type, class other_key_type, template<class> class other_backing_class>
        bool operator==(const time_series<other_value_type, other_key_type, other_backing_class>& other) const {

            const_iterator il = cbegin();
            typename tspp::time_series<other_value_type, other_key_type, other_backing_class>::const_iterator ir = other.cbegin();
            const_iterator ile = cend();
            typename tspp::time_series<other_value_type, other_key_type, other_backing_class>::const_iterator ire = other.cend();
            bool is_same_so_far = size() == other.size();
            while (il != ile && ir != ire) {
                bool first = tspp::compare_extensions::flex_compare<key_type, other_key_type>((*il).first, (*ir).first);
                bool second = tspp::compare_extensions::flex_compare<value_type, other_value_type>((*il).second, (*ir).second);
                is_same_so_far = is_same_so_far && first && second;
                if (!is_same_so_far) {
                    break;
                }
                ++il;
                ++ir;
            }
            return is_same_so_far;
        };

        template<class other_value_type, class other_key_type, template<class> class other_backing_class>
        bool operator!=(const time_series<other_value_type, other_key_type, other_backing_class>& other) const  {

            return !((*this) == other);
        };

        std::pair<bool, difference_type> locate(const dt_type& d) const {
            const_key_iterator key = std::lower_bound(keys_.cbegin(), keys_.cend(), d);
            difference_type index = std::distance(keys_.cbegin(), key);
            return std::make_pair(key != keys_.cend() && (*key) == d, index);
        };

        template<class date_predicate>
        std::pair<bool, difference_type> locate(date_predicate date_pred) const {
            const_key_iterator key = std::find_if(keys_.cbegin(), keys_.cend(), date_pred);
            difference_type index = std::distance(keys_.cbegin(), key);
            return std::make_pair(key != keys_.cend(), index);
        };

        T& get_value(int idx) {
            value_iterator it = values_.begin();
            std::advance(it, idx);
            return *it;
        };

        T get_value(int idx) const {
            value_iterator it = values_.begin();
            std::advance(it, idx);
            return *it;
        };

        T& at(const dt_type& d) { 
            std::pair<bool, difference_type> loc = locate(d);
            time_series_REQUIRE(loc.first, "date key not found");
            return get_value(loc.second);
        };
        T at(const dt_type& d) const {
            std::pair<bool, difference_type> loc = locate(d);
            time_series_REQUIRE(loc.first, "date key not found");
            return get_value(loc.second);
        };

        T& at_or_first_prior(const dt_type& d) { 
            std::pair<bool, difference_type> loc = locate([&](const dt_type& dt) -> bool {return dt > d; });
            bool precond_met = true;
            if (loc.first) { //the cond was true at any point
                if (loc.second == 0) { // if all are greater than there is no prior
                    precond_met = false;
                }
            }
            time_series_REQUIRE(precond_met, "date key not found");
            return get_value(loc.second - 1);
        };
        T at_or_first_prior(const dt_type& d) const { 
            std::pair<bool, difference_type> loc = locate([&](const dt_type& dt) -> bool {return dt > d; });
            bool precond_met = true;
            if (loc.first) { //the cond was true at any point
                if (loc.second == 0) { // if all are greater than there is no prior
                    precond_met = false;
                }
            }
            time_series_REQUIRE(precond_met, "date key not found");
            return get_value(loc.second - 1);
        };
        T& at_or_first_following(const dt_type& d) {
            std::pair<bool, difference_type> loc = locate([&](const dt_type& dt) -> bool {return dt >= d; });
            bool precond_met = loc.first;
            time_series_REQUIRE(precond_met, "date key not found");
            return get_value(loc.second );
        };
        T at_or_first_following(const dt_type& d) const {
            std::pair<bool, difference_type> loc = locate([&](const dt_type& dt) -> bool {return dt >= d; });
            bool precond_met = loc.first;
            time_series_REQUIRE(precond_met, "date key not found");
            return get_value(loc.second );
        };


        std::optional<std::reference_wrapper<T>> try_at(const dt_type& d) { //this could be scray...
            std::pair<bool, difference_type> loc = locate(d);
            if (loc.first) {
                return std::optional<std::reference_wrapper<T> >{get_value(loc.second) };
            }
            return std::nullopt;
        };

        std::optional<T> try_at(const dt_type& d) const { 
            std::pair<bool, difference_type> loc = locate(d);
            if (loc.first){
                return get_value(loc.second);
            }
            return std::nullopt;
        };


        time_series<T,dt_type, backing_class> between(const dt_type& startdt, const dt_type& enddt) {
            time_series_REQUIRE((enddt >= startdt), "end date is not greater than start date");
            container_type<key_type> result_keys;
            container_type<value_type> result_values;
            for (iterator il = begin(); il != end(); ++il) {
                std::pair<key_type, value_type> kvp = *il;
                if (kvp.first > enddt) {
                    break;
                }
                if (startdt <= kvp.first) {
                    result_keys.push_back(kvp.first);
                    result_values.push_back(kvp.second);
                }

            }  
            time_series<T, dt_type, backing_class> time_series_sub(result_keys,result_values);
            return time_series_sub;
        };

        template<class paired_predicate> 
        time_series<T, dt_type, backing_class> where(const paired_predicate& predicate) {
            container_type<key_type> result_keys;
            container_type<value_type> result_values;
            for (iterator il = begin(); il != end(); ++il) {
                std::pair<key_type, value_type> kvp = *il;                
                if (predicate(kvp)) {
                    result_keys.push_back(kvp.first);
                    result_values.push_back(kvp.second);
                }

            }
            time_series<T, dt_type, backing_class> time_series_sub(result_keys, result_values);
            return time_series_sub;
        };



#pragma region reverse iterators

        //the base reverser, will throw if you try to call it an your iterator does not support the relevant calls
        template <class key_container, class value_container, class iterator_category>
        struct reverse {
            using const_reverse_iterator = typename tspp::iterator_extensions::iterator_pair<std::reverse_iterator<typename key_container::const_iterator>, std::reverse_iterator<typename value_container::const_iterator>>;
            using reverse_iterator = typename tspp::iterator_extensions::iterator_pair<std::reverse_iterator<typename key_container::iterator>, std::reverse_iterator<typename value_container::iterator>>;
            reverse(const key_container& kc, const value_container& vc ) : kc_(kc), vc_(vc) {}
            const_reverse_iterator crbegin()  { throw time_series_error(__FILE__, __LINE__, __FUNCTION__, "cant call with no bidirectional iterator support"); }
            const_reverse_iterator crend() { throw time_series_error(__FILE__, __LINE__, __FUNCTION__, "cant call with no bidirectional iterator support"); }
            reverse_iterator rbegin() { throw time_series_error(__FILE__, __LINE__, __FUNCTION__, "cant call with no bidirectional iterator support"); }
            reverse_iterator rend() { throw time_series_error(__FILE__, __LINE__, __FUNCTION__, "cant call with no bidirectional iterator support"); }
            const key_container& kc_;
            const value_container& vc_;
        };

        // This specilization defines reverse iterator features via the chosen container's native calls.
        template <class key_container, class value_container>
        struct reverse<key_container, value_container, std::bidirectional_iterator_tag> {
            using reverse_iterator = typename tspp::iterator_extensions::iterator_pair<reverse_key_iterator, reverse_value_iterator>;
            using const_reverse_iterator = typename tspp::iterator_extensions::iterator_pair<const_reverse_key_iterator, const_reverse_value_iterator>;

            reverse(const key_container& kc, const value_container& vc) : kc_(kc), vc_(vc) {}
            const_reverse_iterator crbegin()  { return get_paired_iter(kc_.crbegin(), vc_.crbegin()); }
            const_reverse_iterator crend() { return get_paired_iter(kc_.crend(), vc_.crend()); }
            reverse_iterator rbegin() { return get_paired_iter(kc_.rbegin(), vc_.rbegin()); }
            reverse_iterator rend() { return get_paired_iter(kc_.rend(), vc_.rend()); }
            const key_container& kc_;
            const value_container& vc_;
        };

        // The following templates let you call the reverse iterators if the chosen container supports reversing
        template<typename _iter>
        using _is_bidir = std::disjunction< // need c++17 for this garbage
            std::is_same<typename std::iterator_traits<_iter>::iterator_category, std::bidirectional_iterator_tag>,
            std::is_base_of<typename std::iterator_traits<_iter>::iterator_category, std::bidirectional_iterator_tag>
        >;

        template<typename _iter>
        using _is_rand_access = std::disjunction< 
            std::is_same<typename std::iterator_traits<_iter>::iterator_category, std::random_access_iterator_tag>,
            std::is_base_of<typename std::iterator_traits<_iter>::iterator_category, std::random_access_iterator_tag>
        >;

        template<typename _iter>
        using _is_native_reverseable = std::disjunction< 
            _is_rand_access<_iter>,
            _is_bidir<_iter>
          >;

         using enable_reverse = typename std::conditional<
            std::conjunction<_is_native_reverseable<key_iterator>, _is_native_reverseable<value_iterator>>::value,
            std::bidirectional_iterator_tag,
            std::input_iterator_tag>::type ;

        using reverser = reverse<container_type<dt_type>, container_type<value_type>, enable_reverse>;
        using const_reverse_iterator = typename reverser::const_reverse_iterator;
        using reverse_iterator = typename reverser::reverse_iterator;

#pragma endregion


        const_iterator cbegin() const { return get_paired_iter(keys_.begin(), values_.begin()); };
        const_iterator cend() const { return get_paired_iter(keys_.end(), values_.end()); };
        iterator begin() { return get_paired_iter(keys_.begin(), values_.begin()); };
        iterator end() { return get_paired_iter(keys_.end(), values_.end()); };
        const_reverse_iterator crbegin() const {
            return reverser(keys_,values_).crbegin();
        }
        const_reverse_iterator crend() const {
            return reverser(keys_, values_).crend();
        }
        reverse_iterator rbegin() { return reverser(keys_, values_).rbegin(); }
        reverse_iterator rend() { return reverser(keys_, values_).rend(); }
        //@}
      private:
         using container_value_type = typename iterator::value_type ;

       public:

        std::vector<dt_type> dates() const {
            std::vector<dt_type> v;
            v.reserve(size());
            std::copy(keys_.begin(), keys_.end(), std::back_inserter(v));
            return v;
        };

        std::vector<T> values() const {
            std::vector<T> v;
            v.reserve(size());
            std::copy(values_.begin(), values_.end(), std::back_inserter(v));
            return v;
        };

        void reserve(size_type size) 
        { 
            keys_.reserve(size);
            values_.reserve(size); 
        }

        inline void insert_end(std::pair<dt_type, value_type> data) { 
            keys_.insert(keys_.end(), data.first);
            values_.insert(values_.end(), data.second); 
        }

        inline void insert_start(std::pair<dt_type, value_type> data) {
            keys_.insert(keys_.begin(), data.first);
            values_.insert(values_.begin(), data.second);
        }


        template <typename out_value, typename apply_func>
        time_series<out_value, dt_type, backing_class> map(const apply_func& func) {
            backing_class<out_value> resvals;
            resvals.reserve(size());

            std::transform(values_.cbegin(), values_.cend(), std::back_inserter(resvals), func);

            time_series<out_value, dt_type, backing_class> result(keys_, resvals);
            return result;
        };

        template <typename out_value, typename apply_func>
        time_series<out_value, dt_type, backing_class> map_with_date(const apply_func& func) {
            backing_class<out_value> resvals;
            resvals.reserve(size());

            std::transform(cbegin(), cend(), std::back_inserter(resvals), func);

            time_series<out_value, dt_type, backing_class> result(keys_, resvals);
            return result;
        };

        template <typename out_value, typename fold_func>
        time_series<out_value, dt_type, backing_class> fold(const fold_func& func, out_value initval) {

            out_value eval = initval;
            backing_class<out_value> resvals;
            resvals.reserve(size());
            for (const_value_iterator it = values_.cbegin(); it != values_.cend(); ++it) {
                resvals.emplace_back(func(eval, *it));
            }

            time_series<out_value, dt_type, backing_class> result(keys_, resvals);
            return result;
        };

        time_series<value_type, dt_type, backing_class> shift(difference_type shift_idx) {
            difference_type cand_size = static_cast<difference_type>(size()) - static_cast<difference_type>(std::abs(shift_idx));
            time_series_REQUIRE((cand_size >= 0), "atttempting to shift out of bounds series is too short for the given shift");
            backing_class<value_type> resvals;
            resvals.reserve(static_cast<std::size_t>(cand_size));
            backing_class<dt_type> resdates;
            resdates.reserve(static_cast<std::size_t>(cand_size));

            if (shift_idx < 0) {
                std::copy(keys_.begin() - shift_idx, keys_.end(), std::back_inserter(resdates));
                std::copy(values_.begin(), values_.end() + shift_idx, std::back_inserter(resvals));
            }
            else {
                std::copy(keys_.begin(), keys_.end() - shift_idx, std::back_inserter(resdates));
                std::copy(values_.begin() + shift_idx, values_.end(), std::back_inserter(resvals));
            }
            time_series<value_type, dt_type, backing_class> result(resdates, resvals);
            return result;
        };

        template <typename out_value, typename skip_func>
        time_series<out_value, dt_type, backing_class> skip(size_type skip_size, skip_func func) {
            time_series_REQUIRE((size() >= skip_size), "atttempting to skip a greater span than the ts");
            auto cand_size = size() - skip_size;
            backing_class<out_value> resvals;
            resvals.reserve(cand_size);
            backing_class<dt_type> resdates;
            resdates.reserve(cand_size);
            
            std::copy(keys_.begin() + skip_size, keys_.end(), std::back_inserter(resdates));

            const_value_iterator itprior = values_.cbegin();
            const_value_iterator it = values_.cbegin();
            std::advance(it, skip_size);
            const_value_iterator ite = values_.cend();
            while (it != ite) {
                resvals.emplace_back(func((*it), (*itprior)));
                ++it;
                ++itprior;
            }
            time_series<value_type, dt_type, backing_class> result(resdates, resvals);
            return result;
        };

        template <typename out_value, typename index_func, typename skip_func>
        time_series<out_value, dt_type, backing_class> skip_on_index(index_func idx_func, skip_func func) {
            backing_class<out_value> resvals;
            backing_class<dt_type> resdates;
            const_iterator it = cbegin();
            const_iterator ite = cend();
            while (it != ite) {
                std::pair<dt_type, value_type> curpair = *it;
                dt_type priort = idx_func(curpair.first);
                std::optional<value_type> priorv = try_at(priort);
                if (priorv.has_value()) {
                    resdates.emplace_back(curpair.first);
                    resvals.emplace_back(func(curpair.second, priorv.value()));
                }
                it++;
            }
            time_series<value_type, dt_type, backing_class> result(resdates, resvals);
            return result;
        };


        template <typename out_value, typename other_value, typename cross_apply_func>
        time_series<out_value, dt_type, backing_class> cross_apply_inner(const time_series<other_value, dt_type, backing_class>& other, const cross_apply_func& func) const {
            
            time_series<out_value, dt_type, backing_class> result;

            const_iterator il = cbegin();
            typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator ir = other.cbegin();
            const_iterator ile = cend();
            typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator ire = other.cend();
            while (il != ile && ir != ire) {
                if ((*il).first < (*ir).first) {
                    ++il;
                }
                else if ((*ir).first < (*il).first) {
                    ++ir;
                }
                else {
                    result.insert_end(std::make_pair((*il).first, func((*il).second, (*ir).second)));
                    ++il;
                    ++ir;
                }
            }
            return result;
        };

        template <typename out_value, typename other_value, typename cross_apply_func>
        time_series<out_value, dt_type, backing_class> cross_apply_left(const time_series<other_value, dt_type, backing_class>& other, other_value default_value, const cross_apply_func& func) const {

            time_series<out_value, dt_type, backing_class> result;
            result.reserve(size());
            const_iterator il = cbegin();
            typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator ir = other.cbegin();
            const_iterator ile = cend();
            typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator ire = other.cend();
            while (il != ile) {
                if ((*il).first < (*ir).first) {

                    result.insert_end(std::make_pair((*il).first, func((*il).second, default_value)));
                    ++il;
                }
                else if ((*ir).first < (*il).first) {
                    result.insert_end(std::make_pair((*il).first, func((*il).second, default_value)));
                    if (ir != ire) {
                        ++ir;
                    }
                    else {
                        ++il; //the first index might still be longer so we gotta keep rolling it forward even though we are out of space on the other index
                    }
                }
                else {
                    result.insert_end(std::make_pair((*il).first, func((*il).second, (*ir).second)));
                    ++il;
                    ++ir;
                }
            }
            return result;
        };


        template <typename out_value, typename other_value, typename apply_func>
        time_series<out_value, dt_type, backing_class> merge_apply_asof(const time_series<other_value, dt_type, backing_class>& other, other_value default_value, 
            const std::optional<std::function<std::pair<tspp::enums::ordering, typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator>(const_iterator, typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator, typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator)>>& compare_func,
            const apply_func& func, 
            const tspp::enums::merge_asof_mode& merge_mode) const {


            using other_const_iterator = typename tspp::time_series<other_value, dt_type, backing_class>::const_iterator;

            std::function<other_const_iterator(other_const_iterator)> cand_idx_func;
            switch (merge_mode) {
                case tspp::enums::merge_asof_mode::no_roll: 
                    cand_idx_func = [](other_const_iterator idx) -> other_const_iterator { return idx;  };
                    break;
                case tspp::enums::merge_asof_mode::roll_following:
                    cand_idx_func = [&](other_const_iterator idx) -> other_const_iterator { return idx == other.cend() ? --(other.cend()) : ++idx;  };
                    break;
                case tspp::enums::merge_asof_mode::roll_prior:
                    cand_idx_func = [&](other_const_iterator idx) -> other_const_iterator { return idx == other.cbegin() ? other.cbegin() : --idx;  };
                    break;
            }

            std::function<std::pair<tspp::enums::ordering, other_const_iterator>(const_iterator, other_const_iterator, other_const_iterator)> comp_func = compare_func.value_or(
                [](const_iterator _this, other_const_iterator _other, other_const_iterator _other_alt) ->std::pair<tspp::enums::ordering, other_const_iterator> {
                    tspp::enums::ordering ord;
                    if ((*_this).first < (*_other).first) {
                        ord = tspp::enums::ordering::less_than;
                    }
                    else if ((*_this).first > (*_other).first) {
                        ord = tspp::enums::ordering::greater_than;
                    }
                    else {
                        ord = tspp::enums::ordering::equal;
                    }
                    return std::make_pair(ord,_other);
                }
            );

            time_series<out_value, dt_type, backing_class> result;
            result.reserve(size());
            const_iterator il = cbegin();
            other_const_iterator ir = other.cbegin();
            const_iterator ile = cend();
            other_const_iterator ire = other.cend();
            while (il != ile) {

                auto comp_res = comp_func(il, ir, cand_idx_func(ir));
                auto comp = comp_res.first;
                switch (comp) {
                    case tspp::enums::ordering::less_than:
                        result.insert_end(std::make_pair((*il).first, func((*il).second, default_value)));
                        ++il;
                        break;
                    case tspp::enums::ordering::greater_than:
                        result.insert_end(std::make_pair((*il).first, func((*il).second, default_value)));
                        if (ir != ire) {
                            ++il; //the first index might still be longer so we gotta keep rolling it forward even though we are out of space on the other index
                        }
                        break;
                    case tspp::enums::ordering::equal:
                        auto ir_other = comp_res.second == ir ? ir : comp_res.second;
                        result.insert_end(std::make_pair((*il).first, func((*il).second, (*ir_other).second)));
                        if (((*il).first == (*ir).first) && ir != ire) {
                            ++ir;
                        }
                        ++il;
                        break;
                }

            }
            return result;
        };



        template<typename selec_func>
        time_series<value_type, dt_type, backing_class> interweave(const time_series<value_type, dt_type, backing_class>& other, const selec_func& selecfn) const {

            time_series<value_type, dt_type, backing_class> result;
            const_iterator il = cbegin();
            typename tspp::time_series<value_type, dt_type, backing_class>::const_iterator ir = other.cbegin();
            const_iterator ile = cend();
            typename tspp::time_series<value_type, dt_type, backing_class>::const_iterator ire = other.cend();
            while (il != ile || ir != ire) {
                if (il == ile) {
                    result.insert_end((*il));
                    ++il;
                }
                else if(ir == ire){
                    result.insert_end((*ir));
                    ++ir;
                }
                else {
                    auto cand_l = *il;
                    auto cand_r = *ir;
                    if (cand_l.first > cand_r.first) {
                        result.insert_end(cand_r);
                        ++ir;
                    }
                    else if (cand_l.first < cand_r.first) {
                        result.insert_end(cand_l);
                        ++il;
                    }
                    else {
                        result.insert_end(selecfn(cand_l, cand_r));
                        ++il;
                        ++ir;
                    }
                }
            
            }
            return result;
        };

        template <typename out_value, typename grp_func, typename agg_func>
        time_series<out_value, dt_type, backing_class> resample_and_agg(const grp_func& grpfn, const agg_func& aggfn) {
            time_series<out_value, dt_type, backing_class> result;
            std::vector<value_type> buffer;
            buffer.reserve(size());
            std::optional<dt_type> prior_key = std::nullopt;
            for (const_iterator it = cbegin(); it != cend() ;++it) {
                auto kvp = *it;
                dt_type keycand = grpfn(kvp.first);
                if (prior_key.has_value()) {
                    if (prior_key.value() != keycand) {
                        auto resval = std::make_pair(prior_key.value(), aggfn(buffer));
                        result.insert_end(resval);
                        buffer.clear();
                    }
                }
                buffer.emplace_back(kvp.second);
                prior_key = keycand;
            }
            // and do the last chunk here
            if (prior_key.has_value()) {
                auto resval = std::make_pair(prior_key.value(), aggfn(buffer));
                result.insert_end(resval);
            }
            return result;
        };

        template <typename out_value, typename roll_func>
        time_series<out_value> apply_rolling(const roll_func& func, std::size_t window_size) { 
            time_series<out_value> result;
            if (window_size > size()) {
                return result;
            }
            std::deque<value_type> buffer(0);

            iterator itlow = begin();
            iterator ithigh = end();
            for (iterator it = itlow; it != ithigh; ++it) {

                if (buffer.size() < window_size - 1 && window_size > 1) {
                    buffer.push_back((*it).second);
                }
                else {

                    if (buffer.size() == window_size) {
                        buffer.pop_front();
                    }
                    buffer.push_back((*it).second);
                    out_value eval = func(buffer);
                    result.insert_end(std::make_pair((*it).first, eval));
                }
            }

            return result;
        };


      private:
        static iterator get_paired_iter(key_iterator key_iter, value_iterator value_iter) {
            return tspp::iterator_extensions::make_iterator_pair(key_iter, value_iter);
        };

        static const_iterator get_paired_iter(const_key_iterator key_iter, const_value_iterator value_iter) {
            return tspp::iterator_extensions::make_iterator_pair(key_iter, value_iter);
        };

        static reverse_iterator get_paired_iter(reverse_key_iterator key_iter, reverse_value_iterator value_iter) {
            return tspp::iterator_extensions::make_iterator_pair(key_iter, value_iter);
        };

        static const_reverse_iterator get_paired_iter(const_reverse_key_iterator key_iter, const_reverse_value_iterator value_iter) {
            return tspp::iterator_extensions::make_iterator_pair(key_iter, value_iter);
        };
        

    };


}
#endif
