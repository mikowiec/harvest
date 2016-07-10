
/*
 * $Author: peter $
 * $Date: 2000/10/29 23:17:29 $
 * $Log: gfx_test.cpp,v $
 * Revision 1.3  2000/10/29 23:17:29  peter
 * documenting and updating...
 *
 * Revision 1.2  2000/10/17 22:35:26  peter
 * events... restructuring..
 *
 * Revision 1.1  2000/10/12 15:47:30  peter
 * tester...
 *
 */

#include <string>
#include <iostream>
#include <vector>
#include <valarray>
#include <cstdio>

#include <GL/gl.h>
#include <GL/glu.h>


#include "compat.h"
#include "sequence.h"
#include "gfx.h"
#include "socket.h"
#include "nameservice.h"
#include "event.h"

typedef valarray<float> vect;

struct bat {
	float pos;
	float speed;
	bat() : pos(0.0), speed(0) { }
};


class GLMain {
	bool stop;
	vect ball;
	vect ballv;
	vector<bat> player;
	GLUquadricObj* ballobj;
	int npl;
public:
	GLMain() : stop(false), ball(0.0, 2), ballv(0.0, 2), npl(0) { 
		ballobj = gluNewQuadric();
		string s = "1 (0.0,0.0,0.01,0.0) (0.0,0.0)";
		set_state(s);
	}
	virtual ~GLMain() { }
	void set_state(const string& s) {
		const char *p = s.c_str();
		npl = atoi(p);
		cerr << "got state: " << npl << endl;
		p = strchr(p, ' ');
		int i;
		float x, y, dx, dy, pos, sp;
		player.resize(npl);
		sscanf(p, " %d (%f,%f,%f,%f)", &npl, &x, &y, &dx, &dy);
		p = strchr(p+1, ' ');
		ball[0] = x;
		ball[1] = y;
		ballv[0] = dx;
		ballv[1] = dy;
		for (i = 0; i < npl; i++) {
			sscanf(p, " (%f,%f)", &pos, &sp);
			p = strchr(p+1, ' ');
			player[i].pos = pos;
			player[i].speed = sp;
		}
		stop = false;
	}
	string get_state() {
		char buf[100];
		int i;
		snprintf(buf, 100, "state %d (%f,%f,%f,%f)", npl, ball[0], ball[1], ballv[0], ballv[1]);
		string s = string(buf);
		for (i = 0; i < npl; i++) {
			snprintf(buf, 100, " (%f,%f)", player[i].pos, player[i].speed);
			s += string(buf);
		}
		return s;
	}
	virtual void redraw() {
		int i;
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.2, 0.2, 0.2);
		glRectf(-0.8, -0.8, 0.8, 0.8);
		glColor3f(1.0, 1.0, 1.0);

		for (i = 0; i < 4; i++) {
			if (i < 2) {
				float x = (i == 0) ? -0.8 : 0.8;
				float y = (i < npl) ? player[i].pos : 0.0;
				float d = (i < npl) ? 0.1 : 0.8;
				glRectf(x - 0.01, y - d, x + 0.01, y + d);
			} else {
				float x = (i < npl) ? player[i].pos : 0.0;
				float y = (i == 2) ? -0.8 : 0.8;
				float d = (i < npl) ? 0.1 : 0.8;
				glRectf(x - d, y - 0.01, x + d, y + 0.01);
			}
		}

