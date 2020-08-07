#include "analog_value_panel.h"

AnalogValuePanel::AnalogValuePanel(wxWindow *parent, const wxString &panel_label)
        : wxWindow(parent, wxID_ANY) {
    auto sizer = new wxStaticBoxSizer(wxVERTICAL, this, panel_label);

    // Init text
    int minimum_space = 40;
    current_value_ = new LabledText(this, "Current value",    "N/A", minimum_space);
    min_value_     = new LabledText(this, "Min Value",        "N/A", minimum_space);
    max_value_     = new LabledText(this, "Max value",        "N/A", minimum_space);
    distance_      = new LabledText(this, "Current Distance", "N/A", minimum_space);
    max_distance_  = new LabledText(this, "Max distance",     "N/A", minimum_space);

    // Layout to box
    int mergin_text = 8;
    sizer->Add(current_value_, 1, wxEXPAND | wxALL, mergin_text);
    sizer->Add(min_value_,     1, wxEXPAND | wxALL, mergin_text);
    sizer->Add(max_value_,     1, wxEXPAND | wxALL, mergin_text);
    sizer->Add(distance_,      1, wxEXPAND | wxALL, mergin_text);
    sizer->Add(max_distance_,  1, wxEXPAND | wxALL, mergin_text);

    sizer->SetSizeHints(this);
    SetSizer(sizer);
}

void AnalogValuePanel::SetCurrentValue(int current_value) {
    current_value_->SetValue(wxString::Format("%i", current_value));
}

void AnalogValuePanel::SetMinValue(int min_value) {
    min_value_->SetValue(wxString::Format("%i", min_value));
}

void AnalogValuePanel::SetMaxValue(int max_value) {
    max_value_->SetValue(wxString::Format("%i", max_value));
}

void AnalogValuePanel::SetDistance(int distance) {
    distance_->SetValue(wxString::Format("%i", distance));
}

void AnalogValuePanel::SetMaxDistance(int max_distance) {
    max_distance_->SetValue(wxString::Format("%i", max_distance));
}