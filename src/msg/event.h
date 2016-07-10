#ifndef REAPER_HW_EVENT_H
#define REAPER_HW_EVENT_H

#include <map>
#include <vector>

#include "os/exceptions.h"

namespace reaper {
namespace os {

namespace gfx { class Gfx; }

/** Event handling.
    Events are retrieved from various input sources, translated
    to Event objects, and queued. A mapping from between event
    may be provided and is installed with Events::set_mapping.
    Events are identified by an identifier, and have a value in
    the interval [-1.0 .. 1.0].
*/

namespace event
{

namespace id {

/** Event identifier.
    Key codes 0 .. 0xFF are ascii,
    others are special keys.
    %Unknown is filtered out.
*/
enum EventID
{
	Key       = 0x000,
	Ext       = 0x100, Down, Up, Left, Right, Enter, Escape, Home, End,
			   PageUp, PageDown, Insert, Delete, Backspace, 
			   F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Btn0	  = 0x200, Btn1, Btn2, Btn3, Btn4, Btn5, Btn6, 
	Axis0	  = 0x400, Axis1, Axis2, Axis3, Axis4, Axis5, Axis6, 
	Unknown   = 0x800
};
EventID mk(int i);

}

/// Timestamp when event should apply.
typedef long Time;


/** Receiver for event.
 *  System is not routed through network.
 */
enum StdReceiver { System = 0, LocalPlayer = 1, Player = 2 };
typedef int Receiver;

typedef int PlayerID;

struct Fixedpt {
	int v;

	Fixedpt() : v(0) { }
	Fixedpt(double f) : v(static_cast<int>(f * 1000)) { }
	Fixedpt(int i) : v(i) { }
	operator double() const { return static_cast<float>(v) / 1000.0; }

	bool operator==(const Fixedpt& o) const { return v == o.v; }
};

/** General event structure. */
class Event {
public:
	Event();
	explicit Event(id::EventID id, bool press);
	explicit Event(int btn, bool press);
	explicit Event(int axis, float val);

	id::EventID id;       ///< Key/button/axis identifier.
	Fixedpt val;	      ///< Event value.
	Time time;	      ///< Timestamp for application.

	Receiver recv;        ///< Event reciever.

	bool is_key() const  { return id < id::Btn0; }
	bool is_btn() const  { return id::Btn0 <= id && id < id::Axis0; }
	bool is_axis() const { return id::Axis0 <= id; }
	bool is_pressed() const { return val.v > 100; }
};

inline
bool operator==(const Event& e1, const Event& e2)
{
	return e1.id == e2.id
	    && e1.val == e2.val
	    && e1.time == e2.time
	    && e1.recv == e2.recv;
}

inline
bool operator!=(const Event& e1, const Event& e2)
{
	return !(e1 == e2);
}

inline
bool operator<(const Event& e1, const Event& e2) {
	return e1.time < e2.time;
}

inline
bool operator>(const Event& e1, const Event& e2) {
	return e1.time > e2.time;
}

std::ostream& operator<<(std::ostream& os, const Event& e);
std::istream& operator>>(std::istream& is, Event& e);


/** Event mapping.
    Use this to map retrieved events, 
    filter out unneeded events and decide whether
    an event should be sent via the server.
 */
class Mapping {
public:
	virtual ~Mapping();

	virtual Event operator()(const Event& ev) = 0;
	virtual bool is_repeat(const Event&) { return false; }
};


/** Event producer. */
class EventSource
{
public:
	virtual ~EventSource();

	/** Retrieves an event.
	  * \return true if an event was found.
	  */
	virtual bool poll(Event&) = 0;
};

class EventFilter : public EventSource
{
public:
	virtual void set_source(EventSource*) = 0;
};

EventFilter* make_savedevice();
EventFilter* make_playbackdevice(const std::string& name);


/** Event consumer. */
class EventSink
{
public:
	virtual ~EventSink();

	/** Send an event
	  * \return true if succesful.
	  */
	virtual bool send(const Event& ev) = 0;
};

class EventQueue
 : public EventSource, public EventSink
{ };


class EventSystem;

class EventDispatcher;


typedef std::map<id::EventID, float> EventTable;

/** Event proxy.
 *  Provides access to current input positions and the event queue.
 *  This is just a temporary hack... 
 */

class EventProxy
{
	EventDispatcher* impl;
	EventTable ev_table;
	PlayerID listener_id;

	friend class EventSystem;
	EventProxy(EventDispatcher*, PlayerID id);

	std::vector<float> state;

	void poll();
public:
	~EventProxy();

	/** Get keystate. */
	bool key(id::EventID k);

	/** Get ascii-keystate. */
	bool key(char k);

	/** Get buttonstate. */
	bool button(int b);

	/** Get axisstate. */
	float axis(int a);

	/** Get change-state of axis */
	bool axis_changed(int a);

	/** Get event.
	    Retrieve an event, returns false if no events are available.
	    Will block and wait for an event to happen if block is true.
	*/
	bool get_event(Event&, bool block = false);
};

/** Event handling system.
 *  Initializes adn configures eventqueues, inputmappings
 *  and possibly network bindings.
 */

class EventSystem
{
	static EventDispatcher* impl;
public:
	EventSystem(os::gfx::Gfx&);
	~EventSystem();

	void enable();
	void disable();
	void reconfigure();

	/** Get an eventsystem handle.
	  * \param id Identifier for listener.
	  * \return EventProxy reference used to retrieve events and/or check status.
	  * \throw busy_id, invalid_id
	  */
	static EventProxy get_ref(PlayerID id);
	static EventProxy* create_ref(PlayerID id);

	/** Set inputmapping.
	 *  Changes eventmaps and discards all queued events.
	 */
	void set_mapping(Mapping*);

	/** Set inputmapping.
	 *  Reads map from config file.
	 *  Changes eventmaps and discards all queued events.
	 */
	void set_mapping(const std::string& id);

	void add_filter(EventFilter*);
};


/** \example hw/test/testevent.cpp
 *  Event usage example.
 */

/** \example hw/test/test_event_serv.cpp
 *  Event example, networking.
 */

}
}
}

#endif



