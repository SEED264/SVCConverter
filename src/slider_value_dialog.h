#ifndef _SVCCONVERTER_SRC_SLIDER_VALUE_DIALOG_H_
#define _SVCCONVERTER_SRC_SLIDER_VALUE_DIALOG_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "parts.h"

class SliderValueDialog : public wxDialog {
public:
    SliderValueDialog(wxWindow *parent,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize);

    void InitGUI();

    void OnClose(wxCloseEvent &event);

private:
    // Labeled text for display analog values
    LabledText *current_slider_;
    LabledText *min_slider_;
    LabledText *max_slider_;
    LabledText *dist_slider_;
    LabledText *max_dist_slider_;

    LabledText *current_dial_;
    LabledText *min_dial_;
    LabledText *max_dial_;
    LabledText *dist_dial_;
    LabledText *max_dist_dial_;
};

#endif // _SVCCONVERTER_SRC_SLIDER_VALUE_DIALOG_H_