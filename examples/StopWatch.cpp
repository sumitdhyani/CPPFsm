#include "StopWatch.h"
#include <thread>

Transition Stopped::process(const EvtStart& evt)
{
	return std::make_unique<Running>();
}

Transition Stopped::process(const EvtSwitchOff& evt)
{
	return std::make_unique<SwitchedOff>();
}

Transition Running::process(const EvtStop& evt)
{
	return std::make_unique<Stopped>();
}

Transition Running::process(const EvtLap& evt)
{
	m_lap++;
	std::cout << "Stopwatch lap " << m_lap << ", time : " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_initTime).count() << " sec" << std::endl;
    return Specialtransition::nullTransition;
}

Transition Running::process(const EvtSwitchOff& evt)
{
	return std::make_unique<SwitchedOff>();
}

int main(int argc, char** argv)
{
    FSM stopWatch([]() {return std::make_unique<Stopped>(); });
    stopWatch.start();
    int sleepInterval = 1;
    try
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(EvtStart());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(EvtLap());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(EvtLap());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(EvtStop());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.handleEvent(EvtSwitchOff());
        stopWatch.handleEvent(EvtSwitchOff());
    }
    catch (const FinalityReachedException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

	return 0;
}

