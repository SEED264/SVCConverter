#include "main_window.h"
#include "analog_value_dialog.h"
#include "config_dialog.h"
#include "binding_dialog.h"

#include "config_parser.h"

MainWindow::MainWindow(const wxString &title,
                       const wxPoint &pos,
                       const wxSize &size)
        : wxFrame(nullptr, wxID_ANY, title, pos, size),
          timer_(this),
          fetcher_(this) {
    InitGUI();
    profiles_ = SVCConfigParser().ParseProfiles();
    RefreshProfileChoice();
    SelectProfile(0);

    // Calc desktop center
    auto desktop = GetDesktopWindow();
    RECT rect;
    GetWindowRect(desktop, &rect);
    desktop_center_x_ = rect.right;
    desktop_center_y_ = rect.bottom;
}

void MainWindow::InitGUI() {
    SetIcon(wxICON(APP_ICON));
    // Init parts
    auto *panel = new wxPanel(this, wxID_ANY);
    choice_profile_ = new wxChoice(panel, ID_CHOICE_PROFILE, wxDefaultPosition,
                                   wxDefaultSize, wxArrayString());
    auto *button_quit = new wxButton(panel, wxID_EXIT, "&Quit");
    auto *button_switch = new wxButton(panel, ID_BUTTON_SWITCH, "&Switch");
    auto *text_label_is_enabled = new wxStaticText(panel, wxID_ANY, "Convert : ");
    text_is_enabled_ = new wxStaticText(panel, wxID_ANY, "OFF");
    auto *button_config = new wxButton(panel, ID_BUTTON_CONFIG, "&Config");

    // Init boxsizer
    auto *sizer_window = new wxBoxSizer(wxVERTICAL);
    auto *sizer_switch = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_bottom = new wxBoxSizer(wxHORIZONTAL);

    // Layout
    sizer_switch->Add(text_label_is_enabled, 0);
    sizer_switch->Add(text_is_enabled_,      0);
    sizer_switch->AddStretchSpacer();
    sizer_switch->Add(button_switch   ,      0);

    sizer_bottom->Add(button_config, 0);
    sizer_bottom->AddStretchSpacer();
    sizer_bottom->Add(button_quit,   0);

    sizer_window->Add(choice_profile_, 0, wxEXPAND | wxALL, 4);
    sizer_window->Add(sizer_switch, 0, wxEXPAND | wxALL, 4);
    sizer_window->AddStretchSpacer();
    sizer_window->Add(sizer_bottom, 0, wxEXPAND | wxALL, 4);

    panel->SetSizer(sizer_window);
    sizer_window->SetSizeHints(this);
    auto size = GetSize();
    size.x *= 1.5;
    size.y *= 1.2;
    SetSize(size);

    // Disable resize and maximize
    SetWindowStyle(GetWindowStyle() & ~wxMAXIMIZE_BOX & ~wxRESIZE_BORDER);

    // Bind events
    Bind(wxEVT_CLOSE_WINDOW , &MainWindow::OnClose, this);
    Bind(wxEVT_CHOICE, &MainWindow::OnChoice, this, ID_CHOICE_PROFILE);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&){ Switch(); }, ID_BUTTON_SWITCH);
    Bind(wxEVT_BUTTON, &MainWindow::OnShowConfig, this,
        ID_BUTTON_CONFIG);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&){ Close(); }, wxID_EXIT);
    Bind(wxEVT_TIMER, &MainWindow::OnTimer, this, timer_.GetId());
    timer_.Start(1000 / 120);
}

void MainWindow::OnShowConfig(wxCommandEvent &event) {
    Disable();
    ConfigDialog dialog(nullptr, profiles_, current_profile_index_);
    //MouseBindingDialog dialog(nullptr, RAWINPUTDEVICELIST());
    dialog.ShowModal();
    if (dialog.IsEdited()) {
        profiles_ = dialog.GetProfiles();
        SVCConfigParser().DumpProfiles(profiles_);
        RefreshProfileChoice();
        SelectProfile(dialog.GetCurrentProfileIndex());
    }
}

