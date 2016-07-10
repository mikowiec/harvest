
#ifndef REAPER_TOOLS_GUI_RENDERER_H
#define REAPER_TOOLS_GUI_RENDERER_H

#include "wx/wx.h"
#include "wx/glcanvas.h"

#include "gameif.h"

namespace reaper {
namespace gui {


class CamCalc {
public:
	virtual void update(Point& tgt, Vector& eye, float dx, float dy) = 0;
};

class GLFrame;

class GLArea : public wxGLCanvas {
	friend class GLFrame;
	Renderer* r;
	GLFrame* frame;
	bool is_obj;
	std::string name;
	Vector cam_eye;
	Point cam_tgt;
	int lx, ly;
	CamCalc* cc_left;
	CamCalc* cc_right;
	CamCalc* cc_middle;
	CamCalc* cc_middle2;
	CamCalc* cc_sel;
public:
	GLArea(GLFrame* parent, Renderer*);

	void restore();

	void OnPaint(wxPaintEvent&);
	void OnSize(wxSizeEvent&);
	void OnEraseBackground(wxEraseEvent&);
	void OnMouse(wxMouseEvent&);
	virtual ~GLArea();
	void reinit(bool is_obj, string name);
	void OnKeyDown(wxKeyEvent&);

DECLARE_EVENT_TABLE()
};


class GLFrame : public wxFrame {
	GameIF* game;
 	GLArea* gl;
	wxStaticText* obj_id;
	wxCheckBox* grab;
	ObjectID id;
public:
	void set_pos(Point);
	void grab_obj_flip();
	void grab_obj();


	GLFrame(wxFrame* p, GameIF*);

	void refresh();
	void reinit();
	void OnClose();
	void OnPaint();

DECLARE_EVENT_TABLE()
};

}
}

#endif

