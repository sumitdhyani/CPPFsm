#include "StopWatch.h"
#include <thread>

State* Stopped::process(const EvtStart& evt)
{
	return new Running();
}

State* Stopped::process(const EvtSwitchOff& evt)
{
	return new SwitchedOff();
}

State* Running::process(const EvtStop& evt)
{
	return new Stopped();
}

State* Running::process(const EvtLap& evt)
{
	m_lap++;
	std::cout << "Stopwatch lap " << m_lap << ", time : " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_initTime).count() << " sec" << std::endl;
    return nullptr;
}

State* Running::process(const EvtSwitchOff& evt)
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

