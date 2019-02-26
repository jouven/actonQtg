#ifndef ACTONQTG_EXTENSIONQLINEEDITFORDELEGATE_HPP
#define ACTONQTG_EXTENSIONQLINEEDITFORDELEGATE_HPP

#include <QLineEdit>

class extensionQLineEdit_c : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(QString String READ text WRITE setText USER true)
public:
    extensionQLineEdit_c(QWidget* parent_par);
};

#endif // ACTONQTG_EXTENSIONQLINEEDITFORDELEGATE_HPP
