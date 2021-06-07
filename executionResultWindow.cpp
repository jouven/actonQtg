#include "executionResultWindow.hpp"

#include "appConfig.hpp"
#include "actonDataHubGlobal.hpp"
#include "../logsWindowWidgets/QDatetimeEditDelegate.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/actionDataExecutionResult.hpp"
#include "actonQtso/checkDataExecutionResult.hpp"
//#include "actonQtso/actionMappings/actionExecutionStateStrMapping.hpp"
#include "actonQtso/dateTimeStringFormat.hpp"
//#include "actonQtso/actonDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QtWidgets>
//#include <QSplitter>

void executionResultWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + executionMessagesTable_pri->objectName() + executionMessagesTable_pri->horizontalHeader()->objectName(), executionMessagesTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + executionMessagesTable_pri->objectName(), executionMessagesTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

void executionResultWindow_c::addExecutionMessageEntry_f(const executionMessage_c* const executionMessage_par_con)
{

#ifdef DEBUGJOUVEN
//    qtOutRef_ext() << "index_par_con " << index_par_con << "\n"
//    //<< "indexTmp " << indexTmp << "\n"
//    << "logsTable_pri->rowCount() " << logsTable_pri->rowCount() << "\n"
//    << "message " << logItem_par_con->message_f()
//    << endl;
#endif

//    QStringList labelsTmp({
//            appConfig_ptr_ext->translate_f("Id") //message id
//            , appConfig_ptr_ext->translate_f("Type") //information, error, warning...
//            , appConfig_ptr_ext->translate_f("Reference") //action or check id? to know what action/check this message pertains
//            , appConfig_ptr_ext->translate_f("Text")
//            , appConfig_ptr_ext->translate_f("Datetime") //datetime of when the message was generated
//    });

#ifdef DEBUGJOUVEN
    qDebug() << "executionMessage_par_con->id_f() " << executionMessage_par_con->id_f() << Qt::endl;
    qDebug() << "executionMessage_par_con->reference_f() " << executionMessage_par_con->reference_f() << Qt::endl;
    qDebug() << "executionMessage_par_con->text_f() " << executionMessage_par_con->text_f().toRawReplace_f() << Qt::endl;
#endif

    int newRowIndexTmp(executionMessagesTable_pri->rowCount());
    executionMessagesTable_pri->insertRow(newRowIndexTmp);
    //QTableWidgetItem *indexCellTmp(new QTableWidgetItem(QString::number(indexTmp)));
    QTableWidgetItem* idCellTmp(new QTableWidgetItem);
    //Note: The default implementation treats Qt::EditRole and Qt::DisplayRole as referring to the same data.
    idCellTmp->setData(Qt::DisplayRole, QVariant::fromValue(executionMessage_par_con->id_f()));
    idCellTmp->setFlags(idCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *typeCellTmp(new QTableWidgetItem);
    typeCellTmp->setData(Qt::DisplayRole, executionMessage_c:: executionMessageTypeToString_f(executionMessage_par_con->type_f()));
    typeCellTmp->setFlags(typeCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem* referenceCellTmp(new QTableWidgetItem);
    referenceCellTmp->setData(Qt::DisplayRole, executionMessage_par_con->reference_f());
    referenceCellTmp->setFlags(referenceCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *textCellTmp(new QTableWidgetItem);
    //only translate if type is not stderr or stdout
    QString textTmp;
    if (equalOnce_ft(executionMessage_par_con->type_f(),
                     executionMessage_c::type_ec::error
                     , executionMessage_c::type_ec::information
                     , executionMessage_c::type_ec::warning))
    {
        textTmp = appConfig_ptr_ext->translateAndReplace_f(executionMessage_par_con->text_f());
    }
    else
    {
        textTmp = executionMessage_par_con->text_f().toRawReplace_f();
    }

    textCellTmp->setData(Qt::DisplayRole, textTmp);
    textCellTmp->setFlags(textCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *timestampCellTmp(new QTableWidgetItem);
    timestampCellTmp->setData(Qt::DisplayRole, QDateTime::fromMSecsSinceEpoch(executionMessage_par_con->timestamp_f()).toLocalTime().toString(dateTimeFormat_con));
    timestampCellTmp->setFlags(timestampCellTmp->flags() bitand compl Qt::ItemIsEditable);

    executionMessagesTable_pri->setItem(newRowIndexTmp, 0, idCellTmp);
    executionMessagesTable_pri->setItem(newRowIndexTmp, 1, typeCellTmp);
    executionMessagesTable_pri->setItem(newRowIndexTmp, 2, referenceCellTmp);
    executionMessagesTable_pri->setItem(newRowIndexTmp, 3, textCellTmp);
    executionMessagesTable_pri->setItem(newRowIndexTmp, 4, timestampCellTmp);

#ifdef DEBUGJOUVEN
//    qtOutRef_ext() << "Log entry added, index " << index_par << endl;
//    qtOutRef_ext() << "Log entry added, message_f " << logItem_par->message_f() << endl;
//    qtOutRef_ext() << "Log entry added, datetime " << logDateTime_par->toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << endl;
#endif
}

void executionResultWindow_c::loadMessages_f()
{
    executionMessagesTable_pri->setSortingEnabled(false);

    while (true)
    {
        if (resultMode_pri == resultMode_ec::allMessages)
        {
            for (const executionMessage_c* const executionMessagePtr_ite : actonDataHub_ptr_ext->executionMessages_f())
            {
                addExecutionMessageEntry_f(executionMessagePtr_ite);
            }
            break;
        }

        if (resultMode_pri == resultMode_ec::selectedMessages)
        {
            for (executionResult_c* executionResult_ite : executionResults_pri)
            {
                 for (const executionMessage_c* const executionMessagePtr_ite : executionResult_ite->messages_f())
                 {
                     addExecutionMessageEntry_f(executionMessagePtr_ite);
                 }
            }
            break;
        }

        if (equalOnce_ft(resultMode_pri, resultMode_ec::action, resultMode_ec::check))
        {
            for (const executionMessage_c* const executionMessagePtr_ite : executionResult_pri->messages_f())
            {
                addExecutionMessageEntry_f(executionMessagePtr_ite);
            }
            break;
        }

        break;
    }
    executionMessagesTable_pri->setSortingEnabled(true);
    executionMessagesTable_pri->scrollToBottom();
}

executionResultWindow_c::executionResultWindow_c(
        const std::vector<executionResult_c*>& executionResults_par_con
        , QWidget *parent_par)
    : QWidget(parent_par)
    , executionResults_pri(executionResults_par_con)
{
    this->setObjectName("executionResultWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlag(Qt::Window, true);
    this->setWindowModality(Qt::WindowModal);
}

void executionResultWindow_c::show_f()
{
    while (true)
    {
        if (executionResults_pri.empty())
        {
            resultMode_pri = resultMode_ec::allMessages;
            break;
        }

        if (executionResults_pri.size() > 1)
        {
            resultMode_pri = resultMode_ec::selectedMessages;
            break;
        }

        executionResult_pri = executionResults_pri.front();
        if (executionResults_pri.front()->type_f() == executionResult_c::type_ec::action)
        {
            resultMode_pri = resultMode_ec::action;
            actionExecutionResult_pri = static_cast<actionExecutionResult_c*>(executionResults_pri.front());
            break;
        }

        if (executionResults_pri.front()->type_f() == executionResult_c::type_ec::check)
        {
            resultMode_pri = resultMode_ec::check;
            checkExecutionResult_pri = static_cast<checkExecutionResult_c*>(executionResults_pri.front());
            break;
        }
        break;
    }

    //state and return code

    //output
    //error

    //actionexternaloutput
    //actionexternalerror

    //execution state, finished?, action return code and check result
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    QLabel* executionStateLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Execution state"));
    //I need to figure a better way to calculate a minmum height for the fields
    //AKA a text line height + the upper+lower height margins? of box containing it
    auto minHeightTmp(executionStateLabelTmp->fontMetrics().lineSpacing() + 14);
    firstRowLayoutTmp->addWidget(executionStateLabelTmp);
    executionStateTE_pri = new QTextEdit;
    executionStateTE_pri->setMinimumHeight(minHeightTmp);
    //executionStateTE_pri->setMinimumHeight(executionStateTE_pri->fontMetrics().lineSpacing());
    //executionStateTE_pri->setMinimumHeight(30);
    executionStateTE_pri->setMaximumSize(QWIDGETSIZE_MAX, minHeightTmp);
    executionStateTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    executionStateTE_pri->setReadOnly(true);
    firstRowLayoutTmp->addWidget(executionStateTE_pri);

    if (resultMode_pri == resultMode_ec::action)
    {
        //only for action type
        anyFinishCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Any finish"));
        anyFinishCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("True if the action is done executing, no matter what state"));
        //anyFinishCheckbox_pri->setMinimumHeight(minHeightTmp);
        firstRowLayoutTmp->addWidget(anyFinishCheckbox_pri);

        //only for action type
        firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Return code")));
        returnCodeTE_pri = new QTextEdit;
        returnCodeTE_pri->setMinimumHeight(minHeightTmp);
        returnCodeTE_pri->setMaximumSize(QWIDGETSIZE_MAX, minHeightTmp);
        returnCodeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        returnCodeTE_pri->setReadOnly(true);
        firstRowLayoutTmp->addWidget(returnCodeTE_pri);
    }

    if (resultMode_pri == resultMode_ec::check)
    {
        //only for check type
        checkResultCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Result"));
        //checkResultCheckbox_pri->setMinimumHeight(minHeightTmp);
        checkResultCheckbox_pri->setTristate(true);
        checkResultCheckbox_pri->setCheckState(Qt::PartiallyChecked);
        firstRowLayoutTmp->addWidget(checkResultCheckbox_pri);
    }

    //start and finish times
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Execution start")));
    executionStartDatetimeTE_pri = new QTextEdit;
    executionStartDatetimeTE_pri->setMinimumHeight(minHeightTmp);
    executionStartDatetimeTE_pri->setMaximumSize(QWIDGETSIZE_MAX, minHeightTmp);
    executionStartDatetimeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionStartDatetimeTE_pri->setReadOnly(true);
    secondRowLayoutTmp->addWidget(executionStartDatetimeTE_pri);

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Execution finish")));
    executionFinishDatetimeTE_pri = new QTextEdit;
    executionFinishDatetimeTE_pri->setMinimumHeight(minHeightTmp);
    executionFinishDatetimeTE_pri->setMaximumSize(QWIDGETSIZE_MAX, minHeightTmp);
    executionFinishDatetimeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionFinishDatetimeTE_pri->setReadOnly(true);
    secondRowLayoutTmp->addWidget(executionFinishDatetimeTE_pri);

    //execution message table
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    executionMessagesTable_pri = new QTableWidget(0, 5);
    executionMessagesTable_pri->verticalHeader()->setVisible(false);
    executionMessagesTable_pri->setObjectName("QTableWidget_");
    executionMessagesTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labelsTmp({
            appConfig_ptr_ext->translate_f("Id") //message id
            , appConfig_ptr_ext->translate_f("Type") //information, error, warning...
            , appConfig_ptr_ext->translate_f("Reference") //action or check id? to know what action/check this message pertains
            , appConfig_ptr_ext->translate_f("Text")
            , appConfig_ptr_ext->translate_f("Datetime") //datetime of when the message was generated
    });
    executionMessagesTable_pri->setHorizontalHeaderLabels(labelsTmp);
    executionMessagesTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    executionMessagesTable_pri->setShowGrid(true);
    executionMessagesTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionMessagesTable_pri->sortItems(0);
    executionMessagesTable_pri->setSortingEnabled(true);
    thirdRowLayoutTmp->addWidget(executionMessagesTable_pri);

    QDateTimeStyledDelegate_c* dateTimeCellDelegate(new QDateTimeStyledDelegate_c(this));
    executionMessagesTable_pri->setItemDelegateForColumn(4, dateTimeCellDelegate);

    //ok-close and tips buttons
    QHBoxLayout* seventhRowTmp = new QHBoxLayout;

    QPushButton* okButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Close"));
    seventhRowTmp->addWidget(okButtonPtrTmp);
    QPushButton* tipsButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    seventhRowTmp->addWidget(tipsButtonPtrTmp);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addLayout(thirdRowLayoutTmp);
    mainLayout_pri->addLayout(seventhRowTmp);

    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Execution Result"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         executionMessagesTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + executionMessagesTable_pri->objectName()));
         executionMessagesTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + executionMessagesTable_pri->objectName() + executionMessagesTable_pri->horizontalHeader()->objectName()));
    }

    //"ok" it's the same as a cancel
    connect(okButtonPtrTmp, &QPushButton::clicked, this, &executionResultWindow_c::cancelButtonClicked_f);
    connect(tipsButtonPtrTmp, &QPushButton::clicked, this, &executionResultWindow_c::tipsButtonClicked_f);

    if (resultMode_pri == resultMode_ec::action)
    {
        //disabling a checkbox disables any "programming", signals won't work,
        //so to prevent the user changing the state, this lambda reverses user check changes
        //"programming" wise since it's known before hand
        //when the check will change the signals can be blocked to disable this behavior and change the value
        //and since the this uses the GUI thread, the user has no chance to go around this "hack"
        connect(anyFinishCheckbox_pri, &QCheckBox::stateChanged, this, [this](int state_par)
        {
            //to prevent signal recursion
            anyFinishCheckbox_pri->blockSignals(true);
            anyFinishCheckbox_pri->setCheckState(state_par == Qt::Checked ? Qt::Unchecked : Qt::Checked);
            anyFinishCheckbox_pri->blockSignals(false);
        });
    }

    if (resultMode_pri == resultMode_ec::check)
    {
        connect(checkResultCheckbox_pri, &QCheckBox::stateChanged, this, [this](int )//state_par)
        {
            //to prevent signal recursion
            checkResultCheckbox_pri->blockSignals(true);

            if (checkExecutionResult_pri->finished_f())
            {
                checkResultCheckbox_pri->setCheckState(checkExecutionResult_pri->result_f() ? Qt::Checked : Qt::Unchecked);
            }
            else
            {
                checkResultCheckbox_pri->setCheckState(Qt::PartiallyChecked);
            }
            checkResultCheckbox_pri->blockSignals(false);
        });
    }

#ifdef DEBUGJOUVEN
    //qDebug() << "executionResultWindow_c::show_f() 2" << Qt::endl;
#endif

    loadMessages_f();

    while (true)
    {
        if (resultMode_pri == resultMode_ec::allMessages)
        {
            connect(actonDataHub_ptr_ext, &actonDataHub_c::executionMessageAdded_signal, this, &executionResultWindow_c::addExecutionMessage_f);
            break;
        }

        if (resultMode_pri == resultMode_ec::selectedMessages)
        {
            for (executionResult_c* executionResult_ite : executionResults_pri)
            {
                connect(executionResult_ite, &executionResult_c::messageAdded_signal, this, &executionResultWindow_c::addMessageAction_f);
            }
            break;
        }

        if (resultMode_pri == resultMode_ec::action)
        {
            updateState_f();
            updateReturnCode_f();
            updateAnyFinish_f();

            connect(actionExecutionResult_pri, &actionExecutionResult_c::messageAdded_signal, this, &executionResultWindow_c::addMessageAction_f);
            connect(actionExecutionResult_pri, &actionExecutionResult_c::executionStateUpdated_signal, this, &executionResultWindow_c::updateState_f);
            connect(actionExecutionResult_pri, &actionExecutionResult_c::returnCodeSet_signal, this, &executionResultWindow_c::updateReturnCode_f);
            connect(actionExecutionResult_pri, &actionExecutionResult_c::finished_signal, this, &executionResultWindow_c::updateAnyFinish_f);
            break;
        }

        if (resultMode_pri == resultMode_ec::check)
        {
            updateState_f();
            updateAnyFinish_f();

            connect(checkExecutionResult_pri, &checkExecutionResult_c::messageAdded_signal, this, &executionResultWindow_c::addMessageCheck_f);
            connect(checkExecutionResult_pri, &checkExecutionResult_c::executionStateUpdated_signal, this, &executionResultWindow_c::updateState_f);
            connect(checkExecutionResult_pri, &checkExecutionResult_c::finished_signal, this, &executionResultWindow_c::updateAnyFinish_f);
            break;
        }
        break;
    }
#ifdef DEBUGJOUVEN
    //qDebug() << "executionResultWindow_c::show_f() 3" << Qt::endl;
#endif
    show();
}

void executionResultWindow_c::cancelButtonClicked_f()
{
    close();
}

void executionResultWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>1 return value and external output/error only apply for runProcess actions</p>"
                    "<p>2 \"Any finish\", checked, means that the action has finished running, successfully or not</p>"
                    )
            , "Tips"
            , this
    );
}

