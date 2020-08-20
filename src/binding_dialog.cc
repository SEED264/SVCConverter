#include "binding_dialog.h"
#include "input_manager.h"
#include "label_string.h"

ButtonBindingDialog::ButtonBindingDialog(
    wxWindow *parent,
    RAWINPUTDEVICELIST target_device,
    RawInputDeviceManager device_manager)
        : wxDialog(parent, wxID_ANY, "Button binding"),
          fetcher_(this) {
    // Get selectable devices
    auto joysticks = device_manager.GetDeviceLists(0x01, 0x04);
    auto game_pads = device_manager.GetDeviceLists(0x01, 0x05);
    selectable_devices_.insert(selectable_devices_.end(), joysticks.begin(), joysticks.end());
    selectable_devices_.insert(selectable_devices_.end(), game_pads.begin(), game_pads.end());

    // Add target device to fetcher if it's valid
    int target_index;
    if (GetSelectedDevice(target_device, &target_index)) {
        fetcher_.AddDevice(target_device);
        target_device_ = &target_device;
    } else {
        fetcher_.AddDevice(selectable_devices_[0]);
        target_device_ = &selectable_devices_[0];
        target_index = 0;
    }
    wxString product_name = GetHIDProductName(selectable_devices_[target_index]);
    recorder_ = new ButtonRecorder;

    auto *panel = new wxPanel(this);
    // Create choice from target device lists
    wxArrayString target_device_names;
    for (auto &sd : selectable_devices_) {
        target_device_names.Add(GetHIDProductName(sd));
    }
    choice_select_device_ = new wxChoice(panel, ID_CHOICE_TARGET_DEVICE, wxDefaultPosition,
                                         wxDefaultSize, target_device_names);
    choice_select_device_->SetSelection(target_index);

    auto *text_how_to_bind = new wxStaticText(panel, wxID_ANY, "Press button and click OK to bind.");
    auto *text_label_target_device = new wxStaticText(panel, wxID_ANY, "Target device : ");
    text_target_device_ = new wxStaticText(panel, wxID_ANY, product_name);
    auto *text_label_pressed_button = new wxStaticText(panel, wxID_ANY, "Pressed button : ");
    text_pressed_button_ = new wxStaticText(panel, wxID_ANY, "");
    button_confirm_binding_ = new wxButton(panel, ID_BUTTON_CONFIRM_BINDING, "&OK");

    // Disable confirm button
    button_confirm_binding_->Enable(false);

    sizer_dialog_ = new wxBoxSizer(wxVERTICAL);
    auto *sizer_select_device = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_selected_device = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_pressed_button = new wxBoxSizer(wxHORIZONTAL);

    sizer_select_device->Add(choice_select_device_, 1);

    sizer_selected_device->Add(text_label_target_device, 0);
    sizer_selected_device->Add(text_target_device_, 1);

    sizer_pressed_button->Add(text_label_pressed_button, 0);
    sizer_pressed_button->Add(text_pressed_button_, 1);

    sizer_dialog_->Add(sizer_select_device, 0);
    sizer_dialog_->Add(text_how_to_bind, 1);
    sizer_dialog_->Add(sizer_selected_device, 1);
    sizer_dialog_->Add(sizer_pressed_button, 1);
    sizer_dialog_->Add(button_confirm_binding_, 0, wxALIGN_RIGHT);

    panel->SetSizer(sizer_dialog_);
    sizer_dialog_->SetSizeHints(this);
    auto size = GetSize();
    size.x *= 1.3;
    size.y *= 1.3;
    SetSize(size);

    Bind(wxEVT_CHOICE, &ButtonBindingDialog::OnSelectDevice, this,
         ID_CHOICE_TARGET_DEVICE);
    Bind(wxEVT_BUTTON, &ButtonBindingDialog::OnConfirm, this,
         ID_BUTTON_CONFIRM_BINDING);
    Bind(wxEVT_TIMER, &ButtonBindingDialog::OnTimer, this);
    timer_.SetOwner(this);
    timer_.Start(1000/120);
}

