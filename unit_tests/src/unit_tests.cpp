#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), TEST
#include <string>         // for basic_string

#include "test_utils.h"   // for run_test

TEST(common, basic_1) { test_utils::run_test("/common/basic_1"); }

TEST(common, local_variables) { test_utils::run_test("/common/local_variables"); }

TEST(common, if_1) { test_utils::run_test("/common/if_1"); }

TEST(common, while_1) { test_utils::run_test("/common/while_1"); }

TEST(common, while_uminus_logical) { test_utils::run_test("/common/while_uminus_logical"); }

TEST(common, while_if_logical_mod) { test_utils::run_test("/common/while_if_logical_mod"); }

TEST(common, expression_1) { test_utils::run_test("/common/expression_1"); }
