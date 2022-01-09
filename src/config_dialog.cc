#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include "config_dialog.h"
#include <fstream>
#include <random>
#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif
#include <winsock2.h>
#include <windows.h>
#include "input_manager.h"
#include "label_string.h"
#include "binding_dialog.h"
#include "util_function.h"

ConfigDialog::ConfigDialog(wxWindow *parent,
                           const std::vector<SVCControllBindProfile> &profiles,
                           unsigned int using_profile)
        : wxDialog(parent, wxID_ANY, "Config")
        , profiles_(profiles)
        , timer_(this) {
    InitImages();
    InitGUI();
    RefreshProfileChoice();
    SelectProfile(using_profile);
}

ConfigDialog::~ConfigDialog() {
    if (fetcher_ != nullptr)
        delete fetcher_;
}

void ConfigDialog::InitGUI() {
    auto *panel = new wxPanel(this);
    image_panel_ = new wxPanel(panel, wxID_ANY);
    image_panel_->SetMinSize(wxSize(960, 540));

    grid_knob_ = new wxGrid(panel, ID_GRID_KNOB, wxDefaultPosition,
                            wxDefaultSize, wxBORDER_SIMPLE);
    grid_button_ = new wxGrid(panel, ID_GRID_BUTTON, wxDefaultPosition,
                              wxDefaultSize, wxBORDER_SIMPLE);
    grid_extra_button_ = new wxGrid(panel, ID_GRID_EXTRA_BUTTON, wxDefaultPosition,
                                    wxDefaultSize, wxBORDER_SIMPLE);

    choice_profile_ = new wxChoice(panel, ID_CHOICE_PROFILE, wxDefaultPosition, wxDefaultSize,
                                   wxArrayString());
    auto *button_add_profile = new wxButton(panel, ID_BUTTON_PROFILE_ADD,
                                            GetUIString("button_add"));
    auto *button_remove_profile = new wxButton(panel, ID_BUTTON_PROFILE_REMOVE,
                                               GetUIString("button_remove"));
    auto *button_rename_profile = new wxButton(panel, ID_BUTTON_PROFILE_RENAME,
                                               GetUIString("button_rename"));

    auto *button_extra_add = new wxButton(panel, ID_BUTTON_EXTRA_ADD,
                                          GetUIString("button_add"));
    auto *button_extra_remove = new wxButton(panel, ID_BUTTON_EXTRA_REMOVE,
                                             GetUIString("button_remove"));

    // Create grid
    grid_knob_->CreateGrid(4, 4);
    grid_button_->CreateGrid(7, 4);
    grid_extra_button_->CreateGrid(0, 4);

    // Hide label
    grid_knob_->SetRowLabelSize(0);
    grid_button_->SetRowLabelSize(0);
    grid_extra_button_->SetRowLabelSize(0);

    // Disable grid resize
    for (int col = 0; col < grid_knob_->GetNumberCols(); ++col) {
        grid_knob_->DisableColResize(col);
    }
    for (int row = 0; row < grid_knob_->GetNumberRows(); ++row) {
        grid_knob_->DisableRowResize(row);
    }

    for (int col = 0; col < grid_button_->GetNumberCols(); ++col) {
        grid_button_->DisableColResize(col);
    }
    for (int row = 0; row < grid_button_->GetNumberRows(); ++row) {
        grid_button_->DisableRowResize(row);
    }

    for (int col = 0; col < grid_extra_button_->GetNumberCols(); ++col) {
        grid_extra_button_->DisableColResize(col);
    }

    // Set col size
    grid_knob_->SetColSize(0, 120);
    grid_knob_->SetColSize(1, 200);
    grid_knob_->SetColSize(2, 100);
    grid_knob_->SetColSize(3, 100);
    grid_button_->SetColSize(0, 120);
    grid_button_->SetColSize(1, 200);
    grid_button_->SetColSize(2, 100);
    grid_button_->SetColSize(3, 100);
    grid_extra_button_->SetColSize(0, 120);
    grid_extra_button_->SetColSize(1, 200);
    grid_extra_button_->SetColSize(2, 100);
    grid_extra_button_->SetColSize(3, 100);

    // Disable knob and button's edit
    grid_knob_->EnableEditing(false);
    grid_button_->EnableEditing(false);

    // Disable focused cell's border
    grid_knob_->SetCellHighlightPenWidth(1);
    grid_button_->SetCellHighlightPenWidth(1);
    grid_extra_button_->SetCellHighlightPenWidth(1);

    // Set grid label
    grid_knob_->SetColLabelValue(0, "");
    grid_knob_->SetColLabelValue(1, GetUIString("grid_label_config_controller"));
    grid_knob_->SetColLabelValue(2, GetUIString("grid_label_config_input"));
    grid_knob_->SetColLabelValue(3, GetUIString("grid_label_config_key"));
    grid_button_->SetColLabelValue(0, "");
    grid_button_->SetColLabelValue(1, GetUIString("grid_label_config_controller"));
    grid_button_->SetColLabelValue(2, GetUIString("grid_label_config_input"));
    grid_button_->SetColLabelValue(3, GetUIString("grid_label_config_key"));
    grid_extra_button_->SetColLabelValue(0, "");
    grid_extra_button_->SetColLabelValue(1, GetUIString("grid_label_config_controller"));
    grid_extra_button_->SetColLabelValue(2, GetUIString("grid_label_config_input"));
    grid_extra_button_->SetColLabelValue(3, GetUIString("grid_label_config_key"));

    // Set grid size
    grid_knob_->SetSize(grid_knob_->GetVirtualSize());
    grid_button_->SetSize(grid_knob_->GetVirtualSize());
    grid_extra_button_->SetSize(grid_knob_->GetVirtualSize(), 100);

    auto *sizer_dialog = new wxBoxSizer(wxVERTICAL);
    auto *sizer_control = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_profile = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_grid = new wxBoxSizer(wxVERTICAL);
    auto *sizer_extra_button = new wxBoxSizer(wxHORIZONTAL);
    auto *group_knob = new wxStaticBoxSizer(wxVERTICAL, panel,
                                            GetUIString("label_knobs"));
    auto *group_button = new wxStaticBoxSizer(wxVERTICAL, panel,
                                              GetUIString("label_buttons"));
    auto *group_extra_button = new wxStaticBoxSizer(wxVERTICAL, panel,
                                                    GetUIString("label_extra_buttons"));
    // Layout
    sizer_profile->Add(choice_profile_,       1, wxEXPAND | wxALL, 2);
    sizer_profile->Add(button_add_profile,    0,            wxALL, 2);
    sizer_profile->Add(button_remove_profile, 0,            wxALL, 2);
    sizer_profile->Add(button_rename_profile, 0,            wxALL, 2);
    sizer_extra_button->Add(button_extra_add,    0, wxALL, 4);
    sizer_extra_button->Add(button_extra_remove, 0, wxALL, 4);
    group_knob->        Add(grid_knob_,         0, wxALL, 4);
    group_button->      Add(grid_button_,       0, wxALL, 4);
    group_extra_button->Add(sizer_extra_button, 0, wxALL, 4);
    group_extra_button->Add(grid_extra_button_, 1, wxALL | wxEXPAND, 4);
    group_extra_button->SetMinSize(-1, 180);
    sizer_grid->Add(sizer_profile,      0, wxEXPAND | wxALL, 2);
    sizer_grid->Add(group_knob,         0, wxALL, 4);
    sizer_grid->Add(group_button,       0, wxALL, 4);
    sizer_grid->Add(group_extra_button, 0, wxALL, 4);

    sizer_control->Add(image_panel_, 0, wxALIGN_BOTTOM | wxALL, 4);
    sizer_control->Add(sizer_grid);

    sizer_dialog->Add(sizer_control);

    // Set sizer and size
    panel->SetSizer(sizer_dialog);
    sizer_dialog->SetSizeHints(this);

    // Bind event
    Bind(wxEVT_CHOICE, &ConfigDialog::OnChoiceProfile, this, ID_CHOICE_PROFILE);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnAddProfile, this, ID_BUTTON_PROFILE_ADD);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnRemoveProfile, this, ID_BUTTON_PROFILE_REMOVE);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnRenameProfile, this, ID_BUTTON_PROFILE_RENAME);
    Bind(wxEVT_GRID_CELL_LEFT_CLICK, &ConfigDialog::SetCursorToRowHead, this);
    Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &ConfigDialog::OnSelectKnobGrid, this,
         ID_GRID_KNOB);
    Bind(wxEVT_GRID_CELL_RIGHT_DCLICK, &ConfigDialog::OnSelectKnobGridAsMouse, this,
         ID_GRID_KNOB);
    Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &ConfigDialog::OnSelectButtonGrid, this,
         ID_GRID_BUTTON);
    Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &ConfigDialog::OnSelectExtraButtonGrid, this,
         ID_GRID_EXTRA_BUTTON);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnAddExtraButton, this, ID_BUTTON_EXTRA_ADD);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnRemoveExtraButton, this, ID_BUTTON_EXTRA_REMOVE);
    Bind(wxEVT_GRID_CELL_CHANGED, &ConfigDialog::ValidateExtraButtonName, this,
         ID_GRID_EXTRA_BUTTON);
    Bind(wxEVT_GRID_RANGE_SELECT, &ConfigDialog::ClearRangeSelection, this);
    Bind(wxEVT_TIMER, &ConfigDialog::OnTimer, this, timer_.GetId());

    timer_.Start(1000 / 30);
}

