#ifndef ACTONQTG_APPCONFIG_HPP
#define ACTONQTG_APPCONFIG_HPP

#include "translatorJSONQtso/translator.hpp"
#include "logsinJSONQtso/logDataHub.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QHash>
#include <QMap>

#include <vector>


//don't call inside the appConfig_c class (like calling from the ctor and hanging the process... without errors...)
//inside use MACRO_ADDMESSAGE
#define MACRO_ADDACTONQTGLOG(MESSAGE, TYPE) appConfig_ptr_ext->addLogMessage_f(MESSAGE, TYPE, MACRO_FILENAME, __func__, __LINE__)

class appConfig_c
{
    bool configLoaded_pri = false;

    QHash<QString, QByteArray> widgetGeometryUMap_pri;

    //like the actionData with row and actionDataId TWO are needed, this time order is required
    //variables and functions to control the directory history
    //key = path, value = time
    QMap<QString, uint_fast64_t> directoryPathToDateTime_pri;
    //lower values are first, ascending order (so time wise older are first),
    //delete old using first
    //key = time, value = path
    QMap<uint_fast64_t, QString> dateTimeToDirectoryPath_pri;

    //to prevent resaving if the file data is the same as the data being saved
    QByteArray loadChecksum_pri;

    //right now, translationConfigFile_pri and logsDirectoryPath_pri
    //can't be set in the program (TODO),
    //the set variables will confirm when a legit SET of these variables happens.
    //When not set the program will use default values but
    //still it won't SET the variables so they don't get saved with the default values
    //because or the default path nature is dynamic
    //or the default value is an absolute path (if the program gets moved it will use the old path still)
    //right now they only get SET if the come from the json config file
    QString translationConfigFile_pri;
    bool translationConfigFileSet_pri = false;

    translator_c translator_pri;

    QString logsDirectoryPath_pri;
    bool logsDirectoryPathSet_pri = false;

    logDataHub_c logDataHub_pri;

    QCommandLineParser commandLineParser_pri;

    void tryLoadTranslations_f();

    void read_f(const QJsonObject &json);
    void write_f(QJsonObject &json) const;
public:
    appConfig_c();

    bool saveConfigFile_f();

    bool configLoaded_f() const;

    //I would use a pair but restoring states with an empty QByteArray doesn't do anything... so it's ok for now
    QByteArray widgetGeometry_f(const QString& key_par_con) const;
    void setWidgetGeometry_f(const QString& key_par_con, const QByteArray& windowGeometry_par_con);

    std::vector<QString> directoryHistory_f() const;
    void addDirectoryHistory_f(const QString& directory_par_con);

    QString translate_f(const QString& key_par_con);
    //FUTURE allow to change language using translationFromToPairs_f (the "to" part)
    //when making a program like translationParserAvidcalm but to generate skeleton json language files
    //from language will always be "hardcode" (because it is)

    bool addLogMessage_f(
            const QString& message_par_con
            , const logItem_c::type_ec logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    );

    std::vector<std::pair<const logItem_c* const, const QDateTime* const> > getLogs_f(const logFilter_c& logFilter_par_con = logFilter_c()) const;

    QStringList commandLinePositionalArguments_f() const;

    logDataHub_c* logDataHub_f();
};

extern appConfig_c* appConfig_ptr_ext;

#endif // ACTONQTG_APPCONFIG_HPP
