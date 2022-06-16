#include "DeferralExample.h"

Transition Focused::process(const click& click)
{
	std::cout << "Image clicked" << std::endl;
	return Specialtransition::nullTransition;
}

Transition Focused::process(const buttonReleased& click)
{
	std::cout << "Image clicked" << std::endl;
	return std::make_unique<Idle>();
}

Transition Focusing::process(const focused& evt)
{
	return std::make_unique<Focused>();
}

Transition Focusing::process(const click& evt)
{
	std::cout << "deferring the click event while in Focusing state." << std::endl;
	return Specialtransition::deferralTransition;
}

Transition Idle::process(const focus& evt)
{
	return std::make_unique<Focusing>();
}

Transition Browsing::process(const shoot& evt)
{
	return std::make_unique<Shooting>();
}

Transition Browsing::process(const displayImage& evt)
{
	std::cout << "Display image, image name: " << evt.m_image << std::endl;
	return Specialtransition::nullTransition;
}


Transition Shooting::process(const browse& evt)
{
	return std::make_unique<Browsing>();
}

int main(int argc, char** argv)
{
	FSM fsm([]() { return std::make_unique<Shooting>(); });
	fsm.start();
	try
	{
		fsm.handleEvent(focus());
		fsm.handleEvent(click());
		fsm.handleEvent(click());
		fsm.handleEvent(focused());
	}
	catch (const FinalityReachedException& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}