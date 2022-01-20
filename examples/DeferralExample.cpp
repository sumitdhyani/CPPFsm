#include "DeferralExample.h"

State* Focused::process(const click& click)
{
	std::cout << "Image clicked" << std::endl;
	return nullptr;
}

State* Focused::process(const buttonReleased& click)
{
	std::cout << "Image clicked" << std::endl;
	return new Idle;
}

State* Focusing::process(const focused& evt)
{
	return new Focused;
}

State* Focusing::process(const click& evt)
{
	return defer;
}

State* Idle::process(const focus& evt)
{
	return new Focusing;
}

State* Browsing::process(const shoot& evt)
{
	return new Shooting;
}

State* Browsing::process(const displayImage& evt)
{
	std::cout << "Display image, image name: " << evt.m_image << std::endl;
	return nullptr;
}


State* Shooting::process(const browse& evt)
{
	return new Browsing;
}

int main(int argc, char** argv)
{
	FSM fsm([]() {return new Shooting; });
	fsm.start();
	try
	{
		fsm.handleEvent(focus());
		fsm.handleEvent(click());
		fsm.handleEvent(focused());
	}
	catch (const FinalityReachedException& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}