void ConfigDialog::InitImages() {
    panel_image_ = DecodeToMat(LoadBinaryResource("PANEL", "PNG"));
    start_image_ = DecodeToMat(LoadBinaryResource("START", "PNG"));
    bt_a_image_ = DecodeToMat(LoadBinaryResource("BT_A", "PNG"));
    bt_b_image_ = DecodeToMat(LoadBinaryResource("BT_B", "PNG"));
    bt_c_image_ = DecodeToMat(LoadBinaryResource("BT_C", "PNG"));
    bt_d_image_ = DecodeToMat(LoadBinaryResource("BT_D", "PNG"));
    fx_l_image_ = DecodeToMat(LoadBinaryResource("FX_L", "PNG"));
    fx_r_image_ = DecodeToMat(LoadBinaryResource("FX_R", "PNG"));
}

std::vector<unsigned char> ConfigDialog::LoadBinaryResource(
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

std::vector<unsigned char> ConfigDialog::LoadBinaryFile(const std::string &file_name) {
    std::vector<unsigned char> out_binary;
    std::ifstream file(file_name, std::ios::binary);
    if (!file)
        return out_binary;

    file.unsetf(std::ios::skipws);
    // Get file size
    unsigned int fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    out_binary.resize(fileSize);
    file.read(reinterpret_cast<char*>(out_binary.data()), fileSize);

    return out_binary;
}

cv::Mat ConfigDialog::DecodeToMat(const std::vector<unsigned char> &source_image) {
    unsigned int width = 0, height = 0;

    lodepng::State state;
    state.info_raw.colortype = LCT_RGB;
    std::vector<unsigned char> decoded_pixels;
    lodepng::decode(decoded_pixels, width, height, state, source_image);

    auto out_image = cv::Mat(cv::Size(width, height), CV_8UC3);
    auto *pixels = out_image.data;
    auto *source_pixels = decoded_pixels.data();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            size_t index = x * 3 + width * y * 3;
            pixels[index    ] = source_pixels[index];
            pixels[index + 1] = source_pixels[index + 1];
            pixels[index + 2] = source_pixels[index + 2];
        }
    }
    return out_image;
}
wxBitmap ConfigDialog::ConvertToBitmap(const cv::Mat &source_image) {
    auto out_image = wxImage();
    auto size = source_image.size();
    out_image.Create(size.width, size.height);
    auto *pixels = out_image.GetData();
    auto *source_pixels = source_image.data;
    for (int y = 0; y < size.height; ++y) {
        for (int x = 0; x < size.width; ++x) {
            size_t index = x * 3 + size.width * y * 3;
            pixels[index    ] = source_pixels[index];
            pixels[index + 1] = source_pixels[index + 1];
            pixels[index + 2] = source_pixels[index + 2];
        }
    }
    return wxBitmap(out_image);
}


