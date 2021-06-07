#ifndef ACTONQTG_APPCONFIG_HPP
#define ACTONQTG_APPCONFIG_HPP

#include "programConfigQtgso/programConfigGUI.hpp"

class action_c;
class check_c;

class appConfig_c : public programConfigGUI_c
{
    Q_OBJECT

    //bool configLoaded_pri = false;

    //QHash<QString, QByteArray> widgetGeometryUMap_pri;
    //first key is dialog name/id, nested qmap key is datetime and value is the directory path
    //QHash<QString, QMap<int_fast64_t, QString>> fileDialogNameToDirectoryNameAndTimeMap_pri;

    //to prevent resaving if the file data is the same as the data being saved
    //QByteArray loadChecksum_pri;

    //right now, translationConfigFile_pri and logsDirectoryPath_pri
    //can't be set from-inside the program (TODO),
    //the set variables will confirm when a legit SET of these variables happens.
    //When not set the program will use default values but
    //still it won't SET the variables so they don't get saved with the default values
    //because the default path nature is dynamic
    //or the default value is an absolute path (if the program gets moved it will use the old path still)
    //right now they only get SET if they come from the json config file
    //QString translationConfigFile_pri;
    //bool translationConfigFileSet_pri = false;

//    translator_c translator_pri;

//    QString logsDirectoryPath_pri;
//    bool logsDirectoryPathSet_pri = false;

//    logDataHub_c logDataHub_pri;

//    QCommandLineParser commandLineParser_pri;

//    void tryLoadTranslations_f();

    QStringList positionalArguments_pri;

    void derivedReadJSON_f(const QJsonObject &json_par_con) override;
    void derivedWriteJSON_f(QJsonObject &json_par) const override;
    void derivedWriteJSONDocumented_f(QJsonObject&) const override
    {}
    bool firstPositionalArgumentCanBeConfig_f() const override
    {
        return false;
    }
    bool translationCreatePlacerholders_f() const override
    {
        return true;
    }
    bool loggingSaveLogFileRequired_f() const override
    {
        return true;
    }
    QString programName_f() const override
    {
        return "ActonQtg, GUI program to manage acton files";
    }
    text_c programDescription_f() const override
    {
        return "ActonQtg description\nCreated by Jouven";
    }
    QString programVersion_f() const override
    {
        return "1.0";
    }
    //only for the programconfigQt stuff but nothing to implement here
    bool requiresJSONDocumented_f() const override
    {
        return true;
    }
    void derivedConfigureCommandLineParser_f(QCommandLineParser& parser_par) const override;
    void derivedCheckCommandLineParser_f(QCommandLineParser& parser_par) override;
    void derivedStart_f() override;
    void derivedQuit_f() override;

public:
    appConfig_c(QObject* parent_par);

    //bool saveConfigFile_f();

    //bool configLoaded_f() const;

    //I would use a pair but restoring states with an empty QByteArray doesn't do anything... so it's ok for now
    //QByteArray widgetGeometry_f(const QString& key_par_con) const;
    //void setWidgetGeometry_f(const QString& key_par_con, const QByteArray& windowGeometry_par_con);

    //std::vector<QString> directoryHistory_f(const QString& fileDialogStringId_par_con) const;
    //void addDirectoryHistory_f(const QString& fileDialogStringId_par_con, const QString& directory_par_con);

    //QString translate_f(const QString& key_par_con);
    //QString translateAndReplace_f(const text_c& text_par_con);
    //QString translateAndReplace_f(const textCompilation_c& textCompilation_par_con);
    //FUTURE allow to change language using translationFromToPairs_f (the "to" part)
    //Making a program like "translationParserAvidcalm" (which is another program made by me, that parses the source js/cpp files for translations)
    //but to generate skeleton json language files
    //(from language will always be hardcoded because the initial source-language is always harcoded)

//    bool addLogMessage_f(
//            const text_c& message_par_con
//            , const messageType_ec logType_par_con
//            , const QString& sourceFile_par_con
//            , const QString& sourceFunction_par_con
//            , const int_fast32_t line_par_con
//    );
//    bool addLogMessage_f(
//            const text_c& message_par_con
//            , const action_c* actionPtr_par
//            , const messageType_ec logType_par_con
//            , const QString& sourceFile_par_con
//            , const QString& sourceFunction_par_con
//            , const int_fast32_t line_par_con
//    );
//    bool addLogMessage_f(
//            const text_c& message_par_con
//            , const check_c* checkPtr_par
//            , const messageType_ec logType_par_con
//            , const QString& sourceFile_par_con
//            , const QString& sourceFunction_par_con
//            , const int_fast32_t line_par_con
//    );

    QStringList commandLinePositionalArguments_f() const;

    logDataHub_c* logDataHub_f();
};

//needs to be initialized
extern appConfig_c* appConfig_ptr_ext;

#endif // ACTONQTG_APPCONFIG_HPP
