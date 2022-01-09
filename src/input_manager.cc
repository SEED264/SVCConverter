#include "input_manager.h"
#include "cstdlib"

RawInputFetcher::RawInputFetcher(wxWindow *parent)
    : wxFrame(parent, wxID_ANY, "Fetcher") {}

RawInputValue& RawInputValueState::GetValue(const HIDUsagePair &usage) {
    if (!values_.count(usage)) {
        values_[usage] = RawInputValue();
    }
    return values_[usage];
}

void RawInputButtonState::SetUsageRange(USHORT usage_page, USHORT usage_min, USHORT usage_max) {
    if (usage_range_.count(usage_page))
        return;

    usage_range_[usage_page] = std::pair<USHORT, USHORT>(usage_min, usage_max);
    std::unordered_map<USHORT, bool> usages;
    usages_[usage_page] = usages;
}

void RawInputButtonState::SetPressedUsages(USHORT usage_page,
                                           const std::vector<USAGE> &pressed_usages) {
    auto &usages = usages_[usage_page];
    // Reset before set each pressed flag
    SetAllUsageReleased(usage_page);

    for (auto pressed_usage : pressed_usages) {
        usages[pressed_usage] = true;
    }
}

void RawInputButtonState::SetAllUsageReleased(USHORT usage_page) {
    auto &usages = usages_[usage_page];
    auto range = usage_range_[usage_page];
    for (size_t i = range.first; i <= range.second; ++i) {
        usages[i] = false;
    }
}

void RawInputMouseState::SetMouseState(const RAWMOUSE& mouse) {
    switch (mouse.usFlags) {
    case MOUSE_MOVE_RELATIVE: {
        x_differences_ += mouse.lLastX;
        y_differences_ += mouse.lLastY;
        break;
    }
    case MOUSE_MOVE_ABSOLUTE: {
        if (!is_first_record_) {
            x_differences_ += mouse.lLastX - x_values_;
            y_differences_ += mouse.lLastY - y_values_;
        }
        x_values_ = mouse.lLastX;
        y_values_ = mouse.lLastY;
        is_first_record_ = true;
        break;
    }
    }
}

void RawInputMouseState::ResetDifferences() {
    x_differences_ = 0;
    y_differences_ = 0;
}

void RawInputFetcher::AddDevice(RAWINPUTDEVICELIST device_list) {
    if (!IsDeviceContained(device_list.hDevice))
        registered_devices_.push_back(device_list);

    // Register device
    auto usage = GetDeviceUsage(device_list);
    RAWINPUTDEVICE device;
    device.usUsagePage = usage.first;
    device.usUsage = usage.second;
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = GetHWND();
    RegisterRawInputDevices(&device, 1, sizeof(device));
    //device_states_[device_list.hDevice] = DeviceState();
}

void RawInputFetcher::RemoveDevice(RAWINPUTDEVICELIST device_list) {
    // Find target device list
    auto compare_device_list = [&](const RAWINPUTDEVICELIST &dl) {
        return dl.hDevice == device_list.hDevice;
    };
    auto target_device = std::find_if(registered_devices_.begin(), registered_devices_.end(),
                                      compare_device_list);
    // Do nothing if device not found
    if (target_device == registered_devices_.end())
        return;

    registered_devices_.erase(target_device);
    // Unregister device if usage only used by it
    auto target_usage = GetDeviceUsage(device_list);
    bool used_by_other = false;
    // Check is target device's usage used by other device
    for (auto &device : registered_devices_) {
        auto device_usage = GetDeviceUsage(device);
        if (device_usage == target_usage) {
            used_by_other = true;
            break;
        }
    }
    if (!used_by_other) {
        RAWINPUTDEVICE device;
        device.usUsagePage = target_usage.first;
        device.usUsage = target_usage.second;
        device.dwFlags = RIDEV_REMOVE;
        device.hwndTarget = GetHWND();
        RegisterRawInputDevices(&device, 1, sizeof(device));
    }
    device_states_.erase(target_device->hDevice);
}

void RawInputFetcher::RemoveAllDevice() {
    for (auto& registered_device : registered_devices_) {
        auto usage = GetDeviceUsage(registered_device);
        RAWINPUTDEVICE device;
        device.usUsagePage = usage.first;
        device.usUsage = usage.second;
        device.dwFlags = RIDEV_REMOVE;
        device.hwndTarget = GetHWND();
        RegisterRawInputDevices(&device, 1, sizeof(device));
    }
    registered_devices_.resize(0);
}

