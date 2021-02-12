#pragma once
#ifndef tspp_test_utils_hpp
#define tspp_test_utils_hpp

#include <exception>
#include <gtest/gtest.h>

#define ASSERT_THROW(expr)                                                                                                                                     \
	try {                                                                                                                                                      \
		expr();                                                                                                                                                \
		FAIL();                                                                                                                                                \
	} catch (const std::exception &expected) {                                                                                                                 \
	}

#pragma endregion

#endif