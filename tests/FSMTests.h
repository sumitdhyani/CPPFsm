#pragma once
#include "FSM.hpp"
#include <string>
#include <memory>
using namespace ULFSM;

struct StartEvent {
    std::string description() const { return "StartEvent"; }
};

struct StopEvent {
    std::string description() const { return "StopEvent"; }
};

struct DataEvent {
    int value = 0;
    std::string description() const { return "DataEvent(" + std::to_string(value) + ")"; }
};

struct UnknownEvent {
    std::string description() const { return "UnknownEvent"; }
};

// ====================== Test States Definitions ======================

struct IdleState : IEventProcessor<StartEvent>, State {
    IdleState() : State(false) {}

    Transition process(const StartEvent&) override;
};

struct RunningState :
    IEventProcessor<StopEvent>, 
    IEventProcessor<DataEvent>,
    State
{
    RunningState() : State(false) {}

    Transition process(const StopEvent&) override;
    Transition process(const DataEvent& e) override;
};

struct FinalState : State {
    FinalState() : State(true) {}
};

// Composite State
struct CompositeTest : CompositeState {
    CompositeTest() : CompositeState([]{ return std::make_unique<IdleState>(); }) {}
};

