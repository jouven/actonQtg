#include "actionFinishedWidgets.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/checks/actionFinished.hpp"
#include "actonQtso/checkData.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"
#include "essentialQtso/macros.hpp"


#include <QtWidgets>
//#include <QSplitter>

void actionFinishedWidgets_c::derivedParentClosing_f()
{
    //appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionResultTypeToStringParserTable_pri->objectName(), actionResultTypeToStringParserTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionResultTypeToStringParserTable_pri->objectName() + actionResultTypeToStringParserTable_pri->horizontalHeader()->objectName(), actionResultTypeToStringParserTable_pri->horizontalHeader()->saveState());
}

void actionFinishedWidgets_c::loadCheckSpecificData_f()
{
    actionFinishedData_c valuesToLoadTmp(actionFinishedCheck_ptr_pri not_eq nullptr ? *actionFinishedCheck_ptr_pri : actionFinishedData_c());

    actionStringIdCombo_pri->setCurrentText(valuesToLoadTmp.actionStringId_f());
    //actionStringIdCombo_pri->setEnabled(false);

    successOnActionSuccessCheckBox_pri->setChecked(valuesToLoadTmp.successOnActionSuccess_f());
    finishedCountLineEdit_pri->setText(QString::number(valuesToLoadTmp.finishedCount_f()));

    for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& ite_con : valuesToLoadTmp.actionExecutionResultFieldToStringTrigger_f())
    {
        for (auto rowIndex_ite = 0, l = actionResultTypeToStringParserTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
        {
            if (actionResultTypeToStringParserTable_pri->item(rowIndex_ite, 0)->data(Qt::UserRole).toString()
                == actionFinishedCheck_c::actionExecutionResultFieldsToStrUMap_sta_con.at(ite_con.first).toLower())
            {
                actionResultTypeToStringParserTable_pri->item(rowIndex_ite, 1)->setText(ite_con.second);
                break;
            }
            else
            {
                //keep going
            }
        }
        //            //get the cell of the result-type
        //             QTableWidgetItem* cellTmp(
        //                         actionResultTypeToStringParserTable_pri->findItems(
        //                             actionFinishedCheck_c::actionExecutionResultFieldsToStrUMap_sta_con.at(ite_con.first).toLower()
        //                             , Qt::MatchFlag::MatchFixedString)
        //                         .first());

#ifdef DEBUGJOUVEN
        //qDebug() << "actionFinishedCheck_c::actionExecutionResultFieldsToStrUMap_sta_con.at(ite_con.first) " << actionFinishedCheck_c::actionExecutionResultFieldsToStrUMap_sta_con.at(ite_con.first);
        //qDebug() << "cellTmp->row() " << QString::number(cellTmp->row());
        //qDebug() << "cellTmp->text() " << cellTmp->text();
#endif
    }

}

