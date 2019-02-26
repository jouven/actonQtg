#include "actionWindow.hpp"

#include "appConfig.hpp"
#include "actionWidgets/runProcessWidgets.hpp"
#include "actionWidgets/createDirectoryWidgets.hpp"
#include "actionWidgets/copyFileWidgets.hpp"

#include "actionChecksWindow.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actionMappings/actionStrMapping.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

void actionWindow_c::closeEvent(QCloseEvent* event)
{
    Q_EMIT closeWindow_signal();
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
        actonDataHub_ptr_ext->updateStringIdDependencies_f(actionData_ptr_pri->stringId_f(), oldStringId_pri);
    }
    askUpdateStringIdDepdenciesWindow_pri->deleteLater();
    askUpdateStringIdDepdenciesWindow_pri = nullptr;
}

void actionWindow_c::openAskUpdateStringIdDepdenciesWindow_f()
{
    askUpdateStringIdDepdenciesWindow_pri = new QMessageBox(this);
    askUpdateStringIdDepdenciesWindow_pri->setText(R"(The updated stringId is used in other actions-checks, update them?)");
    askUpdateStringIdDepdenciesWindow_pri->setTextFormat(Qt::RichText);
    askUpdateStringIdDepdenciesWindow_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    askUpdateStringIdDepdenciesWindow_pri->setDefaultButton(QMessageBox::Yes);
    askUpdateStringIdDepdenciesWindow_pri->setWindowModality(Qt::WindowModal);

    QObject::connect(askUpdateStringIdDepdenciesWindow_pri, &QMessageBox::finished, this, &actionWindow_c::askUpdateStringIdDepdenciesWindowFinished_f);

    askUpdateStringIdDepdenciesWindow_pri->show();
}

//T*& because there is an assign operation, by value would make it useless
template <typename T>
void actionWindow_c::createActionTypeWidgets_f(T*& actionTypeClassPtr_par)
{
    actionTypeClassPtr_par = new T(actionData_ptr_pri, variableLayout_pri, this);

    //make the saving of the window position/size, and its items, generic
    connect(this, &actionWindow_c::closeWindow_signal, actionTypeClassPtr_par, &T::parentClosing_f);
    //save the unique fields of the action type to the JSON object (which is a reference)
    connect(this, &actionWindow_c::saveJSON_signal, actionTypeClassPtr_par, &T::save_f);
    //once the above is done, save *here*
    connect(actionTypeClassPtr_par, &T::JSONSaved_signal, this, &actionWindow_c::save_f);
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
    auto minHeightTmp(actionStringIdPTE_pri->fontMetrics().lineSpacing() + 14);

    //statusBarLabel_pri = new QLabel;

    //combo actions + checkbox halt on fail
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //enabled
    enabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Enabled"));
    enabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Disabled actions will not be executed"));
    enabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    enabledCheckbox_pri->setChecked(true);
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

    QPushButton* manageChecksTmp = new QPushButton("&Manage Checks");
    manageChecksTmp->setToolTip("Manage action checks, checks are \"actions\"-requisites that must be fullfilled for the action to run");
    connect(manageChecksTmp, &QPushButton::clicked, this, &actionWindow_c::manageChecksButtonClicked_f);

    firstRowLayoutTmp->addWidget(manageChecksTmp);

    //enabled checks
    checksEnabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Checks enabled"));
    checksEnabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Disables this actions checks, disabled checks won't execute"));
    checksEnabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    checksEnabledCheckbox_pri->setChecked(true);
    firstRowLayoutTmp->addWidget(checksEnabledCheckbox_pri);

    //action string id
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("String Id")));

    actionStringIdPTE_pri->setMinimumHeight(minHeightTmp);
    actionStringIdPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    secondRowLayoutTmp->addWidget(actionStringIdPTE_pri);

    //QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

//    //enabled
//    enabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Enabled"));
//    enabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Disabled actions will not be executed"));
//    enabledCheckbox_pri->setMinimumHeight(minHeightTmp);
//    thirdRowLayoutTmp->addWidget(enabledCheckbox_pri);

