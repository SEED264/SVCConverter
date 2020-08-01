#ifndef _SVCCONVERTER_SRC_PARTS_H_
#define _SVCCONVERTER_SRC_PARTS_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP

class LabledText : public wxWindow {
public:
    LabledText(wxWindow *parent,
               const wxString &label, const wxString &value, int minimum_space = 0,
               wxWindowID label_id = wxID_ANY, wxWindowID value_id = wxID_ANY);

    void SetLabel(const wxString &label);
    void SetValue(const wxString &value);

private:
    wxStaticText *label_ = nullptr;
    wxStaticText *value_ = nullptr;
};

#endif // _SVCCONVERTER_SRC_PARTS_H_