void ConfigDialog::SelectProfile(unsigned int profile_index) {
    if (profiles_.size() == 0)
        return;
    // Select first profile if the specified profile doesn't exist
    if (profile_index >= profiles_.size())
        SelectProfile(0);
    choice_profile_->SetSelection(profile_index);
    current_profile_ = &profiles_[profile_index];
    current_profile_index_ = profile_index;

    // Remove all extra grid
    int ex_num = grid_extra_button_->GetNumberRows();
    if (ex_num)
        grid_extra_button_->DeleteRows(0, ex_num);

    auto &knob_l = current_profile_->knob_l;
    auto &knob_r = current_profile_->knob_r;
    wxString knob_l_product_name;
    if (knob_l.type == HID)
        knob_l_product_name = GetHIDProductName(knob_l.device_list);
    else
        knob_l_product_name = "Mouse";
    wxString knob_r_product_name;
    if (knob_r.type == HID)
        knob_r_product_name = GetHIDProductName(knob_r.device_list);
    else
        knob_r_product_name = "Mouse";
    SetProfileToRow(grid_knob_, 0, GetUIString("label_knob_ll"),
                    knob_l_product_name,
                    knob_l.usage_page, knob_l.usage_id, knob_l.l_key);
    SetProfileToRow(grid_knob_, 1, GetUIString("label_knob_lr"),
                    knob_l_product_name,
                    knob_l.usage_page, knob_l.usage_id, knob_l.r_key);
    SetProfileToRow(grid_knob_, 2, GetUIString("label_knob_rl"),
                    knob_r_product_name,
                    knob_r.usage_page, knob_r.usage_id, knob_r.l_key);
    SetProfileToRow(grid_knob_, 3, GetUIString("label_knob_rr"),
                    knob_r_product_name,
                    knob_r.usage_page, knob_r.usage_id, knob_r.r_key);
    if (knob_l.type == Mouse) {
        auto direction_str = (knob_l.usage_id == 0x30) ? "X" : "Y";
        grid_knob_->SetCellValue(0, 2, direction_str);
        grid_knob_->SetCellValue(1, 2, direction_str);
    }
    if (knob_r.type == Mouse) {
        auto direction_str = (knob_r.usage_id == 0x30) ? "X" : "Y";
        grid_knob_->SetCellValue(2, 2, direction_str);
        grid_knob_->SetCellValue(3, 2, direction_str);
    }

    auto &start = current_profile_->start;
    auto &bt_a = current_profile_->bt_a;
    auto &bt_b = current_profile_->bt_b;
    auto &bt_c = current_profile_->bt_c;
    auto &bt_d = current_profile_->bt_d;
    auto &fx_l = current_profile_->fx_l;
    auto &fx_r = current_profile_->fx_r;
    SetProfileToRow(grid_button_, 0, "START", GetHIDProductName(start.device_list),
                    start.usage_page, start.usage_id, start.key);
    SetProfileToRow(grid_button_, 1, "BT-A", GetHIDProductName(bt_a.device_list),
                    bt_a.usage_page, bt_a.usage_id, bt_a.key);
    SetProfileToRow(grid_button_, 2, "BT-B", GetHIDProductName(bt_b.device_list),
                    bt_b.usage_page, bt_b.usage_id, bt_b.key);
    SetProfileToRow(grid_button_, 3, "BT-C", GetHIDProductName(bt_c.device_list),
                    bt_c.usage_page, bt_c.usage_id, bt_c.key);
    SetProfileToRow(grid_button_, 4, "BT-D", GetHIDProductName(bt_d.device_list),
                    bt_d.usage_page, bt_d.usage_id, bt_d.key);
    SetProfileToRow(grid_button_, 5, "FX-L", GetHIDProductName(fx_l.device_list),
                    fx_l.usage_page, fx_l.usage_id, fx_l.key);
    SetProfileToRow(grid_button_, 6, "FX-R", GetHIDProductName(fx_r.device_list),
                    fx_r.usage_page, fx_r.usage_id, fx_r.key);

    auto &extra_buttons = current_profile_->extra_buttons;
    int i = 0;
    for (auto eb = extra_buttons.begin(); eb != extra_buttons.end(); ++eb) {
        auto &ex_label = eb->first;
        auto &ex_bt = eb->second;
        AddExtraButton();
        SetProfileToRow(grid_extra_button_, i, ex_label, GetHIDProductName(ex_bt.device_list),
                        ex_bt.usage_page, ex_bt.usage_id, ex_bt.key);
        ++i;
    }
    if (fetcher_ != nullptr)
        delete fetcher_;
    fetcher_ = new RawInputFetcher(nullptr);
    fetcher_->AddDevice(current_profile_->knob_l.device_list);
    fetcher_->AddDevice(current_profile_->knob_r.device_list);
    fetcher_->AddDevice(current_profile_->start.device_list);
    fetcher_->AddDevice(current_profile_->bt_a.device_list);
    fetcher_->AddDevice(current_profile_->bt_b.device_list);
    fetcher_->AddDevice(current_profile_->bt_c.device_list);
    fetcher_->AddDevice(current_profile_->bt_d.device_list);
    fetcher_->AddDevice(current_profile_->fx_l.device_list);
    fetcher_->AddDevice(current_profile_->fx_r.device_list);
}