bool actionFinishedWidgets_c::isFieldsDataValid_f(textCompilation_c* errors_par) const
{
    bool validTmp(false);
    while (true)
    {
        //finished count
        {
            bool goodNumberConversion(false);
            finishedCountLineEdit_pri->text().toLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong finished count value: {0}", finishedCountLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        bool errorTmp(false);
        QStringList stringTriggersTmp;
        for (auto rowIndex_ite = 0, l = actionResultTypeToStringParserTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
        {
            QString stringTriggerTmp(actionResultTypeToStringParserTable_pri->item(rowIndex_ite, 1)->text());
            if (stringTriggerTmp.isEmpty())
            {
                //ignore empty ones
            }
            else
            {
                if (actonDataHub_ptr_ext->stringTriggerDependencyCount_f(stringTriggerTmp, actionFinishedCheck_ptr_pri) > 0)
                {
                    APPENDTEXTPTR(errors_par, "String trigger already in use, in an action-check")
                    errorTmp = true;
                    break;
                }

                int_fast64_t existingStringParserIndexTmp(stringParserMap_ptr_ext->stringTriggerIndex_f(stringTriggerTmp));
                if (existingStringParserIndexTmp not_eq -1)
                {
                    text_c errorTextTmp("String trigger already in use, in another string parser, index: {0}", existingStringParserIndexTmp);
                    APPENDTEXTPTR(errors_par, errorTextTmp)
                    errorTmp = true;
                    break;
                }
                if (stringTriggersTmp.contains(stringTriggerTmp))
                {
                    text_c errorTextTmp("String trigger already in use, in this parser, index: {0}", rowIndex_ite);
                    APPENDTEXTPTR(errors_par, errorTextTmp)
                    errorTmp = true;
                    break;
                }
                else
                {
                    stringTriggersTmp.append(stringTriggerTmp);
                }
            }
        }
        if (errorTmp)
        {
            break;
        }

        validTmp = true;
        break;
    }
    return validTmp;
}

bool actionFinishedWidgets_c::derivedSaveNew_f(const checkData_c& checkData_par_con)
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        actionFinishedData_c objTmp(fieldsToActionFinishedDataObject_f());

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            actionFinishedCheck_ptr_pri = new actionFinishedCheck_c(checkData_par_con, objTmp);

            checkPtr_pro = actionFinishedCheck_ptr_pri;
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool actionFinishedWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        actionFinishedData_c objTmp(fieldsToActionFinishedDataObject_f());

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            actionFinishedCheck_ptr_pri->updateActionFinishedData_f(objTmp);
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

QString actionFinishedWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>action finished widget tips</p>");
}

actionFinishedData_c actionFinishedWidgets_c::fieldsToActionFinishedDataObject_f() const
{
    QString actionStringIdTmp(actionStringIdCombo_pri->currentText());

    std::unordered_map<actionFinishedCheck_c::actionExecutionResultFields_ec, QString> actionExecutionResultFieldToStringTriggerTmp;

    for (auto rowIndex_ite = 0, l = actionResultTypeToStringParserTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        if (actionResultTypeToStringParserTable_pri->item(rowIndex_ite, 1)->text().isEmpty())
        {
            //ignore empty ones
        }
        else
        {
            //result "output" (type) 0 | string trigger 1
            actionExecutionResultFieldToStringTriggerTmp.emplace(
                        //ActionExecutionResultField column
                        actionFinishedCheck_c::strToActionExecutionResultFieldsMap_sta_con.value(actionResultTypeToStringParserTable_pri->item(rowIndex_ite, 0)->data(Qt::UserRole).toString())
                        //string trigger column
                        , actionResultTypeToStringParserTable_pri->item(rowIndex_ite, 1)->text()
            );
        }
    }

    return actionFinishedData_c(
                actionStringIdTmp
                , finishedCountLineEdit_pri->text().toLongLong()
                , successOnActionSuccessCheckBox_pri->isChecked()
                , actionExecutionResultFieldToStringTriggerTmp
    );
}

actionFinishedWidgets_c::actionFinishedWidgets_c(
        check_c*& check_ptr_par
        , QVBoxLayout* const variableLayout_par
        , const action_c* const parentAction_par_con)
    : baseClassCheckTypeWidgets_c(check_ptr_par, variableLayout_par->parentWidget())
    , actionFinishedCheck_ptr_pri(check_ptr_par == nullptr ? nullptr : static_cast<actionFinishedCheck_c*>(check_ptr_par))
{
    this->setObjectName("actionFinishedWidgets_");

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

    //finished count, success on success check
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    QLabel* finishedCountLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Finished count"));
    finishedCountLabelTmp->setToolTip(appConfig_ptr_ext->translate_f(
                "<p>How many times the action must finish to trigger this check success</p>"
                "<p>Minimum 1, maximum INT64MAX</p>"
    ));
    secondRowLayoutTmp->addWidget(finishedCountLabelTmp);

    finishedCountLineEdit_pri = new QLineEdit;
    //that's the "string" length of max 64bit signed number
    ////////////////////////////////////////"9223372036854775808
    QRegExpValidator *a64bitValidatorTmp = new QRegExpValidator(QRegExp("[1-9][0-9]{18}"), this);
    finishedCountLineEdit_pri->setValidator(a64bitValidatorTmp);
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
    secondRowLayoutTmp->addWidget(finishedCountLineEdit_pri);

    successOnActionSuccessCheckBox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Success on action success"));
    successOnActionSuccessCheckBox_pri->setToolTip(
                appConfig_ptr_ext->translate_f("Controls if this checks success depends on the action succcess or otherwise ANY action finished state")
    );
    //successOnActionSuccessCheckBox_pri->setMinimumHeight(minHeightTmp);
    secondRowLayoutTmp->addWidget(successOnActionSuccessCheckBox_pri);

    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    QLabel* labelActionResultTypeToStringParserTableTitle(
                new QLabel(appConfig_ptr_ext->translate_f("Create string replacer parser (see tooltip)")));
    labelActionResultTypeToStringParserTableTitle->setToolTip(appConfig_ptr_ext->translate_f(
                "Create string replacer parser with the results of the action referenced by the String Id,"
                " this parser is removed when the execution finishes"));
    thirdRowLayoutTmp->addWidget(labelActionResultTypeToStringParserTableTitle);

    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    actionResultTypeToStringParserTable_pri = new QTableWidget(0, 2);
    actionResultTypeToStringParserTable_pri->setObjectName("QTableWidget_");
    actionResultTypeToStringParserTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    //result "output" (type) 0 | string trigger 1
    labels << appConfig_ptr_ext->translate_f("Result type") << appConfig_ptr_ext->translate_f("String trigger");
    //stringParserConfigsTable_pri->horizontalHeaderItem(0)->setToolTip(appConfig_ptr_ext->translate_f("Type"));
    actionResultTypeToStringParserTable_pri->setHorizontalHeaderLabels(labels);
    actionResultTypeToStringParserTable_pri->horizontalHeaderItem(0)->setToolTip(appConfig_ptr_ext->translate_f("Which type of the action result \"output\" will replace the string trigger"));
    actionResultTypeToStringParserTable_pri->horizontalHeaderItem(1)->setToolTip(appConfig_ptr_ext->translate_f("String trigger that activates the string replace parser"));
    actionResultTypeToStringParserTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    actionResultTypeToStringParserTable_pri->setShowGrid(true);
    //TODO on android there is no right mouse button (touch holding or doing gestures doesn't make it) by default
    //so no context menus
    //commandsTable_pri->setContextMenuPolicy(Qt::CustomContextMenu);
    actionResultTypeToStringParserTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    stringParserConfigsTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif

    fourthRowLayoutTmp->addWidget(actionResultTypeToStringParserTable_pri);

    int indexCounterTmp(0);
    for (const QString& resultTypeStr_ite_con : actionFinishedCheck_c::strToActionExecutionResultFieldsMap_sta_con.keys())
    {
        QTableWidgetItem* resultTypeCellTmp(new QTableWidgetItem);
        resultTypeCellTmp->setData(Qt::DisplayRole, appConfig_ptr_ext->translate_f(resultTypeStr_ite_con));
        resultTypeCellTmp->setData(Qt::UserRole, resultTypeStr_ite_con);
        resultTypeCellTmp->setToolTip(appConfig_ptr_ext->translate_f("tooltip_resulType_" + resultTypeStr_ite_con));
        resultTypeCellTmp->setFlags(resultTypeCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* stringTriggerCellTmp(new QTableWidgetItem);
        resultTypeCellTmp->setFlags(resultTypeCellTmp->flags() bitor Qt::ItemIsEditable);

        //result "output" (type) 0 | string trigger 1
        actionResultTypeToStringParserTable_pri->insertRow(indexCounterTmp);
        actionResultTypeToStringParserTable_pri->setItem(indexCounterTmp, 0, resultTypeCellTmp);
        actionResultTypeToStringParserTable_pri->setItem(indexCounterTmp, 1, stringTriggerCellTmp);

        indexCounterTmp = indexCounterTmp + 1;
    }

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
    variableLayout_par->addLayout(thirdRowLayoutTmp);
    variableLayout_par->addLayout(fourthRowLayoutTmp);
    //variableLayout_par->addWidget(mainSplitter_pri);

    if (appConfig_ptr_ext->configLoaded_f())
    {
         //mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
         actionResultTypeToStringParserTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionResultTypeToStringParserTable_pri->objectName()));
         actionResultTypeToStringParserTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionResultTypeToStringParserTable_pri->objectName() + actionResultTypeToStringParserTable_pri->horizontalHeader()->objectName()));
    }

    loadCheckSpecificData_f();
}
