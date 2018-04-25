#include "appConfig.hpp"

//for the config file location
#include "essentialQtso/essentialQt.hpp"

//#include <QApplication>
#include <QFile>
//#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCryptographicHash>

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
            for (const auto& jsonArrayItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
            {
                addDirectoryHistory_f(jsonArrayItem_ite_con.toString());
            }
        }
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
        while (iteratorTmp != widgetGeometryUMap_pri.constEnd())
        {
            QString qStringTmp;
            qStringTmp.append(qCompress(iteratorTmp.value()).toBase64());
            pairsTmp[iteratorTmp.key()] = qStringTmp;
            ++iteratorTmp;
        }
        json["widgetGeometry"] = pairsTmp;
    }
}

appConfig_c::appConfig_c()
{
    //no errors here, load if possible else skip
    //if there is any file to load
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
        else
        {
            read_f(jsonDocObj.object());
            loadChecksum_pri = QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Md5);
            configLoaded_pri = true;
        }
        break;
    }
}


bool appConfig_c::saveConfigFile_f() const
{
    bool configSavedTmp(false);

    QString configFileStr;
    if (configFilePath_f().second)
    {
        configFileStr = configFilePath_f().first;
    }
    else
    {
        configFileStr = configFileDefaultPath_f();
    }

    QJsonObject jsonObjectTmp;
    write_f(jsonObjectTmp);
    QJsonDocument jsonDocumentTmp(jsonObjectTmp);
    QByteArray jsonByteArray(jsonDocumentTmp.toJson(QJsonDocument::Indented));

    if (QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Md5) != loadChecksum_pri)
    {
        QFile configFileSaveTmp(configFileStr);
        if (configFileSaveTmp.open(QIODevice::WriteOnly))
        {
            configFileSaveTmp.write(jsonByteArray);
            configSavedTmp = true;
        }
    }

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

appConfig_c&appConfig_f()
{
    static appConfig_c appConfig_sta;
    return appConfig_sta;
}
