#pragma once
#include <memory>
#include <functional>
#include <queue>
#include <variant>
#include <iostream>

namespace ULFSM
{

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

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
	nullTransition,
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

template<class... EvtType>
struct IEventProcessor
{
	virtual Transition process(const EvtType&... arg) = 0;
	virtual ~IEventProcessor() = default;
};

template <class Derived>
struct FSM
{
	FSM(const std::function<std::unique_ptr<State>()>& fn) : m_currState(fn()), m_started(false)
	{}

	template<class... EventType>
	void handleEvent(const EventType&... evt)
	{
		onEvent(evt...);
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
	template <class... EventType>
	void handleUnconsumedEvent(const EventType&... evt) noexcept
	{
	}
private:

	bool m_started;
	std::function<void(State*)> m_deleter;
	std::queue<std::function<void()>> m_deferralQueue;

	template<class... EventType>
	Specialtransition onEvent(const EventType&... evt)
	{
		if (!m_started)
			throw SMInactiveException();
		else if (m_currState->isFinal())
			throw FinalityReachedException();

		
		if (auto* evtProcessor = dynamic_cast<IEventProcessor<EventType...>*>(m_currState.get());
			evtProcessor)
		{
			Transition transition = evtProcessor->process(evt...);
			return std::visit(overload{
				[&](std::unique_ptr<State>& nextState) -> Specialtransition
				{
					if (!nextState)
						throw NullStateException();
					
					handleStateExit(*m_currState);
					m_currState = std::move(nextState);
					handleStateEntry(*m_currState);
					return Specialtransition::nullTransition;
				},
				[&](const Specialtransition st) -> Specialtransition
				{
					if (Specialtransition::deferralTransition == st)
						m_deferralQueue.push([this, evt...]() { handleEvent(evt...); });
					return Specialtransition::nullTransition;
				}
			}, transition);
		}
		else if(auto* childStateMachine = dynamic_cast<FSM*>(m_currState.get());
			 	childStateMachine)
		{
			return childStateMachine->onEvent(evt...);
		}
		else
		{
			onUnconsumedEvent(evt...);
			return Specialtransition::nullTransition;
		}
	}

	void processDeferralQueue()
	{
		std::queue<std::function<void()>> local;
		// This weird swap is to avoid processing events added to the deferral
		// queue while we are processing the deferral queue
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
		if (auto* childStateMachine = dynamic_cast<FSM*>(&state);
			childStateMachine)
		{
			childStateMachine->start();
		}
		processDeferralQueue();
	}

	void handleStateExit(State& state)
	{
		if (auto* childStateMachine = dynamic_cast<FSM*>(&state);
			 childStateMachine)
		{
			handleStateExit(*childStateMachine->m_currState);
		}

		state.beforeExit();
	}

	template<class... EventType>
	void onUnconsumedEvent(const EventType&... evt) noexcept
	{
		static_cast<Derived*>(this)->handleUnconsumedEvent(evt...);
	}
};

struct CompositeState : FSM<CompositeState>, State
{
	CompositeState(	const std::function<std::unique_ptr<State>()>& fn) : FSM<CompositeState>(fn), State(false)
	{}
};

}