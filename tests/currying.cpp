#include <print>
#include <meta>
#include <string>
#include <format>
#include <string_view>
#include <vector>
#include "functionDispatch.hpp"
#include <numeric>
#include <cstdlib>
#include <algorithm>
#include "string.hpp"
#include <gtest/gtest.h>

// Disclaimer: The comparision tests are largely AI generated

struct FunctionTag {};
FunctionTag registerDispatch;

namespace Curry {

enum class Op : uint8_t {
    ADD = 0,
    MULTIPLY = 1, 
    MAX = 2, 
    MIN = 3, 
    LCM = 4,
    GCD = 5,
    ABS_ADD = 6
};


int add [[=registerDispatch, =Op::ADD]] (int a, int b) {
    return a + b;
}

int multiply [[=registerDispatch, =Op::MULTIPLY]] (int a, int b) {
    return a * b;
}

int min [[=registerDispatch, =Op::MIN]] (int a, int b) {
    return a < b ? a : b;
}

int max [[=registerDispatch, =Op::MAX]] (int a, int b) {
    return a > b ? a : b;
}

int lcm [[=registerDispatch, =Op::LCM]](int a, int b){
    return std::lcm(a, b);
}

int gcd [[=registerDispatch, =Op::GCD]](int a, int b){
    return std::gcd(a, b);
}

int absAdd [[=registerDispatch, =Op::ABS_ADD]](int a, int b){
    return std::abs(a) + std::abs(b);
}

}

int reduce1(Curry::Op op, std::vector<int> v, int init){
    switch (op)
    {
    case Curry::Op::ADD:
        return std::reduce(v.begin(), v.end(), init, Curry::add);
    case Curry::Op::MULTIPLY:
        return std::reduce(v.begin(), v.end(), init, Curry::multiply);
    case Curry::Op::MAX:
        return std::reduce(v.begin(), v.end(), init, Curry::max);
    case Curry::Op::MIN:
        return std::reduce(v.begin(), v.end(), init, Curry::min);
    case Curry::Op::LCM:
        return std::reduce(v.begin(), v.end(), init, Curry::lcm);
    case Curry::Op::GCD: 
        return std::reduce(v.begin(), v.end(), init, Curry::gcd);
    case Curry::Op::ABS_ADD:
        return std::reduce(v.begin(), v.end(), init, Curry::absAdd);
    default:
        return -1; 
    }
}

int reduce2(Curry::Op op, std::vector<int> v, int init){
    eaf::FunctionDispatcher<FunctionTag, int, Curry::Op, ^^Curry> dispatcher;
    auto f = dispatcher.curry<int, int>(op);
    return std::reduce(v.begin(), v.end(), init, f);
}


namespace {

    std::string opName(Curry::Op op) {
        template for (constexpr std::meta::info e : std::define_static_array(std::meta::enumerators_of(^^Curry::Op))){
            if ([:e:] == op){
                return std::string(std::meta::identifier_of(e));
            }
        }
        return "UNKNOWN";
    }

}  // namespace

class CurryingTest : public ::testing::Test {
    protected:
    std::vector<int> v1{2,4,6,8};
    std::vector<int> v2{1,-3,6,-10,15,-21};
};

class ReduceEquivalenceTest
    : public CurryingTest,
      public ::testing::WithParamInterface<std::tuple<Curry::Op, int, int>> {
protected:
    const std::vector<int>& vectorFor(int which) const {
        return which == 0 ? v1 : v2;
    }
};

TEST_P(ReduceEquivalenceTest, Reduce1MatchesReduce2) {
    const auto [op, which, init] = GetParam();
    const std::vector<int>& v = vectorFor(which);

    EXPECT_EQ(reduce1(op, v, init), reduce2(op, v, init))
        << "op=" << opName(op) << " vector=v" << (which + 1)
        << " init=" << init;
}

static std::string paramName(
    const ::testing::TestParamInfo<ReduceEquivalenceTest::ParamType>& info) {
    const auto [op, which, init] = info.param;
    std::string initStr = init < 0 ? "neg" + std::to_string(-init)
                                   : std::to_string(init);
    return opName(op) + "_v" + std::to_string(which + 1) + "_init" + initStr;
}


