// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ScraperFrame.h"

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    ScraperFrame *myFrame = new ScraperFrame(wxT("Scraper"));
    myFrame->Show(true);

    return true;
}