void ConfigDialog::SetProfileToRow(wxGrid *grid, int row, const wxString &label,
                                   const wxString &product_name,
                                   USHORT usage_page, USHORT usage_id,
                                   unsigned char key) {
    grid->SetCellValue(row, 0, label);
    grid->SetCellValue(row, 1, product_name);
    grid->SetCellValue(row, 2, wxString::Format("<0x%02x, 0x%02x>", usage_page, usage_id));
    grid->SetCellValue(row, 3, key_str[key]);
}

bool ConfigDialog::SetButton(SVCButtonBindInfo &info) {
    ButtonBindingDialog dialog(nullptr, info.device_list, RawInputDeviceManager());
    dialog.ShowModal();
    if (!dialog.IsButtonBinded())
        return false;
    auto &bind_info = dialog.GetButtonBindingInfo();
    auto device_list = bind_info.device_list;
    info.device_name = GetHIDDeviceName(device_list);
    info.product_name = GetHIDProductName(device_list);
    info.device_list = device_list;
    info.usage_page = bind_info.usage_page;
    info.usage_id = bind_info.usage_id;
    return true;
}

bool ConfigDialog::SetKnob(SVCKnobBindInfo &info) {
    KnobBindingDialog dialog(nullptr, info.device_list, RawInputDeviceManager());
    dialog.ShowModal();
    if (!dialog.IsKnobBinded())
        return false;
    auto &bind_info = dialog.GetKnobBindingInfo();
    auto device_list = bind_info.device_list;
    info.device_name = GetHIDDeviceName(device_list);
    info.product_name = GetHIDProductName(device_list);
    info.device_list = device_list;
    info.increase_direction = bind_info.increase_direction;
    info.usage_page = bind_info.usage_page;
    info.usage_id = bind_info.usage_id;
    return true;
}

