#include "actionWindow.hpp"

#include "appConfig.hpp"
#include "actionWidgets/runProcessWidgets.hpp"
#include "actionWidgets/createDirectoryWidgets.hpp"
#include "actionWidgets/copyFileWidgets.hpp"
#include "actionWidgets/deleteFileDirWidgets.hpp"
#include "actionWidgets/metaEndExecutionCycleWidgets.hpp"
#include "actionWidgets/folderChangeReactionWidgets.hpp"
#include "actionChecksWindow.hpp"
#include "commonWidgets.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actionMappings/actionStrMapping.hpp"
#include "actonQtso/actions/metaEndExecutionCycle.hpp"
#include "actonDataHubGlobal.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

#include <unordered_map>
#include <functional>

void actionWindow_c::closeEvent(QCloseEvent* event)
{
    baseClassActionTypeWidgets_pri->parentClosing_f();
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

void actionWindow_c::clearLayoutItems_f(QLayout* layout_par)
{
    QLayoutItem* child;
    while (layout_par->count() > 0)
    {
        while (true)
        {
            child = layout_par->takeAt(0);
            layout_par->removeItem(child);
            if (child->layout() not_eq 0)
            {
                clearLayoutItems_f(child->layout());
                child->layout()->deleteLater();
                break;
            }
            if (child->widget() not_eq 0)
            {
                child->widget()->deleteLater();
                break;
            }
            if (child->spacerItem() not_eq 0)
            {
                //has no deleteLater
                child->~QLayoutItem();
                break;
            }
            break;
        }
    }
}

void actionWindow_c::askUpdateStringIdDepdenciesWindowFinished_f(const int result_par)
{
    if (result_par == QMessageBox::Yes)
    {
        actonDataHub_ptr_ext->updateStringIdDependencies_f(action_ptr_pri->stringId_f(), oldStringId_pri);
    }
    askUpdateStringIdDepdenciesWindow_pri->deleteLater();
    askUpdateStringIdDepdenciesWindow_pri = nullptr;
}

void actionWindow_c::openAskUpdateStringIdDepdenciesWindow_f()
{
    askUpdateStringIdDepdenciesWindow_pri = new QMessageBox(this);
    askUpdateStringIdDepdenciesWindow_pri->setText(appConfig_ptr_ext->translate_f(R"(The updated stringId is used in other actions-checks, update them?)"));
    askUpdateStringIdDepdenciesWindow_pri->setTextFormat(Qt::RichText);
    askUpdateStringIdDepdenciesWindow_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    askUpdateStringIdDepdenciesWindow_pri->setDefaultButton(QMessageBox::Yes);
    askUpdateStringIdDepdenciesWindow_pri->setWindowModality(Qt::WindowModal);

    QObject::connect(askUpdateStringIdDepdenciesWindow_pri, &QMessageBox::finished, this, &actionWindow_c::askUpdateStringIdDepdenciesWindowFinished_f);

    askUpdateStringIdDepdenciesWindow_pri->show();
}

void actionWindow_c::loadActionData_f()
{
    action_c* valuesToLoadTmp(action_ptr_pri);
    if (valuesToLoadTmp == nullptr)
    {
        //create an action obj to get the default values
        valuesToLoadTmp = new metaEndExecutionCycleAction_c();
        valuesToLoadTmp->deleteLater();
    }
    else
    {
        actionIdLineEdit_pri->setText(QString::number(valuesToLoadTmp->id_f()));
    }
    actionIdLineEdit_pri->setEnabled(false);
    int loadedActionTypeIndexTmp(actionTypeCombo_pri->findData(valuesToLoadTmp->typeStr_f().toLower()));
    actionTypeCombo_pri->setCurrentIndex(loadedActionTypeIndexTmp);
    //action type combo only enabled for new actions
    if (action_ptr_pri not_eq nullptr)
    {
        actionTypeCombo_pri->setEnabled(false);
    }

    actionStringIdPTE_pri->setPlainText(valuesToLoadTmp->stringId_f());
    descriptionPTE_pri->setPlainText(valuesToLoadTmp->description_f());

    enabledCheckbox_pri->setChecked(valuesToLoadTmp->enabled_f());
    checksEnabledCheckbox_pri->setChecked(valuesToLoadTmp->checksEnabled_f());
    checksResultLogicAndCheckbox_pri->setChecked(valuesToLoadTmp->checkResultLogicAnd_f());
    stopAllExecutionOnErrorCheckbox_pri->setChecked(valuesToLoadTmp->stopAllExecutionOnError_f());
    repeatExecutionCheckbox_pri->setChecked(valuesToLoadTmp->repeatExecution_f());

    //haltOnFail_pri->setChecked(actionData_ptr_pri->haltOnFail_f());

    actionTypeComboChanged_f(loadedActionTypeIndexTmp);
}

actionData_c actionWindow_c::fieldsToActionDataObject_f() const
{
    QString actionStringIdTmp(actionStringIdPTE_pri->toPlainText());
    QString descriptionTmp(descriptionPTE_pri->toPlainText());

    return actionData_c(
                    actionStringIdTmp
                    , descriptionTmp
                    , checksResultLogicAndCheckbox_pri->isChecked()
                    , enabledCheckbox_pri->isChecked()
                    , checksEnabledCheckbox_pri->isChecked()
                    , stopAllExecutionOnErrorCheckbox_pri->isChecked()
                    , repeatExecutionCheckbox_pri->isChecked()
    );
}

actionWindow_c::actionWindow_c(
        const int row_par_con
        , QWidget *parent_par)
    : QWidget(parent_par)
    , row_pri_con(row_par_con)
{
    this->setObjectName("actionWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    actionStringIdPTE_pri = new QPlainTextEdit(this);
    auto minHeightTmp(actionStringIdPTE_pri->fontMetrics().lineSpacing() + 12);

    //statusBarLabel_pri = new QLabel;

    //actionId + combo actions + checkbox halt on fail
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    actionIdLineEdit_pri = new QLineEdit;
    actionIdLineEdit_pri->setToolTip(
                appConfig_ptr_ext->translate_f(
                    "ActionId identifies uniquely this action during actonQtg execution, an actionId for the same action might change between actonQtg executions"
                    " because actionId/s are not saved (might change in the future?) when saving actions into a file."
                    "<br>The way the ids are assigned is tied to the order the actions are loaded/created since actonQtg started executing"
                    )
                );
    actionIdLineEdit_pri->setMaximumWidth(50);
    firstRowLayoutTmp->addWidget(new QLabel("ActionId"));
    firstRowLayoutTmp->addWidget(actionIdLineEdit_pri);

    //enabled
    enabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Enabled"));
    enabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("A disabled action will not be executed"));
    //enabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    //enabledCheckbox_pri->setChecked(true);
    firstRowLayoutTmp->addWidget(enabledCheckbox_pri);

    actionTypeCombo_pri = new QComboBox();
    actionTypeCombo_pri->setEditable(true);
    actionTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    actionTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    actionTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Type")));
    firstRowLayoutTmp->addWidget(actionTypeCombo_pri);

    for (const QString& actionStr_ite_con : strToActionTypeMap_ext_con.keys())
    {
        actionTypeCombo_pri->insertItem(actionTypeCombo_pri->count(), appConfig_ptr_ext->translate_f(actionStr_ite_con), actionStr_ite_con);
    }

    //haltOnFail_pri = new QCheckBox("Halt on fail");
    //haltOnFail_pri->setToolTip("Stop running, don't run the following actions, on fail");

    //firstRowLayoutTmp->addWidget(haltOnFail_pri);

    manageChecks_pri = new QPushButton(appConfig_ptr_ext->translate_f("&Manage Checks"));
    manageChecks_pri->setToolTip(appConfig_ptr_ext->translate_f(
R"(Manage action checks, checks are action "requisites" that must be fullfilled for the action to execute.
Action must be saved/exist to enable this button)"
));
    connect(manageChecks_pri, &QPushButton::clicked, this, &actionWindow_c::manageChecksButtonClicked_f);

    firstRowLayoutTmp->addWidget(manageChecks_pri);

    //enabled checks
    checksEnabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Checks enabled"));
    checksEnabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Unchecked: disables this action's check/s i.e., checks are ignored for this action"));
    //checksEnabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    //checksEnabledCheckbox_pri->setChecked(true);
    firstRowLayoutTmp->addWidget(checksEnabledCheckbox_pri);

    //action checks, it this becomes to big make a button+window and put it inside
    QHBoxLayout* secondRowLayoutTmp(new QHBoxLayout);

    //check result logic
    checksResultLogicAndCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Check result logic AND"));
    checksResultLogicAndCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f(R"(Checks result logic:
AND-checked: all checks must success to execute the action
OR-unchecked: the first check to success is enough execute the action)"));
    //checksResultLogicAndCheckbox_pri->setMinimumHeight(minHeightTmp);
    //checksResultLogicAndCheckbox_pri->setChecked(true);
    secondRowLayoutTmp->addWidget(checksResultLogicAndCheckbox_pri);

    //stop all execution if this action has an error
    stopAllExecutionOnErrorCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Stop all execution on error"));
    stopAllExecutionOnErrorCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f(R"(Checked: if THIS action errors stop all execution
Unchecked: continue execution even if THIS action errors)"));
    //stopAllExecutionOnErrorCheckbox_pri->setMinimumHeight(minHeightTmp);
    //stopAllExecutionOnErrorCheckbox_pri->setChecked(true);
    secondRowLayoutTmp->addWidget(stopAllExecutionOnErrorCheckbox_pri);

    //repeat execution of this action
    repeatExecutionCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Repeat execution"));
    repeatExecutionCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f(R"(Checked: keeps executing this action after it finishes.
Overall execution can only be "ended" by manually stopping execution, an action with "Stop all execution on error" or a metaEndExecutionCycle action
Unchecked: only executes once)"));
    //repeatExecutionCheckbox_pri->setMinimumHeight(minHeightTmp);
    //repeatExecutionCheckbox_pri->setChecked(true);
    secondRowLayoutTmp->addWidget(repeatExecutionCheckbox_pri);

    //action string id
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("String Id")));

    actionStringIdPTE_pri->setMinimumHeight(minHeightTmp);
    actionStringIdPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    thirdRowLayoutTmp->addWidget(actionStringIdPTE_pri);

    //description
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    fourthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Description")));
    descriptionPTE_pri = new QPlainTextEdit(this);
    descriptionPTE_pri->setMinimumHeight(minHeightTmp);
    descriptionPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    fourthRowLayoutTmp->addWidget(descriptionPTE_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Save"));
    lastRowLayoutTmp->addWidget(saveButtonTmp);

    QPushButton* closeButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Close"));
    lastRowLayoutTmp->addWidget(closeButtonTmp);

//    QPushButton* tipsButtonTmp = new QPushButton("Tips");
//    lastRowLayoutTmp->addWidget(tipsButtonTmp);

    QWidget* row2Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);
    //QWidget* row4Tmp = new QWidget;

    row2Tmp->setLayout(thirdRowLayoutTmp);
    row3Tmp->setLayout(fourthRowLayoutTmp);
    variableLayout_pri = new QVBoxLayout;
    //variableLayout_pri->setSpacing(0);
    row4Tmp->setLayout(variableLayout_pri);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter_");

    mainSplitter_pri->addWidget(row2Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->addWidget(row4Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addWidget(mainSplitter_pri);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    //mainLayout_pri->addLayout(variableLayout_pri);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    //mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayout_pri);


    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    connect(saveButtonTmp, &QPushButton::clicked, this, &actionWindow_c::saveButtonClicked_f);
    connect(closeButtonTmp, &QPushButton::clicked, this, &actionWindow_c::closeButtonClicked_f);
    //connect(this, &actionWindow_c::askUpdateStringIdDepdencies_signal, this, &actionWindow_c::openAskUpdateStringIdDepdenciesWindow_f);
    //connect(tipsButtonTmp, &QPushButton::clicked, this, &actionWindow_c::tipsButtonClicked_f);

    int_fast32_t actionIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(row_pri_con));
    if (actionIdTmp > 0)
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Update Action"));
        isNew_pri = false;
        action_ptr_pri = actonDataHub_ptr_ext->action_ptr_f(actionIdTmp);
    }
    else
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Add Action"));
        //no checks until the object exists
        manageChecks_pri->setEnabled(false);
    }

    loadActionData_f();

    connect(actionTypeCombo_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &actionWindow_c::actionTypeComboChanged_f);
}



