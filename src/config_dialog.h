#ifndef _SVCCONVERTER_SRC_CONFIG_DIALOG_H_
#define _SVCCONVERTER_SRC_CONFIG_DIALOG_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include <wx/grid.h>
#include "config_data.h"

class ConfigDialog : public wxDialog {
public:
    ConfigDialog(wxWindow *parent, const std::vector<SVCControllBindProfile> &profiles,
                 unsigned int using_profile);

    auto& GetProfiles() const { return profiles_; }
    bool IsEdited() const { return is_edited_; }

private:
    enum {
        ID_CHOICE_PROFILE,
        ID_BUTTON_PROFILE_ADD,
        ID_BUTTON_PROFILE_REMOVE,
        ID_BUTTON_PROFILE_RENAME,
        ID_GRID_KNOB,
        ID_GRID_BUTTON,
        ID_GRID_EXTRA_BUTTON,
        ID_BUTTON_EXTRA_ADD,
        ID_BUTTON_EXTRA_REMOVE
    };

    std::vector<SVCControllBindProfile> profiles_;
    SVCControllBindProfile *current_profile_;
    std::vector<wxString> extra_key_table_;
    bool is_edited_ = false;

    wxChoice *choice_profile_;
    wxGrid *grid_knob_;
    wxGrid *grid_button_;
    wxGrid *grid_extra_button_;

    void InitGUI();

    void SelectProfile(unsigned int profile_index);
    void SetProfileToRow(wxGrid *grid, int row, const wxString &label,
                         const wxString &product_name, USHORT usage_page, USHORT usage_id,
                         unsigned char key);

    void SetButton(SVCButtonBindInfo &info);
    void SetKnob(SVCKnobBindInfo &info);
    void SetKey(unsigned char *out_key);
    void AddExtraButton();
    void RemoveExtraButton();
    void SyncExtraButtonView();
    void RefreshProfileChoice();

    void OnTimer(wxTimerEvent &event);
    void OnChoiceProfile(wxCommandEvent &event);
    void OnAddProfile(wxCommandEvent &event);
    void OnRemoveProfile(wxCommandEvent &event);
    void OnRenameProfile(wxCommandEvent &event);
    void OnAddExtraButton(wxCommandEvent &event);
    void OnRemoveExtraButton(wxCommandEvent &event);
    void OnSelectButtonGrid(wxGridEvent &event);
    void OnSelectKnobGrid(wxGridEvent &event);
    void OnSelectExtraButtonGrid(wxGridEvent &event);
    void ValidateExtraButtonName(wxGridEvent &event);
    void SetCursorToRowHead(wxGridEvent &event);
    void ClearRangeSelection(wxGridRangeSelectEvent &event);
};

class ProfileNameDialog : public wxDialog {
public:
    ProfileNameDialog(wxWindow *parent, const wxString &default_name = "");

    auto GetProfileName() const { return edit_name_->GetValue(); }
    auto IsNameEntered() const { return is_entered_; }

private:
    enum {
        ID_BUTTON_OK
    };
    bool is_entered_ = false;

    wxTextCtrl *edit_name_ = nullptr;
};

#endif // _SVCCONVERTER_SRC_CONFIG_DIALOG_H_