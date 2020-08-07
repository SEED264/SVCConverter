#ifndef _SVCCONVERTER_SRC_ANALOG_VALUE_DIALOG_H_
#define _SVCCONVERTER_SRC_ANALOG_VALUE_DIALOG_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "analog_value_panel.h"

class AnalogValueDialog : public wxDialog {
public:
    AnalogValueDialog(wxWindow *parent,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize);

    void InitGUI();

private:
    AnalogValuePanel *slider_panel_ = nullptr;
    AnalogValuePanel *dial_panel_ = nullptr;
};

#endif // _SVCCONVERTER_SRC_ANALOG_VALUE_DIALOG_H_