void ButtonBindingDialog::OnTimer(wxTimerEvent &event) {
    auto states = fetcher_.GetDeviceState(target_device_->hDevice).button_state.GetUsages();
    recorder_->SetButtonStates(states);
    auto triggered = recorder_->GetTriggeredStates();
    for (auto ts = triggered.begin(); ts != triggered.end(); ++ts) {
        auto usage_page = ts->first;
        auto &tstates = ts->second;
        for (auto tf = tstates.begin(); tf != tstates.end(); ++tf) {
            auto usage_id = tf->first;
            auto is_triggered = tf->second;
            if (is_triggered) {
                if (!binding_info_) {
                    binding_info_ = new SVCButtonBindInfo;
                    binding_info_->device_list = *target_device_;
                }
                binding_info_->usage_page = usage_page;
                binding_info_->usage_id = usage_id;
                text_pressed_button_->SetLabel(wxString::Format("<0x%02x, 0x%02x>",
                                                                usage_page, usage_id));
                button_confirm_binding_->Enable(true);
                break;
            }
        }
    }
}

void ButtonBindingDialog::OnSelectDevice(wxCommandEvent &event) {
    int target_index = choice_select_device_->GetSelection();
    auto &target_device = selectable_devices_[target_index];
    target_device_ = &target_device;
    fetcher_.AddDevice(target_device);
    text_target_device_->SetLabel(GetHIDProductName(*target_device_));
    text_pressed_button_->SetLabel("");
    delete recorder_;
    recorder_ = new ButtonRecorder;
    if (binding_info_) {
        delete binding_info_;
        binding_info_ = nullptr;
    }
    button_confirm_binding_->Enable(false);

    // Resize dialog
    sizer_dialog_->SetSizeHints(this);
    auto size = GetSize();
    size.x *= 1.3;
    size.y *= 1.3;
    SetSize(size);

}

void ButtonBindingDialog::OnConfirm(wxCommandEvent &event) {
    is_button_binded_ = true;
    Close();
}

RAWINPUTDEVICELIST* ButtonBindingDialog::GetSelectedDevice(
        const RAWINPUTDEVICELIST &target_device, int *out_index) {
    auto device =  std::find_if(selectable_devices_.begin(), selectable_devices_.end(),
                                [&](RAWINPUTDEVICELIST &device){
                                    return device.hDevice == target_device.hDevice; });
    bool is_device_valid = device != selectable_devices_.end();
    // Set index if out variable is valid
    if (out_index) {
        if (is_device_valid)
            *out_index = std::distance(selectable_devices_.begin(), device);
        else
            *out_index = -1;
    }
    return (is_device_valid) ? &*device : nullptr;
}

ButtonBindingDialog::~ButtonBindingDialog() {
    delete recorder_;
    delete binding_info_;
}