void actionWindow_c::createWidgetsPerAction_f(
        const int index_par_con
)
{
    QString actionTypeStrTmp(actionTypeCombo_pri->itemData(index_par_con).toString());
#ifdef DEBUGJOUVEN
    //qDebug() << "createWidgetsPerAction_f actionTypeStrTmp " << actionTypeStrTmp << endl;
#endif
    const std::unordered_map<actionType_ec, std::function<baseClassActionTypeWidgets_c*()>> actionTypeToActionCreationFunctionMap_con(
    {
       {	actionType_ec::runProcess, [this]() -> baseClassActionTypeWidgets_c* { return new runProcessWidgets_c(action_ptr_pri, variableLayout_pri);}}
       , {	actionType_ec::createDirectory, [this]() -> baseClassActionTypeWidgets_c* { return new createDirectoryWidgets_c(action_ptr_pri, variableLayout_pri);}}
       , {	actionType_ec::copyFile, [this]() -> baseClassActionTypeWidgets_c* { return new copyFileWidgets_c(action_ptr_pri, variableLayout_pri);}}
       , {	actionType_ec::deleteFileDir, [this]() -> baseClassActionTypeWidgets_c* { return new deleteFileDirWidgets_c(action_ptr_pri, variableLayout_pri);}}
       , {	actionType_ec::metaEndExecutionCycle, [this]() -> baseClassActionTypeWidgets_c* { return new metaEndExecutionCycleWidgets_c(action_ptr_pri, variableLayout_pri);}}
       , {	actionType_ec::folderChangeReaction, [this]() -> baseClassActionTypeWidgets_c* { return new folderChangeReactionWidgets_c(action_ptr_pri, variableLayout_pri);}}
    });

    auto findResultTmp(actionTypeToActionCreationFunctionMap_con.find(strToActionTypeMap_ext_con.value(actionTypeStrTmp)));
    if (findResultTmp not_eq actionTypeToActionCreationFunctionMap_con.cend())
    {
        baseClassActionTypeWidgets_pri = findResultTmp->second();
    }
    else
    {
        text_c errorTextTmp("Action type {0} has no editor widgets", actionTypeStrTmp);
        errorQMessageBox_f(appConfig_ptr_ext->translateAndReplace_f(errorTextTmp));
    }
}

