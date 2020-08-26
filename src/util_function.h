#ifndef _SVCCONVERTER_SRC_UTIL_FUNCTION_H_
#define _SVCCONVERTER_SRC_UTIL_FUNCTION_H_

#include "ui_string_map.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP

void InitUIString();

wxString GetUIString(const wxString& string_name, const wxString& lang);
wxString GetUIString(const wxString& string_name);

std::vector<wxString> GetLangIDs();

std::vector<unsigned char> LoadBinaryResource(
        const wxString &resource_name,
        const wxString &resource_type);

#endif // _SVCCONVERTER_SRC_UTIL_FUNCTION_H_