/* ---------- KnobBindingDialog ---------- */
KnobBindingDialog::KnobBindingDialog(
    wxWindow *parent,
    RAWINPUTDEVICELIST target_device,
    RawInputDeviceManager device_manager)
        : wxDialog(parent, wxID_ANY, "Knob binding"),
          fetcher_(this) {
    // Get selectable devices
    auto joysticks = device_manager.GetDeviceLists(0x01, 0x04);
    auto game_pads = device_manager.GetDeviceLists(0x01, 0x05);
    selectable_devices_.insert(selectable_devices_.end(), joysticks.begin(), joysticks.end());
    selectable_devices_.insert(selectable_devices_.end(), game_pads.begin(), game_pads.end());

    // Add target device to fetcher if it's valid
    int target_index;
    if (GetSelectedDevice(target_device, &target_index)) {
        fetcher_.AddDevice(target_device);
        target_device_ = &target_device;
    } else {
        fetcher_.AddDevice(selectable_devices_[0]);
        target_device_ = &selectable_devices_[0];
        target_index = 0;
    }
    wxString product_name = GetHIDProductName(selectable_devices_[target_index]);
    recorder_ = new KnobRecorderForRegister;

    auto *panel = new wxPanel(this);
    // Create choice from target device lists
    wxArrayString target_device_names;
    for (auto &sd : selectable_devices_) {
        target_device_names.Add(GetHIDProductName(sd));
    }
    choice_select_device_ = new wxChoice(panel, ID_CHOICE_TARGET_DEVICE, wxDefaultPosition,
                                         wxDefaultSize, target_device_names);
    choice_select_device_->SetSelection(target_index);

    auto *text_how_to_bind = new wxStaticText(panel, wxID_ANY, "Keep turning knob to right and click OK to bind.");
    auto *text_label_target_device = new wxStaticText(panel, wxID_ANY, "Target device : ");
    text_target_device_ = new wxStaticText(panel, wxID_ANY, product_name);
    auto *text_label_most_turned_knob = new wxStaticText(panel, wxID_ANY,
                                                         "Most turned knob : ");
    text_most_turned_knob_ = new wxStaticText(panel, wxID_ANY, "");
    button_confirm_binding_ = new wxButton(panel, ID_BUTTON_CONFIRM_BINDING, "&OK");

    // Disable confirm button
    button_confirm_binding_->Enable(false);

    sizer_dialog_ = new wxBoxSizer(wxVERTICAL);
    auto *sizer_select_device = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_selected_device = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_most_turned_knob = new wxBoxSizer(wxHORIZONTAL);

    sizer_select_device->Add(choice_select_device_, 1);

    sizer_selected_device->Add(text_label_target_device, 0);
    sizer_selected_device->Add(text_target_device_, 1);

    sizer_most_turned_knob->Add(text_label_most_turned_knob, 0);
    sizer_most_turned_knob->Add(text_most_turned_knob_, 1);

    sizer_dialog_->Add(sizer_select_device, 0);
    sizer_dialog_->Add(text_how_to_bind, 1);
    sizer_dialog_->Add(sizer_selected_device, 1);
    sizer_dialog_->Add(sizer_most_turned_knob, 1);
    sizer_dialog_->Add(button_confirm_binding_, 0, wxALIGN_RIGHT);

    panel->SetSizer(sizer_dialog_);
    sizer_dialog_->SetSizeHints(this);
    auto size = GetSize();
    size.x *= 1.3;
    size.y *= 1.3;
    SetSize(size);

    Bind(wxEVT_CHOICE, &KnobBindingDialog::OnSelectDevice, this,
         ID_CHOICE_TARGET_DEVICE);
    Bind(wxEVT_BUTTON, &KnobBindingDialog::OnConfirm, this,
         ID_BUTTON_CONFIRM_BINDING);
    Bind(wxEVT_TIMER, &KnobBindingDialog::OnTimer, this);
    timer_.SetOwner(this);
    timer_.Start(1000/60);
}

void KnobBindingDialog::OnTimer(wxTimerEvent &event) {
    auto values = fetcher_.GetDeviceState(target_device_->hDevice).value_state.GetValues();
    recorder_->SetKnobStates(values);
    auto differences = recorder_->GetTotalDifference();
    auto df = differences.begin();
    HIDUsagePair most_turned_usage = df->first;
    auto most_turned_value = df->second;

    for (; df != differences.end(); ++df) {
        auto usage = df->first;
        auto knob_value = df->second;
        if (std::abs(knob_value) > std::abs(most_turned_value)) {
            most_turned_usage = usage;
            most_turned_value = knob_value;
        }
    }
    auto most_turned_ratio = recorder_->GetTotalDifferenceAsRatio()[most_turned_usage];
    if (std::abs(most_turned_value) > 128 && std::abs(most_turned_ratio) > 1.0) {
        if (!binding_info_) {
            binding_info_ = new SVCKnobBindInfo;
            binding_info_->device_list = *target_device_;
        }
        binding_info_->usage_page = most_turned_usage.first;
        binding_info_->usage_id = most_turned_usage.second;
        binding_info_->increase_direction = (most_turned_value > 0)
            ? SVCKnobIncreaseDirection::Right
            : SVCKnobIncreaseDirection::Left;
        text_most_turned_knob_->SetLabel(wxString::Format("<0x%02x, 0x%02x>",
                                                          most_turned_usage.first,
                                                          most_turned_usage.second));
        // Enable confirm button
        button_confirm_binding_->Enable(true);
    }
}

void KnobBindingDialog::OnSelectDevice(wxCommandEvent &event) {
    int target_index = choice_select_device_->GetSelection();
    auto &target_device = selectable_devices_[target_index];
    target_device_ = &target_device;
    fetcher_.AddDevice(target_device);
    text_target_device_->SetLabel(GetHIDProductName(*target_device_));
    text_most_turned_knob_->SetLabel("");
    delete recorder_;
    recorder_ = new KnobRecorderForRegister;
    if (binding_info_) {
        delete binding_info_;
        binding_info_ = nullptr;
    }
    button_confirm_binding_->Enable(false);

    // Resize dialog
    sizer_dialog_->SetSizeHints(this);
    auto size = GetSize();
    size.x *= 1.3;
    size.y *= 1.3;
    SetSize(size);
}

