#include "CompositeExample.h"

Transition SC1::process(const e1& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC2();
}

Transition SC2::process(const e2& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC1();
}

Transition SC2::process(const eTerminate& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new STerminated();
}

Transition SC11::process(const e11& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC12();
}

Transition SC21::process(const e21& evt)
{
	std::cout << "Recieved evt: " << evt.description() << std::endl;
	return new SC22();
}

struct LocalFSM : FSM
{
	LocalFSM(std::function<State* ()> fn) : FSM(fn) {}

	virtual void handleUnconsumedEvent(std::string desc) noexcept
	{
		std::cout << "Recieved unhandled evt: " << desc << std::endl;
	}
};

int main(int argc, char** argv)
{
	LocalFSM fsm([]() { return new SC1(); });
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
