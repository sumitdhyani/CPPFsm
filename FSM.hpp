#include <memory>
#include <variant>
#include <functional>
#include <stack>

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

template<typename EvtType>
struct IEventProcessor
{
	virtual State* process(const EvtType& arg) = 0;
};

struct IFSM
{
	template<typename EventType>
	void onEvent(const EventType& evt)
	{
		if (m_finalityReached)
			throw std::runtime_error("State machine has reched final state and can't process any new events");

		State* nextState = nullptr;
		try
		{
			auto& ref = dynamic_cast<IEventProcessor<EventType>&>(*m_currState);
			nextState = ref.process(evt);
		}
		catch (std::bad_cast)
		{
			nextState = nullptr;
		}

		if (nextState)
		{
			m_currState->beforeExit();
			m_history.push(std::shared_ptr<State>(m_currState));
			m_currState = nextState;
			m_currState->onEntry();
			m_finalityReached = m_currState->isFinal();
		}
	}

	template<typename EventType>
	void onUnconsumedEvent(const EventType& evt) noexcept
	{
		handleUnconsumedEvent(evt.description());
	}

	virtual void handleUnconsumedEvent(std::string desc) noexcept
	{
	}

	IFSM(std::function<State*()> fn)
	{
		m_currState = fn();
		m_currState->onEntry();
		m_finalityReached = m_currState->isFinal();
	}

private:
	State* m_currState;
	std::stack<std::shared_ptr<State>> m_history;
	bool m_finalityReached;
};

