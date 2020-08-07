#include "app.h"
#include "main_window.h"

bool SVCConverterAPP::OnInit() {
    MainWindow *main_window = new MainWindow("SVCConverter");
    main_window->Show(true);
    return true;
}