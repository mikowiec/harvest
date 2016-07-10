
#include "os/compat.h"

#include <list>

#include "gameif.h"

#include "main/types_io.h"
#include "misc/sequence.h"
#include "misc/stlhelper.h"

#include "wx/wx.h"
#include "wx/glcanvas.h"

#include "renderer.h"
#include "main.h"
#include "gui.h"

namespace reaper {
namespace gui {

using namespace std;

class App : public wxApp {
public:
	App();

	bool OnInit();

};

class CfgFileList : public wxListCtrl
{
public:
	CfgFileList(wxWindow* p);
	void add_env(Env& env);
	void add_row(string l, string v);
	void fix_width();
};

CfgFileList::CfgFileList(wxWindow* p)
 : wxListCtrl(p, -1, wxDefaultPosition, wxSize(300,400), wxLC_REPORT)
{
	InsertColumn(0, "Variable");
	InsertColumn(1, "Value");
}

void CfgFileList::add_env(Env& env)
{
	Env::iterator c, e = env.end();
	for (c = env.begin(); c != e; ++c) {
		add_row(c->first, c->second);
	}
	fix_width();
}

void CfgFileList::add_row(string l, string v)
{
	InsertItem(0, l.c_str());
	SetItem(0, 1, v.c_str());
}

void CfgFileList::fix_width()
{
	SetColumnWidth(0, wxLIST_AUTOSIZE);
	SetColumnWidth(1, wxLIST_AUTOSIZE);
}

wxSizer* mk_labeled_data(wxWindow* p, string lbl, string val)
{
	wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(new wxStaticText(p, -1, lbl.c_str(), def_pos(), def_size(), wxALIGN_LEFT | wxEXPAND));
	hs->Add(10, 1, wxEXPAND);
	hs->Add(new wxStaticText(p, -1, val.c_str(), def_pos(), def_size(), wxALIGN_RIGHT | wxEXPAND));
	return hs;
}

DataWndImpl::DataWndImpl(GameIF* gm, wxWindow* p, string n)
 : game(gm), cfg(new CfgFileList(p)), name(n)
{ }

LevelWnd::LevelWnd(Main* m, string lvl)
 : wxFrame(m, -1, "Level"), DataWndImpl(m->game(), this, lvl),
   obj_lst_ch(0), new_obj(0)
{
	li = game->load_level(lvl);

	wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* vs = new wxBoxSizer(wxVERTICAL);

	vs->Add(mk_labeled_data(this, "Name: ", lvl), 0, top_align(), 5);

	vs->Add(new wxButton(this, ID_RndObj, "Set as current"), 0, bottom_align(), 5);
	vs->Add(1, 10, wxEXPAND);

	view_all = new wxCheckBox(this, ID_ViewAll, "View all");
	vs->Add(view_all, 0, mid_align(), 5);

	vs->Add(new wxButton(this, ID_AddOLst, "Add list"), 0, mid_align(), 5);

	vs->Add(1, 10, wxEXPAND);
//	vs->Add(mk_labeled_data(this, "Vertices: ", num_verts), 0, mid_align(), 5);
//	vs->Add(mk_labeled_data(this, "Triangles: ", num_tris), 0, mid_align(), 5);
	const wxString init_ch[1] = { "" };

	obj_lst_ch = new wxChoice(this, -1, def_pos(), def_size(), 0, init_ch);
	for (int i = 0; i < li.lists.size(); ++i) {
		obj_lst_ch->Append(li.lists[i].c_str());
	}
	if (!li.lists.empty()) {
		std::string def = li.lists.front();
		obj_lst_ch->SetStringSelection(def.c_str());
		game->renderer()->sel_objectgroup(def);
	}
	vs->Add(obj_lst_ch, 0, mid_align(), 5);


	vs->Add(1, 10, wxEXPAND);

	vs->Add(new wxButton(this, ID_DelObj, "Remove selected"), 0, mid_align(), 5);

	vs->Add(1, 10, wxEXPAND);
	vs->Add(new wxButton(this, ID_Save, "Save"), 0, mid_align(), 5);

	hs->Add(vs);
	cfg->add_env(li.env);
	hs->Add(cfg, 1, wxEXPAND);


	SetAutoLayout(true);
	SetSizer(hs);
	hs->Fit(this);
	hs->SetSizeHints(this);
}

void LevelWnd::activate_ol(wxCommandEvent& ev)
{
	game->renderer()->sel_objectgroup(obj_lst_ch->GetStringSelection().c_str());
	upd_render();
}

void LevelWnd::set_view_all(wxCommandEvent& ev)
{
	game->renderer()->set_mode(view_all->GetValue() ? Renderer::LevelAll : Renderer::Level);
	upd_render();
}

void LevelWnd::upd_render()
{
	wxCommandEvent wc(wxEVT_COMMAND_MENU_SELECTED, ID_UpdRndWind);
	GetParent()->ProcessEvent(wc);
}

void LevelWnd::add_obj_list()
{
	wxFileDialog fd(this, "Select objectgroup", "",
			"", "Reaper objectgroup|*", wxOPEN);
	if (fd.ShowModal() == wxID_OK) {
		wxString val = fd.GetFilename();
		obj_lst_ch->Append(val.c_str());
		ObjGroupID lid = game->load_objectgroup(val.c_str());
		li.lists.push_back(lid);
		game->renderer()->add_objectgroup(lid);
		obj_lst_ch->SetStringSelection(val.c_str());
		upd_render();
	}
}

void LevelWnd::save()
{
	misc::for_each(misc::seq(li.lists),
		misc::apply_to(game, &GameIF::save_group));
}

void LevelWnd::do_render()
{
	game->renderer()->set_cur_level(name);
	game->renderer()->set_mode(Renderer::Level);
	wxCommandEvent wc(wxEVT_COMMAND_MENU_SELECTED, ID_RndWind);
	GetParent()->ProcessEvent(wc);
}

BEGIN_EVENT_TABLE(LevelWnd, wxFrame)
	EVT_BUTTON(ID_RndObj, LevelWnd::do_render)
	EVT_BUTTON(ID_AddOLst, LevelWnd::add_obj_list)
	EVT_BUTTON(ID_Save, LevelWnd::save)
	EVT_CHECKBOX(ID_ViewAll, LevelWnd::set_view_all)
	EVT_CHOICE(-1, LevelWnd::activate_ol)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ObjectWnd, wxFrame)
	EVT_BUTTON(ID_RndObj, ObjectWnd::DoRender)
	EVT_BUTTON(ID_AddObj, ObjectWnd::add_obj)
END_EVENT_TABLE()

ObjectWnd::ObjectWnd(Main* m, string obj)
 : wxFrame(m, -1, obj.c_str()), DataWndImpl(m->game(), this, obj)
{
	wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* vs = new wxBoxSizer(wxVERTICAL);

	vs->Add(mk_labeled_data(this, "Name: ", obj), 0, top_align(), 5);
//	vs->Add(mk_labeled_data(this, "Vertices: ", num_verts), 0, mid_align(), 5);
//	vs->Add(mk_labeled_data(this, "Triangles: ", num_tris), 0, mid_align(), 5);
	vs->Add(1, 10, wxEXPAND);
	vs->Add(new wxButton(this, ID_RndObj, "Set as current"), 0, mid_align(), 5);
	vs->Add(new wxButton(this, ID_AddObj, "Add to level"), 0, bottom_align(), 5);

	hs->Add(vs);
	auto_ptr<Env> env(m->game()->get_cfg_file("objects", obj));
	cfg->add_env(*env);
	hs->Add(cfg, 1, wxEXPAND);

	SetAutoLayout(true);
	SetSizer(hs);
	hs->Fit(this);
	hs->SetSizeHints(this);
}

void ObjectWnd::add_obj()
{
	ObjectID id = game->mk_object(name.c_str());
	game->add_to_group(game->renderer()->get_current_group(), id);
	game->set_pos(id, game->renderer()->get_tgt_pos());

	wxCommandEvent wc(wxEVT_COMMAND_MENU_SELECTED, ID_UpdRndWind);
	GetParent()->ProcessEvent(wc);
}


void ObjectWnd::DoRender()
{
	game->renderer()->set_cur_object(name);
	game->renderer()->set_mode(Renderer::Object);
	wxCommandEvent wc(wxEVT_COMMAND_MENU_SELECTED, ID_RndWind);
	GetParent()->ProcessEvent(wc);
}

App::App()
{

}

bool App::OnInit()
{
	Main* m = new Main("Reaper editor", wxSize(800, 600));
	m->Show(true);
	SetTopWindow(m);
	return true;
}


}
}

IMPLEMENT_APP(reaper::gui::App)