void actionWindow_c::actionTypeComboChanged_f(int index_par)
{
    if (lastIndex_pri not_eq -1)
    {
        baseClassActionTypeWidgets_pri->deleteLater();
        baseClassActionTypeWidgets_pri = nullptr;
    }

    clearLayoutItems_f(variableLayout_pri);
    createWidgetsPerAction_f(index_par);
    lastIndex_pri = index_par;
}

void actionWindow_c::saveButtonClicked_f()
{
    save_f();
}

void actionWindow_c::closeButtonClicked_f()
{
    close();
}

void actionWindow_c::manageChecksButtonClicked_f()
{
#ifdef DEBUGJOUVEN
    //qDebug() << "actionWindow_c::manageChecksButtonClicked_f() actionData_ptr_pri->actionDataExecutionResult_ptr_f() " << actionData_ptr_pri->actionDataExecutionResult_ptr_f() << endl;
    //qDebug() << "actionWindow_c::manageChecksButtonClicked_f() actionData_ptr_pri->actionDataExecutionResult_ptr_f() == nullptr " << (actionData_ptr_pri->actionDataExecutionResult_ptr_f() == nullptr) << endl;
#endif
    actionChecksWindow_c* actionChecksWindowTmp(new actionChecksWindow_c(action_ptr_pri->checkDataHub_ptr_f(), this));
    actionChecksWindowTmp->setWindowFlag(Qt::Window, true);
    actionChecksWindowTmp->setWindowModality(Qt::WindowModal);
    actionChecksWindowTmp->show_f();
}

