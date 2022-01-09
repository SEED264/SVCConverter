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
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> GetUsages() {
        return usages_; }
    void SetUsageRange(USHORT usage_page, USHORT usage_min, USHORT usage_max);
    void SetPressedUsages(USHORT usage_page, const std::vector<USAGE> &pressed_usages);

private:
    void SetAllUsageReleased(USHORT usage_page);
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> usages_;
    std::unordered_map<USHORT, std::pair<USHORT, USHORT>> usage_range_;
};

class RawInputMouseState {
public:
    void SetMouseState(const RAWMOUSE &mouse);
    void ResetDifferences();

    auto GetX() const { return x_values_; }
    auto GetY() const { return y_values_; }
    auto GetXDifferences() const { return x_differences_; }
    auto GetYDifferences() const { return y_differences_; }
    //auto GetXDifferencesWithReset() {
    //    auto diff = x_differences_;
    //    x_differences_ = 0;
    //    return diff; }
    //auto GetYDifferencesWithReset() {
    //    auto diff = y_differences_;
    //    y_differences_ = 0;
    //    return diff; }


private:
    bool is_first_record_ = false;
    long x_values_ = 0;
    long y_values_ = 0;
    long x_differences_ = 0;
    long y_differences_ = 0;
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

    void RemoveAllDevice();

    bool IsDeviceContained(HANDLE device_handle);

    auto GetRegisteredDevices() const { return registered_devices_; }
    auto& GetMouseState(HANDLE device_handle) {
        return mouse_states_[device_handle]; }
    DeviceState GetDeviceState(HANDLE device_handle) {
        return device_states_[device_handle]; }

    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

private:
    void InterpretRawInputHIDData(PRAWINPUT raw_input);
    void InterpretRawInputMouseData(PRAWINPUT raw_input);
    RawInputDeviceLists registered_devices_;
    std::unordered_map<HANDLE, RawInputMouseState> mouse_states_;
    std::unordered_map<HANDLE, DeviceState> device_states_;
};

// Manage RawInput devices by usage
class RawInputDeviceManager {
public:
    RawInputDeviceManager();
    void FetchDevices();
    RawInputDeviceLists GetMouseDeviceLists() const { return mouse_device_lists_; }
    RawInputDeviceLists GetHIDDeviceLists(USHORT usage_page, USHORT usage);
    RawInputDeviceLists GetRawDeviceLists() const {
        return raw_device_lists_;
    }

private:
    using RawInputDeviceListsMap = std::map<HIDUsagePair, RawInputDeviceLists>;
    RawInputDeviceLists mouse_device_lists_;
    std::map<HIDUsagePair, RawInputDeviceLists> hid_device_lists_;
    RawInputDeviceLists raw_device_lists_;
};

wxString GetHIDDeviceName(const RAWINPUTDEVICELIST &device_list);

wxString GetHIDProductName(const RAWINPUTDEVICELIST &device_list);

wxString ExtractHIDDeviceIDs(const wxString &device_name);

HIDUsagePair GetDeviceUsage(const RAWINPUTDEVICELIST &device_list);


#endif // _SVCCONVERTER_SRC_INPUT_MANAGER_H_