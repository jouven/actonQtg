#include "actionFinishedWidgets.hpp"

#include "actonQtso/checks/actionFinished.hpp"

#include "appConfig.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QJsonObject>
//#include <QSplitter>

void actionFinishedWidgets_c::parentClosing_f()
{

}

void actionFinishedWidgets_c::save_f()
{
    saveCheckDataJSON_f();
    Q_EMIT JSONSaved_signal();
}

void actionFinishedWidgets_c::loadCheckSpecificData_f()
{
    if (not checkData_ptr_pri->checkDataJSON_f().isEmpty())
    {
        actionFinishedCheck_c actionFinishedTmp;
        actionFinishedTmp.read_f(checkData_ptr_pri->checkDataJSON_f());

        actionStringIdCombo_pri->setCurrentText(actionFinishedTmp.actionStringId_f());
        failCheckOnNotSuccessfulCheckbox_pri->setChecked(actionFinishedTmp.failCheckOnNotSuccessfulActionFinish_f());
    }
}

void actionFinishedWidgets_c::saveCheckDataJSON_f() const
{
    QString actionStringIdTmp(actionStringIdCombo_pri->currentText());

    actionFinishedCheck_c actionFinishedCheckTmp(actionStringIdTmp, failCheckOnNotSuccessfulCheckbox_pri->isChecked());

    QJsonObject saveValuesJSONObjectTmp;
    actionFinishedCheckTmp.write_f(saveValuesJSONObjectTmp);
    checkData_ptr_pri->setCheckDataJSON_f(saveValuesJSONObjectTmp);
}

actionFinishedWidgets_c::actionFinishedWidgets_c(
        checkData_c* const checkData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        , QObject *parent)
    : QObject(parent)
    , checkData_ptr_pri(checkData_ptr_par)
{
    this->setObjectName("actionFinishedWidgets_");

    //combo to select an actionStringId to check if it has finished
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    actionStringIdCombo_pri = new QComboBox();
    actionStringIdCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    actionStringIdCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    actionStringIdCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Action String Id")));
    firstRowLayoutTmp->addWidget(actionStringIdCombo_pri);

    for (const QString& actionStringId_ite_con : actonDataHub_ptr_ext->actionStringIdList_f())
    {
        if (actionStringId_ite_con not_eq checkData_ptr_pri->parentAction_f()->stringId_f())
        {
            actionStringIdCombo_pri->insertItem(actionStringIdCombo_pri->count(), actionStringId_ite_con);
        }
    }

    //editable false still allows to autocomplete, at least it searches the first letter on input, needs more testing
    actionStringIdCombo_pri->setEditable(false);

    //create parents checkbox
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    failCheckOnNotSuccessfulCheckbox_pri = new QCheckBox("Fail this check on no action success");
    failCheckOnNotSuccessfulCheckbox_pri->setToolTip("Unchecked: no matter how the action ends, error or success, the checks return true. Checked: the check will only return true if the action ended in success");

    secondRowLayoutTmp->addWidget(failCheckOnNotSuccessfulCheckbox_pri);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);

    loadCheckSpecificData_f();
}
