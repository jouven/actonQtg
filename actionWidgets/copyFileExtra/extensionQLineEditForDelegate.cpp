#include "extensionQLineEditForDelegate.hpp"


extensionQLineEdit_c::extensionQLineEdit_c(QWidget* parent_par)
    : QLineEdit(parent_par)
{
    setMaxLength(255);
}
