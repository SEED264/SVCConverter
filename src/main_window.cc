#include "main_window.h"
#include "slider_value_dialog.h"

MainWindow::MainWindow(const wxString &title,
                       const wxPoint &pos,
                       const wxSize &size)
        : wxFrame(nullptr, wxID_ANY, title, pos, size) {
    SetIcon(wxICON(APP_ICON));
}