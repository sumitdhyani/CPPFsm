#include <memory>
#include <variant>
#include <functional>
#include <stack>
#include <variant>

struct FinalityReachedException : std::runtime_error
{
	FinalityReachedException() : std::runtime_error("State machine has reched final state and can't process any new events") {}
};

struct NullStateException : std::runtime_error
{
	NullStateException() : std::runtime_error("State can't be a nullptr return SpecialTransitions::NullTransition in case there is no state transition") {}
};

struct SMInactiveException : std::runtime_error
{
	SMInactiveException() : std::runtime_error("State machine needs to be started by calling the start() method") {}
};

struct State
{
	State(bool isFinal) : m_isFinal(isFinal)
	{}
	virtual void onEntry() {};
	virtual void beforeExit() {};
	bool isFinal(){	return m_isFinal; }
	virtual ~State() {}
private:
	bool m_isFinal;
};

template<typename EvtType>
struct IEventProcessor
{
	virtual State* process(const EvtType& arg) = 0;
};

struct FSM
{
	FSM(std::function<State*()> fn) : m_currState(fn()), m_started(false) {}
	
	template<typename EventType>
	void handleEvent(const EventType& evt)
	{
		onEvent(evt);
	}

	void start()
	{
		m_started = true;
		if (m_currState->isFinal())
			throw FinalityReachedException();
		else
			handleStateEntry(m_currState);
	}

	virtual ~FSM() { delete m_currState; }

	virtual void handleUnconsumedEvent(std::string desc) noexcept {}

private:

	inline static State* unhandledTransition = reinterpret_cast<State*>(1);
	State* m_currState;
	bool m_started;

	template<typename EventType>
	State* onEvent(const EventType& evt)
	{
		if (!m_started)
			throw SMInactiveException();
		else if (m_currState->isFinal())
			throw FinalityReachedException();

		auto transition = findNextState(evt);
		if (unhandledTransition == transition)
			onUnconsumedEvent(evt);
		else if (nullptr != transition)
		{
			handleStateExit(m_currState);
			delete m_currState;
			m_currState = transition;
			handleStateEntry(m_currState);
		}

		return transition;
	}
	
	template<typename EventType>
	State* findNextState(const EventType& evt)
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
		State* transition = unhandledTransition;
		try
		{
			auto& childStateMachine = dynamic_cast<FSM&>(*m_currState);
			transition = childStateMachine.onEvent(evt);
		}
		catch (std::bad_cast) {}
		catch (FinalityReachedException) {}

		if (unhandledTransition != transition)
			return nullptr;
		else
			return transition;
	}

	void handleStateEntry(State* state)
	{
		state->onEntry();
		try
		{
			auto& childStateMachine = dynamic_cast<FSM&>(*state);
			childStateMachine.start();
		}
		catch (std::bad_cast) {}
	}

	void handleStateExit(State* state)
	{
		try
		{
			auto& childStateMachine = dynamic_cast<FSM&>(*state);
			handleStateExit(childStateMachine.m_currState);
		}
		catch (std::bad_cast) {}

		state->beforeExit();
	}

	template<typename EventType>
	void onUnconsumedEvent(const EventType& evt) noexcept
	{
		handleUnconsumedEvent(evt.description());
	}
};

struct CompositeState : FSM, State
{
	CompositeState(std::function<State* ()> fn) : FSM(fn), State(false) {}
};
