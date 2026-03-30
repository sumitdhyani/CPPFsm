#include "CompositeExample.h"

Transition SC1::process(const e1& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return std::make_unique<SC2>();
}

Transition SC2::process(const e2& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return std::make_unique<SC1>();
}

Transition SC2::process(const eTerminate& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return std::make_unique<STerminated>();
}

Transition SC11::process(const e11& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return std::make_unique<SC12>();
}

Transition SC21::process(const e21& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return std::make_unique<SC22>();
}


struct Fsm : FSM<Fsm>
{
	Fsm() : FSM<Fsm>([]() { return std::make_unique<SC1>(); }) {}
};

int main(int argc, char** argv)
{
	Fsm fsm;
	fsm.start();
	try
	{
		fsm.handleEvent(e1());
		fsm.handleEvent(e2());
		fsm.handleEvent(e11());
		fsm.handleEvent(e12());
		fsm.handleEvent(e1());
		fsm.handleEvent(e21());
		fsm.handleEvent(eTerminate());
		//Exception should be thrown here as we have reached finality here
		fsm.handleEvent(e2());
	}
	catch (const FinalityReachedException& ex)
	{
        std::cout << ex.what() << std::endl;
	}
	return 0;
}
