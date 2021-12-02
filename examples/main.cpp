#include "StopWatch.h"
#include <thread>

int main(int argc, char** argv)
{
	StopWatch stopWatch;
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
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

	return 0;
}