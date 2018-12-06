#include "appConfig.hpp"

//for the config file location
#include "essentialQtso/essentialQt.hpp"

//#include <QApplication>
#include <QFile>
//#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QTimer>

void appConfig_c::read_f(const QJsonObject& json)
{
    if (not json["widgetGeometry"].isUndefined())
    {
        QJsonObject jsonWindowGeometryTmp(json["widgetGeometry"].toObject());
        if (not jsonWindowGeometryTmp.keys().isEmpty() )
        {
            widgetGeometryUMap_pri.reserve(jsonWindowGeometryTmp.keys().size());
            for (const QString& jsonKey_ite_con : jsonWindowGeometryTmp.keys())
            {
                QByteArray qByteArrayTmp;
                qByteArrayTmp.append(jsonWindowGeometryTmp.value(jsonKey_ite_con).toString());
                widgetGeometryUMap_pri.insert(jsonKey_ite_con, qUncompress(QByteArray::fromBase64(qByteArrayTmp)));
            }
        }
    }

    if (not json["selectedDirectoryHistory"].isUndefined())
    {
        QJsonArray jsonArraySelectedDirectoryHistoryTmp(json["selectedDirectoryHistory"].toArray());
        if (not jsonArraySelectedDirectoryHistoryTmp.isEmpty())
        {
            //selectedDirectoryHistory_pri.reserve(jsonArraySelectedDirectoryHistoryTmp.size());
            for (const auto jsonArrayItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
            {
                addDirectoryHistory_f(jsonArrayItem_ite_con.toString());
            }
        }
    }

    if (not json["translationConfigFile"].isUndefined())
    {
        translationConfigFileSet_pri = true;
        translationConfigFile_pri = json["translationConfigFile"].toString();
    }
    if (not json["logsDirectoryPath"].isUndefined())
    {
        logsDirectoryPathSet_pri = true;
        logsDirectoryPath_pri = json["logsDirectoryPath"].toString();
    }
}

void appConfig_c::write_f(QJsonObject& json) const
{
    std::vector<QString> directoryHistoryTmp(directoryHistory_f());
    if (not directoryHistoryTmp.empty())
    {
        QJsonArray jsonArraySelectedDirectoryHistoryTmp;
        for (const QString& directoryItem_ite_con : directoryHistoryTmp)
        {
            jsonArraySelectedDirectoryHistoryTmp.append(QJsonValue(directoryItem_ite_con));
        }
        json["selectedDirectoryHistory"] = jsonArraySelectedDirectoryHistoryTmp;
    }

    if (not widgetGeometryUMap_pri.isEmpty())
    {
        QJsonObject pairsTmp;
        QHash<QString, QByteArray>::const_iterator iteratorTmp = widgetGeometryUMap_pri.constBegin();
        while (iteratorTmp not_eq widgetGeometryUMap_pri.constEnd())
        {
            QString qStringTmp;
            qStringTmp.append(qCompress(iteratorTmp.value()).toBase64());
            pairsTmp[iteratorTmp.key()] = qStringTmp;
            ++iteratorTmp;
        }
        json["widgetGeometry"] = pairsTmp;
    }
    if (translationConfigFileSet_pri)
    {
        json["translationConfigFile"] = translationConfigFile_pri;
    }
    if (logsDirectoryPathSet_pri)
    {
        json["logsDirectoryPath"] = logsDirectoryPath_pri;
    }
}

logDataHub_c* appConfig_c::logDataHub_f()
{
    return std::addressof(logDataHub_pri);
}

void appConfig_c::tryLoadTranslations_f()
{
    //check json field, else use default path
    if (translationConfigFile_pri.isEmpty())
    {
        translationConfigFile_pri = fileTypePath_f(fileTypes_ec::translationConfig);
    }

    if (QFile::exists(translationConfigFile_pri))
    {
        translator_pri.readConfigJSONFile_f(translationConfigFile_pri);
        if (translator_pri.isConfigSet_f())
        {
            MACRO_ADDMESSAGE(logDataHub_pri, "Translation/s loaded successful", logItem_c::type_ec::info);
        }
        else
        {

        }
    }

#ifdef DEBUGJOUVEN
    translator_pri.setAddNotFoundKeys_f(true);
#endif
    if (translator_pri.addNotFoundKeys_f() and not translator_pri.isConfigSet_f())
    {
        MACRO_ADDMESSAGE(logDataHub_pri, R"(Translation/s not loaded, adding "empty", "hard-coded"-"english" translation)", logItem_c::type_ec::info);
        translator_pri.addEmptyLanguageLink_f("hard-coded", "english");
        translator_pri.setTranslateFromLanguage_f("hard-coded");
        translator_pri.setTranslateToLanguageChain_f({"english"});
    }
}

appConfig_c::appConfig_c()
{
    commandLineParser_pri.setApplicationDescription("ActonQtg, GUI program to manage acton files");
    commandLineParser_pri.addHelpOption();
    commandLineParser_pri.addVersionOption();
    //FUTURE parse arguments for, languagefile, log location...
    commandLineParser_pri.addPositionalArgument("action file paths", "Optional, path/s to action files to be loaded, they must be compatible (all sequential or all dependency)");
    //FUTURE flag to run the loaded action files

    commandLineParser_pri.process(*qApp);

    locateConfigFilePath_f(commandLineParser_pri, false);


    //no errors here, load if possible else skip
    //if there is any file to load

    //load actonQtg generic config
    while (configFilePath_f().second)
    {
        QFile configFileLoad(configFilePath_f().first);
        QByteArray jsonByteArray;
        if (configFileLoad.open(QIODevice::ReadOnly))
        {
            jsonByteArray = configFileLoad.readAll();
        }
        else
        {
            break;
        }

        QJsonDocument jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
        if (jsonDocObj.isNull())
        {
            break;
        }

        read_f(jsonDocObj.object());
        loadChecksum_pri = QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Md5);
        configLoaded_pri = true;
#ifdef DEBUGJOUVEN
        qtOutRef_ext() << "App config loaded successful" << endl;
#endif
        break;
    }

    logDataHub_pri.setLogSaveDirectoryPath_f(logsDirectoryPath_pri);
    if (logDataHub_pri.isValidLogPathBaseName_f())
    {
        logDataHub_pri.setSaveLogFiles_f(true);
    }
    else
    {
#ifdef DEBUGJOUVEN
        qtOutRef_ext() << "Default log path couldn't be set" << endl;
#endif
    }
    logDataHub_pri.loadLogFiles_f(QString(), logFilter_c(), true, true);

    //load actonQtg translations
    tryLoadTranslations_f();
}


