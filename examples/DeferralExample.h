#include "Common.h"

struct focus : EvtBase {};
struct focused : EvtBase {};
struct browse : EvtBase {};
struct shoot : EvtBase {};
struct click : EvtBase {};
struct buttonReleased : EvtBase {};

struct displayImage : EvtBase
{
	std::string m_image;
	displayImage(std::string image) : m_image(image){ }
};

struct Focused : NonFinalState, IEventProcessor<click>, IEventProcessor<buttonReleased>
{
	State* process(const click& click);
	State* process(const buttonReleased& click);
};

struct Focusing : NonFinalState, IEventProcessor<focused>, IEventProcessor<click>
{
	State* process(const focused& evt);
	State* process(const click& evt);
};

struct Idle : NonFinalState, IEventProcessor<focus>
{
	State* process(const focus& evt);
};

struct Browsing : NonFinalState, IEventProcessor<shoot>, IEventProcessor<displayImage>
{
	State* process(const shoot& evt);
	State* process(const displayImage& evt);
};

struct Shooting : CompositeStateBase, IEventProcessor<browse>
{
	Shooting() : CompositeStateBase([]() {return new Idle(); }) {}
	State* process(const browse& evt);
};
