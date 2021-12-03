#include "StopWatch.h"
#include <thread>

Transition Stopped::process(const EvtStart& evt)
{
	return new Running();
}

Transition Stopped::process(const EvtSwitchOff& evt)
{
	return new SwitchedOff();
}

Transition Running::process(const EvtStop& evt)
{
	return new Stopped();
}

Transition Running::process(const EvtLap& evt)
{
	m_lap++;
	std::cout << "Stopwatch lap " << m_lap << ", time : " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_initTime).count() << " sec" << std::endl;
	return SpecialTransitions::NullTransition;
}

Transition Running::process(const EvtSwitchOff& evt)
{
	return new SwitchedOff();
}

int main(int argc, char** argv)
{
    FSM stopWatch([]() {return new Stopped(); });
    stopWatch.start();
    int sleepInterval = 1;
    try
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.onEvent(EvtStart());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.onEvent(EvtLap());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.onEvent(EvtLap());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.onEvent(EvtStop());

        std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));
        stopWatch.onEvent(EvtSwitchOff());
        stopWatch.onEvent(EvtSwitchOff());
    }
    catch (const FinalityReachedException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

	return 0;
}

