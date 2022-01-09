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
    void ParseButtonBindInfo(nlohmann::json &config, SVCButtonBindInfo *out_info,
                             const RawInputDeviceManager &device_manager);
    void ParseKnobBindInfo(nlohmann::json &config, SVCKnobBindInfo *out_info,
                           const RawInputDeviceManager &device_manager);

    void ParseDevice(const nlohmann::json &device_name_config,
                     const nlohmann::json &product_name_config,
                     RAWINPUTDEVICELIST *out_device_list,
                     std::string *out_device_name,
                     std::string *out_product_name,
                     const RawInputDeviceManager &device_manager);
    void ParseKnobDeviceType(const nlohmann::json &config, SVCKnobDeviceType *out_device_type);
    void ParseKey(const nlohmann::json &config, unsigned char *out_key);
    void ParseKnobIncreaseDirection(const nlohmann::json &config,
                                    SVCKnobIncreaseDirection *out_direction);

    void GetUsage(const nlohmann::json &config,
                  unsigned short *out_usage_page, unsigned short *out_usage_id);

    void DumpButtonBindInfo(nlohmann::json &out_config, const SVCButtonBindInfo &info);
    void DumpKnobBindInfo(nlohmann::json &out_config, const SVCKnobBindInfo &info);

    void DumpKnobDeviceType(nlohmann::json &out_config, SVCKnobDeviceType device_type);
    void DumpKnobIncreaseDirection(nlohmann::json &out_config,
                                   SVCKnobIncreaseDirection direction);
};

#endif // _SVCCONVERTER_SRC_CONFIG_PARSER_H_