//no need for now
//void actionWindow_c::tipsButtonClicked_f()
//{
//    plainQMessageBox_f
//    (
//                appConfig_ptr_ext->translate_f(
//                    "<p>1 If a previous action is edited and the type is changed the old values last until the action is saved again, it is always possible to switch back to the original type (no need to cancel and edit again)</p>"
//                    "<p>2 \"Any finish\", checked, means that the action has finished running, successfully or not</p>"
//                    )
//            , "Tips"
//            , this
//    );
//}

bool actionWindow_c::save_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    actionData_c objTmp(fieldsToActionDataObject_f());
    if (objTmp.isFieldsActionDataValid_f(std::addressof(errorsTmp)))
    {
        bool askUpdateStringIdDepdencies(false);
        bool actionTypeSaveResultTmp(false);
        if (isNew_pri)
        {
            actionTypeSaveResultTmp = baseClassActionTypeWidgets_pri->saveNew_f(objTmp, actonDataHub_ptr_ext);
            if (actionTypeSaveResultTmp)
            {
                actonDataHub_ptr_ext->insertActionData_f(action_ptr_pri, row_pri_con, std::addressof(errorsTmp));
                //load the id field
                actionIdLineEdit_pri->setText(QString::number(action_ptr_pri->id_f()));
                //enable the checks button after saving the new action
                manageChecks_pri->setEnabled(true);
                //change window title
                setWindowTitle(appConfig_ptr_ext->translate_f("Update Action"));
                isNew_pri = false;
            }
        }
        else
        {
            actionTypeSaveResultTmp = baseClassActionTypeWidgets_pri->saveUpdate_f();
            if (actionTypeSaveResultTmp)
            {
                oldStringId_pri = action_ptr_pri->stringId_f();
                action_ptr_pri->updateActionData_f(objTmp, false, std::addressof(errorsTmp));
                if (oldStringId_pri not_eq action_ptr_pri->stringId_f()
                    and (actonDataHub_ptr_ext->stringTriggerDependencyCount_f(oldStringId_pri) > 0))
                {
                    askUpdateStringIdDepdencies = true;
                }
            }
        }

        if (actionTypeSaveResultTmp)
        {
            Q_EMIT updateRow_Signal(row_pri_con);
            informationQMessageBox_f(appConfig_ptr_ext->translate_f("Action data saved"), appConfig_ptr_ext->translate_f("Information"));
            if (askUpdateStringIdDepdencies)
            {
                openAskUpdateStringIdDepdenciesWindow_f();
                //Q_EMIT askUpdateStringIdDepdencies_signal();
            }
            else
            {
                //saving shouldn't imply closing, since checks might be edited after
                //close();
            }
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}
