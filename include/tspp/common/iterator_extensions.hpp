#pragma once
#ifndef tspp_iterator_extens_hpp
#define tspp_iterator_extens_hpp

#include <vector>

#include <iterator>
#include <utility>

namespace tspp {
	namespace iterator_extensions {
		template <class Iterator1, class Iterator2>
		class iterator_pair
			: public std::iterator<std::output_iterator_tag,
								   std::pair<typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type>, void,
								   void, void> {
		  public:
			using iterator_type = std::pair<Iterator1, Iterator2>;
			using difference_type = typename std::iterator_traits<Iterator2>::difference_type;
			using value_type = std::pair<typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type>;
			iterator_pair(Iterator1 it1, Iterator2 it2) : it(it1, it2){};
			explicit iterator_pair(const std::pair<Iterator1, Iterator2> &it_pair) : it(it_pair){};
			explicit iterator_pair(std::pair<Iterator1, Iterator2> &&it_pair) : it(it_pair){};

			iterator_type base() const { return (it); };

			iterator_pair<Iterator1, Iterator2> &
			operator=(const std::pair<typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type> &value) {

				*(it.first) = value.first;
				*(it.second) = value.second;
				return (*this);
			};

			iterator_pair<Iterator1, Iterator2> &
			operator=(std::pair<typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type> &&value) {
				*(it.first) = value.first;
				*(it.second) = value.second;

				return (*this);
			};

			value_type operator*() { return std::make_pair((*it.first), (*it.second)); };
			iterator_pair<Iterator1, Iterator2> &operator++() {
				++it.first;
				++it.second;

				return (*this);
			};
			iterator_pair<Iterator1, Iterator2> operator++(int) {
				iterator_pair<Iterator1, Iterator2> tmp(it);

				it.first++;
				it.second++;

				return (tmp);
			};

			// only allow this if iterators share a difference type
			typename std::enable_if<
				std::is_same<typename std::iterator_traits<Iterator1>::difference_type, typename std::iterator_traits<Iterator2>::difference_type>::value,
				iterator_pair<Iterator1, Iterator2>>::type
			operator-(difference_type n) const {

				iterator_pair<Iterator1, Iterator2> tmp(it.first - n, it.second - n);
				return (tmp);
			};

			// only allow this if iterators share a difference type
			typename std::enable_if<
				std::is_same<typename std::iterator_traits<Iterator1>::difference_type, typename std::iterator_traits<Iterator2>::difference_type>::value,
				iterator_pair<Iterator1, Iterator2>>::type
			operator+(difference_type n) const {

				iterator_pair<Iterator1, Iterator2> tmp(it.first + n, it.second + n);
				return (tmp);
			};

			iterator_pair<Iterator1, Iterator2> &operator--() {
				--it.first;
				--it.second;

				return (*this);
			};
			iterator_pair<Iterator1, Iterator2> operator--(int) {
				iterator_pair<Iterator1, Iterator2> tmp(it);

				it.first--;
				it.second--;

				return (tmp);
			};

			bool operator==(const iterator_pair<Iterator1, Iterator2> &other) const {
				bool first = (it.first) == (other.it.first);
				bool second = (it.second) == (other.it.second);
				return first && second;
			};
			bool operator!=(const iterator_pair<Iterator1, Iterator2> &other) const { return !((*this) == other); };

		  protected:
			iterator_type it;
		};

		template <class Iterator1, class Iterator2>
		iterator_pair<Iterator1, Iterator2> make_iterator_pair(Iterator1 it1, Iterator2 it2) {
			return (iterator_pair<Iterator1, Iterator2>(it1, it2));
		};

		template <class Iterator1, class Iterator2>
		iterator_pair<Iterator1, Iterator2> make_iterator_pair(const std::pair<Iterator1, Iterator2> &it_pair) {
			return (iterator_pair<Iterator1, Iterator2>(it_pair));
		};

		template <class Iterator1, class Iterator2>
		iterator_pair<Iterator1, Iterator2> make_iterator_pair(std::pair<Iterator1, Iterator2> &&it_pair) {
			return (iterator_pair<Iterator1, Iterator2>(it_pair));
		};
	}; // namespace iterator_extensions
};	   // namespace tspp

#endif
