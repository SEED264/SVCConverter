#ifndef _SVCCONVERTER_SRC_CONFIG_DATA_H_
#define _SVCCONVERTER_SRC_CONFIG_DATA_H_

#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <limits>
#include <vector>
#include <windows.h>
extern "C" {
#include <hidsdi.h>
}

const unsigned long SVCUsageNotMapped = ULONG_MAX;

// Knob rotate direction when the value is increasing
enum SVCKnobIncreaseDirection {
    Right,
    Left
};

// Information for binding the button to the key
struct SVCButtonBindInfo {
    // RawInput device list
    RAWINPUTDEVICELIST device_list;

    // Usage page
    unsigned long usage_page;
    // Usage id
    unsigned long usage_id;

    // Key code
    unsigned char key;
};

// Information for binding the knob to the key
struct SVCKnobBindInfo {
    // RawInput device list
    RAWINPUTDEVICELIST device_list;

    // Usage page
    unsigned long usage_page;
    // Usage id
    unsigned long usage_id;

    // Key code sent when the knob is rotates to the left
    unsigned char l_key;
    // Key code sent when the knob is rotates to the right
    unsigned char r_key;

    // Knob rotate direction when the value is increasing
    SVCKnobIncreaseDirection increase_direction;
};

// Controller binding info 
struct SVCControllBindInfo {
    // Knobs
    SVCKnobBindInfo knob_l,
                    knob_r;

    // BT buttons
    SVCButtonBindInfo bt_a,
                      bt_b,
                      bt_c,
                      bt_d;

    // FX buttons
    SVCButtonBindInfo fx_l,
                      fx_r;

    // Additional button bindings
    std::vector<SVCButtonBindInfo> extra_buttons;
};

#endif // _SVCCONVERTER_SRC_CONFIG_DATA_H_