#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>  
#include <FSM.hpp>
#include <string>

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
