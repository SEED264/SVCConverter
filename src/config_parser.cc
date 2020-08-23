#include "config_parser.h"
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

std::vector<SVCControllBindProfile> SVCConfigParser::ParseProfiles() {
    std::vector<SVCControllBindProfile> profiles;
    std::ifstream config_file(config_file_name);
    // Return default profile if failed to open
    if (!config_file) {
        profiles.push_back(CreateDefaultProfile());
        return profiles;
    }
    json config;
    config_file >> config;
    config_file.close();
    // Reserve profiles
    profiles.reserve(config.size());
    RawInputDeviceManager device_manager;
    if (config.is_object()) {
        // Parse all profiles
        for (auto pf_it = config.begin(); pf_it != config.end(); ++pf_it) {
            // Do nothing if profile name is empty
            auto profile_name = pf_it.key();
            if (profile_name.empty())
                continue;
            SVCControllBindProfile profile = CreateDefaultProfile();
            profile.profile_name = profile_name;
            auto &pf_js = pf_it.value();
            ParseKnobBindInfo(pf_js["knob_l"], &profile.knob_l, device_manager);
            ParseKnobBindInfo(pf_js["knob_r"], &profile.knob_r, device_manager);
            ParseButtonBindInfo(pf_js["start"], &profile.start, device_manager);
            ParseButtonBindInfo(pf_js["bt_a"], &profile.bt_a, device_manager);
            ParseButtonBindInfo(pf_js["bt_b"], &profile.bt_b, device_manager);
            ParseButtonBindInfo(pf_js["bt_c"], &profile.bt_c, device_manager);
            ParseButtonBindInfo(pf_js["bt_d"], &profile.bt_d, device_manager);
            ParseButtonBindInfo(pf_js["fx_l"], &profile.fx_l, device_manager);
            ParseButtonBindInfo(pf_js["fx_r"], &profile.fx_r, device_manager);
            // Parse extra buttons
            auto extra_buttons = pf_js["ex_bt"];
            if (extra_buttons.is_object()) {
                for (auto ex_bt = extra_buttons.begin(); ex_bt != extra_buttons.end(); ++ex_bt) {
                    // Do nothing if profile name is empty
                    auto ex_profile_name = ex_bt.key();
                    if (ex_profile_name.empty())
                        continue;
                    ParseButtonBindInfo(ex_bt.value(), &profile.extra_buttons[ex_profile_name],
                                        device_manager);
                }
            }
            profiles.push_back(profile);
        }
    }

    return profiles;
}

void SVCConfigParser::DumpProfiles(const std::vector<SVCControllBindProfile> &profiles) {
    json dump_config;
    for (auto &profile : profiles) {
        auto profile_name = profile.profile_name.ToStdString();
        if (profile_name.empty())
            continue;
        auto &config = dump_config[profile_name];
        DumpKnobBindInfo(config["knob_l"], profile.knob_l);
        DumpKnobBindInfo(config["knob_r"], profile.knob_r);
        DumpButtonBindInfo(config["start"], profile.start);
        DumpButtonBindInfo(config["bt_a"], profile.bt_a);
        DumpButtonBindInfo(config["bt_b"], profile.bt_b);
        DumpButtonBindInfo(config["bt_c"], profile.bt_c);
        DumpButtonBindInfo(config["bt_d"], profile.bt_d);
        DumpButtonBindInfo(config["fx_l"], profile.fx_l);
        DumpButtonBindInfo(config["fx_r"], profile.fx_r);
        // Dump extra buttons
        auto &extra_buttons = profile.extra_buttons;
        auto &ex_config = config["ex_bt"];
        for (auto eb = extra_buttons.begin(); eb != extra_buttons.end(); ++eb) {
            auto ex_profile_name = eb->first.ToStdString();
            auto &ex_profile = eb->second;
            bool is_valid = !ex_profile_name.empty()
                            && ex_profile.device_list.hDevice != nullptr
                            && ex_profile.usage_page != 0
                            && ex_profile.usage_id != 0
                            && ex_profile.key != 0;
            if (is_valid)
                DumpButtonBindInfo(ex_config[ex_profile_name], ex_profile);
        }
    }
    std::ofstream config_file(config_file_name, std::ios::trunc);
    config_file << dump_config.dump(4);
}

