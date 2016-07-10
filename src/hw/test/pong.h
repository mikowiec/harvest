
/*
 * $Author: pstrand $
 * $Date: 2002/12/29 16:59:27 $
 * $Log: pong.h,v $
 * Revision 1.5  2002/12/29 16:59:27  pstrand
 * hw/ moved to os/
 *
 * Revision 1.4  2001/01/11 22:58:01  peter
 * *** empty log message ***
 *
 * Revision 1.3  2001/01/01 15:43:00  peter
 * mer testkod
 *
 * Revision 1.2  2000/11/24 17:32:35  peter
 * win32 fixar...
 *
 * Revision 1.1  2000/11/23 23:37:56  peter
 * pong!
 *
 *
 */

#include "os/compat.h"

#include <vector>
#include <string>

#include "world/world.h"
#include "main/types.h"
#include "os/gl.h"


struct bat {
	float pos;
	float speed;
	bat() : pos(0.0), speed(0) { }
};

class Pong_data;

enum Player_Cmd { Left, Right };

class Pong  {
	Pong_data* data;
	bool stop;
	bool check_ballx(int);
	bool check_bally(int);
public:
	Pong();
	~Pong();
	void init_state();
	std::string get_state();
	void redraw();
	void resize(int w, int h);
	void cmd(int ply, Player_Cmd c);
	bool finished();
	bool tick();
};

