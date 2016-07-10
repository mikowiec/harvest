
#include "renderer.h"
#include "main/types_ops.h"
#include "main/types_io.h"
#include "os/gl_helper.h"

#include "main.h"

namespace reaper {
namespace gui {

struct CamMove : public CamCalc {
	void update(Point& tgt, Vector& eye, float dx, float dy) {
		Vector at(-eye.x, 0, -eye.z);
		Vector right = cross(at, Vector(0,1,0));
		tgt += 0.1 * (at*dy - right*dx);
	}
};

struct CamRot : public CamCalc {
	void update(Point& tgt, Vector& eye, float dx, float dy) {
		Vector at(-eye.x, 0, -eye.z);
		Vector right = norm(cross(at, Vector(0,1,0)));
		eye = Matrix3x3(-0.5*dy, right) * Matrix3x3(-0.5*dx, Vector(0,1,0)) * eye;
	}
};

struct CamZoom : public CamCalc {
	void update(Point& tgt, Vector& eye, float dx, float dy) {
		float sc = 1.0 - (0.01 * dx);
		eye = eye * sc;
		tgt.y += 2 * dy;
	}
};

struct CamZoomZ : public CamCalc {
	void update(Point& tgt, Vector& eye, float dx, float dy) {
		float sc = 1.0 - (0.01 * dy);
		eye = eye * sc;
	}
};

GLArea::GLArea(GLFrame* w, Renderer* rr)
 : wxGLCanvas(w, -1, def_pos(), wxSize(1280, 768)), r(rr), frame(w), 
   cc_left(new CamMove()), cc_right(new CamRot()), cc_middle(new CamZoom()),
   cc_middle2(new CamZoomZ()), 
   cc_sel(0)
{ 
	restore();
}

void GLArea::restore()
{
	if (r->get_mode() == Renderer::Object) {
		cam_eye = Vector(0, 0, -20);
		cam_tgt = Point(0, 0, 0);
	} else {
		cam_eye = Vector(300, 300, 300);
		cam_tgt = Point(0, 500, 0);
	}
}

GLArea::~GLArea()
{
	delete cc_left;
	delete cc_right;
	delete cc_middle;
}

BEGIN_EVENT_TABLE(GLArea, wxGLCanvas)
    EVT_SIZE(GLArea::OnSize)
    EVT_PAINT(GLArea::OnPaint)
    EVT_ERASE_BACKGROUND(GLArea::OnEraseBackground)
    EVT_MOUSE_EVENTS(GLArea::OnMouse)
    EVT_KEY_DOWN(GLArea::OnKeyDown)
END_EVENT_TABLE()

void GLArea::OnKeyDown(wxKeyEvent& ev)
{
	printf("key %d\n", ev.GetKeyCode());
	if (ev.GetKeyCode() == WXK_SPACE) {
		frame->grab_obj_flip();
	}
}

void GLArea::OnPaint(wxPaintEvent& ev)
{
	static GLUquadricObj* glu = gluNewQuadric();
	wxPaintDC dc(this);

	SetCurrent();

	r->set_camera(cam_tgt + cam_eye, cam_tgt);
	r->render();

	SwapBuffers();
}

void GLArea::OnSize(wxSizeEvent& ev)
{
	int w, h;
	GetClientSize(&w, &h);
	glViewport(0, 0, w, h);
}

void GLArea::OnEraseBackground(wxEraseEvent& ev)
{
}


void GLArea::OnMouse(wxMouseEvent& ev)
{
	int x = ev.GetX(), y = ev.GetY();
	if (ev.ButtonDown()) {
		lx = x;
		ly = y;
	}
	if (ev.LeftDown())
		cc_sel = cc_left;
	if (ev.RightDown())
		cc_sel = cc_right;
	if (ev.MiddleDown())
		cc_sel = r->get_mode() == Renderer::Object ? cc_middle2 : cc_middle;
	if (ev.Dragging() && cc_sel) {
		float scale = ev.ShiftDown() ? 0.1 : 1;
		cc_sel->update(cam_tgt, cam_eye, scale * (x-lx), scale * (y-ly));
		lx = x;
		ly = y;
		if (r->get_mode() != Renderer::Object)
			frame->set_pos(cam_tgt);
		Refresh(false);
	}
}



GLFrame::GLFrame(wxFrame* p, GameIF* g)
 : wxFrame(p, -1, "foo", wxDefaultPosition, wxDefaultSize),
   game(g), id(-1)
{
	wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(new wxStaticText(this, -1, "Object:"),0,mid_align(),5);
	obj_id = new wxStaticText(this, -1, "");
	vs->Add(obj_id, 0, mid_align(), 5);
	vs->Add(grab = new wxCheckBox(this, ID_Grab, "Grab object"),0,mid_align(),5);
	hs->Add(vs);

	gl = new GLArea(this, game->renderer());
	hs->Add(gl, 1, wxEXPAND);
	

	SetAutoLayout(true);
	SetSizer(hs);
	hs->Fit(this);
	hs->SetSizeHints(this);
}	

void GLFrame::grab_obj_flip()
{
	grab->SetValue(!grab->GetValue());
	grab_obj();
}


void GLFrame::grab_obj()
{
	printf("grab_obj %d\n", grab->GetValue());
	if (grab->GetValue()) {
		id = game->get_obj_at_pos(gl->cam_tgt);
		if (id != -1)
			gl->cam_tgt = game->get_pos(id);
	} else {
		id = -1;
	}
	obj_id->SetLabel(write<int>(id).c_str());
}

void GLFrame::set_pos(Point p)
{
	printf("set pos %d\n", id);
	if (id == -1 || !grab->GetValue())
		id = game->get_obj_at_pos(p);
	obj_id->SetLabel(write<int>(id).c_str());
	if (!grab->GetValue())
		return;
	if (id != -1)
		game->set_pos(id, p);
	Refresh(false);
}

void GLFrame::refresh()
{
	if (gl) {
		gl->Refresh(false);
	}
}

void GLFrame::reinit()
{
	game->renderer()->reinit();
	Show();
	Raise();
	if (gl) {
		gl->restore();
		gl->Refresh(false);
	}
}

void GLFrame::OnClose()
{
	Hide();
}


BEGIN_EVENT_TABLE(GLFrame, wxFrame)
	EVT_CLOSE(GLFrame::OnClose)
	EVT_CHECKBOX(ID_Grab, GLFrame::grab_obj)
END_EVENT_TABLE()


}
}