SVCControllBindProfile SVCConfigParser::CreateDefaultProfile() {
    SVCControllBindProfile profile;
    // Set default key
    profile.knob_l.l_key = 'A';
    profile.knob_l.r_key = 'S';
    profile.knob_r.l_key = 'L';
    profile.knob_r.r_key = VK_OEM_PLUS;
    profile.start.key = VK_RETURN;
    profile.bt_a.key = 'D';
    profile.bt_b.key = 'F';
    profile.bt_c.key = 'J';
    profile.bt_d.key = 'K';
    profile.fx_l.key = 'C';
    profile.fx_r.key = 'M';

    return profile;
}

void SVCConfigParser::ParseButtonBindInfo(const nlohmann::json &config,
                                          SVCButtonBindInfo *out_info,
                                          const RawInputDeviceManager &device_manager) {
    // Do nothing if config isn't object
    if (!config.is_object())
        return;
    ParseDevice(config["device"]["device_name"], &out_info->device_list, device_manager);
    GetUsage(config, &out_info->usage_page, &out_info->usage_id);
    ParseKey(config["key"], &out_info->key);
}

void SVCConfigParser::ParseKnobBindInfo(const nlohmann::json &config,
                                        SVCKnobBindInfo *out_info,
                                        const RawInputDeviceManager &device_manager) {
    // Do nothing if config isn't object
    if (!config.is_object())
        return;
    ParseDevice(config["device"]["device_name"], &out_info->device_list, device_manager);
    GetUsage(config, &out_info->usage_page, &out_info->usage_id);
    ParseKey(config["l_key"], &out_info->l_key);
    ParseKey(config["r_key"], &out_info->r_key);
    ParseKnobIncreaseDirection(config["direction"], &out_info->increase_direction);
}

void SVCConfigParser::ParseDevice(const nlohmann::json &config,
                                  RAWINPUTDEVICELIST *out_device_list,
                                  const RawInputDeviceManager &device_manager) {
    if (!config.is_string())
        return;
    auto device_name = config.get<std::string>();
    auto device_lists = device_manager.GetRawDeviceLists();
    // Return device list if the device names match
    for (auto &device_list : device_lists) {
        if (GetHIDDeviceName(device_list) == device_name) {
            *out_device_list = device_list;
            break;
        }
    }
}

void SVCConfigParser::ParseKey(const nlohmann::json &config, unsigned char *out_key) {
    if (!config.is_number_integer() || !config.is_number_unsigned())
        return;
    *out_key = config.get<unsigned char>();
}

void SVCConfigParser::ParseKnobIncreaseDirection(const nlohmann::json &config,
                                                 SVCKnobIncreaseDirection *out_direction) {
    if (!config.is_string())
        return;
    // Get config string and convert to lower case
    auto direction_str = config.get<std::string>();
    if (direction_str == "Right")
        *out_direction = SVCKnobIncreaseDirection::Right;
    else if (direction_str == "Left")
        *out_direction = SVCKnobIncreaseDirection::Left;
}

void SVCConfigParser::GetUsage(const nlohmann::json &config,
                               unsigned short *out_usage_page, unsigned short *out_usage_id) {
    auto &usage_page = config["usage_page"];
    auto &usage_id = config["usage_id"];
    if (!usage_page.is_number_integer() || !usage_page.is_number_unsigned()
        || !usage_id.is_number_integer() || !usage_id.is_number_unsigned())
        return;
    *out_usage_page = usage_page.get<unsigned char>();
    *out_usage_id = usage_id.get<unsigned char>();
}

void SVCConfigParser::DumpButtonBindInfo(nlohmann::json &out_config,
                                         const SVCButtonBindInfo &info) {
    out_config["device"]["device_name"] = GetHIDDeviceName(info.device_list);
    out_config["device"]["product_name"] = GetHIDProductName(info.device_list);
    out_config["usage_page"] = info.usage_page;
    out_config["usage_id"] = info.usage_id;
    out_config["key"] = info.key;
}

void SVCConfigParser::DumpKnobBindInfo(nlohmann::json &out_config,
                                       const SVCKnobBindInfo &info) {
    out_config["device"]["device_name"] = GetHIDDeviceName(info.device_list);
    out_config["device"]["product_name"] = GetHIDDeviceName(info.device_list);
    out_config["usage_page"] = info.usage_page;
    out_config["usage_id"] = info.usage_id;
    out_config["l_key"] = info.l_key;
    out_config["r_key"] = info.r_key;
    DumpKnobIncreaseDirection(out_config["direction"], info.increase_direction);
}

void SVCConfigParser::DumpKnobIncreaseDirection(nlohmann::json &out_config,
                                                SVCKnobIncreaseDirection direction) {
    switch(direction) {
    case Right: {
        out_config = "Right";
        break;
    }
    case Left: {
        out_config = "Left";
        break;
    }
    }
}