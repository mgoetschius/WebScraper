#pragma once
#include <wx/wx.h>
#include <wx/protocol/http.h>
#include <wx/url.h>
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>

#include <vector>

class ImageListPanel :
	public wxScrolledWindow
{
public:
	ImageListPanel(wxWindow *parent);
	void Update(const std::vector<std::string> images, std::string URL);
	~ImageListPanel(void);
private:
	wxBoxSizer *mainBox;
	wxCheckBox *mainCheckBox;
	wxButton *downloadButton;
	std::vector<wxCheckBox*> boxes;
	std::vector<std::string> imgs;
	std::string url;
	void OnCheck(wxCommandEvent &event);
	void OnButton(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};

