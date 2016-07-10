#ifndef REAPER_RES_RES_H
#define REAPER_RES_RES_H

#include <string>
#include <iosfwd>
#include <fstream>
#include "main/exceptions.h"

namespace reaper {
namespace res {


/// Resource category.
enum ResourceClass {
	File,		///< File (only for testing/debugging)
	Object,		///< Object.
	ObjectData,	///< Object data.
	Mesh,		///< Mesh data.
	Texture,	///< Texture (picture).
        Level,          ///< Level description.
        Terrain,        ///< Terrain mesh.
        Sound,          ///< Wave sound
        Music,          ///< Music (mp3)
        Video,          ///< Video (mpeg)
	GameState,      ///< Gamestate (saved games, state-dumps, ...)
	Config,		///< Generic config-file
	Playback,	///< Playback (event) files.
	Cache,          ///< Temporary data
	Screenshot,     ///< Screenshots (raw pictures)
	Scenario,	///< Object data and dialogs for the scenario
	Shader,         ///< Shader specification
	Plugin,		///< Plugin
	AI		///< AI data
};

std::string to_string(ResourceClass);

std::string gen_name();

typedef std::istream std_istream;
typedef std::ostream std_ostream;

enum res_flags {
	normal = 0,
	throw_on_error = 1
};

class res_stream : public std_istream
{
	std::streambuf* sb;
	std::streambuf* sb2;

	struct ResID {
		ResourceClass rc;
		std::string id;
		res_flags flags;
		ResID(ResourceClass r, const std::string& i, res_flags flg);
	};
	ResID res;
	void res_init(const ResID&);

	// copy/assign not supported (expensive)! Use clone.
	res_stream(const res_stream&);
	res_stream& operator=(const res_stream&);
public:
	res_stream(const ResID&);

	/** Load resource. 
	 *  \param rc Resource class
	 *  \param id Resource identifier
	 */
	res_stream(ResourceClass rc, const std::string& id, res_flags = normal);
	~res_stream();
	const std::string id() const;

	ResID clone() const;

	void close();
};


class res_out_stream : public std_ostream
{
	std::streambuf* sb;
public:
	res_out_stream(ResourceClass, const std::string& id, bool text_mode = false);
	~res_out_stream();
};

class resource_not_found
 : public error_base
{
	std::string msg;
public:
	resource_not_found(const std::string&);
	const char* what() const;
};

void add_datapath(const std::string&);
void add_pluginpath(const std::string&);

std::string find_plugin(const std::string& id, const std::string& alt_ext = "");

bool is_newer(ResourceClass rc, const std::string& id1, const std::string& id2);

bool is_newer(ResourceClass rc1, const std::string& id1,
	      ResourceClass rc2, const std::string& id2);


std::string sanity_check();

}
}

#endif

