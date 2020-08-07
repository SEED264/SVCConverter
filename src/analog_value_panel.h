#ifndef _SVCCONVERTER_SRC_ANALOG_VALUE_PANEL_H_
#define _SVCCONVERTER_SRC_ANALOG_VALUE_PANEL_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "parts.h"

class AnalogValuePanel : public wxWindow {
public:
    AnalogValuePanel(wxWindow *parent, const wxString &panel_label);

    void SetCurrentValue(int current_value);
    void SetMinValue(int min_value);
    void SetMaxValue(int max_value);
    void SetDistance(int distance);
    void SetMaxDistance(int max_distance);

private:
    LabledText *current_value_ = nullptr;
    LabledText *min_value_ = nullptr;
    LabledText *max_value_ = nullptr;
    LabledText *distance_ = nullptr;
    LabledText *max_distance_ = nullptr;
};

#endif // _SVCCONVERTER_SRC_ANALOG_VALUE_PANEL_H_