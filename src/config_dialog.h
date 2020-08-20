#ifndef _SVCCONVERTER_SRC_CONFIG_DIALOG_H_
#define _SVCCONVERTER_SRC_CONFIG_DIALOG_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP

class ConfigDialog : public wxDialog {
public:
    ConfigDialog(wxWindow *parent = nullptr);

private:
    void OnTimer(wxTimerEvent &event);
};

#endif // _SVCCONVERTER_SRC_CONFIG_DIALOG_H_