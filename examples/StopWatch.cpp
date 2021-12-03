#include "StopWatch.h"

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
