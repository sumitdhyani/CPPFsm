#include <memory>
#include <functional>
#include <queue>
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


enum class Specialtransition
{
	nulltransition,
	deferralTransition
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

typedef std::variant<std::unique_ptr<State>, Specialtransition> Transition;

template<typename EvtType>
struct IEventProcessor
{
	virtual Transition process(const EvtType& arg) = 0;
};

struct FSM
{
	FSM(std::function<std::unique_ptr<State> ()> fn, 
		std::function<void(std::string)> unconsumedEventHandler = [](std::string desc) {}
		) : m_currState(fn()), m_unconsumedEventHandler(unconsumedEventHandler), m_started(false)
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
			handleStateEntry(*m_currState);
	}

	virtual ~FSM() { }
protected:
	std::unique_ptr<State> m_currState;
private:

	bool m_started;
	std::function<void(std::string)> m_unconsumedEventHandler;
	std::function<void(State*)> m_deleter;
	std::queue<std::function<void()>> m_deferralQueue;

	template<typename EventType>
	Specialtransition onEvent(const EventType& evt)
	{
		if (!m_started)
			throw SMInactiveException();
		else if (m_currState->isFinal())
			throw FinalityReachedException();

		try
		{
			auto& evtProcessor = dynamic_cast<IEventProcessor<EventType>&>(*m_currState);
			Transition transition = evtProcessor.process(evt);
			try
			{
				auto nextState = std::move(std::get<std::unique_ptr<State>>(transition));
				handleStateExit(*m_currState);
				m_currState = std::move(nextState);
				handleStateEntry(*m_currState);
			}
			catch(std::bad_variant_access)
			{
				if (Specialtransition::deferralTransition == std::get<Specialtransition>(transition))
					m_deferralQueue.push([this, evt]() { handleEvent(evt); });
			}

			return Specialtransition::nulltransition;
		}
		catch (std::bad_cast) {}

		try
		{
			auto& childStateMachine = dynamic_cast<FSM&>(*m_currState);
			return childStateMachine.onEvent(evt);
		}
		catch (std::bad_cast)
		{
			onUnconsumedEvent(evt);
		}
		catch (FinalityReachedException) {}

		return Specialtransition::nulltransition;
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
	
	void handleStateEntry(State& state)
	{
		state.onEntry();
		try
		{
			auto& childStateMachine = dynamic_cast<FSM&>(state);
			childStateMachine.start();
		}
		catch (std::bad_cast) {}
		processDeferralQueue();
	}

	void handleStateExit(State& state)
	{
		try
		{
			auto& childStateMachine = dynamic_cast<FSM&>(state);
			handleStateExit(*childStateMachine.m_currState);
		}
		catch (std::bad_cast) {}

		state.beforeExit();
	}

	template<typename EventType>
	void onUnconsumedEvent(const EventType& evt) noexcept
	{
		m_unconsumedEventHandler(evt.description());
	}
};

struct CompositeState : FSM, State
{
	CompositeState(	std::function<std::unique_ptr<State> ()> fn, 
					std::function<void(std::string)> unconsumedEventHandler = [](std::string desc) {}
				   ) : FSM(fn, unconsumedEventHandler), State(false)
	{}
};