//    //enabled checks
//    checksEnabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Checks enabled"));
//    checksEnabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Disables this actions checks, disabled checks won't execute"));
//    checksEnabledCheckbox_pri->setMinimumHeight(minHeightTmp);
//    thirdRowLayoutTmp->addWidget(checksEnabledCheckbox_pri);

    //description
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Description")));
    descriptionPTE_pri = new QPlainTextEdit(this);
    descriptionPTE_pri->setMinimumHeight(minHeightTmp);
    descriptionPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    thirdRowLayoutTmp->addWidget(descriptionPTE_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("&Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);

//    QPushButton* tipsButtonTmp = new QPushButton("Tips");
//    lastRowLayoutTmp->addWidget(tipsButtonTmp);

    QWidget* row2Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);
    //QWidget* row4Tmp = new QWidget;

    row2Tmp->setLayout(secondRowLayoutTmp);
    row3Tmp->setLayout(thirdRowLayoutTmp);
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
    mainLayout_pri->addWidget(mainSplitter_pri);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    //mainLayout_pri->addLayout(variableLayout_pri);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    //mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Add/update action"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    connect(saveButtonTmp, &QPushButton::clicked, this, &actionWindow_c::saveButtonClicked_f);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &actionWindow_c::cancelButtonClicked_f);
    connect(this, &actionWindow_c::askUpdateStringIdDepdencies_signal, this, &actionWindow_c::openAskUpdateStringIdDepdenciesWindow_f);
    //connect(tipsButtonTmp, &QPushButton::clicked, this, &actionWindow_c::tipsButtonClicked_f);

    int loadedActionTypeIndexTmp(0);
    int_fast32_t actionIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(row_pri_con));
    if (actionIdTmp > 0)
    {
        isNew_pri = false;
        actionData_ptr_pri = actonDataHub_ptr_ext->actionData_ptr_f(actionIdTmp);

        QString actionTypeStrTmp(actionTypeToStrUMap_ext_con.at(actionData_ptr_pri->type_f()));
        loadedActionTypeIndexTmp = actionTypeCombo_pri->findData(actionTypeStrTmp.toLower());
        actionTypeCombo_pri->setCurrentIndex(loadedActionTypeIndexTmp);
        //actionTypeCombo_pri->setEditable(false);

        actionStringIdPTE_pri->setPlainText(actionData_ptr_pri->stringId_f());
        descriptionPTE_pri->setPlainText(actionData_ptr_pri->description_f());

        enabledCheckbox_pri->setChecked(actionData_ptr_pri->enabled_f());
        checksEnabledCheckbox_pri->setChecked(actionData_ptr_pri->checksEnabled_f());

        //haltOnFail_pri->setChecked(actionData_ptr_pri->haltOnFail_f());
    }
    actionComboChanged_f(loadedActionTypeIndexTmp);

    connect(actionTypeCombo_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &actionWindow_c::actionComboChanged_f);
}

void actionWindow_c::createWidgetsPerAction_f(
        const int index_par_con
)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "createWidgetsPerAction_f index_par_con " << index_par_con << endl;
#endif
    //switch?
    while (true)
    {
        QString actionTypeStrTmp(actionTypeCombo_pri->itemData(index_par_con).toString());
#ifdef DEBUGJOUVEN
        //qDebug() << "createWidgetsPerAction_f actionTypeStrTmp " << actionTypeStrTmp << endl;
#endif
        if (actionTypeStrTmp == actionTypeToStrUMap_ext_con.at(actionType_ec::runProcess).toLower())
        {
            createActionTypeWidgets_f(runProcessWidgets_pri);
            break;
        }
        if (actionTypeStrTmp == actionTypeToStrUMap_ext_con.at(actionType_ec::createDirectory).toLower())
        {
            createActionTypeWidgets_f(createDirectoryWidgets_pri);
            break;
        }
        if (actionTypeStrTmp == actionTypeToStrUMap_ext_con.at(actionType_ec::copyFile).toLower())
        {
            createActionTypeWidgets_f(copyFileWidgets_pri);
            break;
        }
        break;
    }
}

