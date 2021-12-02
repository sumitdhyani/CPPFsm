#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>  
#include <FSM.hpp>

struct EvtBase
{
    EvtBase(std::string name) : m_name(name) {}

    std::string description()
    {
        return m_name;
    }

private:
    std::string m_name;
};

struct EvtStart : EvtBase
{
    EvtStart() : EvtBase("Start") {}
};

struct EvtStop: EvtBase
{
    EvtStop() : EvtBase("Stop") {}
};

struct EvtSwitchOff : EvtBase
{
    EvtSwitchOff() : EvtBase("SwitchOff") {}
};

struct EvtLap : EvtBase
{
    EvtLap() : EvtBase("Lap") {}
};

struct Stopped : State, IEventProcessor<EvtStart>, IEventProcessor<EvtSwitchOff>
{
	Stopped() : State(false) {}

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

struct Running: State, IEventProcessor<EvtStop>, IEventProcessor<EvtSwitchOff>, IEventProcessor<EvtLap>
{
	Running() : State(false), m_lap(0), m_initTime(std::chrono::system_clock::now()) {}

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


struct SwitchedOff : State
{
	SwitchedOff() : State(true) {}

    virtual void onEntry()
    {
        std::cout << "Stopwatch switchedOff state at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    }
};

struct StopWatch : IFSM
{
    StopWatch() : IFSM([]() {return new Stopped(); }) {}

	virtual void handleUnconsumedEvent(std::string desc) noexcept
	{
        std::cout << "Unhandled evt: " << desc << std::endl;
	}
};