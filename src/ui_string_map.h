#ifndef _SVCCONVERTER_SRC_UI_STRING_MAP_H_
#define _SVCCONVERTER_SRC_UI_STRING_MAP_H_


#include <unordered_map>
#include <vector>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP

using MultiLanguageStringMap =
        std::unordered_map<wxString, std::unordered_map<wxString, wxString>>;

static MultiLanguageStringMap ui_strings;

static std::vector<wxString> lang_ids;

#endif // _SVCCONVERTER_SRC_UI_STRING_MAP_H_