void actionWindow_c::removeWidgetClassPerAction_f(
        const int index_par_con
)
{
    //switch?
    while (true)
    {
        QString actionTypeStrTmp(actionTypeCombo_pri->itemData(index_par_con).toString());
        if (actionTypeStrTmp == actionTypeToStrUMap_ext_con.at(actionType_ec::runProcess).toLower())
        {
            runProcessWidgets_pri->deleteLater();
            runProcessWidgets_pri = nullptr;
            break;
        }
        if (actionTypeStrTmp == actionTypeToStrUMap_ext_con.at(actionType_ec::createDirectory).toLower())
        {
            createDirectoryWidgets_pri->deleteLater();
            createDirectoryWidgets_pri = nullptr;
            break;
        }
        if (actionTypeStrTmp == actionTypeToStrUMap_ext_con.at(actionType_ec::copyFile).toLower())
        {
            copyFileWidgets_pri->deleteLater();
            copyFileWidgets_pri = nullptr;
            break;
        }
        break;
    }
}

void actionWindow_c::actionComboChanged_f(int index_par)
{
    if (lastIndex_pri not_eq -1)
    {
        removeWidgetClassPerAction_f(lastIndex_pri);
    }

    clearLayoutItems_f(variableLayout_pri);
    createWidgetsPerAction_f(index_par);
    lastIndex_pri = index_par;
}

void actionWindow_c::saveButtonClicked_f()
{
    //first the json part is saved then from there it signals (back) to save the action
    Q_EMIT saveJSON_signal();
}

void actionWindow_c::cancelButtonClicked_f()
{
    close();
}

void actionWindow_c::manageChecksButtonClicked_f()
{
#ifdef DEBUGJOUVEN
    //qDebug() << "actionWindow_c::manageChecksButtonClicked_f() actionData_ptr_pri->actionDataExecutionResult_ptr_f() " << actionData_ptr_pri->actionDataExecutionResult_ptr_f() << endl;
    //qDebug() << "actionWindow_c::manageChecksButtonClicked_f() actionData_ptr_pri->actionDataExecutionResult_ptr_f() == nullptr " << (actionData_ptr_pri->actionDataExecutionResult_ptr_f() == nullptr) << endl;
#endif
    actionChecksWindow_c* actionChecksWindowTmp(new actionChecksWindow_c(actionData_ptr_pri->checkDataHub_ptr_f(), this));
    actionChecksWindowTmp->setWindowFlag(Qt::Window, true);
    actionChecksWindowTmp->setWindowModality(Qt::WindowModal);
    actionChecksWindowTmp->show();
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

void actionWindow_c::save_f()
{
    while (true)
    {
        actionType_ec actionTypeTmp(strToActionTypeMap_ext_con.value(actionTypeCombo_pri->currentData().toString()));
        QString actionStringIdTmp(actionStringIdPTE_pri->toPlainText());
        QString descriptionTmp(descriptionPTE_pri->toPlainText());
        if (descriptionTmp.isEmpty())
        {
            errorQMessageBox_f("Empty description", "Error", this);
            break;
            //FUTURE if description is empty, create a descrition Gen for each action, i.e. for process "run " + processPath + argument (limit the length of this)
        }
        if (actionStringIdTmp.isEmpty())
        {
            errorQMessageBox_f("Empty action string Id", "Error", this);
            break;
        }
        bool askUpdateStringIdDepdencies(false);
        if (actionData_ptr_pri->stringId_f() not_eq actionStringIdTmp and actonDataHub_ptr_ext->hasStringIdAnyDependency_f(actionStringIdTmp))
        {
            askUpdateStringIdDepdencies = true;
            oldStringId_pri = actionData_ptr_pri->stringId_f();
        }

        actionData_ptr_pri->setType_f(actionTypeTmp);
        actionData_ptr_pri->setStringId_f(actionStringIdTmp);
        actionData_ptr_pri->setDescription_f(descriptionTmp);
        actionData_ptr_pri->setEnabled_f(enabledCheckbox_pri->isChecked());
        actionData_ptr_pri->setChecksEnabled_f(checksEnabledCheckbox_pri->isChecked());
        //it's set in the each action type widget
        //actionData_ptr_pri->setActionDataJSON_f(childSaveResult_par_con);
        if (isNew_pri)
        {
            actonDataHub_ptr_ext->insertActionData_f(*actionData_ptr_pri, row_pri_con);
        }

        Q_EMIT updateRow_Signal(row_pri_con);
        if (askUpdateStringIdDepdencies)
        {
            Q_EMIT askUpdateStringIdDepdencies_signal();
        }
        else
        {
            close();
        }
        break;
    }
}