bool RawInputFetcher::IsDeviceContained(HANDLE device_handle) {
    for (auto &device_list : registered_devices_) {
        if (device_handle == device_list.hDevice)
            return true;
    }
    return false;
}

void RawInputFetcher::InterpretRawInputHIDData(PRAWINPUT raw_input) {
    HANDLE device = raw_input->header.hDevice;
    // Create device state if doesn't exists
    if (!device_states_.count(device)) {
        device_states_[device] = DeviceState();
    }
    auto &device_state = device_states_[device];

    // Get prepersed data
    unsigned int buffer_size;
    GetRawInputDeviceInfo(device, RIDI_PREPARSEDDATA, nullptr                , &buffer_size);
    std::vector<char> prepersed_buffer(buffer_size);
    GetRawInputDeviceInfo(device, RIDI_PREPARSEDDATA, prepersed_buffer.data(), &buffer_size);
    PHIDP_PREPARSED_DATA prepersed_data =
        reinterpret_cast<PHIDP_PREPARSED_DATA>(prepersed_buffer.data());

    // Get capability data
    HIDP_CAPS caps;
    HidP_GetCaps(prepersed_data, &caps);

    // Get button capabilities and states
    auto &button_state = device_state.button_state;
    unsigned short button_caps_num = caps.NumberInputButtonCaps;
    std::vector<HIDP_BUTTON_CAPS> button_caps(button_caps_num);
    HidP_GetButtonCaps(HidP_Input, button_caps.data(), &button_caps_num, prepersed_data);
    // Set each usage page's usage range and get pressed usage
    for (auto &bcap : button_caps) {
        button_state.SetUsageRange(bcap.UsagePage, bcap.Range.UsageMin, bcap.Range.UsageMax);
        unsigned long usage_length = bcap.Range.UsageMax - bcap.Range.UsageMin + 1;
        std::vector<USAGE> usages(usage_length);
        HidP_GetUsages(HidP_Input, bcap.UsagePage, 0, usages.data(), &usage_length,
                       prepersed_data, reinterpret_cast<char*>(raw_input->data.hid.bRawData),
                       raw_input->data.hid.dwSizeHid);
        usages.resize(usage_length);
        button_state.SetPressedUsages(bcap.UsagePage, usages);
    }

    // Get button capabilities and states
    auto &value_state = device_state.value_state;
    unsigned short value_caps_num = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> value_caps(value_caps_num);
    HidP_GetValueCaps(HidP_Input, value_caps.data(), &value_caps_num, prepersed_data);
    // Get each value and caps
    for (auto &vcap : value_caps) {
        auto &states_value = value_state.GetValue(
            HIDUsagePair(vcap.UsagePage, vcap.Range.UsageMin));
        // Set range
        states_value.min = 0;
        states_value.max = static_cast<long>((1ll << vcap.BitSize) - 1);
        unsigned long input_value;
        HidP_GetUsageValue(HidP_Input, vcap.UsagePage, 0, vcap.Range.UsageMin,
                           &input_value, prepersed_data,
                           reinterpret_cast<char*>(raw_input->data.hid.bRawData),
                           raw_input->data.hid.dwSizeHid);
        states_value.value = input_value;
    }
}

void RawInputFetcher::InterpretRawInputMouseData(PRAWINPUT raw_input) {
    auto &state = mouse_states_[raw_input->header.hDevice];
    state.SetMouseState(raw_input->data.mouse);
}

WXLRESULT RawInputFetcher::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) {
    switch (nMsg) {
    case WM_INPUT: {
        PRAWINPUT raw_input;
        unsigned int buffer_size;

        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr,
                        &buffer_size, sizeof(RAWINPUTHEADER));
        std::vector<char> raw_input_buffer(buffer_size);
        raw_input = reinterpret_cast<PRAWINPUT>(raw_input_buffer.data());

        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, raw_input,
                        &buffer_size, sizeof(RAWINPUTHEADER));

        HANDLE device = raw_input->header.hDevice;
        // Do nothing if device is not registerd
        if (!IsDeviceContained(device))
            return 0;

        switch (raw_input->header.dwType) {
        case RIM_TYPEMOUSE: {
            InterpretRawInputMouseData(raw_input);
            break;
        }
        case RIM_TYPEHID: {
            InterpretRawInputHIDData(raw_input);
            break;
        }
        }

        break;
    }
    }

    return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
}