bool ConfigDialog::SetKnobAsMouse(SVCKnobBindInfo &info) {
    MouseBindingDialog dialog(nullptr, info.device_list, RawInputDeviceManager());
    dialog.ShowModal();
    if (!dialog.IsKnobBinded())
        return false;
    auto &bind_info = dialog.GetKnobBindingInfo();
    info.type = Mouse;
    auto device_list = bind_info.device_list;
    info.device_name = GetHIDDeviceName(device_list);
    info.product_name = GetHIDProductName(device_list);
    info.device_list = device_list;
    info.increase_direction = bind_info.increase_direction;
    info.usage_page = bind_info.usage_page;
    info.usage_id = bind_info.usage_id;
    return true;
}

bool ConfigDialog::SetKey(unsigned char *out_key) {
    KeyBindingDialog dialog(nullptr);
    dialog.ShowModal();
    if (!dialog.IsKeyBinded())
        return false;
    auto key = dialog.GetKey();
    *out_key = key;
    return true;
}

void ConfigDialog::AddExtraButton() {
    auto row_pos = grid_extra_button_->GetNumberRows();
    grid_extra_button_->InsertRows(row_pos);

    // Set readonly
    grid_extra_button_->SetReadOnly(row_pos, 1);
    grid_extra_button_->SetReadOnly(row_pos, 2);
    grid_extra_button_->SetReadOnly(row_pos, 3);
    grid_extra_button_->DisableRowResize(row_pos);
}

void ConfigDialog::RemoveExtraButton() {
    auto row = grid_extra_button_->GetGridCursorRow();
    auto label = grid_extra_button_->GetCellValue(row, 0);
    current_profile_->extra_buttons.erase(label);
    extra_key_table_.erase(extra_key_table_.begin() + row);
    grid_extra_button_->DeleteRows(row);
}

void ConfigDialog::SyncExtraButtonView() {
    auto &extra_buttons = current_profile_->extra_buttons;
    for (int row = 0; row < grid_extra_button_->GetNumberRows(); ++row) {
        auto &ex_bt = extra_buttons[grid_extra_button_->GetCellValue(row, 0)];
        grid_extra_button_->SetCellValue(row, 1, GetHIDProductName(ex_bt.device_list));
        grid_extra_button_->SetCellValue(row, 2, wxString::Format("<0x%02x, 0x%02x>",
                                                                  ex_bt.usage_page, ex_bt.usage_id));
        grid_extra_button_->SetCellValue(row, 3, key_str[ex_bt.key]);
    }
}

void ConfigDialog::RefreshProfileChoice() {
    // Adjust size
    int sub = profiles_.size() - choice_profile_->GetCount();
    if (sub < 0) {
        for (int i = 0; i < -sub; ++i) {
            choice_profile_->Delete(0);
        }
    } else {
        for (int i = 0; i < sub; ++i) {
            choice_profile_->Append("");
        }
    }
    // Replace names
    for (int i = 0; i < profiles_.size(); ++i) {
        choice_profile_->SetString(i, profiles_[i].profile_name);
    }
}

