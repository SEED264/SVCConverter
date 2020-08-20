#include "config_dialog.h"
#include "input_manager.h"
#include "label_string.h"

ConfigDialog::ConfigDialog(wxWindow *parent)
        : wxDialog(parent, wxID_ANY, "Config") {
    
}

void ConfigDialog::OnTimer(wxTimerEvent &evt) {
}