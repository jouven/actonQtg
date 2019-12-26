#include "baseClassCheckTypeWidgets.hpp"

#include "../appConfig.hpp"

#include "actonQtso/checkData.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "textQtso/text.hpp"

baseClassCheckTypeWidgets_c::baseClassCheckTypeWidgets_c(
        check_c*& checkPtrRef_par
        , QObject* parent_par)
 : QObject(parent_par)
 , checkPtr_pro(checkPtrRef_par)
{}

bool baseClassCheckTypeWidgets_c::saveUpdate_f()
{
    return derivedSaveUpdate_f();
}

bool baseClassCheckTypeWidgets_c::saveNew_f(const checkData_c& checkData_par_con)
{
    return derivedSaveNew_f(checkData_par_con);
}

void baseClassCheckTypeWidgets_c::parentClosing_f()
{
    derivedParentClosing_f();
}

QString baseClassCheckTypeWidgets_c::extraTips_f() const
{
    return derivedExtraTips_f();
}