void ConfigDialog::OnTimer(wxTimerEvent &evt) {
    auto &start = current_profile_->start;
    bool start_pressed = fetcher_->GetDeviceState(start.device_list.hDevice)
                            .button_state.GetUsages(start.usage_page)[start.usage_id];
    auto &bt_a = current_profile_->bt_a;
    bool bt_a_pressed = fetcher_->GetDeviceState(bt_a.device_list.hDevice)
                            .button_state.GetUsages(bt_a.usage_page)[bt_a.usage_id];
    auto &bt_b = current_profile_->bt_b;
    bool bt_b_pressed = fetcher_->GetDeviceState(bt_b.device_list.hDevice)
                            .button_state.GetUsages(bt_b.usage_page)[bt_b.usage_id];
    auto &bt_c = current_profile_->bt_c;
    bool bt_c_pressed = fetcher_->GetDeviceState(bt_c.device_list.hDevice)
                            .button_state.GetUsages(bt_c.usage_page)[bt_c.usage_id];
    auto &bt_d = current_profile_->bt_d;
    bool bt_d_pressed = fetcher_->GetDeviceState(bt_d.device_list.hDevice)
                            .button_state.GetUsages(bt_d.usage_page)[bt_d.usage_id];
    auto &fx_l = current_profile_->fx_l;
    bool fx_l_pressed = fetcher_->GetDeviceState(fx_l.device_list.hDevice)
                            .button_state.GetUsages(fx_l.usage_page)[fx_l.usage_id];
    auto &fx_r = current_profile_->fx_r;
    bool fx_r_pressed = fetcher_->GetDeviceState(fx_r.device_list.hDevice)
                            .button_state.GetUsages(fx_r.usage_page)[fx_r.usage_id];
    cv::Mat image;
    panel_image_.copyTo(image);

    // Add lighting images
    if (start_pressed)
        image += start_image_;
    if (bt_a_pressed)
        image += bt_a_image_;
    if (bt_b_pressed)
        image += bt_b_image_;
    if (bt_c_pressed)
        image += bt_c_image_;
    if (bt_d_pressed)
        image += bt_d_image_;
    if (fx_l_pressed)
        image += fx_l_image_;
    if (fx_r_pressed)
        image += fx_r_image_;

    wxClientDC dc(image_panel_);
    dc.DrawBitmap(ConvertToBitmap(image), 0, 0);
}

void ConfigDialog::OnChoiceProfile(wxCommandEvent &event) {
    auto index = choice_profile_->GetSelection();
    SelectProfile(index);
}

void ConfigDialog::OnAddProfile(wxCommandEvent &event) {
    bool is_valid = false;
    ProfileNameDialog name_dialog(nullptr);
    wxString name;
    while (!is_valid) {
        is_valid = true;
        name_dialog.ShowModal();
        if (!name_dialog.IsNameEntered())
            return;
        name = name_dialog.GetProfileName();
        is_valid = !name.empty();
        for (auto &profile : profiles_) {
            if (name == profile.profile_name)
                is_valid = false;
        }
    }
    SVCControllBindProfile profile;
    profile.profile_name = name;
    profiles_.push_back(profile);
    RefreshProfileChoice();
    SelectProfile(profiles_.size() - 1);
    is_edited_ = true;
}

void ConfigDialog::OnRemoveProfile(wxCommandEvent &event) {
    if (profiles_.size() < 2)
        return;
    int index = choice_profile_->GetSelection();
    auto &profile_name = profiles_[index].profile_name;
    auto message = wxString::Format(GetUIString("text_confirm_to_erase"), profile_name);
    auto confirm = wxMessageBox(message, "Confirm", wxYES_NO);
    if (confirm != wxYES)
        return;
    profiles_.erase(profiles_.begin() + choice_profile_->GetSelection());
    RefreshProfileChoice();
    SelectProfile(0);
    is_edited_ = true;
}

void ConfigDialog::OnRenameProfile(wxCommandEvent &event) {
    bool is_valid = false;
    auto index = choice_profile_->GetSelection();
    auto &old_name = profiles_[index].profile_name;
    ProfileNameDialog name_dialog(nullptr, old_name);
    wxString name;
    while (!is_valid) {
        is_valid = true;
        name_dialog.ShowModal();
        if (!name_dialog.IsNameEntered())
            return;
        name = name_dialog.GetProfileName();
        is_valid = !name.empty();
        for (auto &profile : profiles_) {
            if (name == profile.profile_name)
                is_valid = false;
        }
    }
    profiles_[index].profile_name = name;
    RefreshProfileChoice();
    is_edited_ = true;
}

