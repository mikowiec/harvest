
#ifndef REAPER_TOOLS_GUI_GUI_H
#define REAPER_TOOLS_GUI_GUI_H

#include "wx/wx.h"

#include "gameif.h"

namespace reaper {
namespace gui {

class CfgFileList;


struct DataWndImpl {
public:
	GameIF* game;
	CfgFileList* cfg;
	string name;

	DataWndImpl(GameIF* gm, wxWindow* p, string n);
	
};

class LevelWnd : public wxFrame, private DataWndImpl {
	LevelInfo li;
	wxChoice* obj_lst_ch;
	wxStaticText* new_obj;
	wxCheckBox* view_all;

	void upd_render();
public:
	LevelWnd(Main* m, string);

	void activate_ol(wxCommandEvent&);
	void add_obj_list();
	void set_view_all(wxCommandEvent&);
	void do_render();

	void save();

DECLARE_EVENT_TABLE()
};

class ObjectWnd : public wxFrame, private DataWndImpl {
public:
	ObjectWnd(Main* m, string);
	void DoRender();

	void add_obj();

DECLARE_EVENT_TABLE()
};




}
}

#endif

