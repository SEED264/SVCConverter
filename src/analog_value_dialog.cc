#include "analog_value_dialog.h"

AnalogValueDialog::AnalogValueDialog(wxWindow *parent, const wxPoint &pos, const wxSize &size)
        : wxDialog(nullptr, wxID_ANY, "Slider value check", pos, size) {
    // Init GUI parts
    InitGUI();
}

void AnalogValueDialog::InitGUI() {
    // Set icon
    SetIcon(wxICON(APP_ICON));

    // Layout boxes
    auto *box_dialog = new wxBoxSizer(wxHORIZONTAL);

    slider_panel_ = new AnalogValuePanel(this, wxT("Slider"));
    dial_panel_ = new AnalogValuePanel(this, "Dial");

    int mergin_panel = 4;
    box_dialog->Add(slider_panel_, 1, wxALL, mergin_panel);
    box_dialog->Add(dial_panel_,   1, wxALL, mergin_panel);

    box_dialog->SetSizeHints(this);
    SetSizer(box_dialog);

    // Set background color to default
    auto default_bg_color = wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK);
    SetBackgroundColour(default_bg_color);
}