void MainWindow::Switch() {
    if (is_enabled_)
        Disable();
    else
        Enable();
}

void MainWindow::Enable() {
    is_enabled_ = true;
    text_is_enabled_->SetLabel("ON");
    prev_knob_l_ = 0;
    prev_knob_r_ = 0;
    button_recorders_ = std::map<HANDLE, ButtonRecorder>();
    knob_recorders_ = std::map<HANDLE, KnobRecorder>();
    RegisterCurrentProfileDevice();
}

void MainWindow::Disable() {
    is_enabled_ = false;
    text_is_enabled_->SetLabel("OFF");
    auto &current_profile = profiles_[current_profile_index_];
    // Up all key before disable
    SendKeyUp(current_profile.knob_l.l_key);
    SendKeyUp(current_profile.knob_l.r_key);
    SendKeyUp(current_profile.knob_r.l_key);
    SendKeyUp(current_profile.knob_r.r_key);
    SendKeyUp(current_profile.start.key);
    SendKeyUp(current_profile.bt_a.key);
    SendKeyUp(current_profile.bt_b.key);
    SendKeyUp(current_profile.bt_c.key);
    SendKeyUp(current_profile.bt_d.key);
    SendKeyUp(current_profile.fx_l.key);
    SendKeyUp(current_profile.fx_r.key);
    for (auto& ex_bt : current_profile.extra_buttons) {
        SendKeyUp(ex_bt.second.key);
    }
    fetcher_.RemoveAllDevice();
}

void MainWindow::SelectProfile(int profile_index) {
    if (profile_index >= profiles_.size())
        SelectProfile(0);
    Disable();
    current_profile_index_ = profile_index;
    choice_profile_->SetSelection(profile_index);
}

void MainWindow::RegisterCurrentProfileDevice() {
    auto& current_profile = profiles_[current_profile_index_];
    fetcher_.AddDevice(current_profile.knob_l.device_list);
    fetcher_.AddDevice(current_profile.knob_r.device_list);
    fetcher_.AddDevice(current_profile.start.device_list);
    fetcher_.AddDevice(current_profile.bt_a.device_list);
    fetcher_.AddDevice(current_profile.bt_b.device_list);
    fetcher_.AddDevice(current_profile.bt_c.device_list);
    fetcher_.AddDevice(current_profile.bt_d.device_list);
    fetcher_.AddDevice(current_profile.fx_l.device_list);
    fetcher_.AddDevice(current_profile.fx_r.device_list);
    for (auto& ex_bt : current_profile.extra_buttons) {
        fetcher_.AddDevice(ex_bt.second.device_list);
    }
}

void MainWindow::RefreshProfileChoice() {
    // Adjust size
    int sub = profiles_.size() - choice_profile_->GetCount();
    if (sub < 0) {
        for (int i = 0; i < -sub; ++i) {
            choice_profile_->Delete(0);
        }
    } else {
        for (int i = 0; i < sub; ++i) {
            choice_profile_->Append("");
        }
    }
    // Replace names
    for (int i = 0; i < profiles_.size(); ++i) {
        choice_profile_->SetString(i, profiles_[i].profile_name);
    }
}

void MainWindow::SendKey(unsigned char key, bool is_key_down) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.wScan = MapVirtualKey(key, 0);
    input.ki.dwFlags = (is_key_down) ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
}

