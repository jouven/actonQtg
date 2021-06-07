#include "actionStartedExecutingWidgets.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"
#include "../actonDataHubGlobal.hpp"

#include "actonQtso/checks/actionStartedExecuting.hpp"
#include "actonQtso/checkData.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"
#include "essentialQtso/macros.hpp"


#include <QtWidgets>
//#include <QSplitter>

void actionStartedExecutingWidgets_c::derivedParentClosing_f()
{
    //appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
}

void actionStartedExecutingWidgets_c::loadCheckSpecificData_f()
{
    actionStartedExecutingData_c valuesToLoadTmp(actionStartedExecutionCheck_ptr_pri not_eq nullptr ? *actionStartedExecutionCheck_ptr_pri : actionStartedExecutingData_c());

    actionStringIdCombo_pri->setCurrentText(valuesToLoadTmp.actionStringId_f());
    //actionStringIdCombo_pri->setEnabled(false);

    waitFinishMsLineEdit_pri->setText(QString::number(valuesToLoadTmp.waitFinishMs_f()));
}

bool actionStartedExecutingWidgets_c::isFieldsDataValid_f(textCompilation_c* errors_par) const
{
    bool validTmp(false);
    while (true)
    {
        //finished count
        {
            bool goodNumberConversion(false);
            waitFinishMsLineEdit_pri->text().toLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong finished count value: {0}", waitFinishMsLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        validTmp = true;
        break;
    }
    return validTmp;
}

bool actionStartedExecutingWidgets_c::derivedSaveNew_f(const checkData_c& checkData_par_con)
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        actionStartedExecutingData_c objTmp(fieldsToActionFinishedDataObject_f());

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            actionStartedExecutionCheck_ptr_pri = new actionStartedExecutingCheck_c(checkData_par_con, objTmp);

            checkPtr_pro = actionStartedExecutionCheck_ptr_pri;
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool actionStartedExecutingWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        actionStartedExecutingData_c objTmp(fieldsToActionFinishedDataObject_f());

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            actionStartedExecutionCheck_ptr_pri->updateAcionStartedExecutingData_f(objTmp);
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

QString actionStartedExecutingWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>action finished widget tips</p>");
}

actionStartedExecutingData_c actionStartedExecutingWidgets_c::fieldsToActionFinishedDataObject_f() const
{
    QString actionStringIdTmp(actionStringIdCombo_pri->currentText());

    return actionStartedExecutingData_c(
                actionStringIdTmp
                , waitFinishMsLineEdit_pri->text().toLongLong()
    );
}

actionStartedExecutingWidgets_c::actionStartedExecutingWidgets_c(
        check_c*& check_ptr_par
        , QVBoxLayout* const variableLayout_par
        , const action_c* const parentAction_par_con)
    : baseClassCheckTypeWidgets_c(check_ptr_par, variableLayout_par->parentWidget())
    , actionStartedExecutionCheck_ptr_pri(check_ptr_par == nullptr ? nullptr : static_cast<actionStartedExecutingCheck_c*>(check_ptr_par))
{
    this->setObjectName("actionStartedExecutingWidgets_");

#ifdef DEBUGJOUVEN
    //qDebug() << "-1 actionFinishedWidgets_c" << endl;
#endif
    //combo to select an actionStringId to check if it has finished
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    actionStringIdCombo_pri = new QComboBox;
    //this allows autocomplete
    actionStringIdCombo_pri->setEditable(true);
    actionStringIdCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    actionStringIdCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    actionStringIdCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QLabel* actionStringIdLabel(new QLabel(appConfig_ptr_ext->translate_f("Action String Id")));
    //auto minHeightTmp(actionStringIdLabel->fontMetrics().lineSpacing() + 14);

    firstRowLayoutTmp->addWidget(actionStringIdLabel);
    firstRowLayoutTmp->addWidget(actionStringIdCombo_pri);
#ifdef DEBUGJOUVEN
    //qDebug() << "0 actionFinishedWidgets_c" << endl;
#endif
    for (const QString& actionStringId_ite_con : actonDataHub_ptr_ext->actionStringIdList_f())
    {
#ifdef DEBUGJOUVEN
        //qDebug() << "parentAction_par_con " << parentAction_par_con << endl;
#endif
        if (actionStringId_ite_con not_eq parentAction_par_con->stringId_f())
        {
            actionStringIdCombo_pri->insertItem(actionStringIdCombo_pri->count(), actionStringId_ite_con);
        }
    }

    //wait before finish check
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;
    //TODO review QLabels widgets tooltips, do it after I commit/push this whole batch of changes
    QLabel* waitFinishMsLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Wait before finishing"));
    QString waitFinishMsTooltipTextTmp(appConfig_ptr_ext->translate_f(
                                           "<p>Delay, in milliseconds, before finishing the check when it is detected that an action has started</p>"
                                           "<p>Minimum 0, maximum INT64MAX</p>"));
    waitFinishMsLabelTmp->setToolTip(waitFinishMsTooltipTextTmp);
    secondRowLayoutTmp->addWidget(waitFinishMsLabelTmp);

    waitFinishMsLineEdit_pri = new QLineEdit;
    //that's the "string" length of max 64bit signed number
    ////////////////////////////////////////"9223372036854775808
    QRegExpValidator *a64bitValidatorTmp = new QRegExpValidator(QRegExp("[0-9][0-9]{18}"), this);
    waitFinishMsLineEdit_pri->setValidator(a64bitValidatorTmp);
    waitFinishMsLineEdit_pri->setToolTip(waitFinishMsTooltipTextTmp);
    //20191115 validator reviews:
    //1 QIntValidator(0, INT32_MAX, this)
    //  it's 32 bit only, doesn't allow negative sign, requires an active function call in the code, allows 0 even when the minimum is 1
    //2 setInputMask("000000000000000000D")
    //  doesn't allow negative sign, doesn't check numeric range only the "text length"
    //3 QRegExpValidator(QRegExp("[0-9]{19}"), this) plus QRegExp("^-?[0-9]{19}") for negative numbers
    //  doesn't check numeric range only the "text length"

    //20191115 best option, right now, is 3

    //QValidator *validatorTmp = new QIntValidator(0, INT32_MAX, this);
    //finishedCountLineEdit_pri->setValidator(validatorTmp);
    secondRowLayoutTmp->addWidget(waitFinishMsLineEdit_pri);

//    QWidget* row2Tmp = new QWidget;
//    //row1Tmp->setContentsMargins(0,0,0,0);
//    QWidget* row3Tmp = new QWidget;
//    //row2Tmp->setContentsMargins(0,0,0,0);

//    row2Tmp->setLayout(secondRowLayoutTmp);
//    row3Tmp->setLayout(thirdRowLayoutTmp);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
//    mainSplitter_pri = new QSplitter(Qt::Vertical);
//    mainSplitter_pri->setObjectName("QSplitter_");

//    mainSplitter_pri->addWidget(row2Tmp);
//    mainSplitter_pri->addWidget(row3Tmp);
//    mainSplitter_pri->setCollapsible(0, false);
//    mainSplitter_pri->setCollapsible(1, false);
//    mainSplitter_pri->setContentsMargins(0,0,0,0);

    variableLayout_par->addLayout(firstRowLayoutTmp);
    variableLayout_par->addLayout(secondRowLayoutTmp);
    //variableLayout_par->addWidget(mainSplitter_pri);

    if (appConfig_ptr_ext->configLoaded_f())
    {
         //mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    loadCheckSpecificData_f();
}
