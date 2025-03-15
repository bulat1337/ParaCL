#pragma once

#include <gtest/gtest.h>

#include "log.hh"
#include "test_utils_detail.hh"

namespace test_utils
{

void run_test(const std::string &test_name)
{
    std::string test_folder = "data";

    std::string test_path =
        std::string(TEST_DATA_DIR) + test_folder + test_name;

    std::string result = detail::getResult(test_path + ".dat");
    std::string answer = detail::getAnswer(test_path + ".ans");

    EXPECT_EQ(result, answer);
}

} // namespace test_utils
