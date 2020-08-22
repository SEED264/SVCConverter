#ifndef _SVCCONVERTER_SRC_CONFIG_PARSER_H_
#define _SVCCONVERTER_SRC_CONFIG_PARSER_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "config_data.h"
#include "input_manager.h"

static const std::string config_file_name("svc_converter_config.json");

class SVCConfigParser {
public:
    std::vector<SVCControllBindProfile> ParseProfiles();

    void DumpProfiles(const std::vector<SVCControllBindProfile> &profiles);

    SVCControllBindProfile CreateDefaultProfile();

private:
    void ParseButtonBindInfo(const nlohmann::json &config, SVCButtonBindInfo *out_info,
                             const RawInputDeviceManager &device_manager);
    void ParseKnobBindInfo(const nlohmann::json &config, SVCKnobBindInfo *out_info,
                           const RawInputDeviceManager &device_manager);

    void ParseDevice(const nlohmann::json &config, RAWINPUTDEVICELIST *out_device_list,
                     const RawInputDeviceManager &device_manager);
    void ParseKey(const nlohmann::json &config, unsigned char *out_key);
    void ParseKnobIncreaseDirection(const nlohmann::json &config,
                                    SVCKnobIncreaseDirection *out_direction);

    void GetUsage(const nlohmann::json &config,
                  unsigned short *out_usage_page, unsigned short *out_usage_id);

    void DumpButtonBindInfo(nlohmann::json &out_config, const SVCButtonBindInfo &info);
    void DumpKnobBindInfo(nlohmann::json &out_config, const SVCKnobBindInfo &info);

    void DumpKnobIncreaseDirection(nlohmann::json &out_config,
                                   SVCKnobIncreaseDirection direction);
};

#endif // _SVCCONVERTER_SRC_CONFIG_PARSER_H_