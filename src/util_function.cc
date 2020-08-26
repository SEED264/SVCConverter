#include "util_function.h"
#include <rapidcsv.h>
#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif
#include "config_data.h"

void InitUIString() {
    auto csv_res = LoadBinaryResource("UI_STRING", "CSV");
    std::stringbuf csv_buf(reinterpret_cast<char*>(csv_res.data()));
    std::istream csv_stream(&csv_buf);
    rapidcsv::Document csv(csv_stream, rapidcsv::LabelParams(0, 0));


    // Set each word in [lang][id]
    auto langs = csv.GetColumnNames();
    auto ids = csv.GetRowNames();
    for (auto &lang : langs) {
        lang_ids.push_back(lang);
        auto lang_index = csv.GetColumnIdx(lang);
        for (auto &id : ids) {
            auto id_index = csv.GetRowIdx(id);
            ui_strings[lang][id] = wxString::FromUTF8(
                csv.GetCell<std::string>(lang_index, id_index));
        }
    }
}

wxString GetUIString(const wxString& string_name, const wxString& lang) {
    auto &ui_str = ui_strings[lang][string_name];
    if (ui_str.empty())
        return ui_strings["ENG"][string_name];

    return ui_str;
}

wxString GetUIString(const wxString& string_name) {
    return GetUIString(string_name, SVCSetting::GetCurreentLang());
}

std::vector<wxString> GetLangIDs() {
    return lang_ids;
}

std::vector<unsigned char> LoadBinaryResource(
        const wxString &resource_name,
        const wxString &resource_type) {
    auto instance = wxGetInstance();
    auto resource_info = FindResource(instance, resource_name, resource_type);
    auto resource_handle = LoadResource(instance, resource_info);
    auto *data = reinterpret_cast<unsigned char*>(LockResource(resource_handle));
    auto size = SizeofResource(instance, resource_info);
    std::vector<unsigned char> out_binary(size);

    std::copy(data, data + size, out_binary.begin());

    FreeResource(data);

    return out_binary;
}
