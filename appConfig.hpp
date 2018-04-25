#ifndef ACTONQTG_APPCONFIG_HPP
#define ACTONQTG_APPCONFIG_HPP

#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QHash>
#include <QMap>

#include <vector>

class appConfig_c
{
    bool configLoaded_pri = false;

    QHash<QString, QByteArray> widgetGeometryUMap_pri;

    //like the actionData with row and actionDataId two are needed, this time order is required
    //variables and functions to control the directory history
    //key = path, value = time
    QMap<QString, uint_fast64_t> directoryPathToDateTime_pri;
    //lower values are first, ascending order (so time wise older are first),
    //delete old using first
    //key = time, value = path
    QMap<uint_fast64_t, QString> dateTimeToDirectoryPath_pri;

    //to prevent resaving if the file data is the same as the data being saved
    QByteArray loadChecksum_pri;

    void read_f(const QJsonObject &json);
    void write_f(QJsonObject &json) const;
public:
    appConfig_c();

    bool saveConfigFile_f() const;

    bool configLoaded_f() const;

    //I would use a pair but restoring states with an empty QByteArray doesn't do anything... so it's ok for now
    QByteArray widgetGeometry_f(const QString& key_par_con) const;
    void setWidgetGeometry_f(const QString& key_par_con, const QByteArray& windowGeometry_par_con);

    std::vector<QString> directoryHistory_f() const;
    void addDirectoryHistory_f(const QString& directory_par_con);
};

extern appConfig_c& appConfig_f();

#endif // ACTONQTG_APPCONFIG_HPP
