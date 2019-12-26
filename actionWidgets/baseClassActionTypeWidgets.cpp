#include "baseClassActionTypeWidgets.hpp"

#include "../appConfig.hpp"

#include "actonQtso/actionData.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"

baseClassActionTypeWidgets_c::baseClassActionTypeWidgets_c(
        action_c*& actionPtrRef_par
        , QObject* parent_par)
 : QObject(parent_par)
 , actionPtr_pro(actionPtrRef_par)
{}

bool baseClassActionTypeWidgets_c::saveUpdate_f()
{
    return derivedSaveUpdate_f();
}

bool baseClassActionTypeWidgets_c::saveNew_f(const actionData_c& actionData_par_con)
{
    return derivedSaveNew_f(actionData_par_con);
}

void baseClassActionTypeWidgets_c::parentClosing_f()
{
    derivedParentClosing_f();
}

QString baseClassActionTypeWidgets_c::extraTips_f() const
{
    return derivedExtraTips_f();
}

