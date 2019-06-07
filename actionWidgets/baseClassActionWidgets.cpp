#include "baseClassActionWidgets.hpp"

#include "actonQtso/actionData.hpp"

baseClassActionWidgets_c::baseClassActionWidgets_c(
        action_c*& actionPtrRef_par
        , QObject* parent_par)
 : QObject(parent_par)
 , actionPtr_pro(actionPtrRef_par)
{}

bool baseClassActionWidgets_c::saveUpdate_f()
{
    return derivedSaveUpdate_f();
}

bool baseClassActionWidgets_c::saveNew_f(const actionData_c& actionData_par_con)
{
    return derivedSaveNew_f(actionData_par_con);
}

void baseClassActionWidgets_c::parentClosing_f()
{
    derivedParentClosing_f();
}

QString baseClassActionWidgets_c::extraTips_f() const
{
    return derivedExtraTips_f();
}