RawInputDeviceManager::RawInputDeviceManager() {
    FetchDevices();
}

void RawInputDeviceManager::FetchDevices() {
    // Init map of device lists
    hid_device_lists_ = RawInputDeviceListsMap();
    raw_device_lists_ = RawInputDeviceLists();

    // Get all device lists
    RawInputDeviceLists device_lists;
    unsigned int device_list_num = 0;
    GetRawInputDeviceList(nullptr,             &device_list_num, sizeof(RAWINPUTDEVICELIST));
    device_lists.resize(device_list_num);
    GetRawInputDeviceList(device_lists.data(), &device_list_num, sizeof(RAWINPUTDEVICELIST));

    raw_device_lists_ = device_lists;

    // Divide up device lists to map
    for (auto &device_list : device_lists) {
        if (device_list.dwType == RIM_TYPEMOUSE) {
            mouse_device_lists_.push_back(device_list);
            continue;
        }
        auto key = GetDeviceUsage(device_list);
        // Create new lists if not exists
        if (!hid_device_lists_.count(key))
            hid_device_lists_[key] = RawInputDeviceLists();

        hid_device_lists_[key].push_back(device_list);
    }
}

RawInputDeviceLists RawInputDeviceManager::GetHIDDeviceLists(USHORT usage_page, USHORT usage_id) {
    HIDUsagePair usage(usage_page, usage_id);
    // Return empty lists if not found
    if (!hid_device_lists_.count(usage)) {
        return RawInputDeviceLists();
    }
    return hid_device_lists_[usage];
}

wxString GetHIDDeviceName(const RAWINPUTDEVICELIST &device_list) {
    // Get device name size(byte)
    unsigned int device_name_length;
    GetRawInputDeviceInfo(device_list.hDevice, RIDI_DEVICENAME, nullptr           , &device_name_length);
    std::vector<wxChar> device_name(device_name_length);
    GetRawInputDeviceInfo(device_list.hDevice, RIDI_DEVICENAME, device_name.data(), &device_name_length);

    // Return string from char vector
    return wxString(device_name.data());
}

wxString GetHIDProductName(const RAWINPUTDEVICELIST &device_list) {
    // Get HID device_name
    wxString device_name = GetHIDDeviceName(device_list);

    std::vector<wchar_t> product_name(1024);
    // Open HID device
    HANDLE hid_handle = CreateFile(device_name.data(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   nullptr, OPEN_EXISTING, 0, nullptr);
    // Get devices product string
    HidD_GetProductString(hid_handle, product_name.data(), sizeof(wchar_t) * product_name.size());
    // Close HID device
    CloseHandle(hid_handle);

    return wxString(product_name.data());
}

wxString ExtractHIDDeviceIDs(const wxString &device_name) {
    auto first = device_name.find('#');
    auto second = device_name.find('#', first + 1);
    if (first == -1 || second == -1)
        return "";

    return device_name.SubString(first + 1, second - 1);
}

HIDUsagePair GetDeviceUsage(const RAWINPUTDEVICELIST &device_list) {
    RID_DEVICE_INFO info;
    info.cbSize = sizeof(RID_DEVICE_INFO);
    unsigned int info_size = info.cbSize;
    GetRawInputDeviceInfo(device_list.hDevice, RIDI_DEVICEINFO, &info, &info_size);

    // Set usage
    USHORT usage_page;
    USHORT usage_id;
    switch (info.dwType) {
    case RIM_TYPEMOUSE: {
        usage_page = 0x01;
        usage_id = 0x02;
        break;
    }
    case RIM_TYPEKEYBOARD: {
        usage_page = 0x01;
        usage_id = 0x06;
        break;
    }
    case RIM_TYPEHID: {
        RID_DEVICE_INFO_HID &hid = info.hid;
        usage_page = hid.usUsagePage;
        usage_id = hid.usUsage;
        break;
    }
    }

    return HIDUsagePair(usage_page, usage_id);
}