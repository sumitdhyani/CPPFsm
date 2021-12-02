#include "StopWatch.h"

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