void executionResultWindow_c::addExecutionMessage_f(const executionMessage_c* executionMessage_par)
{
    addExecutionMessageEntry_f(executionMessage_par);
}

void executionResultWindow_c::addMessageAction_f(executionResult_c* , executionMessage_c* executionMessage_par)
{
    addExecutionMessageEntry_f(executionMessage_par);
}

void executionResultWindow_c::addMessageCheck_f(executionResult_c* , executionMessage_c* executionMessage_par)
{
    addExecutionMessageEntry_f(executionMessage_par);
}

void executionResultWindow_c::updateState_f()
{
    if (executionStartDatetimeTE_pri->toPlainText().isEmpty() and executionResult_pri->started_f())
    {
        executionStartDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(executionResult_pri->startTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    executionStateTE_pri->setText(executionResult_pri->stateString_f());
}

void executionResultWindow_c::updateReturnCode_f()
{
    returnCodeTE_pri->setText(QString::number(actionExecutionResult_pri->returnCode_f()));
}

void executionResultWindow_c::updateAnyFinish_f()
{
    if (executionResult_pri->finished_f() and executionResult_pri->finishedTime_f() not_eq 0)
    {
        executionFinishDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(executionResult_pri->finishedTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    anyFinishCheckbox_pri->blockSignals(true);
    anyFinishCheckbox_pri->setCheckState(executionResult_pri->finished_f() ? Qt::Checked : Qt::Unchecked);
    anyFinishCheckbox_pri->blockSignals(false);
}