void KnobBindingDialog::OnConfirm(wxCommandEvent &event) {
    is_knob_binded_ = true;
    Close();
}

RAWINPUTDEVICELIST* KnobBindingDialog::GetSelectedDevice(
        const RAWINPUTDEVICELIST &target_device, int *out_index) {
    auto device =  std::find_if(selectable_devices_.begin(), selectable_devices_.end(),
                                [&](RAWINPUTDEVICELIST &device){
                                    return device.hDevice == target_device.hDevice; });
    bool is_device_valid = device != selectable_devices_.end();
    // Set index if out variable is valid
    if (out_index) {
        if (is_device_valid)
            *out_index = std::distance(selectable_devices_.begin(), device);
        else
            *out_index = -1;
    }
    return (is_device_valid) ? &*device : nullptr;
}

KnobBindingDialog::~KnobBindingDialog() {
    delete recorder_;
    delete binding_info_;
}

/* ---------- KeyBindingDialog ---------- */
KeyBindingDialog::KeyBindingDialog(wxWindow *parent)
        : wxDialog(parent, wxID_ANY, "Key binding") {

    auto *panel = new wxPanel(this);

    auto *text_how_to_bind = new wxStaticText(panel, wxID_ANY, "Press any key and click OK to bind.");
    auto *text_label_pressed_key = new wxStaticText(panel, wxID_ANY,
                                                    "Pressed key : ");
    text_pressed_key_ = new wxStaticText(panel, wxID_ANY, "");
    button_confirm_binding_ = new wxButton(panel, ID_BUTTON_CONFIRM_BINDING, "OK");


    auto *sizer_dialog = new wxBoxSizer(wxVERTICAL);
    auto *sizer_how_to_bind = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_pressed_key = new wxBoxSizer(wxHORIZONTAL);

    sizer_how_to_bind->Add(text_how_to_bind, 0);

    sizer_pressed_key->Add(text_label_pressed_key, 0);
    sizer_pressed_key->Add(text_pressed_key_, 1);

    sizer_dialog->Add(sizer_how_to_bind, 0);
    sizer_dialog->Add(sizer_pressed_key, 1);
    sizer_dialog->Add(button_confirm_binding_, 0, wxALIGN_RIGHT);

    panel->SetSizer(sizer_dialog);
    sizer_dialog->SetSizeHints(this);
    auto size = GetSize();
    size.x *= 1.3;
    size.y *= 1.3;
    SetSize(size);

    Bind(wxEVT_BUTTON, &KeyBindingDialog::OnConfirm, this,
         ID_BUTTON_CONFIRM_BINDING);
    Bind(wxEVT_TIMER, &KeyBindingDialog::OnTimer, this);

    timer_.SetOwner(this);
    timer_.Start(1000/60);
}

void KeyBindingDialog::OnTimer(wxTimerEvent &event) {
    std::vector<unsigned char> states(256, 0);
    GetKeyboardState(states.data());
    recorder_.SetKeyStates(states);
    auto triggered = recorder_.GetTriggeredStates();
    for (auto i = 0; i < triggered.size(); ++i) {
        auto is_triggered = triggered[i];
        // Continue if mouse button or abstract modifier key
        auto is_mouse_button = i == VK_LBUTTON || i == VK_RBUTTON || i == VK_MBUTTON ||
                               i == VK_XBUTTON1 || i == VK_XBUTTON2;
        auto is_abstract_modifier = i >= 0x10 && i <= 0x12;
        if (is_mouse_button || is_abstract_modifier)
            continue;

        if (is_triggered) {
            if (!binding_key_) {
                binding_key_ = new unsigned char;
            }
            *binding_key_ = static_cast<unsigned char>(i);
            text_pressed_key_->SetLabel(key_str[*binding_key_]);
            // Enable confirm button
            button_confirm_binding_->Enable(true);
            break;
        }
    }
}

void KeyBindingDialog::OnConfirm(wxCommandEvent &event) {
    if (binding_key_)
        is_key_binded_ = true;
    Close();
}

KeyBindingDialog::~KeyBindingDialog() {
    delete binding_key_;
}