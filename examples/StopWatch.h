#pragma once
#include <iostream>
#include <memory>
#include <variant>
#include <queue>
#include <iostream>
#include <functional>
#include "Common.h"


// =============================================
// Concrete States
// =============================================

/*
    State transition overview of StopWatch
            Start
    Idle ---------> Running 

*/

struct Start {};
struct Stop {};
struct Reset {};
struct SwitchOff {};
struct SwitchOn {};
struct Lap {};

struct Idle final : StateBase,
    IEventProcessor<Start>,
    IEventProcessor<SwitchOff>
{
    Idle() : StateBase(false) {}

    Transition process(const Start&) override;
    Transition process(const SwitchOff&) override;
};

struct SwitchedOff : StateBase
{
    SwitchedOff() : StateBase(true) {}
};

struct Stopped final : StateBase,
    IEventProcessor<Start>,
    IEventProcessor<SwitchOff>,
    IEventProcessor<Reset>
{
    Stopped() : StateBase(false) {}

    Transition process(const Start& evt) override;
    Transition process(const SwitchOff& evt) override;
    Transition process(const Reset& evt) override;
};

struct Running final : StateBase,
    IEventProcessor<Stop>,
    IEventProcessor<Lap>,
    IEventProcessor<SwitchOff>,
    IEventProcessor<Reset>
{
    Running() : StateBase(false), m_lap(0), m_initTime(std::chrono::system_clock::now()) {}

    Transition process(const Stop& evt) override;
    Transition process(const Lap& evt) override;
    Transition process(const SwitchOff& evt) override;
    Transition process(const Reset& evt) override;
private:
    int m_lap;
    std::chrono::time_point<std::chrono::system_clock> m_initTime;
};




