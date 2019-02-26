#ifndef ACTONQTG_REGEXQLINEEDITFORDELEGATE_HPP
#define ACTONQTG_REGEXQLINEEDITFORDELEGATE_HPP

#include <QLineEdit>

class regexQLineEdit_c : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(QString String READ text WRITE setText USER true)
public:
    regexQLineEdit_c(QWidget* parent_par);
};

#endif // ACTONQTG_REGEXQLINEEDITFORDELEGATE_HPP
