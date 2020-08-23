#ifndef _SVCCONVERTER_SRC_MAIN_WINDOW_H_
#define _SVCCONVERTER_SRC_MAIN_WINDOW_H_

#include <map>
#include <unordered_set>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "config_data.h"
#include "input_manager.h"
#include "input_recorder.h"

class MainWindow : public wxFrame {
public:
    enum {
        ID_CHOICE_PROFILE,
        ID_BUTTON_SWITCH,
        ID_BUTTON_CONFIG
    };

    MainWindow(const wxString &title,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize);

private:
    std::vector<SVCControllBindProfile> profiles_;
    int current_profile_index_ = 0;
    RawInputFetcher fetcher_;
    bool is_enabled_ = false;

    int prev_knob_l_ = 0;
    int prev_knob_r_ = 0;

    wxTimer timer_;

    wxChoice *choice_profile_ = nullptr;
    wxStaticText *text_is_enabled_ = nullptr;

    std::map<HANDLE, ButtonRecorder> button_recorders_;
    std::map<HANDLE, KnobRecorder> knob_recorders_;

    void InitGUI();
    void Switch();
    void Enable();
    void Disable();
    void SelectProfile(int profile_index);
    void RegisterCurrentProfileDevice();
    void RefreshProfileChoice();

    void SendKnobKey(const SVCKnobBindInfo &info, int *prev_status);
    void SendButtonKey(const SVCButtonBindInfo &info);
    void SendKey(unsigned char key, bool is_key_down);
    void SendKeyDown(unsigned char key);
    void SendKeyUp(unsigned char key);

    void OnClose(wxCloseEvent &event);
    void OnChoice(wxCommandEvent &event);
    void OnTimer(wxTimerEvent &event);

    void OnShowConfig(wxCommandEvent &event);
};

#endif // _SVCCONVERTER_SRC_MAIN_WINDOW_H_