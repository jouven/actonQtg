#include "actionWindow.hpp"

#include "actonQtso/actionData.hpp"
#include "appConfig.hpp"
#include "actionWidgets/runProcessWidgets.hpp"
#include "actionWidgets/createDirectoryWidgets.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

void actionWindow_c::closeEvent(QCloseEvent* event)
{
    Q_EMIT closeWindow_signal();
    appConfig_f().setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_f().setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
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
            if (child->layout() != 0)
            {
                clearLayoutItems_f(child->layout());
                child->layout()->deleteLater();
                break;
            }
            if (child->widget() != 0)
            {
                child->widget()->deleteLater();
                break;
            }
            if (child->spacerItem() != 0)
            {
                //has no deleteLater
                child->~QLayoutItem();
                break;
            }
            break;
        }
    }
}

//T*& because connects on a copy pointer variable doesn't work well
//when the copy is dtored the connections go away
template <typename T>
void actionWindow_c::createActionTypeWidgets_f(T*& actionTypeClassPtr_par)
{
    actionTypeClassPtr_par = new T(actionDataId_pri_con, variableLayout_pri, this);

    //make the saving of the window position/size, and its items, generic
    connect(this, &actionWindow_c::closeWindow_signal, actionTypeClassPtr_par, &T::parentClosing_f);
    //save the unique fields of the action type
    connect(this, &actionWindow_c::saveChild_signal, actionTypeClassPtr_par, &T::save_f);
    //once the above is done, get the save result from the "action type"/parent and save *here*
    connect(actionTypeClassPtr_par, &T::saveResult_signal, this, &actionWindow_c::save_f);
}

actionWindow_c::actionWindow_c(
        const int row_par_con
        , const int_fast32_t actionDataId_par_con
        , QWidget *parent_par)
    : QWidget(parent_par)
    , row_pri_con(row_par_con)
    , actionDataId_pri_con(actionDataId_par_con)
{
    this->setObjectName("actionWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //statusBarLabel_pri = new QLabel;

    //combo actions + checkbox halt on fail
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    actionsCombo_pri = new QComboBox();
    actionsCombo_pri->setEditable(true);
    actionsCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    actionsCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    actionsCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(tr("Actions")));
    firstRowLayoutTmp->addWidget(actionsCombo_pri);

    //FUTURE translations
    actionsCombo_pri->insertItem(static_cast<int>(action_ec::createDirectory), "Create directory");
    actionsCombo_pri->insertItem(static_cast<int>(action_ec::runProcess), "Run process");


    haltOnFail_pri = new QCheckBox("Halt on fail");
    haltOnFail_pri->setToolTip("Stop running, don't run the following actions, on fail");

    firstRowLayoutTmp->addWidget(haltOnFail_pri);

    //description
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(tr("Description")));
    descriptionPTE_pri = new QPlainTextEdit(this);
    auto minHeightTmp(descriptionPTE_pri->fontMetrics().lineSpacing() + 14);
    descriptionPTE_pri->setMinimumHeight(minHeightTmp);
    descriptionPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    secondRowLayoutTmp->addWidget(descriptionPTE_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);

    QPushButton* cancelButtonTmp = new QPushButton("Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);

    QPushButton* tipsButtonTmp = new QPushButton("Tips");
    lastRowLayoutTmp->addWidget(tipsButtonTmp);

    QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row2Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);

    row1Tmp->setLayout(secondRowLayoutTmp);
    variableLayout_pri = new QVBoxLayout;
    //variableLayout_pri->setSpacing(0);
    row2Tmp->setLayout(variableLayout_pri);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter");

    mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row2Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);


    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addWidget(mainSplitter_pri);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    //mainLayout_pri->addLayout(variableLayout_pri);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    //mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayout_pri);

    setWindowTitle(tr("Add/update action"));

    if (appConfig_f().configLoaded_f())
    {
         restoreGeometry(appConfig_f().widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_f().widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    connect(actionsCombo_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &actionWindow_c::actionComboChanged_f);
    connect(saveButtonTmp, &QPushButton::clicked, this, &actionWindow_c::saveButtonClicked_f);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &actionWindow_c::cancelButtonClicked_f);
    connect(tipsButtonTmp, &QPushButton::clicked, this, &actionWindow_c::tipsButtonClicked_f);

    int loadedActionIndexTmp(-1);
    bool manualLoadRequiredTmp(false);
    if (actionDataId_pri_con > 0)
    {
        actionData_c& itemTmp(dataHub_f().actionData_f(actionDataId_par_con).first);
        loadedActionIndexTmp = static_cast<int>(itemTmp.type_f());
        if (actionsCombo_pri->currentIndex() == loadedActionIndexTmp)
        {
            manualLoadRequiredTmp = true;
        }
        //this triggers the creation of the fields for the loaded type, if the index CHANGED
        actionsCombo_pri->setCurrentIndex(loadedActionIndexTmp);
        actionsCombo_pri->setEditable(false);

        descriptionPTE_pri->setPlainText(itemTmp.description_f());

        haltOnFail_pri->setChecked(itemTmp.haltOnFail_f());
    }
    else
    {
        actionsCombo_pri->setCurrentIndex(0);
    }

    if (manualLoadRequiredTmp)
    {
        actionComboChanged_f(loadedActionIndexTmp);
    }
}

