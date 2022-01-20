#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>  
#include <FSM.hpp>
#include <string>
#include <typeinfo>

//struct FSMBase : FSM
//{
//    std::string typeof(FSM);
//};

struct EvtBase
{
    EvtBase() {}

    virtual std::string description() const
    {
        return typeid(*this).name();
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
		std::cout << "Entered " << typeid(*this).name() << std::endl;
	}

    virtual void beforeExit()
	{
		std::cout << "Exiting " << typeid(*this).name() << std::endl;
	}
};

struct NonFinalState : StateBase
{
    NonFinalState() : StateBase(false) {}
};

struct FinalState : StateBase
{
    FinalState() : StateBase(true) {}
};

