#pragma once
#include "Common.h"

struct eTerminate : EvtBase {};

struct e1 : EvtBase {};

struct e2 : EvtBase {};

struct e11 : EvtBase {};

struct e12 : EvtBase {};

struct e21 : EvtBase {};

struct e22 : EvtBase {};

struct SC11 : StateBase , IEventProcessor<e11>
{
	SC11() : StateBase(false) {}
	virtual State* process(const e11& evt);
};

struct SC12 : StateBase 
{
	SC12() : StateBase(true) {}
};

struct SC21 : StateBase, IEventProcessor<e21> 
{
	SC21() : StateBase(false) {}
	virtual State* process(const e21& evt);
};

struct SC22 : StateBase
{
	SC22() : StateBase(true) {}
};

struct SC1 : CompositeStateBase, IEventProcessor<e1> 
{
	SC1() : CompositeStateBase([]() {return new SC11();}) {}
	virtual State* process(const e1& evt);
};

struct SC2 : CompositeStateBase, IEventProcessor<e2>, IEventProcessor<eTerminate>
{
	SC2() : CompositeStateBase([]() {return new SC21();}) {}
	virtual State* process(const e2& evt);
	virtual State* process(const eTerminate& evt);
};

struct STerminated : StateBase
{
	STerminated() : StateBase(true) {}
};
