#ifndef _SVCCONVERTER_SRC_MAIN_WINDOW_H_
#define _SVCCONVERTER_SRC_MAIN_WINDOW_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP

class MainWindow : public wxFrame {
public:
    MainWindow(const wxString &title,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize);
};

#endif // _SVCCONVERTER_SRC_MAIN_WINDOW_H_