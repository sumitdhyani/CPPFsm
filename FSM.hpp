#include <memory>
#include <functional>
#include <queue>

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
protected:
	inline static State* defer = reinterpret_cast<State*>(2);
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
	FSM(std::function<State* ()> fn, 
		std::function<void(std::string)> unconsumedEventHandler = [](std::string desc) {},
		std::function<void(State*)> deleter = [](State* state) {delete state; }
		) : m_currState(fn()), m_unconsumedEventHandler(unconsumedEventHandler), m_deleter(deleter), m_started(false)
	{}
	
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

	virtual ~FSM() { m_deleter(m_currState); }

	inline static State* defer = reinterpret_cast<State*>(2);
protected:
	State* m_currState;
private:

	inline static State* unhandledTransition = reinterpret_cast<State*>(1);
	bool m_started;
	std::function<void(std::string)> m_unconsumedEventHandler;
	std::function<void(State*)> m_deleter;
	std::queue<std::function<void()>> m_deferralQueue;

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
		else if (defer == transition)
			m_deferralQueue.push([this, evt]() { handleEvent(evt); });
		else if (nullptr != transition)
		{
			handleStateExit(m_currState);
			m_deleter(m_currState);
			m_currState = transition;
			handleStateEntry(m_currState);
		}

		return transition;
	}

	void processDeferralQueue()
	{
		std::queue<std::function<void()>> local;
		local.swap(m_deferralQueue);
		
		while (!local.empty())
		{
			local.front()();
			local.pop();
		}
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
			transition = nullptr;
		
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
		processDeferralQueue();
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
		m_unconsumedEventHandler(evt.description());
	}
};

struct CompositeState : FSM, State
{
	CompositeState(	std::function<State* ()> fn, 
					std::function<void(std::string)> unconsumedEventHandler = [](std::string desc) {},
					std::function<void(State*)> deleter = [](State* state) {delete state; }
				   ) : FSM(fn, unconsumedEventHandler, deleter), State(false)
	{}
};
