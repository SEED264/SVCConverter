#ifndef _SVCCONVERTER_SRC_CONFIG_DIALOG_H_
#define _SVCCONVERTER_SRC_CONFIG_DIALOG_H_

#include <opencv2/opencv.hpp>
#include <lodepng.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include <wx/grid.h>
#include "config_data.h"
#include "input_manager.h"

class ConfigDialog : public wxDialog {
public:
    ConfigDialog(wxWindow *parent, const std::vector<SVCControllBindProfile> &profiles,
                 unsigned int using_profile);
    ~ConfigDialog();

    auto& GetProfiles() const { return profiles_; }
    bool IsEdited() const { return is_edited_; }
    int GetCurrentProfileIndex() const { return current_profile_index_; }

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
    RawInputFetcher *fetcher_ = nullptr;
    bool is_edited_ = false;
    int current_profile_index_;

    wxPanel *image_panel_ = nullptr;

    cv::Mat panel_image_;
    cv::Mat start_image_;
    cv::Mat bt_a_image_;
    cv::Mat bt_b_image_;
    cv::Mat bt_c_image_;
    cv::Mat bt_d_image_;
    cv::Mat fx_l_image_;
    cv::Mat fx_r_image_;

    wxTimer timer_;

    wxChoice *choice_profile_;
    wxGrid *grid_knob_;
    wxGrid *grid_button_;
    wxGrid *grid_extra_button_;

    void InitGUI();
    void InitImages();
    std::vector<unsigned char> LoadBinaryResource(const wxString &resource_name,
                                                  const wxString &resource_type);
    std::vector<unsigned char> LoadBinaryFile(const std::string &file_name);
    cv::Mat DecodeToMat(const std::vector<unsigned char> &source_image);
    wxBitmap ConvertToBitmap(const cv::Mat &source_image);

    void SelectProfile(unsigned int profile_index);
    void SetProfileToRow(wxGrid *grid, int row, const wxString &label,
                         const wxString &product_name, USHORT usage_page, USHORT usage_id,
                         unsigned char key);

    bool SetButton(SVCButtonBindInfo &info);
    bool SetKnob(SVCKnobBindInfo &info);
    bool SetKnobAsMouse(SVCKnobBindInfo &info);
    bool SetKey(unsigned char *out_key);
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
    void OnSelectKnobGridAsMouse(wxGridEvent &event);
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