#include "main_window.h"
#include "analog_value_dialog.h"
#include "config_dialog.h"

MainWindow::MainWindow(const wxString &title,
                       const wxPoint &pos,
                       const wxSize &size)
        : wxFrame(nullptr, wxID_ANY, title, pos, size) {
    SetIcon(wxICON(APP_ICON));
    // Init parts
    auto *panel = new wxPanel(this, wxID_ANY);
    auto *button_quit = new wxButton(panel, wxID_EXIT, "&Quit");
    auto *label_show_dialog = new wxStaticText(panel, wxID_ANY, "Value check dialog : ");
    auto *button_show_dialog = new wxButton(
        panel, ID_BUTTON_SHOW_ANALOG_VALUE_DIALOG, "&Show");

    // Init boxsizer
    auto *sizer_window = new wxBoxSizer(wxVERTICAL);
    auto *sizer_show_dialog = new wxBoxSizer(wxHORIZONTAL);

    // Layout
    sizer_show_dialog->Add(label_show_dialog, 0);
    sizer_show_dialog->AddStretchSpacer();
    sizer_show_dialog->Add(button_show_dialog, 0);

    sizer_window->Add(sizer_show_dialog, 0, wxEXPAND);
    sizer_window->AddStretchSpacer();
    sizer_window->Add(button_quit, 0, wxALIGN_RIGHT);

    panel->SetSizer(sizer_window);
    sizer_window->SetSizeHints(this);
    SetSize(GetSize()*1.2);

    // Bind events
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this](wxCommandEvent&){ Close(); }, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::OnShowAnalogValueDialog, this,
         ID_BUTTON_SHOW_ANALOG_VALUE_DIALOG);
}

void MainWindow::OnShowAnalogValueDialog(wxCommandEvent &event) {
    RawInputDeviceManager manager;
    auto device = manager.GetDeviceLists(1, 5)[1];
    KnobBindingDialog dialog(nullptr, device);
    dialog.ShowModal();
}