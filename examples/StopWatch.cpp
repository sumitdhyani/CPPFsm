#include "StopWatch.h"
#include <thread>

//===============================================================
Transition Idle::process(const Start&)
{
    return std::make_unique<Running>();
}

Transition Idle::process(const SwitchOff&)
{
    return std::make_unique<SwitchedOff>();
}

//===============================================================
Transition Stopped::process(const Start& evt)
{
    return std::make_unique<Running>();
}

Transition Stopped::process(const SwitchOff& evt)
{
	return std::make_unique<SwitchedOff>();
}

Transition Stopped::process(const Reset& evt)
{
    return std::make_unique<Idle>();
}

//===============================================================
Transition Running::process(const Stop& evt)
{
	return std::make_unique<Stopped>();
}

Transition Running::process(const Lap& evt)
{
	m_lap++;
	std::cout << "Stopwatch lap " << m_lap << ", time : " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_initTime).count() << " sec" << std::endl;
    return Specialtransition::nullTransition;
}

Transition Running::process(const SwitchOff& evt)
{
	return std::make_unique<SwitchedOff>();
}

Transition Running::process(const Reset&)
{
    return std::make_unique<Idle>();
}

struct Fsm : FSM<Fsm>
{
    Fsm() : FSM<Fsm>([]() { return std::make_unique<Idle>(); }) {}
};

int main(int argc, char** argv)
{
    Fsm stopWatch;
    stopWatch.start();
    int sleepInterval = 1;
    try
    {
        stopWatch.handleEvent(Start{});

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(Lap{});

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(Lap{});

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(Stop{});

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(SwitchOff{});
        stopWatch.handleEvent(SwitchOff{});
    }
    catch (const FinalityReachedException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

	return 0;
}

