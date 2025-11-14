#include <qapplication.h>
#include <qlogcollector/server/logcollector.h>

#include "overlay/overlaytest.h"

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    LogCollector::styleConfig
            .wordWrap(360)
            .projectSourceCodeRootPath(ROOT_PROJECT_PATH)
        ;
    LogCollector::addOutputTarget(OutputTarget::currentConsoleOutput(Ide::clion));
    LogCollector::registerLog();

    OverlayTest test;
    test.show();

    return app.exec();
}
