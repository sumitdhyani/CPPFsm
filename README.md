# CPPFsm
A C++ module which provides interface for a finite state machine.

Classes and Terms:
- struct State: Base class of all user defined states, to create a user defined state, this class should be extended

- enum class Specialtransition: An enum class representing some special transitions that can happen when a state machine
  handles an event, it has 2 values, nulltransition and deferraltransition, which are elaborated in the following text

- Transition: a typedef for std::variant<std::unique_ptr<State>, Specialtransition>, the current 
  state of the state machine, after receiving an event return a Transition object. In other words,
  a Transition object represents the response to an event received by the State machine.

===================================================================================
Basic:
A StateMachine is represented by an object of class FSM.
Its constructor accepts a function/functor/lambda which shuould return a std::unique_ptr<State>.
This unique_pointer will be the initial State for the State Machine.

The State class is the Base class of all user defined states.
It accepts a boolean in constructor which signifies whether the state is final or not.
To create a user defined state, this class should be extended.
A State subclass must override the onEntry and beforeExit methods if it needs to perform some action just after entry and just before exit, respectively for those states in the state machine.

The application can raise an event by invoking the "handleEvent" method of the state machine.
For the current state to be able to process an event of type T, the State must extend the class IEventProcessor<T>, and override the pure virtual method "virtual State* process(const EvtType& arg)" to handle the event.
To transition to a new state the event handler must return a std::unique<State>. If there is no transition, the "process" method mentioned
above should return Specialtransition::nullTransition 
Any class which is used as an event should define the method "std::string description()" which ideally should return the description of the event.

Raising an event on a state machine which has reached final state leads to a FinalityReachedException exception.

Please refer to the StopWatch example to get a feel of the API. 
===================================================================================
Composite states:
The API also supports composite states, these are the states which have one or internal states and run their own state machine within them.
So, conceptually, a CompositeState is both a state and a state machine.
To outside code, only the outermost state is visible and any event passed to a composite state is first attempted to be handled by the outermost state.
If there isn't a handler for the event in the outermost state, then the event is passed to the current inner state and this process goes on untill we find an event handler or we reach the innermost atomic state.

By design the outside code can't directly transition to the inner state of a composite state. They will have to trasnsition to the composite state whose starting state will always be the one returned in the constructor

Also, an inner state can't transition outside its parent state as the outside world is essentially unknown to it.

A composite state can't be final.
The Composite State is represented by a subclass of CompositeState class is.
Since it is also a StateMachine, it requires a function/functor/lambda which shuould return a State* pointer.
This State* pointer will be the initial State for its internal State Machine.
Please refer to the CompositeExample example to get a feel of this feature.
===================================================================================
Deferal of events:
Its is also possible to defer an event to the next state. If the current state knows that the current event may be relavant in one of the next states it might make sense to defer it to be handled after the next transition. Whenever a transition takes place, the events in the deferal queue are processed in FIFO order by the state machine before processing any new events. As a real life analogy, in many cameras, if user clicks while the camera is still trying to set focus, the software does nothing at the moment but automatically takes a shot as soons as the focus is set.
So the "click" event was deferred while in "Focussing" state as it was relavant in the next "Focussed" state and was eventually processed.

To achieve this, the event handler must return SpecialTransition::deferraltransition in the respective Event handler to defer an event to the next state.
Please refer to the DeferalExample example to get a feel of this feature.
===================================================================================
Examples:
Please refer to the example code snippets in the "examples" directory to see coding examples.

