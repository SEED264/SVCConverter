#ifndef _SVCCONVERTER_SRC_INPUT_MANAGER_H_
#define _SVCCONVERTER_SRC_INPUT_MANAGER_H_

#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
extern "C" {
#include <hidsdi.h>
}

using RawInputDeviceLists = std::vector<RAWINPUTDEVICELIST>;
using HIDUsagePair = std::pair<USHORT, USHORT>;

struct RawInputValue {
    long min, max;
    unsigned long value;
};

class RawInputValueState {
public:
    RawInputValue& GetValue(const HIDUsagePair &usage);
    std::map<HIDUsagePair, RawInputValue> GetValues() { return values_; }

private:
    std::map<HIDUsagePair, RawInputValue> values_;
};

class RawInputButtonState {
public:
    std::unordered_map<USHORT, bool> GetUsages(USHORT usage_page) {
        return usages_[usage_page]; }
    void SetUsageRange(USHORT usage_page, USHORT usage_min, USHORT usage_max);
    void SetPressedUsages(USHORT usage_page, const std::vector<USAGE> &pressed_usages);

private:
    void SetAllUsageReleased(USHORT usage_page);
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> usages_;
    std::unordered_map<USHORT, std::pair<USHORT, USHORT>> usage_range_;
};

struct DeviceState {
    RawInputButtonState button_state;
    RawInputValueState value_state;
};

// Fetch specified device's input data via RawInput
class RawInputFetcher : public wxFrame {
public:
    RawInputFetcher(wxWindow *parent);

    void AddDevice(RAWINPUTDEVICELIST device_list);
    void RemoveDevice(RAWINPUTDEVICELIST device_list);

    bool IsDeviceContained(HANDLE device_handle);

    DeviceState GetDeviceState(HANDLE device_handle) {
        return device_states_[device_handle];}

    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

private:
    void InterpretRawInputData(PRAWINPUT raw_input);
    RawInputDeviceLists registered_devices_;
    std::unordered_map<HANDLE, DeviceState> device_states_;
};

// Manage RawInput devices by usage
class RawInputDeviceManager {
public:
    RawInputDeviceManager();
    void FetchDevices();
    RawInputDeviceLists GetDeviceLists(USHORT usage_page, USHORT usage);
    RawInputDeviceLists GetRawDeviceLists() const {
        return raw_device_lists_;
    }
 
private:
    using RawInputDeviceListsMap = std::map<HIDUsagePair, RawInputDeviceLists>;
    std::map<HIDUsagePair, RawInputDeviceLists> device_lists_;
    RawInputDeviceLists raw_device_lists_;
};

wxString GetHIDDeviceName(const RAWINPUTDEVICELIST &device_list);

wxString GetHIDProductName(const RAWINPUTDEVICELIST &device_list);

HIDUsagePair GetDeviceUsage(const RAWINPUTDEVICELIST &device_list);


#endif // _SVCCONVERTER_SRC_INPUT_MANAGER_H_