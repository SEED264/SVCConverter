#include "config_dialog.h"
#include <random>
#include "input_manager.h"
#include "label_string.h"
#include "binding_dialog.h"

ConfigDialog::ConfigDialog(wxWindow *parent,
                           const std::vector<SVCControllBindProfile> &profiles,
                           unsigned int using_profile)
        : wxDialog(parent, wxID_ANY, "Config")
        , profiles_(profiles) {
    InitGUI();
    RefreshProfileChoice();
    SelectProfile(using_profile);
}

void ConfigDialog::InitGUI() {
    auto *panel = new wxPanel(this);

    grid_knob_ = new wxGrid(panel, ID_GRID_KNOB, wxDefaultPosition,
                            wxDefaultSize, wxBORDER_SIMPLE);
    grid_button_ = new wxGrid(panel, ID_GRID_BUTTON, wxDefaultPosition,
                              wxDefaultSize, wxBORDER_SIMPLE);
    grid_extra_button_ = new wxGrid(panel, ID_GRID_EXTRA_BUTTON, wxDefaultPosition,
                                    wxDefaultSize, wxBORDER_SIMPLE);

    choice_profile_ = new wxChoice(panel, ID_CHOICE_PROFILE, wxDefaultPosition, wxDefaultSize,
                                   wxArrayString());
    auto *button_add_profile = new wxButton(panel, ID_BUTTON_PROFILE_ADD, "Add");
    auto *button_remove_profile = new wxButton(panel, ID_BUTTON_PROFILE_REMOVE, "Remove");
    auto *button_rename_profile = new wxButton(panel, ID_BUTTON_PROFILE_RENAME, "Rename");

    auto *button_extra_add = new wxButton(panel, ID_BUTTON_EXTRA_ADD, "Add");
    auto *button_extra_remove = new wxButton(panel, ID_BUTTON_EXTRA_REMOVE, "Remove");

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

    // for (int col = 0; col < grid_knob_->GetNumberCols(); ++col) {
    //     grid_extra_button_->DisableColResize(col);
    // }
    for (int row = 0; row < grid_extra_button_->GetNumberCols(); ++row) {
        grid_extra_button_->DisableRowResize(row);
    }

    // Set col size
    grid_knob_->SetColSize(0, 100);
    grid_knob_->SetColSize(1, 180);
    grid_knob_->SetColSize(2, 100);
    grid_knob_->SetColSize(3, 100);
    grid_button_->SetColSize(0, 100);
    grid_button_->SetColSize(1, 180);
    grid_button_->SetColSize(2, 100);
    grid_button_->SetColSize(3, 100);
    grid_extra_button_->SetColSize(0, 100);
    grid_extra_button_->SetColSize(1, 180);
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
    grid_knob_->SetColLabelValue(1, "Controller");
    grid_knob_->SetColLabelValue(2, "Input usage");
    grid_knob_->SetColLabelValue(3, "Key");
    grid_button_->SetColLabelValue(0, "");
    grid_button_->SetColLabelValue(1, "Controller");
    grid_button_->SetColLabelValue(2, "Input usage");
    grid_button_->SetColLabelValue(3, "Key");
    grid_extra_button_->SetColLabelValue(0, "");
    grid_extra_button_->SetColLabelValue(1, "Controller");
    grid_extra_button_->SetColLabelValue(2, "Input usage");
    grid_extra_button_->SetColLabelValue(3, "Key");

    // Set grid size
    grid_knob_->SetSize(grid_knob_->GetVirtualSize());
    grid_button_->SetSize(grid_knob_->GetVirtualSize());
    grid_extra_button_->SetSize(grid_knob_->GetVirtualSize(), 100);

    auto *sizer_dialog = new wxBoxSizer(wxVERTICAL);
    auto *sizer_profile = new wxBoxSizer(wxHORIZONTAL);
    auto *sizer_grid = new wxBoxSizer(wxVERTICAL);
    auto *sizer_extra_button = new wxBoxSizer(wxHORIZONTAL);
    auto *group_knob = new wxStaticBoxSizer(wxVERTICAL, panel, "Knobs");
    auto *group_button = new wxStaticBoxSizer(wxVERTICAL, panel, "Buttons");
    auto *group_extra_button = new wxStaticBoxSizer(wxVERTICAL, panel, "Extra buttons");
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
    sizer_grid->Add(group_knob,         0, wxALL, 4);
    sizer_grid->Add(group_button,       0, wxALL, 4);
    sizer_grid->Add(group_extra_button, 0, wxALL, 4);

    sizer_dialog->Add(sizer_profile, 0, wxEXPAND);
    sizer_dialog->Add(sizer_grid);

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
    Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &ConfigDialog::OnSelectButtonGrid, this,
         ID_GRID_BUTTON);
    Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &ConfigDialog::OnSelectExtraButtonGrid, this,
         ID_GRID_EXTRA_BUTTON);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnAddExtraButton, this, ID_BUTTON_EXTRA_ADD);
    Bind(wxEVT_BUTTON, &ConfigDialog::OnRemoveExtraButton, this, ID_BUTTON_EXTRA_REMOVE);
    Bind(wxEVT_GRID_CELL_CHANGED, &ConfigDialog::ValidateExtraButtonName, this,
         ID_GRID_EXTRA_BUTTON);
    Bind(wxEVT_GRID_RANGE_SELECT, &ConfigDialog::ClearRangeSelection, this);
}

