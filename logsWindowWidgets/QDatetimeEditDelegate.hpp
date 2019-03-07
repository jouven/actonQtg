#ifndef ACTONQTG_QDATETIMEEDITDELEGATE_HPP
#define ACTONQTG_QDATETIMEEDITDELEGATE_HPP

#include <QStyledItemDelegate>

//see ctor
class QDateTimeStyledDelegate_c : public QStyledItemDelegate
{
    Q_OBJECT

    QString displayText(const QVariant &value, const QLocale &locale) const;
public:
    QDateTimeStyledDelegate_c(QWidget* parent_par);
};

#endif // ACTONQTG_QDATETIMEEDITFORDELEGATE_HPP
