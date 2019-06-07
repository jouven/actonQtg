#include "baseClassCheckWidgets.hpp"

#include "actonQtso/checkData.hpp"

baseClassCheckWidgets_c::baseClassCheckWidgets_c(
        check_c*& checkPtrRef_par
        , QObject* parent_par)
 : QObject(parent_par)
 , checkPtr_pro(checkPtrRef_par)
{}

bool baseClassCheckWidgets_c::saveUpdate_f()
{
    return derivedSaveUpdate_f();
}

bool baseClassCheckWidgets_c::saveNew_f(const checkData_c& checkData_par_con)
{
    return derivedSaveNew_f(checkData_par_con);
}

void baseClassCheckWidgets_c::parentClosing_f()
{
    derivedParentClosing_f();
}

QString baseClassCheckWidgets_c::extraTips_f() const
{
    return derivedExtraTips_f();
}

