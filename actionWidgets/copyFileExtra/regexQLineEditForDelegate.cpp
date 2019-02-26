#include "regexQLineEditForDelegate.hpp"


regexQLineEdit_c::regexQLineEdit_c(QWidget* parent_par)
    : QLineEdit(parent_par)
{
    setMaxLength(1024);
}
