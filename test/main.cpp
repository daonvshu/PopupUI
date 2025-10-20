#include <qapplication.h>
#include <qlogcollector.h>

#include "overlay/overlaytest.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    logcollector::styleConfig
        .consoleApp()
        .ide_clion(false)
        .wordWrap(360)
        .projectSourceCodeRootPath(ROOT_PROJECT_PATH)
        ;
    logcollector::QLogCollector::instance().registerLog();

    OverlayTest test;
    test.show();

    return app.exec();
}
