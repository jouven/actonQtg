#include "mainWindow.hpp"

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

    QStringList positionalArgs;
    {
        QCommandLineParser commandLineParser;
        commandLineParser.setApplicationDescription("ActonQtg, GUI program to manage acton files");
        commandLineParser.addHelpOption();
        commandLineParser.addVersionOption();
        commandLineParser.addPositionalArgument("action file paths", "Optional, path/s to action files to be loaded, they must be compatible (all sequential or all dependency)");
        //FUTURE flag to run the loaded action files
        //FUTURE flag to specify non default path config file

        commandLineParser.process(*qApp);
        positionalArgs = commandLineParser.positionalArguments();

        locateConfigFilePath_f(commandLineParser, false);
    }

    mainWindow_c mainWindowTmp;// = new mainWindow_c;
    mainWindowTmp.show();
    mainWindowTmp.processPositionalArguments_f(positionalArgs);
    returnValue_ext = qtapp.exec();

    if (eines::signal::isRunning_f())
    {
        eines::signal::stopRunning_f();
    }
    while (not eines::signal::isTheEnd_f())
    {}

    return returnValue_ext;
}
