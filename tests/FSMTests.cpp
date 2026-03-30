// FSM_test.cpp
#include <gtest/gtest.h>
#include "FSMTests.h"

// ====================== Test States Definitions ======================

Transition IdleState::process(const StartEvent&) {
    return std::make_unique<RunningState>();
}

Transition RunningState::process(const StopEvent&) {
    return std::make_unique<IdleState>();
}

Transition RunningState::process(const DataEvent& e) {
    if (e.value >= 100)
        return std::make_unique<FinalState>();
    return Specialtransition::deferralTransition;
}

// ====================== UNIT TESTS ======================

// ====================== Test Fixture ======================
class FSMTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

struct TestFsm: FSM<TestFsm>
{
    TestFsm(const std::function<std::unique_ptr<State>()>& fn) : FSM<TestFsm>(fn) {}
};

TEST_F(FSMTest, BasicTransition) {
     TestFsm sm([]{ return std::make_unique<IdleState>(); });
    sm.start();

    EXPECT_NO_THROW(sm.handleEvent(StartEvent{}));
}

// TEST_F(FSMTest, UnconsumedEventHandler) {
//     bool called = false;
//     std::string desc;

//     FSM sm([]{ return std::make_unique<RunningState>(); },
//            [&](std::string d) { called = true; desc = d; });

//     sm.start();
//     sm.handleEvent(UnknownEvent{});

//     EXPECT_TRUE(called);
//     EXPECT_EQ(desc, "UnknownEvent");
// }

TEST_F(FSMTest, DeferralMechanism) {
    TestFsm sm([]{ return std::make_unique<RunningState>(); });
    sm.start();

    EXPECT_NO_THROW({
        sm.handleEvent(DataEvent{50});   // defer
        sm.handleEvent(StopEvent{});     // transition -> defer should process
    });
}

TEST_F(FSMTest, CompositeStateSupport) {
    TestFsm sm([]{ return std::make_unique<CompositeTest>(); });
    sm.start();

    EXPECT_NO_THROW(sm.handleEvent(StartEvent{}));
}

TEST_F(FSMTest, FinalStateThrowsException) {
    TestFsm sm([]{ return std::make_unique<FinalState>(); });
    EXPECT_THROW(sm.start(), FinalityReachedException);
}

TEST_F(FSMTest, InactiveSMThrowsException) {
    TestFsm sm([]{ return std::make_unique<IdleState>(); });
    EXPECT_THROW(sm.handleEvent(StartEvent{}), SMInactiveException);
}

TEST_F(FSMTest, VariadicMultipleEvents) {
    TestFsm sm([]{ return std::make_unique<RunningState>(); });
    sm.start();

    EXPECT_NO_THROW({
        sm.handleEvent(StopEvent{}, DataEvent{10});
    });
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}