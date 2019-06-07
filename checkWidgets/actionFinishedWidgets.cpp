#include "actionFinishedWidgets.hpp"

#include "../appConfig.hpp"

#include "actonQtso/checks/actionFinished.hpp"
#include "actonQtso/checkData.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include "essentialQtgso/messageBox.hpp"

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
    if (actionFinishedCheck_ptr_pri not_eq nullptr)
    {
        actionStringIdCombo_pri->setCurrentText(actionFinishedCheck_ptr_pri->actionStringId_f());
        actionStringIdCombo_pri->setEnabled(false);
        failCheckOnNotSuccessfulCheckbox_pri->setChecked(actionFinishedCheck_ptr_pri->failCheckOnNotSuccessfulActionFinish_f());

        for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& ite_con : actionFinishedCheck_ptr_pri->actionExecutionResultFieldToStringTrigger_f())
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
}

bool actionFinishedWidgets_c::isFieldsDataValid_f() const
{
    bool validTmp(false);
    while (true)
    {
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
                if (actonDataHub_ptr_ext->hasStringTriggerAnyDependency_f(stringTriggerTmp, actionFinishedCheck_ptr_pri))
                {
                    errorQMessageBox_f(
                                appConfig_ptr_ext->translate_f("String trigger already in use, in an action-check")
                                , appConfig_ptr_ext->translate_f("Error")
                                , static_cast<QWidget*>(this->parent()));
                    errorTmp = true;
                    break;
                }

                int_fast64_t existingStringParserIndexTmp(stringParserMap_ptr_ext->stringTriggerIndex_f(stringTriggerTmp));
                if (existingStringParserIndexTmp not_eq -1)
                {
                    errorQMessageBox_f(appConfig_ptr_ext->translate_f("String trigger already in use, in another string parser, index ") + QString::number(existingStringParserIndexTmp)
                                       , appConfig_ptr_ext->translate_f("Error")
                                       , static_cast<QWidget*>(this->parent()));
                    errorTmp = true;
                    break;
                }
                if (stringTriggersTmp.contains(stringTriggerTmp))
                {
                    errorQMessageBox_f(appConfig_ptr_ext->translate_f("String trigger already in use, in this parser, index ") + QString::number(rowIndex_ite)
                                       , appConfig_ptr_ext->translate_f("Error")
                                       , static_cast<QWidget*>(this->parent()));
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
    if (isFieldsDataValid_f())
    {
        MACRO_ADDACTONQTGLOG("Create actionFinishedCheck_c from fields", logItem_c::type_ec::debug);
        actionFinishedCheck_ptr_pri = new actionFinishedCheck_c(checkData_par_con, fieldsToActionFinishedDataObject_f());

        checkPtr_pro = actionFinishedCheck_ptr_pri;
        resultTmp = true;
    }
    return resultTmp;
}

bool actionFinishedWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    if (isFieldsDataValid_f())
    {
        actionFinishedCheck_ptr_pri->actionFinishedData_c::operator=(fieldsToActionFinishedDataObject_f());
        resultTmp = true;
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
                , failCheckOnNotSuccessfulCheckbox_pri->isChecked()
                , actionExecutionResultFieldToStringTriggerTmp
    );
}

actionFinishedWidgets_c::actionFinishedWidgets_c(
        check_c*& check_ptr_par
        , QVBoxLayout* const variableLayout_par
        , const action_c* const parentAction_par_con)
    : baseClassCheckWidgets_c(check_ptr_par, variableLayout_par->parentWidget())
    , actionFinishedCheck_ptr_pri(check_ptr_par == nullptr ? nullptr : static_cast<actionFinishedCheck_c*>(check_ptr_par))
{
    this->setObjectName("actionFinishedWidgets_");
#ifdef DEBUGJOUVEN
    //qDebug() << "-1 actionFinishedWidgets_c" << endl;
#endif
    //combo to select an actionStringId to check if it has finished
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    actionStringIdCombo_pri = new QComboBox();
    //this allows autocomplete
    actionStringIdCombo_pri->setEditable(true);
    actionStringIdCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    actionStringIdCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    actionStringIdCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Action String Id")));
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

    //Fail check on no action success checkbox
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    failCheckOnNotSuccessfulCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Fail this check on no action success"));
    failCheckOnNotSuccessfulCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Unchecked: no matter how the action ends, error or success, the checks return true. Checked: the check will only return true if the action ended in success"));

    secondRowLayoutTmp->addWidget(failCheckOnNotSuccessfulCheckbox_pri);

    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    QLabel* labelActionResultTypeToStringParserTableTitle(
                new QLabel(appConfig_ptr_ext->translate_f("Create string replacer parser (see tooltip)")));
    labelActionResultTypeToStringParserTableTitle->setToolTip("Create string replacer parser with the results of the action referenced by the String Id,"
                                                              " this parser is removed when the execution finishes");
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
