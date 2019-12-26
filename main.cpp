#include "mainWindow.hpp"

#include "appConfig.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "signalso/signal.hpp"
#include "essentialQtso/essentialQt.hpp"
#include "stringParserMapQtso/stringParserMap.hpp"

#include <QApplication>

#ifdef DEBUGJOUVEN
#include <QDebug>
#ifndef Q_OS_WIN
#include "backwardSTso/backward.hpp"
#endif
#endif

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

    //statics + references = who the F knows the initialization order
    //plus initialization must be done when the reference is declared
    //so they must be separated per units to really account for that
    //which makes it more cumbersome
    //let's just use some pointers to local variables

    //translation and logs are initialized here
    appConfig_c appConfigTmp;
    appConfig_ptr_ext = std::addressof(appConfigTmp);

    actonDataHub_c actonDataHubTmp(std::addressof(qtapp));
    actonDataHub_ptr_ext = std::addressof(actonDataHubTmp);

    //the parser class can be used from the ptr in the parser library or from the
    //actonHub.executionOptions ptr member (they both point to this local variable)
    stringParserMap_c stringParserMapTmp;
    stringParserMap_ptr_ext = std::addressof(stringParserMapTmp);

    {
        //add stringparser ptr to the actonhub_c execution options
        executionOptions_c executionOptionsTmp(actonDataHubTmp.executionOptions_f());
        executionOptionsTmp.setStringParserMap_f(std::addressof(stringParserMapTmp));
        actonDataHubTmp.setExecutionOptions_f(executionOptionsTmp);
    }
    //TODO add a way to see checks execution results from the action action execution result
    //show id values, read-only, on the action and check editors and the grids of actions/checks

    actonDataHubTmp.setLogDataHub_f(appConfigTmp.logDataHub_f());
//#ifdef DEBUGJOUVEN
//    qDebug() << "actonDataHubTmp.setLogDataHub_f(appConfigTmp.logDataHub_f());";
//#endif
    MACRO_ADDACTONQTGLOG("Create main window", logItem_c::type_ec::info);

    mainWindow_c mainWindowTmp;
    mainWindow_ptr_ext = std::addressof(mainWindowTmp);
    mainWindowTmp.show();

    returnValue_ext = QApplication::exec();

    MACRO_ADDACTONQTGLOG("Exit", logItem_c::type_ec::info);
    //already in the mainwindow close function
//    if (signalso::isRunning_f())
//    {
//        signalso::stopRunning_f();
//    }
    while (not signalso::isTheEnd_f())
    {}

    return returnValue_ext;
}
