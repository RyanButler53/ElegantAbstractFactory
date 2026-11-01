#include <print>
#include <meta>
#include <string>
#include <format>
#include <string_view>
#include "functionDispatch.hpp"
#include "string.hpp"
#include <gtest/gtest.h>


struct FunctionTag {};
FunctionTag registerDispatch;

namespace Fns {

int add [[=registerDispatch, =eaf::String("add")]] (int a, int b) {
    return a + b;
}

int mayThrow [[=registerDispatch, =eaf::String("maythrow")]](int x, int y){
    if (x == y){
        throw std::invalid_argument("X and Y must not be equal");
    }
    return x + y;
}

}


class FunctionDispatchTest : public ::testing::Test {
    protected:
    eaf::FunctionDispatcher<FunctionTag, int, eaf::String, ^^Fns> dispatcher;
};

TEST_F(FunctionDispatchTest, add){
   EXPECT_EQ(dispatcher.dispatch(eaf::String("add"), 5, 5), 10);
}

TEST_F(FunctionDispatchTest, fromFunctionName){
    EXPECT_EQ(dispatcher.dispatch("add", 15, 5), 20);
}

TEST_F(FunctionDispatchTest, NotRegistered){
    EXPECT_THROW(dispatcher.dispatch("not added", 10, 0),std::invalid_argument);
    EXPECT_THROW(dispatcher.dispatch(eaf::String("mayThrow"), -4, 5), std::invalid_argument);
}

TEST_F(FunctionDispatchTest, tryDispatch){
    EXPECT_EQ(dispatcher.dispatch("mayThrow", 10, 20), 30);

    EXPECT_THROW(dispatcher.dispatch("mayThrow", 10, 10), std::invalid_argument);
    std::expected<int, std::string> err;
    EXPECT_NO_THROW(err = dispatcher.try_dispatch(eaf::String("maythrow"), 10, 10));
    
    EXPECT_FALSE(err.has_value());
    EXPECT_EQ(err.error(), "X and Y must not be equal");
}

TEST_F(FunctionDispatchTest, simpleCurry){
    auto curried = dispatcher.curry<int, int>(eaf::String("add"));
    EXPECT_EQ(curried(5, 10), 15);
}
