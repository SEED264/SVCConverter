#ifndef _SVCCONVERTER_SRC_CONFIG_DATA_H_
#define _SVCCONVERTER_SRC_CONFIG_DATA_H_

#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <limits>
#include <map>
#include <vector>
#include <winsock2.h>
#include <windows.h>
extern "C" {
#include <hidsdi.h>
}
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP

const unsigned long SVCUsageNotMapped = ULONG_MAX;

class SVCSetting {
public:
    static auto GetCurreentLang() { return current_lang_; }
    static void SetCurrentLang(const wxString &current_lang) {
        current_lang_ = current_lang; }

private:
    static wxString current_lang_;
};

// Knob rotate direction when the value is increasing
enum SVCKnobIncreaseDirection {
    Right,
    Left
};

enum SVCKnobDeviceType {
    Mouse,
    HID
};

// Information for binding the button to the key
struct SVCButtonBindInfo {
    // RawInput device list
    RAWINPUTDEVICELIST device_list = { .hDevice = nullptr };

    // Usage page
    unsigned short usage_page = 0;
    // Usage id
    unsigned short usage_id = 0;

    // Key code
    unsigned char key = 0;
};

// Information for binding the knob to the key
struct SVCKnobBindInfo {
    // Knob device type
    SVCKnobDeviceType type = HID;

    // RawInput device list
    RAWINPUTDEVICELIST device_list = { .hDevice = nullptr };

    // Usage page
    unsigned short usage_page = 0;
    // Usage id
    unsigned short usage_id = 0;

    // Key code sent when the knob is rotates to the left
    unsigned char l_key = 0;
    // Key code sent when the knob is rotates to the right
    unsigned char r_key = 0;

    // Knob rotate direction when the value is increasing
    SVCKnobIncreaseDirection increase_direction = SVCKnobIncreaseDirection::Right;
};

// Controller binding info 
struct SVCControllBindProfile {
    wxString profile_name;
    // Knobs
    SVCKnobBindInfo knob_l,
                    knob_r;

    // START button
    SVCButtonBindInfo start;

    // BT buttons
    SVCButtonBindInfo bt_a,
                      bt_b,
                      bt_c,
                      bt_d;

    // FX buttons
    SVCButtonBindInfo fx_l,
                      fx_r;

    // Additional button bindings
    std::map<wxString, SVCButtonBindInfo> extra_buttons;

    SVCControllBindProfile(wxString profile_name_ = "default");
};

#endif // _SVCCONVERTER_SRC_CONFIG_DATA_H_