INSTANTIATE_TEST_SUITE_P(
    AllOpsVectorsAndInits,
    ReduceEquivalenceTest,
    ::testing::Combine(
        ::testing::Values(Curry::Op::ADD,
                          Curry::Op::MULTIPLY,
                          Curry::Op::MAX,
                          Curry::Op::MIN,
                          Curry::Op::LCM,
                          Curry::Op::GCD,
                          Curry::Op::ABS_ADD),
        ::testing::Values(0, 1),                  // 0 -> v1, 1 -> v2
        ::testing::Values(0, 1, -5, 100)),         // initial values
        paramName
);

// ---------- Edge case: empty input returns the initial value ----------

TEST_F(CurryingTest, EmptyVectorReturnsInitForAllOps) {
    const std::vector<int> empty;
    for (auto op : {Curry::Op::ADD, Curry::Op::MULTIPLY, Curry::Op::MAX,
                    Curry::Op::MIN, Curry::Op::LCM, Curry::Op::GCD, Curry::Op::ABS_ADD}) {
        SCOPED_TRACE(opName(op).c_str());
        EXPECT_EQ(reduce1(op, empty, 7), 7);
        EXPECT_EQ(reduce2(op, empty, 7), 7);
    }
}

// ---------- Anchor tests: guard against both being wrong in the same way ----------

TEST_F(CurryingTest, AddKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::ADD, v1, 0), 20);
    EXPECT_EQ(reduce2(Curry::Op::ADD, v1, 0), 20);
    EXPECT_EQ(reduce1(Curry::Op::ADD, v2, 0), -12);
    EXPECT_EQ(reduce2(Curry::Op::ADD, v2, 0), -12);
}

TEST_F(CurryingTest, MultiplyKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::MULTIPLY, v1, 1), 384);
    EXPECT_EQ(reduce2(Curry::Op::MULTIPLY, v1, 1), 384);
    EXPECT_EQ(reduce1(Curry::Op::MULTIPLY, v2, 1), -56700);
    EXPECT_EQ(reduce2(Curry::Op::MULTIPLY, v2, 1), -56700);
}

TEST_F(CurryingTest, MaxKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::MAX, v1, std::numeric_limits<int>::min()), 8);
    EXPECT_EQ(reduce2(Curry::Op::MAX, v1, std::numeric_limits<int>::min()), 8);
    EXPECT_EQ(reduce1(Curry::Op::MAX, v2, std::numeric_limits<int>::min()), 15);
    EXPECT_EQ(reduce2(Curry::Op::MAX, v2, std::numeric_limits<int>::min()), 15);
}

TEST_F(CurryingTest, MinKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::MIN, v1, std::numeric_limits<int>::max()), 2);
    EXPECT_EQ(reduce2(Curry::Op::MIN, v1, std::numeric_limits<int>::max()), 2);
    EXPECT_EQ(reduce1(Curry::Op::MIN, v2, std::numeric_limits<int>::max()), -21);
    EXPECT_EQ(reduce2(Curry::Op::MIN, v2, std::numeric_limits<int>::max()), -21);
}

TEST_F(CurryingTest, GcdKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::GCD, v1, 0), 2);
    EXPECT_EQ(reduce2(Curry::Op::GCD, v1, 0), 2);
    EXPECT_EQ(reduce1(Curry::Op::GCD, v2, 0), 1);
    EXPECT_EQ(reduce2(Curry::Op::GCD, v2, 0), 1);
}

TEST_F(CurryingTest, LcmKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::LCM, v1, 1), 24);
    EXPECT_EQ(reduce2(Curry::Op::LCM, v1, 1), 24);
    EXPECT_EQ(reduce1(Curry::Op::LCM, v2, 1), 210);
    EXPECT_EQ(reduce2(Curry::Op::LCM, v2, 1), 210);
}

TEST_F(CurryingTest, AddAbsKnownValues) {
    EXPECT_EQ(reduce1(Curry::Op::ABS_ADD, v1, 0), 20);
    EXPECT_EQ(reduce2(Curry::Op::ABS_ADD, v1, 0), 20);
    EXPECT_EQ(reduce1(Curry::Op::ABS_ADD, v2, 0), 56);
    EXPECT_EQ(reduce2(Curry::Op::ABS_ADD, v2, 0), 56);
}
