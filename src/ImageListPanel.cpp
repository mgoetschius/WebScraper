#include "ImageListPanel.h"

enum
{
	Download = 999,
	Check = 1000
};

BEGIN_EVENT_TABLE(ImageListPanel, wxPanel)
	EVT_CHECKBOX(Check, ImageListPanel::OnCheck)
	EVT_BUTTON(Download, ImageListPanel::OnButton)
END_EVENT_TABLE()


ImageListPanel::ImageListPanel(wxWindow *parent) : wxScrolledWindow(parent)
{
	mainBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainBox);
}

void ImageListPanel::Update(const std::vector<std::string> images, std::string URL)
{
	DestroyChildren();
	boxes.clear();
	imgs.clear();
	imgs = images;
	url = URL;
	wxBoxSizer *hBox1 = new wxBoxSizer(wxHORIZONTAL);
	if(images.size() > 0)
	{
		mainCheckBox = new wxCheckBox(this, Check, "  Check All");
		hBox1->Add(mainCheckBox, 0, wxALL, 10);
	}
	else
	{
		wxStaticText *st = new wxStaticText(this, wxID_ANY, wxT("No Images"));
		hBox1->Add(st);
	}

	mainBox->Add(hBox1);
	
	int j = 0;
	for(unsigned int i = Check + 1; i < (Check + images.size() + 1); i++)
	{
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		wxCheckBox *cb = new wxCheckBox(this, i, "  " + imgs[j++]);
		boxes.push_back(cb);
		sizer->Add(cb, 0, wxLEFT, 10);
		mainBox->Add(sizer);
	}
	if(boxes.size() > 0)
	{
		downloadButton = new wxButton(this, Download, "Download");
		wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
		hbox2->Add(downloadButton, 0, wxTOP | wxLEFT, 10);
		mainBox->Add(hbox2);
	}

	this->FitInside();
	this->SetScrollRate(5,5);
	this->Layout();
}

void ImageListPanel::OnCheck(wxCommandEvent &WXUNUSED(event))
{
	if(mainCheckBox->GetValue())
	{
		for(wxCheckBox *cb : boxes)
		{
			cb->SetValue(true);
		}
	}
	else
	{
		for(wxCheckBox *cb : boxes)
		{
			cb->SetValue(false);
		}
	}
}

void ImageListPanel::OnButton(wxCommandEvent &WXUNUSED(event))
{
	wxInitAllImageHandlers();
	if(imgs.size() != boxes.size())
	{
		wxMessageBox("An unexpected error has occurred");
		return;
	}
	wxString savePath = "";
	wxDirDialog *dirDialog = new wxDirDialog(this, "Choose Directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if(dirDialog->ShowModal() == wxID_OK)
	{
		savePath = dirDialog->GetPath();
	}
	dirDialog->Destroy();
	
	for(unsigned int i = 0; i < boxes.size(); i++)
	{
		if(boxes[i]->GetValue())
		{
			wxString str;
			if(imgs[i].substr(0,2) == "//")
			{
				str = "http:" + imgs[i];
			}
			else if(imgs[i].substr(0,7) == "http://")
				str = imgs[i];
			else
				str = "http://" + url + "/" +imgs[i];

			wxURL url(str);
		
			if(url.GetError() == wxURL_NOERR)
			{
				wxInputStream *in = url.GetInputStream();
				if(in && in->IsOk())
				{
					unsigned char buffer[8192];
					size_t count = -1;
					wxMemoryBuffer memoryBuffer;

					while(!in->Eof() && count != 0)
					{
						in->Read(buffer, 8192);
						count = in->LastRead();
						if(count > 0)
							memoryBuffer.AppendData(buffer, count);
					}
					wxMemoryInputStream stream(memoryBuffer.GetData(), memoryBuffer.GetDataLen());
					wxImage image;
					image.LoadFile(stream, wxBITMAP_TYPE_ANY);
					if(image.Ok())
					{
						wxString filename = savePath + "\\" + imgs[i].substr(imgs[i].find_last_of("/")+1);
						if(!image.SaveFile(filename, wxBITMAP_TYPE_PNG))
							wxMessageBox("Error saving file:\n" + filename);
					}
					else
						wxLogMessage("No good image found");
				}
				else
					wxMessageBox("Bad Input Stream");
				delete in;
			}
			else
				wxMessageBox("Unable to connect");
		}
		
	}
}


ImageListPanel::~ImageListPanel(void)
{
}
