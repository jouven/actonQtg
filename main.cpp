#include "mainWindow.hpp"

#include "appConfig.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "signalso/signal.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    MACRO_signalHandler

    //qSetMessagePattern(QString("[%{type}] %{appname} (%{file}:%{line}) - %{message} %{backtrace}"));
#ifdef __ANDROID__
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //qputenv("QT_SCALE_FACTOR", "0.6");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "true");
#endif

    QApplication qtapp(argc, argv);
    QApplication::setApplicationName("actonQtg");
    QApplication::setApplicationVersion("1.0");

    //statics = who the F knows the initialization order...
    //let's just use some pointers to local variables
    appConfig_c appConfigTmp;
    appConfig_ptr_ext = std::addressof(appConfigTmp);

    actonDataHub_c actonDataHubTmp;
    actonDataHub_ptr_ext = std::addressof(actonDataHubTmp);

    actonDataHubTmp.setLogDataHub_f(appConfigTmp.logDataHub_f());

    MACRO_ADDACTONQTGLOG("Create main window", logItem_c::type_ec::info);

    mainWindow_c mainWindowTmp;
    mainWindow_ptr_ext = std::addressof(mainWindowTmp);
    mainWindowTmp.show();

    returnValue_ext = QApplication::exec();

    MACRO_ADDACTONQTGLOG("Exit", logItem_c::type_ec::info);
    //already in the mainwindow close function
//    if (eines::signal::isRunning_f())
//    {
//        eines::signal::stopRunning_f();
//    }
    while (not eines::signal::isTheEnd_f())
    {}

    return returnValue_ext;
}
