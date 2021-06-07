#include "mainWindow.hpp"

#include "appConfig.hpp"
#include "actonDataHubGlobal.hpp"
#include "stringParserGlobal.hpp"

#include "signalso/signal.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QApplication>

#ifdef DEBUGJOUVEN
#include <QDebug>
#ifndef Q_OS_WIN
//this is to get pretty stacktrace when the execution crashes
//instructions:
//1 this only applies to program projects, libs don't need this (libs need debug, -gX flags when compiling)
//2 link to -ldw or the elftutils library
//3 set the DEFINES in the .pro BACKWARD_HAS_UNWIND BACKWARD_HAS_DW (check backward.hpp source for more info about the macros)
//more info https://github.com/bombela/backward-cpp
#include "backward-cpp/backward.hpp"
namespace {
backward::SignalHandling sh;
}
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

    mainWindow_c mainWindowTmp;
    mainWindow_ptr_ext = std::addressof(mainWindowTmp);

    //translation and logs are initialized here
    appConfig_c appConfigTmp(nullptr);
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

//#ifdef DEBUGJOUVEN
//    qDebug() << "actonDataHubTmp.setLogDataHub_f(appConfigTmp.logDataHub_f());";
//#endif
    //logs here won't happen because appConfig hasn't loaded the logs yet
    //MACRO_ADDLOG("Create main window", QString(), messageType_ec::information);

    returnValue_ext = QApplication::exec();

    MACRO_ADDLOG("Exit", QString(), messageType_ec::information);

    return returnValue_ext;
}