void ConfigDialog::SelectProfile(unsigned int profile_index) {
    if (profiles_.size() == 0)
        return;
    // Select first profile if the specified profile doesn't exist
    if (profile_index > profiles_.size())
        SelectProfile(0);
    choice_profile_->SetSelection(profile_index);
    current_profile_ = &profiles_[profile_index];

    // Remove all extra grid
    int ex_num = grid_extra_button_->GetNumberRows();
    if (ex_num)
        grid_extra_button_->DeleteRows(0, ex_num);

    auto &knob_l = current_profile_->knob_l;
    auto &knob_r = current_profile_->knob_r;
    SetProfileToRow(grid_knob_, 0, "Knob L (Turn L)", GetHIDProductName(knob_l.device_list),
                    knob_l.usage_page, knob_l.usage_id, knob_l.l_key);
    SetProfileToRow(grid_knob_, 1, "Knob L (Turn R)", GetHIDProductName(knob_l.device_list),
                    knob_l.usage_page, knob_l.usage_id, knob_l.r_key);
    SetProfileToRow(grid_knob_, 2, "Knob R (Turn L)", GetHIDProductName(knob_r.device_list),
                    knob_r.usage_page, knob_r.usage_id, knob_r.l_key);
    SetProfileToRow(grid_knob_, 3, "Knob R (Turn R)", GetHIDProductName(knob_r.device_list),
                    knob_r.usage_page, knob_r.usage_id, knob_r.r_key);

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

void ConfigDialog::SetButton(SVCButtonBindInfo &info) {
    ButtonBindingDialog dialog(nullptr, info.device_list, RawInputDeviceManager());
    dialog.ShowModal();
    if (!dialog.IsButtonBinded())
        return;
    auto &bind_info = dialog.GetButtonBindingInfo();
    info.device_list = bind_info.device_list;
    info.usage_page = bind_info.usage_page;
    info.usage_id = bind_info.usage_id;
}

void ConfigDialog::SetKnob(SVCKnobBindInfo &info) {
    KnobBindingDialog dialog(nullptr, info.device_list, RawInputDeviceManager());
    dialog.ShowModal();
    if (!dialog.IsKnobBinded())
        return;
    auto &bind_info = dialog.GetKnobBindingInfo();
    info.device_list = bind_info.device_list;
    info.increase_direction = bind_info.increase_direction;
    info.usage_page = bind_info.usage_page;
    info.usage_id = bind_info.usage_id;
}

void ConfigDialog::SetKey(unsigned char *out_key) {
    KeyBindingDialog dialog(nullptr);
    dialog.ShowModal();
    if (!dialog.IsKeyBinded())
        return;
    auto key = dialog.GetKey();
    *out_key = key;
}

void ConfigDialog::AddExtraButton() {
    auto row_pos = grid_extra_button_->GetNumberRows();
    grid_extra_button_->InsertRows(row_pos);

    // Set readonly
    grid_extra_button_->SetReadOnly(row_pos, 1);
    grid_extra_button_->SetReadOnly(row_pos, 2);
    grid_extra_button_->SetReadOnly(row_pos, 3);
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
    auto message = wxString::Format("Are you sure you want to erase %s?", profile_name);
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
    SVCButtonBindInfo *current_button;
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
        SetButton(*current_button);
        auto product_name = GetHIDProductName(current_button->device_list);
        auto usage_str = wxString::Format("<0x%02x, 0x%02x>",
                                          current_button->usage_page, current_button->usage_id);
        ev_obj->SetCellValue(row, 1, product_name);
        ev_obj->SetCellValue(row, 2, usage_str);
        break;
    }
    case 3: {
        SetKey(&current_button->key);
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
        SetKnob(current_knob);
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
        break;
    }
    case 3: {
        bool is_l_key = (row % 2) == 0;
        auto &key = (is_l_key) ? current_knob.l_key : current_knob.r_key;
        SetKey(&key);
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
        SetButton(current_ex_bt);
        SyncExtraButtonView();
        break;
    }
    case 3: {
        SetKey(&current_ex_bt.key);
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
    wxMessageBox("Extra button labels must be unique", "Warning",
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

    auto *text = new wxStaticText(panel, wxID_ANY, "Enter profile name. It must be unique.");
    edit_name_ = new wxTextCtrl(panel, wxID_ANY, default_name);
    auto *button_ok = new wxButton(panel, ID_BUTTON_OK, "&OK");

    sizer->Add(text, 1, wxALL, 4);
    sizer->Add(edit_name_, 0, wxEXPAND | wxALL, 4);
    sizer->Add(button_ok, 0, wxALIGN_RIGHT | wxBOTTOM | wxALL, 4);

    panel->SetSizer(sizer);
    SetSize(240, 120);

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