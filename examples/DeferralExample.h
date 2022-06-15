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
	Transition process(const click& click);
	Transition process(const buttonReleased& click);
};

struct Focusing : NonFinalState, IEventProcessor<focused>, IEventProcessor<click>
{
	Transition process(const focused& evt);
	Transition process(const click& evt);
};

struct Idle : NonFinalState, IEventProcessor<focus>
{
	Transition process(const focus& evt);
};

struct Browsing : NonFinalState, IEventProcessor<shoot>, IEventProcessor<displayImage>
{
	Transition process(const shoot& evt);
	Transition process(const displayImage& evt);
};

struct Shooting : CompositeStateBase, IEventProcessor<browse>
{
	Shooting() : CompositeStateBase([]() {return std::make_unique<Idle>(); }) {}
	Transition process(const browse& evt);
};
