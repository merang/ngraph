/*******************************************************************************
* Copyright 2017-2018 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <gtest/gtest.h>

#include "ngraph/assertion.hpp"

using namespace ngraph;
using namespace std;

TEST(assertion, assertion_true)
{
    NGRAPH_ASSERT(true) << "this should not throw";
}

TEST(assertion, assertion_false)
{
    EXPECT_THROW({ NGRAPH_ASSERT(false) << "this should throw"; }, AssertionFailure);
}

TEST(assertion, assertion_what_string)
{
    bool assertion_failure_thrown = false;

    try
    {
        NGRAPH_ASSERT(false) << "testing the contents of the 'what()' string";
    }
    catch (const AssertionFailure& e)
    {
        assertion_failure_thrown = true;
        EXPECT_STREQ(e.what(), "testing the contents of the 'what()' string");
    }

    EXPECT_TRUE(assertion_failure_thrown);
}

// Internally, NGRAPH_ASSERT works by throwing from the destructor of an "AssertionHelper" object
// generated inside the macro. This can be dangerous if a throw happens somewhere else while the
// AssertionHelper is in scope, because stack unwinding will cause a call ~AssertionHelper, and
// this causes a "double-throw", resulting in uncatchable program termination.
//
// To avoid this, ~AssertionHelper destructor checks std::uncaught_exception() and does not throw
// if it returns true. This avoids the most likely double-throw scenario in ordinary usage, where
// the expressions feeding the stream throw exceptions themselves.
//
// Here we are testing to make sure that the exception from the stream-feeding expression is
// propagated properly, and that ~AssertionHelper itself does not throw even though the assertion
// is false.
TEST(assertion, throw_in_stream)
{
    auto f = []() -> std::string {
        // The choice of exception class here is arbitrary.
        throw std::domain_error("this should throw std::domain_error");
    };

    EXPECT_THROW({ NGRAPH_ASSERT(false) << f(); }, std::domain_error);
}
