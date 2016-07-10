
#ifndef REAPER_TOOLS_GUI_MAIN_H
#define REAPER_TOOLS_GUI_MAIN_H

#include "wx/wx.h"

#include "gameif.h"

namespace reaper {
namespace gui {

inline wxSize def_size() { return wxDefaultSize; }
inline wxPoint def_pos() { return wxDefaultPosition; }
inline int mid_align() { return wxLEFT | wxRIGHT | wxEXPAND; }
inline int top_align() { return mid_align() | wxTOP | wxALIGN_TOP; }
inline int bottom_align() { return mid_align() | wxBOTTOM | wxALIGN_BOTTOM; }

wxSizer* mk_labeled_data(wxWindow* p, string lbl, string val);


enum { ID_Quit = 1,
       ID_FstFile, ID_OpenLevel, ID_OpenTerrain, ID_OpenObject, ID_OpenAllObjects, 
       ID_StartServer, ID_LstFile,
       ID_FstWind, ID_RndWind, ID_UpdRndWind, ID_LstWind,
       ID_RndObj, ID_RndLvl, ID_AddOLst, ID_ViewAll, ID_AddObj,
       ID_DelObj, ID_Save, ID_Grab };

class GLFrame;

class Main : public wxFrame {
	GameIF* gm;
	wxMenu* file;
	wxMenu* wnd;
	wxMenu* help;
	GLFrame* gl;
public:
	Main(const wxString& title, const wxSize& size);

	void quit(wxCommandEvent& ev);

	void open(wxCommandEvent& ev);

	void open_window(wxCommandEvent& ev);


	GameIF* game();

DECLARE_EVENT_TABLE()
};

}
}

#endif

