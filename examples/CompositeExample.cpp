#include "CompositeExample.h"

State* SC1::process(const e1& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC2();
}

State* SC2::process(const e2& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC1();
}

State* SC2::process(const eTerminate& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new STerminated();
}

State* SC11::process(const e11& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC12();
}

State* SC21::process(const e21& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC22();
}

int main(int argc, char** argv)
{
	FSM fsm([]() { return new SC1(); }, [](std::string desc) { std::cout << "Recieved unhandled evt: " << desc << std::endl; } );
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
