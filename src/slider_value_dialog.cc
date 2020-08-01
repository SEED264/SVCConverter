#include "slider_value_dialog.h"

SliderValueDialog::SliderValueDialog(wxWindow *parent, const wxPoint &pos, const wxSize &size)
        : wxDialog(nullptr, wxID_ANY, "Slider value check", pos, size) {
    // Init GUI parts
    InitGUI();
}

void SliderValueDialog::InitGUI() {
    // Set icon
    SetIcon(wxICON(APP_ICON));

    // Layout boxes
    auto *box_dialog = new wxBoxSizer(wxHORIZONTAL);

    auto *box_slider = new wxStaticBoxSizer(wxVERTICAL, this, "Slider");
    auto *box_dial   = new wxStaticBoxSizer(wxVERTICAL, this, "Dial");

    // Init text
    int minimum_space = 40;
    current_slider_  = new LabledText(this, "Current value",    "N/A", minimum_space);
    min_slider_      = new LabledText(this, "Min Value",        "N/A", minimum_space);
    max_slider_      = new LabledText(this, "Max value",        "N/A", minimum_space);
    dist_slider_     = new LabledText(this, "Current Distance", "N/A", minimum_space);
    max_dist_slider_ = new LabledText(this, "Max distance",     "N/A", minimum_space);

    current_dial_    = new LabledText(this, "Current value",    "N/A", minimum_space);
    min_dial_        = new LabledText(this, "Min value",        "N/A", minimum_space);
    max_dial_        = new LabledText(this, "Max value",        "N/A", minimum_space);
    dist_dial_       = new LabledText(this, "Current Distance", "N/A", minimum_space);
    max_dist_dial_   = new LabledText(this, "Max distance",     "N/A", minimum_space);

    // Layout to box
    int mergin_text = 8;
    box_slider->Add(current_slider_,  1, wxEXPAND | wxALL, mergin_text);
    box_slider->Add(min_slider_,      1, wxEXPAND | wxALL, mergin_text);
    box_slider->Add(max_slider_,      1, wxEXPAND | wxALL, mergin_text);
    box_slider->Add(dist_slider_,     1, wxEXPAND | wxALL, mergin_text);
    box_slider->Add(max_dist_slider_, 1, wxEXPAND | wxALL, mergin_text);

    box_dial->Add(current_dial_,  1, wxEXPAND | wxALL, mergin_text);
    box_dial->Add(min_dial_,      1, wxEXPAND | wxALL, mergin_text);
    box_dial->Add(max_dial_,      1, wxEXPAND | wxALL, mergin_text);
    box_dial->Add(dist_dial_,     1, wxEXPAND | wxALL, mergin_text);
    box_dial->Add(max_dist_dial_, 1, wxEXPAND | wxALL, mergin_text);

    int mergin_panel = 4;
    box_dialog->Add(box_slider, 1, wxALL, mergin_panel);
    box_dialog->Add(box_dial,   1, wxALL, mergin_panel);

    box_dialog->SetSizeHints(this);
    SetSizer(box_dialog);

    // Set background color to default
    auto default_bg_color = wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK);
    SetBackgroundColour(default_bg_color);

    // Bind event
    Bind(wxEVT_CLOSE_WINDOW, &SliderValueDialog::OnClose, this);
}

void SliderValueDialog::OnClose(wxCloseEvent &event) {
    delete this;
}