		glPushMatrix();
			glTranslatef(ball[0], ball[1], ball[2]);
			gluSphere(ballobj, 0.03, 5, 5);
		glPopMatrix();
	}
	virtual void resize(int w, int h) {
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
		glClearColor(0, 0, 0, 1.0);
	}
	virtual bool tick() {
		if (! stop) {
			ball += ballv;
			if (ball[0] <= -0.8) {
				if (npl >= 1) {
					if (ball[1] < player[0].pos - 0.1 || ball[1] > player[0].pos + 0.1)
					//	ballv[0] = -ballv[0];
						stop = true;
					else {
						ballv[0] = -ballv[0];
						ballv[1] -= (ball[1] - player[0].pos) / 10.0;
					}
				} else
					ballv[0] = -ballv[0];
			}
			if (ball[0] >=  0.8) {
				if (npl >= 2) {
					if (ball[1] < player[1].pos - 0.1 || ball[1] > player[1].pos + 0.1)
					//	ballv[0] = - ballv[0];
						stop = true;
					else {
						ballv[0] = - ballv[0];
						ballv[1] -= (ball[1] - player[1].pos) / 10.0;
					}
				} else
					ballv[0] = -ballv[0];
			}
			if (ball[1] <= -0.8) {
				if (npl >= 3) {
					if (ball[0] < player[2].pos - 0.1 || ball[0] > player[2].pos + 0.1)
					//	ballv[1] = - ballv[1];
						stop = true;
					else {
						ballv[1] = - ballv[1];
						ballv[0] -= (ball[0] - player[2].pos) / 10.0;
					}
				} else
					ballv[1] = -ballv[1];
			}
			if (ball[1] >=  0.8) {
				if (npl == 4) {
					if (ball[0] < player[3].pos - 0.1 || ball[0] > player[3].pos + 0.1)
					//	ballv[1] = - ballv[1];
						stop = true;
					else {
						ballv[1] = - ballv[1];
						ballv[0] -= (ball[0] - player[3].pos) / 10.0;
					}
				} else
					ballv[1] = -ballv[1];
			}
		}
		int i;
		for (i = 0; i < npl; i++) {
			player[i].pos += player[i].speed;
			if (player[i].pos >= 0.7) player[i].pos = 0.7;
			if (player[i].pos <= -0.7) player[i].pos = -0.7;
		}
		return true;
	}
	void move(int pl, int mv) {
		player[pl].speed = 0.05 * mv;
	}
	bool finished() {
		return stop;
	}
};

/*
class Net {
	NameData nd;
	Socket socket;
public:
	Net() : events(0) { }
	void init(string server, eventqueue* ev = 0) {
		events = ev;
		NameService::Instance()->Lookup(server);
		NameService::Instance()->Result(true, nd);
		socket.set_proto(Socket::UDP);
	}
	void handle_events() {
		char buf[500];
		int r;
		while ((r = socket.recv(buf, 500, false)) != -1) {
			buf[r] = 0;
			events->push(Event(buf));
		}
	}
	void send_event(Event e) {
		string s(e);
		socket.send(s.data(), s.size(), nd.addrs.front(), 4242);
	}
	void send(const string& s) {
		socket.send(s.data(), s.size(), nd.addrs.front(), 4242);
	}
	string recv() {
		char buf[500];
		int r = socket.recv(buf, 500);
		return string(buf, r);
	}
};
*/

void loop(int player, string server) {
	GLMain glm;
	Gfx gfx;
	int now, past;
	bool stop = false;
	bool quit = false;
	try {
		gfx->set_mode(1024, 768);
		glm.resize(1024, 768);
		while (! quit) {
			past = get_time();
			stop = false;
			while (! stop) {
				process_input();
				while (! events.bin.empty()) {
					BEvent ev = events.bin.front();
					switch(ev.key) {
					case Up:   glm.move(0, ev.pressed);
						   break;
					case Down: glm.move(0, ev.pressed);
						   break;
					case LowLevel::Escape:  quit = stop = true;
						      break;
					}
					events.bin.pop();
				}
				now = get_time();
				if (now - past > 10) {
					past = now;
					glm.tick();
				}
				msleep(5);
				if (glm.finished()) {
					msleep(500);
					stop = true;
				}
				glm.redraw();
				gfx->update_screen();
			}
		}
	} catch (string& s) {
		cerr << "Excpt: " << s << endl;
	} catch (...) {
		cerr << "Excpt" << endl;
	}
}

#ifdef WIN32

int APIENTRY
WinMain(HINSTANCE current, HINSTANCE, LPSTR argv, int cmdshow) {
    string serv;
    int player = 1;
    char* p = strchr(argv, ' ');
    if (p) {
	serv = string(argv, p - argv);
	player = *(p+1) - '0';
    } else
	serv = argv;
    loop(player, serv);

    return 0;
}

#else


int main(int argc, char **argv) {
    int player = 0;
    string serv = argc == 2 ? argv[1] : "";
    if (argc == 3)
	player = argv[2][0] - '0';
    loop(player, serv);
    return 0;
}


#endif

