
#ifndef REAPER_TOOLS_GUI_GAMEIF_H
#define REAPER_TOOLS_GUI_GAMEIF_H

#include "main/types.h"

#include <string>
#include <map>
#include <memory>
#include <vector>


namespace reaper {
namespace gui {

using std::string;
using std::map;
using std::auto_ptr;

typedef map<string,string> Env;

typedef int ObjectID;
typedef std::string ObjGroupID;
typedef std::vector<ObjectID> ObjectGroup;

struct ObjectInfo {
	ObjectID id;
	std::string name;
	Env info;
	Env cdata;
};

struct LevelInfo {
	std::string name;
	std::vector<ObjGroupID> lists;
	Env env;
};

// FIXME, the distinction below is somewhat arbitrary, divide properly or merge...


class Renderer {
public:
	virtual void reinit() = 0;
	virtual void render() = 0;

	virtual void set_camera(const Point& pos, const Point& tgt) = 0;

	virtual Point get_cam_pos() = 0;
	virtual Point get_tgt_pos() = 0;

	enum Mode { NotInit, Object, Level, LevelAll };
	virtual void set_mode(Mode) = 0;
	virtual Mode get_mode() = 0;

	virtual void set_cur_object(string) = 0;
	virtual string get_cur_object() = 0;

	virtual void set_cur_level(string) = 0;
	virtual string get_cur_level() = 0;

	virtual void add_objectgroup(ObjGroupID) = 0;
	virtual void sel_objectgroup(ObjGroupID) = 0;
	virtual ObjGroupID get_current_group() = 0;
	virtual void clear_objectgroups() = 0;

	virtual float get_altitude(Point p) = 0;


	virtual ~Renderer() { }
};

class GameIF {
public:

	virtual auto_ptr<Env> get_cfg_file(string, string) = 0;

	virtual Renderer* renderer() = 0;

	virtual void set_pos(ObjectID, Point) = 0;
	virtual Point get_pos(ObjectID) = 0;

	virtual ObjectID get_obj_at_pos(Point p) = 0;

	virtual LevelInfo load_level(string) = 0;
	virtual ObjGroupID load_objectgroup(string) = 0;

	virtual ObjectID mk_object(string) = 0;

	virtual ObjectInfo get_object(ObjectID) = 0;

	virtual void add_to_group(ObjGroupID, ObjectID) = 0;
	virtual void del_from_group(ObjGroupID, ObjectID) = 0;
	virtual void save_group(ObjGroupID) = 0;

	virtual ObjectGroup get_group(ObjGroupID) = 0;

	virtual string get_errmsg() = 0;

	virtual ~GameIF() { }
};

GameIF* get_game_interface();

}

}

#endif
