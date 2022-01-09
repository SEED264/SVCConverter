#ifndef _SVCCONVERTER_SRC_BINDING_DIALOG_H_
#define _SVCCONVERTER_SRC_BINDING_DIALOG_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "input_manager.h"
#include "input_recorder.h"
#include "config_data.h"

class ButtonBindingDialog : public wxDialog {
public:
    ButtonBindingDialog(wxWindow *parent,
                        RAWINPUTDEVICELIST target_device,
                        RawInputDeviceManager device_manager = RawInputDeviceManager());
    ~ButtonBindingDialog();

    auto IsButtonBinded() const { return is_button_binded_; }
    auto& GetButtonBindingInfo() const { return *binding_info_; };

private:
    enum {
        ID_CHOICE_TARGET_DEVICE,
        ID_BUTTON_CONFIRM_BINDING
    };
    RawInputFetcher fetcher_;
    ButtonRecorder *recorder_ = nullptr;
    RAWINPUTDEVICELIST *target_device_;
    RawInputDeviceLists selectable_devices_;
    SVCButtonBindInfo *binding_info_ = nullptr;
    bool is_button_binded_ = false;

    wxBoxSizer *sizer_dialog_;
    wxChoice *choice_select_device_;
    wxStaticText *text_target_device_;
    wxStaticText *text_pressed_button_;
    wxButton *button_confirm_binding_;

    wxTimer timer_;

    void OnTimer(wxTimerEvent &event);
    void OnSelectDevice(wxCommandEvent &event);
    void OnConfirm(wxCommandEvent &event);

    RAWINPUTDEVICELIST* GetSelectedDevice(const RAWINPUTDEVICELIST &target_device,
                                          int *out_index = nullptr);
};

class KnobBindingDialog : public wxDialog {
public:
    KnobBindingDialog(wxWindow *parent,
                      RAWINPUTDEVICELIST target_device,
                      RawInputDeviceManager device_manager = RawInputDeviceManager());
    ~KnobBindingDialog();

    auto IsKnobBinded() const { return is_knob_binded_; }
    auto& GetKnobBindingInfo() const { return *binding_info_; };

private:
    enum {
        ID_CHOICE_TARGET_DEVICE,
        ID_BUTTON_CONFIRM_BINDING
    };
    RawInputFetcher fetcher_;
    KnobRecorderForRegister *recorder_ = nullptr;
    RAWINPUTDEVICELIST *target_device_;
    RawInputDeviceLists selectable_devices_;
    SVCKnobBindInfo *binding_info_ = nullptr;
    bool is_knob_binded_ = false;

    wxBoxSizer *sizer_dialog_;
    wxChoice *choice_select_device_;
    wxStaticText *text_target_device_;
    wxStaticText *text_most_turned_knob_;
    wxButton *button_confirm_binding_;

    wxTimer timer_;

    void OnTimer(wxTimerEvent &event);
    void OnSelectDevice(wxCommandEvent &event);
    void OnConfirm(wxCommandEvent &event);

    RAWINPUTDEVICELIST* GetSelectedDevice(const RAWINPUTDEVICELIST &target_device,
                                          int *out_index = nullptr);
};

class KeyBindingDialog : public wxDialog {
public:
    KeyBindingDialog(wxWindow *parent);
    ~KeyBindingDialog();

    auto IsKeyBinded() const { return is_key_binded_; }
    auto GetKey() const { return *binding_key_; };

private:
    enum {
        ID_BUTTON_CONFIRM_BINDING
    };
    KeyRecorder recorder_;
    unsigned char *binding_key_ = nullptr;
    bool is_key_binded_ = false;

    wxTextCtrl *text_pressed_key_;
    wxButton *button_confirm_binding_;

    wxTimer timer_;

    void OnTimer(wxTimerEvent &event);
    void OnConfirm(wxCommandEvent &event);
};

class MouseBindingDialog : public wxDialog {
public:
    MouseBindingDialog(wxWindow *parent,
        RAWINPUTDEVICELIST target_device,
        RawInputDeviceManager device_manager = RawInputDeviceManager());
    ~MouseBindingDialog();

    auto IsKnobBinded() const { return is_knob_binded_; }
    auto& GetKnobBindingInfo() const { return *binding_info_; };

private:
    enum {
        ID_CHOICE_TARGET_DEVICE,
        ID_BUTTON_CONFIRM_BINDING
    };
    RawInputFetcher fetcher_;
    KnobRecorderForRegister *recorder_ = nullptr;
    RAWINPUTDEVICELIST *target_device_;
    RawInputDeviceLists selectable_devices_;
    SVCKnobBindInfo *binding_info_ = nullptr;
    bool is_knob_binded_ = false;

    wxBoxSizer *sizer_dialog_;
    wxChoice *choice_select_device_;
    wxStaticText *text_target_device_;
    wxStaticText *text_most_turned_knob_;
    wxButton *button_confirm_binding_;

    wxTimer timer_;

    void OnTimer(wxTimerEvent &event);
    void OnSelectDevice(wxCommandEvent &event);
    void OnConfirm(wxCommandEvent &event);

    RAWINPUTDEVICELIST* GetSelectedDevice(const RAWINPUTDEVICELIST &target_device,
        int *out_index = nullptr);
};

#endif // _SVCCONVERTER_SRC_BINDING_DIALOG_H_