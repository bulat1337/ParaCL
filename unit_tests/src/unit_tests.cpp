#include "test_utils.h"

#include <gtest/gtest.h> // for Test, TestInfo (ptr only), Message, TEST

TEST(common, basic_1) { test_utils::run_test("/common/basic_1"); }
TEST(common, local_variables) { test_utils::run_test("/common/local_variables"); }

#ifdef ENABLE_BD_TESTS

// TEST(bd_tests, dim_100) { test_utils::run_test("/bd_tests/test_1"); }

#endif
