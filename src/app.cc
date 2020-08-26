#include "app.h"
#include "main_window.h"
#include "setting_parser.h"
#include "util_function.h"

bool SVCConverterAPP::OnInit() {
    ParseSetting();
    InitUIString();

    MainWindow *main_window = new MainWindow("SVCConverter");
    main_window->Show(true);
    return true;
}