void actionWindow_c::createWidgetsPerAction_f(
        const int index_par_con
)
{
#ifdef DEBUGJOUVEN
    qDebug() << "createWidgetsPerAction_f index_par_con " << index_par_con << endl;
#endif
    //switch?
    while (true)
    {
        if (index_par_con == static_cast<int>(action_ec::runProcess))
        {
            createActionTypeWidgets_f(runProcessWidgets_pri);
            break;
        }
        if (index_par_con == static_cast<int>(action_ec::createDirectory))
        {
            createActionTypeWidgets_f(createDirectoryWidgets_pri);
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
        if (index_par_con == static_cast<int>(action_ec::runProcess))
        {
            runProcessWidgets_pri->deleteLater();
            runProcessWidgets_pri = Q_NULLPTR;
            break;
        }
        if (index_par_con == static_cast<int>(action_ec::createDirectory))
        {
            createDirectoryWidgets_pri->deleteLater();
            createDirectoryWidgets_pri = Q_NULLPTR;
            break;
        }
        break;
    }
}

void actionWindow_c::actionComboChanged_f(int index_par)
{
    if (lastIndex_pri != -1)
    {
        removeWidgetClassPerAction_f(lastIndex_pri);
    }

    clearLayoutItems_f(variableLayout_pri);
    createWidgetsPerAction_f(index_par);
    lastIndex_pri = index_par;
}

void actionWindow_c::saveButtonClicked_f()
{
    Q_EMIT saveChild_signal();
}

void actionWindow_c::cancelButtonClicked_f()
{
    close();
}

void actionWindow_c::tipsButtonClicked_f()
{
    //TODO
}

void actionWindow_c::save_f(const QJsonObject& childSaveResult_par_con)
{
    action_ec actionTypeTmp(static_cast<action_ec>(actionsCombo_pri->currentIndex()));
    QString descriptionTmp(descriptionPTE_pri->toPlainText());
    if (descriptionTmp.isEmpty())
    {
        errorQMessageBox_f("Empty description", "Error", this);
        return;
        //FUTURE if description is empty, create a descrition Gen for each action, i.e. for process "run " + processPath + argument (limit the length of this)
    }
    bool haltOnFailTmp(haltOnFail_pri->isChecked());

    if (actionDataId_pri_con > 0)
    {
        //"edit", relace the actionData obj with a new one with the "new/edited" values
        actionData_c& objIterator(dataHub_f().actionData_f(actionDataId_pri_con).first);
        objIterator.setType_f(actionTypeTmp);
        objIterator.setDescription_f(descriptionTmp);
        objIterator.setHaltOnFail_f(haltOnFailTmp);
        objIterator.setActionDataJSON_f(childSaveResult_par_con);
        //the setActionDataJSON_f part is done on the specific action class (processaction_c, createDirectory_c...)
    }
    else
    {
        //create
        actionData_c actionDataTmp(actionTypeTmp, descriptionTmp, haltOnFailTmp, childSaveResult_par_con);

        //qInfo() << "1 actionDataTmp.id_f() " << actionDataTmp.id_f() << endl;
        dataHub_f().insertActionData_f(actionDataTmp, row_pri_con);
        //qInfo() << "2 actionDataTmp.id_f() " << actionDataTmp.id_f() << endl;
    }

    Q_EMIT updateRow_Signal(row_pri_con);
    close();
}