void ConfigDialog::OnAddExtraButton(wxCommandEvent &event) {
    auto row_pos = grid_extra_button_->GetNumberRows();
    AddExtraButton();

    static std::random_device rand;
    auto label = wxString::Format("Extra button_%x", rand());
    extra_key_table_.push_back(label);
    current_profile_->extra_buttons[label] = SVCButtonBindInfo();
    SyncExtraButtonView();
    // Enter label edit
    // grid_extra_button_->SetFocus();
    grid_extra_button_->SetCellValue(row_pos, 0, label);
    grid_extra_button_->SetGridCursor(row_pos, 0);
    grid_extra_button_->EnableCellEditControl();
}

void ConfigDialog::OnRemoveExtraButton(wxCommandEvent &event) {
    RemoveExtraButton();
}

void ConfigDialog::OnSelectButtonGrid(wxGridEvent &event) {
    auto row = event.GetRow();
    auto col = event.GetCol();
    auto *ev_obj = reinterpret_cast<wxGrid*>(event.GetEventObject());
    ev_obj->SetGridCursor(row, 0);
    SVCButtonBindInfo *current_button = nullptr;
    switch (row) {
    case 0: {
        current_button = &current_profile_->start;
        break;
    }
    case 1: {
        current_button = &current_profile_->bt_a;
        break;
    }
    case 2: {
        current_button = &current_profile_->bt_b;
        break;
    }
    case 3: {
        current_button = &current_profile_->bt_c;
        break;
    }
    case 4: {
        current_button = &current_profile_->bt_d;
        break;
    }
    case 5: {
        current_button = &current_profile_->fx_l;
        break;
    }
    case 6: {
        current_button = &current_profile_->fx_r;
        break;
    }
    }
    switch (col) {
    case 1:
    case 2: {
        if (!SetButton(*current_button))
            return;
        auto product_name = GetHIDProductName(current_button->device_list);
        auto usage_str = wxString::Format("<0x%02x, 0x%02x>",
                                          current_button->usage_page, current_button->usage_id);
        ev_obj->SetCellValue(row, 1, product_name);
        ev_obj->SetCellValue(row, 2, usage_str);
        fetcher_->AddDevice(current_button->device_list);
        break;
    }
    case 3: {
        if (!SetKey(&current_button->key))
            return;
        ev_obj->SetCellValue(row, col, key_str[current_button->key]);
        break;
    }
    }
    is_edited_ = true;
}

void ConfigDialog::OnSelectKnobGrid(wxGridEvent &event) {
    auto row = event.GetRow();
    auto col = event.GetCol();
    auto *ev_obj = reinterpret_cast<wxGrid*>(event.GetEventObject());
    ev_obj->SetGridCursor(row, 0);
    bool is_l_knob = row == 0 || row == 1;
    auto &current_knob = (is_l_knob) ? current_profile_->knob_l
                                     : current_profile_->knob_r;
    switch (col) {
    case 1:
    case 2: {
        if (!SetKnob(current_knob))
            return;
        auto product_name = GetHIDProductName(current_knob.device_list);
        auto usage_str = wxString::Format("<0x%02x, 0x%02x>",
                                          current_knob.usage_page, current_knob.usage_id);
        if (is_l_knob) {
            ev_obj->SetCellValue(0, 1, product_name);
            ev_obj->SetCellValue(1, 1, product_name);
            ev_obj->SetCellValue(0, 2, usage_str);
            ev_obj->SetCellValue(1, 2, usage_str);
        } else {
            ev_obj->SetCellValue(2, 1, product_name);
            ev_obj->SetCellValue(3, 1, product_name);
            ev_obj->SetCellValue(2, 2, usage_str);
            ev_obj->SetCellValue(3, 2, usage_str);
        }
        fetcher_->AddDevice(current_knob.device_list);
        break;
    }
    case 3: {
        bool is_l_key = (row % 2) == 0;
        auto &key = (is_l_key) ? current_knob.l_key : current_knob.r_key;
        if (!SetKey(&key))
            return;
        ev_obj->SetCellValue(row, col, key_str[key]);
        break;
    }
    }
    is_edited_ = true;
}

