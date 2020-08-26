#include "setting_parser.h"
#include <fstream>
#include <nlohmann/json.hpp>

using nlohmann::json;

void ParseSetting() {
    std::ifstream setting_file(setting_file_name);
    if (!setting_file)
        return;
    json setting;
    setting_file >> setting;

    auto &lang_setting = setting["lang"];
    if (lang_setting.is_string())
        SVCSetting::SetCurrentLang(lang_setting.get<std::string>());
}

void DumpSettings() {
    json dump_setting;

    dump_setting["lang"] = SVCSetting::GetCurreentLang();

    std::ofstream setting_file(setting_file_name, std::ios::trunc);
    setting_file << dump_setting.dump(4);
}