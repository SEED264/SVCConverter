#include "app.h"
#include "main_window.h"
#include "slider_value_dialog.h"

bool SVCConverterAPP::OnInit() {
    // MainWindow *main_window = new MainWindow("SVCConverter");
    // main_window->Show(true);
    auto *dialog = new SliderValueDialog(nullptr);
    dialog->Show();
    return true;
}