void ConfigDialog::OnSelectKnobGridAsMouse(wxGridEvent &event) {
    auto row = event.GetRow();
    auto col = event.GetCol();
    auto *ev_obj = reinterpret_cast<wxGrid*>(event.GetEventObject());
    ev_obj->SetGridCursor(row, 0);
    bool is_l_knob = row == 0 || row == 1;
    auto &current_knob = (is_l_knob) ? current_profile_->knob_l
        : current_profile_->knob_r;
    switch (col) {
    case 1:
    case 2: {
        if (!SetKnobAsMouse(current_knob))
            return;
        auto product_name = "Mouse";
        auto direction_str = (current_knob.usage_id == 0x30) ? "X" : "Y";
        if (is_l_knob) {
            ev_obj->SetCellValue(0, 1, product_name);
            ev_obj->SetCellValue(1, 1, product_name);
            ev_obj->SetCellValue(0, 2, direction_str);
            ev_obj->SetCellValue(1, 2, direction_str);
        } else {
            ev_obj->SetCellValue(2, 1, product_name);
            ev_obj->SetCellValue(3, 1, product_name);
            ev_obj->SetCellValue(2, 2, direction_str);
            ev_obj->SetCellValue(3, 2, direction_str);
        }
        fetcher_->AddDevice(current_knob.device_list);
        break;
    }
    case 3: {
        bool is_l_key = (row % 2) == 0;
        auto &key = (is_l_key) ? current_knob.l_key : current_knob.r_key;
        if (!SetKey(&key))
            return;
        ev_obj->SetCellValue(row, col, key_str[key]);
        break;
    }
    }
    is_edited_ = true;
}

void ConfigDialog::OnSelectExtraButtonGrid(wxGridEvent &event) {
    auto row = event.GetRow();
    auto col = event.GetCol();
    auto *ev_obj = reinterpret_cast<wxGrid*>(event.GetEventObject());
    ev_obj->SetGridCursor(row, 0);
    auto label = grid_extra_button_->GetCellValue(row, 0);
    auto &current_ex_bt = current_profile_->extra_buttons[label];
    switch (col) {
    case 1:
    case 2: {
        if (!SetButton(current_ex_bt))
            return;
        SyncExtraButtonView();
        fetcher_->AddDevice(current_ex_bt.device_list);
        break;
    }
    case 3: {
        if (!SetKey(&current_ex_bt.key))
            return;
        SyncExtraButtonView();
        break;
    }
    }
    is_edited_ = true;
}

void ConfigDialog::ValidateExtraButtonName(wxGridEvent &event) {
    auto row = event.GetRow();
    auto label = grid_extra_button_->GetCellValue(row, 0);
    bool is_valid = !label.empty();
    for (auto i = 0; i < grid_extra_button_->GetNumberRows(); ++i) {
        if (i == row)
            continue;
        is_valid &= label != grid_extra_button_->GetCellValue(i, 0);
    }
    if (is_valid) {
        auto old_label = extra_key_table_[row];
        auto &ex_bt = current_profile_->extra_buttons;
        ex_bt[label] = ex_bt[old_label];
        ex_bt.erase(old_label);
        extra_key_table_[row] = label;
        return;
    }
    // Show warning dialog
    wxMessageBox(GetUIString("text_warn_extra_button_unique"), "Warning",
                 wxOK | wxICON_EXCLAMATION);
    // Enter label edit
    grid_extra_button_->SetGridCursor(row, 0);
    grid_extra_button_->EnableCellEditControl();
}

void ConfigDialog::SetCursorToRowHead(wxGridEvent &event) {
    auto *ev_obj = reinterpret_cast<wxGrid*>(event.GetEventObject());
    ev_obj->SetGridCursor(event.GetRow(), 0);
}

void ConfigDialog::ClearRangeSelection(wxGridRangeSelectEvent &event) {
    if (event.Selecting()) {
        wxGrid *grid = reinterpret_cast<wxGrid*>(event.GetEventObject());
        grid->ClearSelection();
    }
}

ProfileNameDialog::ProfileNameDialog(wxWindow *parent, const wxString &default_name)
        : wxDialog(parent, wxID_ANY, "Enter profile name") {
    auto *panel = new wxPanel(this);

    auto *sizer = new wxBoxSizer(wxVERTICAL);

    auto *text = new wxStaticText(panel, wxID_ANY, GetUIString("text_enter_profile_name"));
    edit_name_ = new wxTextCtrl(panel, wxID_ANY, default_name);
    auto *button_ok = new wxButton(panel, ID_BUTTON_OK, "&OK");

    sizer->Add(text, 1, wxALL, 4);
    sizer->Add(edit_name_, 0, wxEXPAND | wxALL, 4);
    sizer->Add(button_ok, 0, wxALIGN_RIGHT | wxBOTTOM | wxALL, 4);

    panel->SetSizer(sizer);
    SetMinSize(wxSize(240, 120));
    sizer->SetSizeHints(this);

    Bind(wxEVT_SHOW, [&](wxShowEvent &) {
            edit_name_->SetSelection(-1, -1);
         });
    Bind(wxEVT_BUTTON, [&](wxCommandEvent &) {
            is_entered_ = true;
            Show(false);}, ID_BUTTON_OK);
    Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent &) {
            is_entered_ = false;
            Show(false);
        });
}