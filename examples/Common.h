#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>  
#include <FSM.hpp>
#include <string>
#include <typeinfo>

struct EvtBase
{
    EvtBase(std::string name) : m_name(name) {}

    std::string description() const
    {
        return m_name;
    }

private:
    std::string m_name;
};

struct StateBase : State
{
    StateBase(bool isFinal) : State(isFinal) {}
    
    virtual void onEntry()
	{
		std::cout << "Entered " << typeid(*this).name() << std::endl;
	}

    virtual void beforeExit()
	{
		std::cout << "Exiting " << typeid(*this).name() << std::endl;
	}
};

struct CompositeStateBase : CompositeState
{
    CompositeStateBase(std::function<State*()> fn) : CompositeState(fn) {}
    
    virtual void onEntry()
	{
		std::cout << "Entered " << typeid(*this).name();
	}

    virtual void beforeExit()
	{
		std::cout << "Exiting " << typeid(*this).name();
	}
};
