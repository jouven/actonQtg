#include "metaEndExecutionCycleWidgets.hpp"

//#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/actions/metaEndExecutionCycle.hpp"

//#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
//#include <QSplitter>


void metaEndExecutionCycleWidgets_c::derivedParentClosing_f()
{
    //no widget geometry to save yet
}

QString metaEndExecutionCycleWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>meta end execution cycle widget tips</p>");
}

metaEndExecutionCycleData_c metaEndExecutionCycleWidgets_c::fieldsToMetaEndExecutionCycleDataObject_f() const
{
    metaEndExecutionCycleData_c::endType_ec endTypeTmp(metaEndExecutionCycleData_c::strToEndExecutionTypeMap_sta_con.value(endTypeCombo_pri->currentData().toString()));

    return metaEndExecutionCycleData_c(
                endTypeTmp
                , killAfterStopTimeoutCheckbox_pri->isChecked()
    );
}

bool metaEndExecutionCycleWidgets_c::derivedSaveNew_f(const actionData_c& actionDataBlock_par_con, actonDataHub_c* parentActonDataHub_par)
{
    bool resultTmp(false);
    metaEndExecutionCycleData_c objTmp(fieldsToMetaEndExecutionCycleDataObject_f());
    metaEndExecutionCycleAction_ptr_pri = new metaEndExecutionCycleAction_c(parentActonDataHub_par, actionDataBlock_par_con, objTmp);
    actionPtr_pro = metaEndExecutionCycleAction_ptr_pri;
    resultTmp = true;
    return resultTmp;
}

bool metaEndExecutionCycleWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    metaEndExecutionCycleData_c objTmp(fieldsToMetaEndExecutionCycleDataObject_f());
    metaEndExecutionCycleAction_ptr_pri->updateMetaEndExecutionCycleData_f(objTmp);
    resultTmp = true;
    return resultTmp;
}

void metaEndExecutionCycleWidgets_c::loadActionSpecificData_f()
{
    metaEndExecutionCycleData_c valuesToLoadTmp(metaEndExecutionCycleAction_ptr_pri not_eq nullptr ? *metaEndExecutionCycleAction_ptr_pri : metaEndExecutionCycleData_c());

    QString endTypeStrTmp(metaEndExecutionCycleData_c::endExecutionTypeToStrUMap_sta_con.at(valuesToLoadTmp.endType_f()));
    int loadedIndexTmp(endTypeCombo_pri->findData(endTypeStrTmp.toLower()));
    endTypeCombo_pri->setCurrentIndex(loadedIndexTmp);

    killAfterStopTimeoutCheckbox_pri->setChecked(valuesToLoadTmp.killAfterTimeout_f());
}

metaEndExecutionCycleWidgets_c::metaEndExecutionCycleWidgets_c(
        action_c*& actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        )
    : baseClassActionTypeWidgets_c(actionData_ptr_par, variableLayout_par_con->parentWidget())
    , metaEndExecutionCycleAction_ptr_pri(actionData_ptr_par == nullptr ? nullptr : static_cast<metaEndExecutionCycleAction_c*>(actionData_ptr_par))
{
    this->setObjectName("metaEndExecutionCycleWidgets_");

    //end type combo and kill after timeout checkbox
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    endTypeCombo_pri = new QComboBox();
    endTypeCombo_pri->setEditable(true);
    endTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    endTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    endTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    endTypeCombo_pri->setToolTip(appConfig_ptr_ext->translate_f(
R"(Wait: wait for the executing actions to finish and don't execution loop (global or per action)
Stop: stop all the actions)"
    ));
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("End type")));
    firstRowLayoutTmp->addWidget(endTypeCombo_pri);

    for (const QString& endTypeStr_ite_con : metaEndExecutionCycleData_c::strToEndExecutionTypeMap_sta_con.keys())
    {
        endTypeCombo_pri->insertItem(endTypeCombo_pri->count(), appConfig_ptr_ext->translate_f(endTypeStr_ite_con), endTypeStr_ite_con);
    }

    killAfterStopTimeoutCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Kill after stop timeout"));
    killAfterStopTimeoutCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Timeout is set in the execution options"));
    firstRowLayoutTmp->addWidget(killAfterStopTimeoutCheckbox_pri);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);

    loadActionSpecificData_f();
}

