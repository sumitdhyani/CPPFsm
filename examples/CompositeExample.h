#pragma once
#include "Common.h"

struct eTerminate : EvtBase
{
	eTerminate() : EvtBase("eTerminate") {};
};

struct e1 : EvtBase
{
	e1() : EvtBase("e1") {};
};

struct e2 : EvtBase
{
	e2() : EvtBase("e2") {};
};

struct e11 : EvtBase
{
	e11() : EvtBase("e11") {};
};

struct e12 : EvtBase
{
	e12() : EvtBase("e12") {};
};

struct e21 : EvtBase
{
	e21() : EvtBase("e21") {};
};

struct e22 : EvtBase
{
	e22() : EvtBase("e12") {};
};

struct SC11 : StateBase , IEventProcessor<e12>
{
	SC11() : StateBase(false) {}
	virtual Transition process(const e12& evt);
};

struct SC12 : StateBase 
{
	SC12() : StateBase(true) {}
};

struct SC21 : StateBase, IEventProcessor<e22> 
{
	SC21() : StateBase(false) {}
	virtual Transition process(const e22& evt);
};

struct SC22 : StateBase
{
	SC22() : StateBase(true) {}
};

struct SC1 : CompositeStateBase, IEventProcessor<e2> 
{
	SC1() : CompositeStateBase([]() {return new SC11();}) {}
	virtual Transition process(const e2& evt);
};

struct SC2 : CompositeStateBase, IEventProcessor<e1>, IEventProcessor<eTerminate>
{
	SC2() : CompositeStateBase([]() {return new SC21();}) {}
	virtual Transition process(const e1& evt);
	virtual Transition process(const eTerminate& evt);
};

struct STerminated : StateBase
{
	STerminated() : StateBase(true) {}
};
