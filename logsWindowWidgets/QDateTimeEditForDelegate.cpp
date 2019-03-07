#include "QDateTimeEditForDelegate.hpp"


QDatetimeEdit_c::QDatetimeEdit_c(QWidget* parent_par)
    : QDateTimeEdit(parent_par)
{
    //setTimeSpec(Qt::LocalTime);
    setDisplayFormat("yyyy-MM-dd hh:mm:ss.zzz");
}
