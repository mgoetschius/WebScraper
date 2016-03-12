#include "ScraperFrame.h"

void test_regex_search(const std::string& input);

wxBEGIN_EVENT_TABLE(ScraperFrame, wxFrame)
	EVT_MENU(Quit, ScraperFrame::OnQuit)
	EVT_MENU(About, ScraperFrame::OnAbout)
wxEND_EVENT_TABLE()

ScraperFrame::ScraperFrame(const wxString &title)
	: wxFrame(NULL, wxID_ANY, title)
{
	SetSize(640, 480);

	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(Quit, "E&xit", "Quit program");
	wxMenu *aboutMenu = new wxMenu;
	aboutMenu->Append(About, "About", "About Scraper");

	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "File");
	menuBar->Append(aboutMenu, "About");

	SetMenuBar(menuBar);

	CreateStatusBar(2);
    SetStatusText("Scraper");

	wxPanel *panel = new wxPanel(this, -1);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *st1 =  new wxStaticText(panel, wxID_ANY, wxT("URL"));

    hbox1->Add(st1, 0, wxRIGHT, 8);
    tcInput = new wxTextCtrl(panel, wxID_ANY, wxT("www.example.com"), wxDefaultPosition, wxSize(400, -1));
    hbox1->Add(tcInput, 2);
	wxButton *goButton = new wxButton(panel, Go, "Go");
    goButton->SetFocus();
    Connect(Go, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScraperFrame::HandleButton));
    hbox1->Add(goButton, 0, wxLEFT, 10);
    vbox->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

	vbox->Add(-1, 15);

	wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);

	notebook = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxNB_MULTILINE);
		linksPanel = new wxPanel(notebook, wxID_ANY);
		wxBoxSizer *linksBox = new wxBoxSizer(wxVERTICAL);
		linksTextCtrl = new wxTextCtrl(linksPanel, wxID_ANY, wxT(""), wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);
		linksButton = new wxButton(linksPanel, Export, "Export");
		Connect(Export, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScraperFrame::OnExport));
		linksBox->Add(linksTextCtrl, 1, wxEXPAND);
		linksBox->Add(linksButton, 0, wxRIGHT | wxBOTTOM |wxTOP | wxALIGN_RIGHT, 10);
		linksPanel->SetSizer(linksBox);
		notebook->AddPage(linksPanel, "Links");
		
		imageListPanel = new ImageListPanel(notebook);

		notebook->AddPage(imageListPanel, "Images");

		

	hbox2->Add(notebook, 1, wxEXPAND);

	vbox->Add(hbox2, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

  

	panel->SetSizer(vbox);
}

void ScraperFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
	Close(true);
}

void ScraperFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
{
	wxMessageBox("This application extracts stuff from a web page");
}

void ScraperFrame::HandleButton(wxCommandEvent &WXUNUSED(event))
{
	linksTextCtrl->Clear();
	wxString result;
	wxString urlString;
	wxString pathString;
	if(tcInput->GetValue() != "")
	{
		wxString str = tcInput->GetValue();

		if(str.GetChar(str.length()-1) != '/')
		{
			str = str + "/";
		}		
		
		if(str.substr(0,7) == "http://")
		{
			str = str.substr(7, str.length());
		}
		
		size_t loc = str.find('/');

		if(loc == str.length()-1)
		{
			urlString = str.substr(0, str.length()-1);
			pathString = "/";
		}
		else
		{
			urlString = str.substr(0, loc);
			pathString = str.substr(loc, str.length());
			pathString.erase(pathString.length()-1, pathString.length());
		}
		
		wxHTTP url;
		url.Connect(urlString);
		wxInputStream *stream = url.GetInputStream(pathString);
		if(url.GetError() == wxPROTO_NOERR)
		{
			wxStringOutputStream outStream(&result);
			stream->Read(outStream);
		}
		else
			wxMessageBox("error");

		wxDELETE(stream);
		url.Close();

		std::string s = std::string(result.mb_str());

		std::regex reg("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"", std::regex::icase);

		std::sregex_iterator it(s.begin(), s.end(), reg);
		std::sregex_iterator itEnd;

		while(it != itEnd)
		{
			linksTextCtrl->AppendText(Filter(it->str(), "<a href="));
			linksTextCtrl->AppendText("\n");
			++it;
		}


		// search for images ********************************
		std::vector<std::string> imgs;
		const std::string imgstr = std::string(result.mb_str());
		std::string::const_iterator start;

		std::match_results<std::string::const_iterator> imgres;
		start = imgstr.begin();


		std::regex imgreg("<img(.*?)>");
		while(std::regex_search(start, imgstr.end(), imgres, imgreg))
		{
			const std::string s1 = imgres[1];
			std::match_results<std::string::const_iterator> srcres;
			std::string::const_iterator srcStart = s1.begin();
			std::regex srcreg("src=\"(.*?)\"");
			std::regex_search(srcStart, s1.end(), srcres, srcreg);
			std::string s2 = srcres[1];
			imgs.push_back(s2);
			start = imgres[0].second;
		}

		imageListPanel->Update(imgs, std::string(urlString.mb_str()));

	} // end !tcInput.empty()
	else
		wxMessageBox("No Input");
}



std::string ScraperFrame::Filter(std::string str, const std::string remove)
{
	std::string::size_type i = str.find(remove);
	if(i != std::string::npos)
		str.erase(i, remove.length());

	str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());

	return str;
}

void ScraperFrame::OnExport(wxCommandEvent &WXUNUSED(event))
{
	wxString path = "";
	wxFileDialog *saveDialog = new wxFileDialog(this, "Export", wxEmptyString, wxEmptyString, "Text files(*.txt) | *.txt",
												wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
	{
		path = saveDialog->GetPath();
		
		std::ofstream file;
		file.open(path.mb_str());
		if(file.is_open())
		{
			file << tcInput->GetValue();
			file << "\n\n";
			file << linksTextCtrl->GetValue();
		}
		else
			wxMessageBox("Error writing to file");
		file.close();

	}
 
	// Clean up after ourselves
	saveDialog->Destroy();
}

ScraperFrame::~ScraperFrame(void)
{
}
