
#include "main.h"
#include "gui.h"
#include "renderer.h"

namespace reaper {
namespace gui {


Main::Main(const wxString& t, const wxSize& s)
 : wxFrame(0, -1, t), gm(get_game_interface()),
   file(0), wnd(0), help(0), gl(0)
{
	file = new wxMenu;
	file->Append(ID_OpenLevel, "Open &Level");
	file->Append(ID_OpenObject, "Open &Object");
	file->Append(ID_OpenAllObjects, "Open &Most Objects");
	file->AppendSeparator();
	file->Append(ID_Quit, "E&xit");

	wnd = new wxMenu;
	wnd->Append(ID_RndWind, "&OpenGL view");

	wxMenuBar* menubar = new wxMenuBar;
	menubar->Append(file, "&File");
	menubar->Append(wnd, "&Windows");



	SetMenuBar(menubar);

	CreateStatusBar();
	SetStatusText("life is pain");
}

BEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_MENU(ID_Quit,  Main::quit)
	EVT_MENU_RANGE(ID_FstFile, ID_LstFile, Main::open)
	EVT_MENU_RANGE(ID_FstWind, ID_LstWind, Main::open_window)
END_EVENT_TABLE()


class FileOpenDialog : public wxFileDialog {
public:
	FileOpenDialog(wxWindow* p, GameIF* g, string msg, string dir, string wild)
	 : wxFileDialog(p, msg.c_str(), "", "", (wild+"|All files|*.*").c_str(), wxOPEN)
	{ }
};

string choose(wxFrame* f, GameIF* gm, string msg, string dir, string wild) {
	FileOpenDialog open(f, gm, msg, dir, wild);
	if (open.ShowModal() == wxID_OK)
		return open.GetFilename().c_str();
	else
		return "";
}


void Main::open(wxCommandEvent& ev)
{
	switch (ev.GetId()) {
	case ID_OpenLevel: {
		string s = choose(this, gm, "Choose level", "levels", "Reaper level files|*.rl");
		if (! s.empty()) {
			LevelWnd* l = new LevelWnd(this, s);
			l->Show();
		}
	    } break;
	case ID_OpenObject: {
		string s = choose(this, gm, "Choose object", "objects", "Reaper object files|*");
		if (! s.empty()) {
			ObjectWnd* o = new ObjectWnd(this, s);
			o->Show();
		}
	    } break;
	case ID_OpenAllObjects: {
		const char* objs[] = { "ammobox", "base", "building1", "building2",
			"building3", "bush1", "cactus1", "cactus2", "fuelBarrel",
			"ground-container", "ground-turret", "ground-vehicle",
			"mothership", "ship2", "ship3", "ship4", "shipX",
			"tower", "tree1", "turret", "wall" };
		for (int i = 0; i < sizeof(objs)/sizeof(char*); ++i) {
			ObjectWnd* o = new ObjectWnd(this, objs[i]);
			o->Show();
		}
	    } break;
	}


}

void Main::quit(wxCommandEvent&)
{
	Close(true);
}

void Main::open_window(wxCommandEvent& ev)
{
	switch (ev.GetId()) {
	case ID_UpdRndWind:
		if (gl)
			gl->refresh();
		break;
	case ID_RndWind:
		if (gl) {
			gl->reinit();
		} else {
			gl = new GLFrame(this, gm);
			gl->Show();
		}
		break;
	default:
		;
	}
}

GameIF* Main::game() { return gm; }

}
}


