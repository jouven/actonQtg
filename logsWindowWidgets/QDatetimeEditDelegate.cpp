#include "QDatetimeEditDelegate.hpp"

#include <QDateTime>

QString QDateTimeStyledDelegate_c::displayText(const QVariant& value, const QLocale& ) const
{
    return value.toDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QDateTimeStyledDelegate_c::QDateTimeStyledDelegate_c(QWidget* parent_par)
    : QStyledItemDelegate(parent_par)
{
}
