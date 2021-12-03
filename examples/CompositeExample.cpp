#include "CompositeExample.h"

Transition SC1::process(const e2& evt)
{
	return new SC2();
}

Transition SC2::process(const e1& evt)
{
	return new SC1();
}

Transition SC2::process(const eTerminate& evt)
{
	return new STerminated();
}

Transition SC11::process(const e12& evt)
{
	return new SC12();
}

Transition SC21::process(const e22& evt)
{
	return new SC22();
}

int main(int argc, char** argv)
{
	CompositeSM fsm;
	fsm.start();
	try
	{
		fsm.onEvent(e1());
		fsm.onEvent(e2());
		fsm.onEvent(e11());
		fsm.onEvent(e12());
		fsm.onEvent(e1());
		fsm.onEvent(e21());
		fsm.onEvent(eTerminate());
		//Exception should be thrown here as we have reached finality here
		fsm.onEvent(e2());
	}
	catch (const FinalityReachedException& ex)
	{
        std::cout << ex.what() << std::endl;
	}
	return 0;
}
