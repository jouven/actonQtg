#ifndef ACTONQTG_QDATETIMEEDITDELEGATE_HPP
#define ACTONQTG_QDATETIMEEDITDELEGATE_HPP

//TODO move file to a more "generic" path, do it when I create another class that like this
#include <QStyledItemDelegate>

//"this issue" should not affect what this object is being used for,
//since this delegate it's used in field/s that have no editor (readonly)
//from setItemDelegateForColumn qt documentation:
//Warning: You should not share the same instance of a delegate between views.
//Doing so can cause incorrect or unintuitive editing behavior since each view connected to a given delegate
//may receive the closeEditor() signal, and attempt to access, modify or close an editor that has already
//been closed.

//see ctor
class QDateTimeStyledDelegate_c : public QStyledItemDelegate
{
    Q_OBJECT

    QString displayText(const QVariant &value, const QLocale &) const;
public:
    QDateTimeStyledDelegate_c(QWidget* parent_par);
};

#endif // ACTONQTG_QDATETIMEEDITFORDELEGATE_HPP