bool appConfig_c::saveConfigFile_f()
{
    bool configSavedTmp(false);

    QString configFileStr;
    if (configFilePath_f().second)
    {
        configFileStr = configFilePath_f().first;
    }
    else
    {
        configFileStr = fileTypePath_f(fileTypes_ec::config);
    }

    QJsonObject jsonObjectTmp;
    write_f(jsonObjectTmp);
    QJsonDocument jsonDocumentTmp(jsonObjectTmp);
    QByteArray jsonByteArray(jsonDocumentTmp.toJson(QJsonDocument::Indented));

    if (QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Md5) not_eq loadChecksum_pri)
    {
        QFile configFileSaveTmp(configFileStr);
        if (configFileSaveTmp.open(QIODevice::WriteOnly))
        {
            configFileSaveTmp.write(jsonByteArray);
            configSavedTmp = true;
        }
    }

#ifdef DEBUGJOUVEN
    translator_pri.setAddNotFoundKeys_f(false);
    translator_pri.writeConfigJSONFile_f(fileTypePath_f(fileTypes_ec::translationConfig), true);
#endif

    return configSavedTmp;
}

bool appConfig_c::configLoaded_f() const
{
    return configLoaded_pri;
}

QByteArray appConfig_c::widgetGeometry_f(const QString& key_par_con) const
{
    return widgetGeometryUMap_pri.value(key_par_con);
}

void appConfig_c::setWidgetGeometry_f(const QString& key_par_con, const QByteArray& windowGeometry_par_con)
{
    widgetGeometryUMap_pri.insert(key_par_con, windowGeometry_par_con);
}


void appConfig_c::addDirectoryHistory_f(const QString& directory_par_con)
{
    uint_fast64_t nowTmp(std::chrono::steady_clock::now().time_since_epoch().count());
    directoryPathToDateTime_pri.insert(directory_par_con, nowTmp);
    dateTimeToDirectoryPath_pri.insert(nowTmp, directory_par_con);
    if (dateTimeToDirectoryPath_pri.size() > 10)
    {
        uint_fast64_t lastItemKeyTmp(dateTimeToDirectoryPath_pri.firstKey());
        QString lastItemValueTmp(dateTimeToDirectoryPath_pri.first());

        directoryPathToDateTime_pri.remove(lastItemValueTmp);
        dateTimeToDirectoryPath_pri.remove(lastItemKeyTmp);
    }
}

QString appConfig_c::translate_f(const QString& key_par_con)
{
    QString resultTmp;
    if (translator_pri.isConfigSet_f() or translator_pri.addNotFoundKeys_f())
    {
        resultTmp = translator_pri.translate_f(key_par_con);
        //because the result will be an empty string
        if (resultTmp.isEmpty() and translator_pri.addNotFoundKeys_f())
        {
             resultTmp = key_par_con;
        }
    }
    else
    {
        resultTmp = key_par_con;
    }
    return resultTmp;
}

bool appConfig_c::addLogMessage_f(
        const QString& message_par_con
        , const logItem_c::type_ec logType_par_con
        , const QString& sourceFile_par_con
        , const QString& sourceFunction_par_con
        , const int_fast32_t line_par_con
)
{
    return logDataHub_pri.addMessage_f(message_par_con, logType_par_con, sourceFile_par_con, sourceFunction_par_con, line_par_con);
}

std::vector<std::pair<const logItem_c* const, const QDateTime* const>> appConfig_c::getLogs_f(const logFilter_c& logFilter_par_con) const
{
    return logDataHub_pri.filter_f(logFilter_par_con);
}

QStringList appConfig_c::commandLinePositionalArguments_f() const
{
    return commandLineParser_pri.positionalArguments();
}

std::vector<QString> appConfig_c::directoryHistory_f() const
{
    std::vector<QString> historyVector;
    historyVector.reserve(dateTimeToDirectoryPath_pri.size());
    //(from qt doc) "If you only need to extract the values from a map (not the keys), you can also use foreach:"
    for (const QString& directory_ite_con : dateTimeToDirectoryPath_pri)
    {
        historyVector.emplace_back(directory_ite_con);
    }
    //since it's on ascending order the old ones will be first, reverse
    std::reverse(historyVector.begin(), historyVector.end());
    return historyVector;
}

appConfig_c* appConfig_ptr_ext = nullptr;
//appConfig_c& appConfig_f()
//{
//    static appConfig_c appConfig_sta;
//    return appConfig_sta;
//}
