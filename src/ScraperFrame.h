#pragma once
#include "wx/wx.h"
#include <wx/protocol/http.h>
#include <wx/sstream.h>
#include "wx/notebook.h"

#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "ImageListPanel.h"

class ScraperFrame :
	public wxFrame
{
public:
	ScraperFrame(const wxString &title);
	void OnQuit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnExport(wxCommandEvent &event);

	//void Pop(wxCommandEvent &event);
	void HandleButton(wxCommandEvent &event);
	~ScraperFrame(void);
private:
	wxTextCtrl *tcInput;

	wxNotebook *notebook;
		wxPanel *linksPanel;
		wxTextCtrl *linksTextCtrl;
		wxButton *linksButton;
		ImageListPanel *imageListPanel;

	wxDECLARE_EVENT_TABLE();
	std::string Filter(std::string str, const std::string remove);
};

enum
{
	Quit = wxID_EXIT,
	About = wxID_ABOUT,
	Go,
	Export
};
