#include <memory>
#include <variant>
#include <functional>
#include <stack>
#include <variant>

enum class SpecialTransitions
{
	UnhandledEvt,
	NullTransition
};

struct FinalityReachedException : std::runtime_error
{
	FinalityReachedException() : std::runtime_error("State machine has reched final state and can't process any new events") {}
};

struct NullStateException : std::runtime_error
{
	NullStateException() : std::runtime_error("State can't be a nullptr return SpecialTransitions::NullTransition in case there is no state transition") {}
};

struct State
{
	State(bool isFinal) : m_isFinal(isFinal)
	{}
	virtual void onEntry() {};
	virtual void beforeExit() {};
	bool isFinal(){	return m_isFinal; }
private:
	bool m_isFinal;
};

typedef std::variant<State*, SpecialTransitions> Transition;

template<typename EvtType>
struct IEventProcessor
{
	virtual Transition process(const EvtType& arg) = 0;
};

struct IFSM
{
	IFSM(std::function<State* ()> fn) : m_currState(fn()) {}
	
	template<typename EventType>
	
	Transition onEvent(const EventType& evt)
	{
		if (m_currState->isFinal())
			throw FinalityReachedException();

		auto transition = findNextState(evt);
		if (isUnhandled(transition))
			onUnconsumedEvent(evt);
		else if (!isNullTransition(transition))
		{
			handleStateExit(m_currState);
			auto nextState = std::get<State*>(transition);
			if (!nextState)
				throw NullStateException();
			m_currState = nextState;
			handleStateEntry(m_currState);
		}

		return transition;
	}

	void start()
	{
		if (m_currState->isFinal())
			throw FinalityReachedException();
		else
			handleStateEntry(m_currState);
	}

private:
	State* m_currState;

	bool isUnhandled(const Transition& transition)
	{
		try
		{
			return SpecialTransitions::UnhandledEvt == std::get<SpecialTransitions>(transition);
		}
		catch (std::bad_variant_access)
		{
			return false;
		}
	}

	bool isNullTransition(const Transition& transition)
	{
		try
		{
			return SpecialTransitions::NullTransition == std::get<SpecialTransitions>(transition);
		}
		catch (std::bad_variant_access)
		{
			return false;
		}
	}
	
	template<typename EventType>
	Transition findNextState(const EventType& evt)
	{
		try
		{
			auto& evtProcessor = dynamic_cast<IEventProcessor<EventType>&>(*m_currState);
			return evtProcessor.process(evt);
		}
		catch (std::bad_cast) {}

		//Us reaching here means that this is an unhandled event,
		//we now need to check whether the current state is a composite state and if yes,
		//then pass this event to its internal state machine
		Transition next = SpecialTransitions::UnhandledEvt;
		try
		{
			while (isUnhandled(next))
			{
				auto& childStateMachine = dynamic_cast<IFSM&>(*m_currState);
				next = childStateMachine.onEvent(evt);
			}
		}
		catch (std::bad_cast) {}
		catch (FinalityReachedException) {}

		return next;
	}

	void handleStateEntry(State* state)
	{
		state->onEntry();
		try
		{
			auto& childStateMachine = dynamic_cast<IFSM&>(*state);
			childStateMachine.start();
		}
		catch (std::bad_cast) {}
	}

	void handleStateExit(State* state)
	{
		try
		{
			auto& childStateMachine = dynamic_cast<IFSM&>(*state);
			handleStateExit(childStateMachine.m_currState);
		}
		catch (std::bad_cast) {}

		state->beforeExit();
		delete m_currState;
	}

	template<typename EventType>
	void onUnconsumedEvent(const EventType& evt) noexcept
	{
		handleUnconsumedEvent(evt.description());
	}

	virtual void handleUnconsumedEvent(std::string desc) noexcept {}
};

