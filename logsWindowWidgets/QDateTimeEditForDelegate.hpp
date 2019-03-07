#ifndef ACTONQTG_QDATETIMEEDITFORDELEGATE_HPP
#define ACTONQTG_QDATETIMEEDITFORDELEGATE_HPP

#include <QDateTimeEdit>

//NOT in use
//see ctor
class QDatetimeEdit_c : public QDateTimeEdit
{
    Q_OBJECT

    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime USER true)
public:
    QDatetimeEdit_c(QWidget* parent_par);
};

#endif // ACTONQTG_QDATETIMEEDITFORDELEGATE_HPP