void MainWindow::SendKnobKey(const SVCKnobBindInfo& info, int *prev_status) {
    /*
     * prev_status
     *  0 = stopped
     *  1 = turned to right
     * -1 = turned to left
     */
    long diff_knob = 0;
    if (info.type == HID)
        diff_knob = knob_recorders_[info.device_list.hDevice]
                        .GetDifference()[HIDUsagePair(info.usage_page, info.usage_id)];
    else {
        auto state = fetcher_.GetMouseState(info.device_list.hDevice);
        if (info.usage_id == 0x30)
            diff_knob = state.GetXDifferences();
        else
            diff_knob = state.GetYDifferences();
    }
    bool turning_r;
    if (diff_knob > 0) {
        if (info.increase_direction == Right) {
            turning_r = true;
        } else {
            turning_r = false;
        }
    } else if (diff_knob < 0) {
        if (info.increase_direction == Right) {
            turning_r = false;
        } else {
            turning_r = true;
        }
        // When stopped
    } else {
        if (*prev_status == 1)
            SendKeyUp(info.r_key);
        else if (*prev_status == -1)
            SendKeyUp(info.l_key);
        *prev_status = 0;
        return;
    }
    // When the knob is turning
    if (turning_r) {
        if (*prev_status != 1){
            SendKeyUp(info.l_key);
            SendKeyDown(info.r_key);
        }
        *prev_status = 1;
    } else {
        if (*prev_status != -1) {
            SendKeyUp(info.r_key);
            SendKeyDown(info.l_key);
        }
        *prev_status = -1;
    }
}

void MainWindow::SendButtonKey(const SVCButtonBindInfo& info) {
    auto &states_bt = button_recorders_[info.device_list.hDevice];
    if (states_bt.GetTriggeredStates()[info.usage_page][info.usage_id]) {
        SendKeyDown(info.key);
    OutputDebugString(wxString::Format("Key pressed."));
    }
    if (states_bt.GetReleasedStates()[info.usage_page][info.usage_id]) {
        SendKeyUp(info.key);
    }
}

void MainWindow::SendKeyDown(unsigned char key) {
    SendKey(key, true);
}
void MainWindow::SendKeyUp(unsigned char key) {
    SendKey(key, false);
}

void MainWindow::OnChoice(wxCommandEvent& event) {
    SelectProfile(choice_profile_->GetSelection());
}

void MainWindow::OnClose(wxCloseEvent &event) {
    Disable();
    Destroy();
}

void MainWindow::OnTimer(wxTimerEvent &event) {
    SHORT ctrl_pressing = GetAsyncKeyState(VK_CONTROL);
    // Detect hotkey press
    SHORT f12_pressing = GetAsyncKeyState(VK_F12);
    static SHORT f12_prev = 0;
    bool f12_down = (!f12_prev && f12_pressing);
    f12_prev = f12_pressing;
    if (ctrl_pressing && f12_down) {
        Switch();
        return;
    }

    if (!is_enabled_)
        return;

    auto current_profile = profiles_[current_profile_index_];
    // Set all devices states
    for (auto& device : fetcher_.GetRegisteredDevices()) {
        auto state = fetcher_.GetDeviceState(device.hDevice);
        button_recorders_[device.hDevice].SetButtonStates(state.button_state.GetUsages());
        knob_recorders_[device.hDevice].SetKnobStates(state.value_state.GetValues());
    }
    // Left knob
    SendKnobKey(current_profile.knob_l, &prev_knob_l_);
    // Right knob
    SendKnobKey(current_profile.knob_r, &prev_knob_r_);
    // START
    SendButtonKey(current_profile.start);
    // BT-A
    SendButtonKey(current_profile.bt_a);
    // BT-B
    SendButtonKey(current_profile.bt_b);
    // BT-C
    SendButtonKey(current_profile.bt_c);
    // BT-D
    SendButtonKey(current_profile.bt_d);
    // FX-L
    SendButtonKey(current_profile.fx_l);
    // FX-R
    SendButtonKey(current_profile.fx_r);
    // Extra buttons
    for (auto& ex_bt : current_profile.extra_buttons) {
        SendButtonKey(ex_bt.second);
    }
    fetcher_.GetMouseState(current_profile.knob_l.device_list.hDevice).ResetDifferences();
    fetcher_.GetMouseState(current_profile.knob_r.device_list.hDevice).ResetDifferences();

    // Set cursor to center if using mouse input as knob
    if (current_profile.knob_l.type == Mouse
     || current_profile.knob_r.type == Mouse) {
        SetCursorPos(desktop_center_x_, desktop_center_y_);
    }
}