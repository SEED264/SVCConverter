#include "parts.h"
#include <string>

LabledText::LabledText(wxWindow *parent,
                       const wxString &label, const wxString &value, int minimum_space,
                       wxWindowID label_id, wxWindowID value_id)
                       : wxWindow(parent, wxID_ANY) {
    label_ = new wxStaticText(this, label_id, label);
    value_ = new wxStaticText(this, value_id, value);

    auto *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(label_);
    sizer->AddStretchSpacer();
    sizer->Add(value_);

    int label_width = label_->GetSize().x;
    int value_width = value_->GetSize().x;
    sizer->SetMinSize(label_width + value_width + minimum_space, 0);

    sizer->SetSizeHints(this);
    SetSizer(sizer);
}

void LabledText::SetLabel(const wxString &label) {
    label_->SetLabel(label);
}

void LabledText::SetValue(const wxString &value) {
    value_->SetLabel(value);
}