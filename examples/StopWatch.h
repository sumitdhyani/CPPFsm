#pragma once
#include "Common.h"

struct EvtStart : EvtBase {};

struct EvtStop : EvtBase {};

struct EvtSwitchOff : EvtBase {};

struct EvtLap : EvtBase {};

struct Stopped : StateBase, IEventProcessor<EvtStart>, IEventProcessor<EvtSwitchOff>
{
	Stopped() : StateBase(false) {}

    virtual void onEntry()
    {
        std::cout << "Stopwatch stopped state at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    }

    virtual void beforeExit()
    {
        std::cout << "Stopwatch leaving stopped state at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    }

    virtual State* process(const EvtStart& evt);
    virtual State* process(const EvtSwitchOff& evt);
};

struct Running: StateBase, IEventProcessor<EvtStop>, IEventProcessor<EvtSwitchOff>, IEventProcessor<EvtLap>
{
	Running() : StateBase(false), m_lap(0), m_initTime(std::chrono::system_clock::now()) {}

    virtual void onEntry()
    {
        std::cout << "Stopwatch running state at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    }

    virtual void beforeExit()
    {
        std::cout << "Stopwatch leaving running state at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    }

    virtual State* process(const EvtStop& evt);
    virtual State* process(const EvtSwitchOff& evt);
    virtual State* process(const EvtLap& evt);
private:
    int m_lap;
    std::chrono::time_point<std::chrono::system_clock> m_initTime;
};


struct SwitchedOff : StateBase
{
	SwitchedOff() : StateBase(true) {}

    virtual void onEntry()
    {
        std::cout << "Stopwatch switchedOff state at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    }
};
