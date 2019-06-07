#include "appConfig.hpp"

//#include <QApplication>
#include <QFile>
//#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QTimer>
#include <QJsonObject>

void appConfig_c::read_f(const QJsonObject& json_par_con)
{
    if (not json_par_con["widgetGeometry"].isUndefined())
    {
        QJsonObject jsonWindowGeometryTmp(json_par_con["widgetGeometry"].toObject());
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

    if (not json_par_con["fileDialogsSettings"].isUndefined())
    {
        QJsonArray jsonArraySelectedDirectoryHistoryTmp(json_par_con["fileDialogsSettings"].toArray());
        if (not jsonArraySelectedDirectoryHistoryTmp.isEmpty())
        {
            for (const auto jsonArrayFileDialogItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
            {
                QJsonObject fileDialogSettingObjectTmp(jsonArrayFileDialogItem_ite_con.toObject());
                QJsonArray jsonArraySelectedDirectoryHistoryTmp(fileDialogSettingObjectTmp["selectedDirectoryHistory"].toArray());
                if (not jsonArraySelectedDirectoryHistoryTmp.isEmpty())
                {
                    //put them in a vector because since they were written in the array in order,
                    //first one had the bigger/recent time,
                    //when using addDirectoryHistory_f, the first one from the array will get
                    //the smaller/later time, a reverse is necessary
                    std::vector<QString> directoryVectorTmp;
                    for (const auto jsonArrayDirectoryHistoryItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
                    {
                        directoryVectorTmp.emplace_back(jsonArrayDirectoryHistoryItem_ite_con.toString());
                    }
                    std::reverse(directoryVectorTmp.begin(), directoryVectorTmp.end());
                    for (const QString& directoryItem_ite_con : directoryVectorTmp)
                    {
                        addDirectoryHistory_f(fileDialogSettingObjectTmp["fileDialogStringId"].toString(), directoryItem_ite_con);
                    }
                }
            }
        }
    }

    if (not json_par_con["translationConfigFile"].isUndefined())
    {
        translationConfigFileSet_pri = true;
        translationConfigFile_pri = json_par_con["translationConfigFile"].toString();
    }
    if (not json_par_con["logsDirectoryPath"].isUndefined())
    {
        logsDirectoryPathSet_pri = true;
        logsDirectoryPath_pri = json_par_con["logsDirectoryPath"].toString();
    }
}

void appConfig_c::write_f(QJsonObject& json_par) const
{
    //create a filedialogsSettings arraay
    //and the object will have the directory history for now
    QJsonArray jsonArrayFileDialongSettingsTmp;
    QHash<QString, QMap<int_fast64_t, QString>>::const_iterator iteTmp(fileDialogNameToDirectoryNameAndTimeMap_pri.constBegin());
    while (iteTmp not_eq fileDialogNameToDirectoryNameAndTimeMap_pri.constEnd())
    {
        std::vector<QString> directoryHistoryTmp(directoryHistory_f(iteTmp.key()));
        if (not directoryHistoryTmp.empty())
        {
            QJsonArray jsonArraySelectedDirectoryHistoryTmp;
            for (const QString& directoryItem_ite_con : directoryHistoryTmp)
            {
                jsonArraySelectedDirectoryHistoryTmp.append(QJsonValue(directoryItem_ite_con));
            }
            QJsonObject fileDialogSettingsTmp;
            fileDialogSettingsTmp["fileDialogStringId"] = iteTmp.key();
            fileDialogSettingsTmp["selectedDirectoryHistory"] = jsonArraySelectedDirectoryHistoryTmp;
            jsonArrayFileDialongSettingsTmp.append(fileDialogSettingsTmp);
        }
        ++iteTmp;
    }
    json_par["fileDialogsSettings"] = jsonArrayFileDialongSettingsTmp;

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
        json_par["widgetGeometry"] = pairsTmp;
    }
    if (translationConfigFileSet_pri)
    {
        json_par["translationConfigFile"] = translationConfigFile_pri;
    }
    if (logsDirectoryPathSet_pri)
    {
        json_par["logsDirectoryPath"] = logsDirectoryPath_pri;
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


void appConfig_c::addDirectoryHistory_f(
        const QString& fileDialogStringId_par_con
        , const QString& directory_par_con
)
{
    uint_fast64_t nowTmp(std::chrono::steady_clock::now().time_since_epoch().count());
    if (fileDialogNameToDirectoryNameAndTimeMap_pri.count(fileDialogStringId_par_con) > 0)
    {

    }
    else
    {
        QMap<int_fast64_t, QString> accessTimeToDirectoryMapTmp;
        fileDialogNameToDirectoryNameAndTimeMap_pri.insert(fileDialogStringId_par_con, accessTimeToDirectoryMapTmp);
    }

    QHash<QString, QMap<int_fast64_t, QString>>::iterator accessTimeToDirectoryMapTmp(fileDialogNameToDirectoryNameAndTimeMap_pri.find(fileDialogStringId_par_con));
    accessTimeToDirectoryMapTmp->insert(nowTmp, directory_par_con);

    if (accessTimeToDirectoryMapTmp->size() > 10)
    {
        uint_fast64_t lastItemKeyTmp(accessTimeToDirectoryMapTmp->firstKey());
        accessTimeToDirectoryMapTmp->remove(lastItemKeyTmp);
    }
}

QString appConfig_c::translate_f(const QString& key_par_con)
{
    QString resultTmp;
    bool foundTmp(false);
    if (translator_pri.isConfigSet_f())
    {
        resultTmp = translator_pri.translate_f(key_par_con, std::addressof(foundTmp));
    }
    if (not foundTmp)
    {
        resultTmp = key_par_con;
    }
    //try to translate else return the same key, otherwise a lot of texts/labels will be empty
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

std::vector<QString> appConfig_c::directoryHistory_f(const QString& fileDialogStringId_par_con) const
{
    std::vector<QString> historyVector;
    if (fileDialogNameToDirectoryNameAndTimeMap_pri.count(fileDialogStringId_par_con) > 0)
    {
        QMap<int_fast64_t, QString> accessTimeToDirectoryMapTmp(fileDialogNameToDirectoryNameAndTimeMap_pri.value(fileDialogStringId_par_con));
        historyVector.reserve(accessTimeToDirectoryMapTmp.size());
        //(from qt doc) "If you only need to extract the values from a map (not the keys), you can also use foreach:"
        for (const QString& directory_ite_con : accessTimeToDirectoryMapTmp)
        {
            historyVector.emplace_back(directory_ite_con);
        }
        //since the default order is using "less" the old ones will be first, reverse
        std::reverse(historyVector.begin(), historyVector.end());
    }
    return historyVector;
}

appConfig_c* appConfig_ptr_ext = nullptr;
//appConfig_c& appConfig_f()
//{
//    static appConfig_c appConfig_sta;
//    return appConfig_sta;
//}
