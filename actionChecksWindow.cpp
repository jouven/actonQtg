#include "actionChecksWindow.hpp"

#include "checkWindow.hpp"
#include "appConfig.hpp"
#include "executionResultsWindow.hpp"
#include "../stringFormatting.hpp"

#include "actonQtso/checkDataExecutionResult.hpp"
#include "actonQtso/actonDataHub.hpp"
#include "actonQtso/checkData.hpp"
#include "actonQtso/checkDataExecutionResult.hpp"
#include "actonQtso/checksDataHub.hpp"
#include "actonQtso/checkMappings/checkStrMapping.hpp"
#include "actonQtso/checkMappings/checkExecutionStateStrMapping.hpp"

#include "essentialQtgso/messageBox.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"

#include "signalso/signal.hpp"

#include <QtWidgets>

#include <set>
#include <unordered_set>

void actionChecksWindow_c::closeEvent(QCloseEvent* event)
{
    bool acceptEventTmp(false);
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            text_c logMessageTmp(R"(Action, stringId {0}, "checks window" close while executing checks)", static_cast<action_c*>(checkDataHub_ptr_pri->parent())->stringId_f());
            MACRO_ADDLOG(logMessageTmp, QString(), messageType_ec::information);
            createMessageBoxAskAboutExecutingChecksOnClose_f();
            break;
        }
        appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionChecksTable_pri->objectName() + actionChecksTable_pri->horizontalHeader()->objectName(), actionChecksTable_pri->horizontalHeader()->saveState());
        appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionChecksTable_pri->objectName(), actionChecksTable_pri->saveGeometry());
        appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
        acceptEventTmp = true;
        break;
    }
    if (acceptEventTmp)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

actionChecksWindow_c::actionChecksWindow_c(
        checksDataHub_c* checkDataHub_ptr_par
        , QWidget *parent_par)
    : QWidget(parent_par)
    , checkDataHub_ptr_pri(checkDataHub_ptr_par)
{
    this->setObjectName("actionChecksWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);


//    connect(commandsTable_pri, &QTableWidget::customContextMenuRequested,
//            this, &Window_c::contextMenu);
#ifdef __ANDROID__
    baseWidget_pri = new QWidget(this);
    baseWidget_pri->setAcceptDrops(true);
#else
    this->setAcceptDrops(true);
#endif

}

void actionChecksWindow_c::show_f()
{
    statusBarLabel_pri = new QLabel;

    //add/edit check, remove checks and copy checks
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //add/edit check
    //QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/hash.png"));
    QPushButton* addEditActionCheckButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Add/Edit Check"));
    firstRowLayoutTmp->addWidget(addEditActionCheckButtonTmp);
    QObject::connect(addEditActionCheckButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::addEditActionCheckButtonClicked_f);

    //remove checks
    QPushButton* removeChecksButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Remove Checks"));
    firstRowLayoutTmp->addWidget(removeChecksButtonTmp);
    QObject::connect(removeChecksButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::removeChecksButtonClicked_f);

    //copy check
    QPushButton* copyCheckButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Copy check"));
    firstRowLayoutTmp->addWidget(copyCheckButtonTmp);
    QObject::connect(copyCheckButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::copyCheckButtonClicked_f);

    //execute checks and execution details
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    //execute/stop checks
    executeChecksButton_pri = new QPushButton(appConfig_ptr_ext->translate_f("&Execute Checks"));
    executeChecksButton_pri->setToolTip(appConfig_ptr_ext->translate_f("No selection executes all checks, selection executes selected checks"));
    secondRowLayoutTmp->addWidget(executeChecksButton_pri);
    QObject::connect(executeChecksButton_pri, &QPushButton::clicked, this, &actionChecksWindow_c::executeChecksButtonClicked_f);

    QPushButton* executionDetailsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Execution &details"));
    secondRowLayoutTmp->addWidget(executionDetailsButtonTmp);
    QObject::connect(executionDetailsButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::showExecutionDetailsButtonClicked_f);


    actionChecksTable_pri = new QTableWidget(0, 6);
    actionChecksTable_pri->setObjectName("QTableWidget_");
    actionChecksTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    //0 checkId | 1 check (type) | 2 description | 3 Execution state | 4 Last error | 5 Result
    labels
            << appConfig_ptr_ext->translate_f("CheckId")
            << appConfig_ptr_ext->translate_f("Check Type")
            << appConfig_ptr_ext->translate_f("Description")
            << appConfig_ptr_ext->translate_f("Execution state")
            << appConfig_ptr_ext->translate_f("Last error")
            << appConfig_ptr_ext->translate_f("Result");
    actionChecksTable_pri->setHorizontalHeaderLabels(labels);
    actionChecksTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    actionChecksTable_pri->setShowGrid(true);
    //TODO on android there is no right mouse button by default
    //FUTURE implement context on holding for x seconds?
    actionChecksTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    actionChecksTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif

    //grid and index changing widgwets
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(actionChecksTable_pri);

    QVBoxLayout* indexChangingLayoutTmp = new QVBoxLayout;

    //move checks up button
    QPushButton* moveCheckUpButtonTmp = new QPushButton(QIcon(":/images/upArrow.png"), "");
    moveCheckUpButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Move the selected checks up one index"));
    indexChangingLayoutTmp->addWidget(moveCheckUpButtonTmp);
    QObject::connect(moveCheckUpButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::moveSelectedChecksUpByOneButtonClicked_f);

    //change the index of the selected check button
    QPushButton* changeIndexButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Change\ncheck\nindex"));
    changeIndexButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Changes the first selected check index to a new one moving the destination and all the checks below one index down (except if it's the last index)"));
    indexChangingLayoutTmp->addWidget(changeIndexButtonTmp);
    QObject::connect(changeIndexButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::openChangeCheckIndexWindowButtonClicked_f);

    //move checks down button
    QPushButton* moveCheckDownButtonTmp = new QPushButton(QIcon(":/images/downArrow.png"), "");
    moveCheckDownButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Move the selected checks down one index"));
    indexChangingLayoutTmp->addWidget(moveCheckDownButtonTmp);
    QObject::connect(moveCheckDownButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::moveSelectedChecksDownByOneButtonClicked_f);

    thirdRowLayoutTmp->addLayout(indexChangingLayoutTmp);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* okButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Ok"));
    lastRowLayoutTmp->addWidget(okButtonTmp);
    connect(okButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::okButtonClicked_f);

    //tips button
    QPushButton* tipsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f(appConfig_ptr_ext->translate_f("Tips")));
    lastRowLayoutTmp->addWidget(tipsButtonTmp);
    QObject::connect(tipsButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::tipsButtonClicked_f);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addLayout(thirdRowLayoutTmp);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayout_pri);

#ifdef __ANDROID__
    scrollArea_pri = new QScrollArea(this);
    scrollArea_pri->setWidget(baseWidget_pri);
    //scrollArea_pri->verticalScrollBar()->setFixedWidth(screenGeometry.width() * 0.05);
    //scrollArea_pri->horizontalScrollBar()->setFixedHeight(screenGeometry.height() * 0.04);
    scrollArea_pri->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea_pri->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //scrollArea_pri->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    scrollArea_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea_pri->resize(screenGeometry.width(), screenGeometry.height());
    //scrollArea_pri->setWidgetResizable(true);
#endif

    setWindowTitle(appConfig_ptr_ext->translate_f("Check management"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         actionChecksTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionChecksTable_pri->objectName()));
         actionChecksTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionChecksTable_pri->objectName() + actionChecksTable_pri->horizontalHeader()->objectName()));
    }

    //using checkhub populate the rows
    for (int rowIndex_ite = 0, l = checkDataHub_ptr_pri->size_f(); rowIndex_ite < l; ++rowIndex_ite)
    {
        updateCheckRow_f(rowIndex_ite);
        if (not signalso::isRunning_f())
        {
            break;
        }
    }

#ifdef DEBUGJOUVEN
    //qDebug() << "actionWindow_c::manageChecksButtonClicked_f() actionData_ptr_pri->actionDataExecutionResult_ptr_f() " << checkDataHub_ref_pri.pa actionData_ptr_pri->actionDataExecutionResult_ptr_f() << endl;
    //qDebug() << "actionWindow_c::manageChecksButtonClicked_f() actionData_ptr_pri->actionDataExecutionResult_ptr_f() == nullptr " << (actionData_ptr_pri->actionDataExecutionResult_ptr_f() == nullptr) << endl;
#endif

    if (checkDataHub_ptr_pri->executingChecks_f())
    {
        executionStarted_f();
    }

    QObject::connect(checkDataHub_ptr_pri, &checksDataHub_c::checksExecutionStarted_signal, this, &actionChecksWindow_c::executionStarted_f);
    QObject::connect(checkDataHub_ptr_pri, &checksDataHub_c::checksExecutionFinished_signal, this, &actionChecksWindow_c::executionFinished_f);
    QObject::connect(checkDataHub_ptr_pri, &checksDataHub_c::stoppingChecksExecution_signal, this, &actionChecksWindow_c::stoppingExecution_f);
}

void actionChecksWindow_c::okButtonClicked_f()
{
    close();
}

void actionChecksWindow_c::addEditActionCheckButtonClicked_f()
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Add/edit check disallowed while executing checks")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        int rowTmp(-1);
        if (not actionChecksTable_pri->selectedItems().isEmpty())
        {
            rowTmp = actionChecksTable_pri->selectedItems().first()->row();
        }
        else
        {
            rowTmp = actionChecksTable_pri->rowCount();
        }

        checkWindow_c* checkWindowTmp(new checkWindow_c(rowTmp, checkDataHub_ptr_pri, this));
        //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
        checkWindowTmp->setWindowFlag(Qt::Window, true);
        checkWindowTmp->setWindowModality(Qt::WindowModal);

        connect(checkWindowTmp, &checkWindow_c::updateRow_Signal, this, &actionChecksWindow_c::updateCheckRow_f);

        checkWindowTmp->show();
        break;
    }
}

void actionChecksWindow_c::insertCheckRow_f(
        const check_c* checkPtr_par_con
        , const int row_par_con)
{
    QTableWidgetItem *checkIdCellTmp(new QTableWidgetItem(QString::number(checkPtr_par_con->id_f())));
    checkIdCellTmp->setFlags(checkIdCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *checkTypeCellTmp(new QTableWidgetItem(checkPtr_par_con->typeStr_f()));
    checkTypeCellTmp->setFlags(checkTypeCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *descriptionCellTmp(new QTableWidgetItem(checkPtr_par_con->description_f()));
    descriptionCellTmp->setFlags(descriptionCellTmp->flags() bitand compl Qt::ItemIsEditable);

    int newIndexTmp(row_par_con);
    if (newIndexTmp == -1)
    {
        newIndexTmp = actionChecksTable_pri->rowCount();
    }

    actionChecksTable_pri->insertRow(newIndexTmp);
    actionChecksTable_pri->setItem(newIndexTmp, 0, checkIdCellTmp);
    actionChecksTable_pri->setItem(newIndexTmp, 1, checkTypeCellTmp);
    actionChecksTable_pri->setItem(newIndexTmp, 2, descriptionCellTmp);
    //3 execution state, 4 last error, 5 last result
    actionChecksTable_pri->setItem(newIndexTmp, 3, new QTableWidgetItem);
    actionChecksTable_pri->setItem(newIndexTmp, 4, new QTableWidgetItem);
    actionChecksTable_pri->setItem(newIndexTmp, 5, new QTableWidgetItem);
}

void actionChecksWindow_c::updateExistingCheckRow_f(
        const check_c* checkPtr_par_con
        , const int row_par_con)
{
    actionChecksTable_pri->item(row_par_con, 1)->setText(checkPtr_par_con->typeStr_f());
    actionChecksTable_pri->item(row_par_con, 2)->setText(checkPtr_par_con->description_f());
}

void actionChecksWindow_c::clearAllRowsResultColumns_f()
{
    for (auto rowIndex_ite = 0, l = actionChecksTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        //3 execution state, 4 last error, 5 last result
        actionChecksTable_pri->item(rowIndex_ite, 3)->setText("");
        actionChecksTable_pri->item(rowIndex_ite, 4)->setText("");
        actionChecksTable_pri->item(rowIndex_ite, 5)->setText("");
    }
}

void actionChecksWindow_c::updateCheckRow_f(const int row_par_con)
{
    int_fast32_t checkDataIdTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(row_par_con));
    check_c* checkPtrTmp(checkDataHub_ptr_pri->check_ptr_f(checkDataIdTmp));
    //qInfo() << "3 actionDataTmp.id_f() " << actionDataTmp.id_f() << endl;
#ifdef DEBUGJOUVEN
    //qDebug() << "checkDataIdTmp " << checkDataIdTmp << endl;
#endif
    if (row_par_con == actionChecksTable_pri->rowCount())
    {
        insertCheckRow_f(checkPtrTmp);
        if (checkPtrTmp->checkDataExecutionResult_ptr_f() not_eq nullptr)
        {
            updateCheckExecutionState_f(checkPtrTmp);
            if (checkPtrTmp->checkDataExecutionResult_ptr_f()->messageCount_f() > 0)
            {
                updateCheckError_f(checkPtrTmp->checkDataExecutionResult_ptr_f(), checkPtrTmp->checkDataExecutionResult_ptr_f()->lastMessage_f());
            }
            if (checkPtrTmp->checkDataExecutionResult_ptr_f()->finished_f())
            {
                updateCheckResult_f(checkPtrTmp->checkDataExecutionResult_ptr_f());
            }
            //and connect
            QObject::connect(checkPtrTmp->checkDataExecutionResult_ptr_f(), &checkExecutionResult_c::executionStateUpdated_signal, this, &actionChecksWindow_c::updateCheckExecutionState_f, Qt::UniqueConnection);
            QObject::connect(checkPtrTmp->checkDataExecutionResult_ptr_f(), &checkExecutionResult_c::errorMessageAdded_signal, this, &actionChecksWindow_c::updateCheckError_f, Qt::UniqueConnection);
            QObject::connect(checkPtrTmp->checkDataExecutionResult_ptr_f(), &checkExecutionResult_c::finished_signal , this, &actionChecksWindow_c::updateCheckResult_f, Qt::UniqueConnection);
            //QObject::connect(checkPtrTmp->checkDataExecutionResult_ptr_f(), &checkExecutionResult_c::resultsCleared_signal, this, &actionChecksWindow_c::checkResultsCleared_f, Qt::UniqueConnection);
        }
    }
    else
    {
        updateExistingCheckRow_f(checkPtrTmp, row_par_con);
    }
}

void actionChecksWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>TODO</p>"
                    )
            , appConfig_ptr_ext->translate_f("Tips")
            , this
    );
}

void actionChecksWindow_c::removeChecksButtonClicked_f()
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Check removal disallowed while executing checks")
                               , appConfig_ptr_ext->translate_f("Error")
                               , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No check rows selected for removal")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::set<int> rowIndexSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexSetTmp.emplace(item_ite_con->row());
        }

        std::vector<int> rowsToRemoveTmp(rowIndexSetTmp.begin(), rowIndexSetTmp.end());
        std::reverse(rowsToRemoveTmp.begin(), rowsToRemoveTmp.end());
        for (const int item_ite_con : rowsToRemoveTmp)
        {
            actionChecksTable_pri->removeRow(item_ite_con);
            checkDataHub_ptr_pri->removeCheckUsingRow_f(item_ite_con);
        }
        break;
    }
}

void actionChecksWindow_c::inputDialogCopyCheckIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        int newIndexTmp(copyCheckIndexInputDialog_pri->intValue());
        if (newIndexTmp >= 0 and newIndexTmp <= actionChecksTable_pri->rowCount())
        {
            QList<QTableWidgetItem *> selectionTmp(actionChecksTable_pri->selectedItems());
            int selectedRowTmp(selectionTmp.first()->row());
            check_c* checkCopyTmp(checkDataHub_ptr_pri->check_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(selectedRowTmp))->clone_f());

            checkDataHub_ptr_pri->insertCheck_f(checkCopyTmp, newIndexTmp);
            insertCheckRow_f(checkCopyTmp, newIndexTmp);
            //actionChecksTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f(appConfig_ptr_ext->translateAndReplace_f({"Wrong index to copy the row: {0}", newIndexTmp})
                               , appConfig_ptr_ext->translate_f("Error")
                               , this);
        }
    }
    copyCheckIndexInputDialog_pri->deleteLater();
    copyCheckIndexInputDialog_pri = nullptr;
}

void actionChecksWindow_c::copyCheckButtonClicked_f()
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Copy Check disallowed while checks are executing")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No check row selected to copy")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Check row copy can only be applied to a single row")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        int selectedRowTmp(selectionTmp.first()->row());

        check_c* checkSourcePtrTmp(checkDataHub_ptr_pri->check_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(selectedRowTmp)));

        copyCheckIndexInputDialog_pri = new QInputDialog(this);
        copyCheckIndexInputDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Copy Check"));
        copyCheckIndexInputDialog_pri->setLabelText(appConfig_ptr_ext->translateAndReplace_f({
                    "Input a new index to copy the check to.\nCheck source: "
                    "\nType: {0}"
                    "\nDescription: {1}"
                    "\nRow index: {2}"
                    , checkTypeToStrUMap_ext_con.at(checkSourcePtrTmp->type_f())
                    , truncateString_f(checkSourcePtrTmp->description_f(), 255)
                    , selectedRowTmp
        }));
        copyCheckIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        copyCheckIndexInputDialog_pri->setIntValue(selectedRowTmp);
        copyCheckIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(copyCheckIndexInputDialog_pri, &QFileDialog::finished, this, &actionChecksWindow_c::inputDialogCopyCheckIndexFinished_f);

        copyCheckIndexInputDialog_pri->show();
        break;
    }
}

void actionChecksWindow_c::updateCheckError_f(const executionResult_c* executionResult_par, const executionMessage_c* errorMessage_par)
{
    const checkExecutionResult_c* checkExecutionResultPtrTmp(static_cast<const checkExecutionResult_c*>(executionResult_par));
    int rowTmp(checkDataHub_ptr_pri->checkDataIdToRow_f(checkExecutionResultPtrTmp->check_ptr_f()->id_f()));
    //0 checkId | 1 check (type) | 2 description | 3 Execution state | 4 Last error | 5 Result
    QString translationTmp(appConfig_ptr_ext->translateAndReplace_f(errorMessage_par->text_f()));
    actionChecksTable_pri->item(rowTmp, 4)->setText(truncateString_f(translationTmp, 32));
    actionChecksTable_pri->item(rowTmp, 4)->setToolTip(translationTmp);
}

void actionChecksWindow_c::updateCheckExecutionState_f(check_c* const check_par_ptr_con)
{
    int rowTmp(checkDataHub_ptr_pri->checkDataIdToRow_f(check_par_ptr_con->id_f()));
    QString checkExecutionStateStrTmp(checkExecutionStateToString_f(check_par_ptr_con->checkDataExecutionResult_ptr_f()->lastState_f()));
    QString translationTmp(appConfig_ptr_ext->translateAndReplace_f(checkExecutionStateStrTmp));
    //0 checkId | 1 check (type) | 2 description | 3 Execution state | 4 Last error | 5 Result
    actionChecksTable_pri->item(rowTmp, 3)->setText(truncateString_f(translationTmp, 32));
    actionChecksTable_pri->item(rowTmp, 3)->setToolTip(translationTmp);
}

void actionChecksWindow_c::updateCheckResult_f(const executionResult_c* executionResult_par_con)
{
    const checkExecutionResult_c* checkExecutionResultPtrTmp(static_cast<const checkExecutionResult_c*>(executionResult_par_con));
    int rowTmp(checkDataHub_ptr_pri->checkDataIdToRow_f(checkExecutionResultPtrTmp->check_ptr_f()->id_f()));
    if (executionResult_par_con not_eq nullptr and executionResult_par_con->finished_f())
    {
        //0 checkId | 1 check (type) | 2 description | 3 Execution state | 4 Last error | 5 Result
        actionChecksTable_pri->item(rowTmp, 5)->setText(checkExecutionResultPtrTmp->result_f() ? appConfig_ptr_ext->translate_f("True") : appConfig_ptr_ext->translate_f("False"));
    }
    else
    {
        //0 checkId | 1 check (type) | 2 description | 3 Execution state | 4 Last error | 5 Result
        actionChecksTable_pri->item(rowTmp, 5)->setText("");
    }
}

void actionChecksWindow_c::executionFinished_f()
{
#ifdef DEBUGJOUVEN
    //qDebug() << "actionChecksWindow_c::executionFinished_f()";
#endif
    //FUTURE this might need more details like duration, if errors or not, x/y successfull
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Check/s execution finished"));
    //restore execute button text values
    executeChecksButton_pri->setText(appConfig_ptr_ext->translate_f("&Execute Checks"));
    executeChecksButton_pri->setToolTip(appConfig_ptr_ext->translate_f("No selection executes all actions, selection executes selected actions"));
}

void actionChecksWindow_c::executionStarted_f()
{
#ifdef DEBUGJOUVEN
    //qDebug() << "actionChecksWindow_c::executionStarted_f()";
#endif
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Action/s execution started"));

    executeChecksButton_pri->setText(appConfig_ptr_ext->translate_f("Executing... Press again to Stop"));
    executeChecksButton_pri->setToolTip(appConfig_ptr_ext->translate_f("Will stop after finishing already executing check/s"));
}

void actionChecksWindow_c::stoppingExecution_f()
{
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Trying to stop execution..."));
}

//void actionChecksWindow_c::checkResultsCleared_f(check_c* const check_par_ptr_con)
//{
//    updateCheckResult_f(check_par_ptr_con);
//    updateCheckError_f(check_par_ptr_con);
//    updateCheckExecutionState_f(check_par_ptr_con);
//}

void actionChecksWindow_c::createMessageBoxAskAboutExecutingChecksOnClose_f()
{
    askAboutExecutingChecksOnCloseMessageBox_pri = new QMessageBox(this);
    askAboutExecutingChecksOnCloseMessageBox_pri->setText(appConfig_ptr_ext->translate_f(R"(Check/s are executing. What to do?)"));
    askAboutExecutingChecksOnCloseMessageBox_pri->setTextFormat(Qt::RichText);
    askAboutExecutingChecksOnCloseMessageBox_pri->setWindowModality(Qt::WindowModal);
    askAboutExecutingChecksOnCloseMessageBox_pri->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton* cancelButton = new QPushButton(appConfig_ptr_ext->translate_f("Nothing"));
    cancelButton->setToolTip(appConfig_ptr_ext->translate_f("Close this window"));
    connect(cancelButton, &QPushButton::clicked, askAboutExecutingChecksOnCloseMessageBox_pri, &QMessageBox::close);
    askAboutExecutingChecksOnCloseMessageBox_pri->addButton(cancelButton, QMessageBox::ButtonRole::AcceptRole);

    QPushButton* stopExecutionButton = new QPushButton(appConfig_ptr_ext->translate_f("Stop execution"));
    stopExecutionButton->setToolTip(appConfig_ptr_ext->translate_f("Stop the execution and, after it has stopped, close"));
    connect(stopExecutionButton, &QPushButton::clicked, this, &actionChecksWindow_c::stopExecutingChecksAndClose_f);
    askAboutExecutingChecksOnCloseMessageBox_pri->addButton(stopExecutionButton, QMessageBox::ButtonRole::AcceptRole);

    askAboutExecutingChecksOnCloseMessageBox_pri->show();
}

void actionChecksWindow_c::stopExecutingChecksAndClose_f()
{
    if (checkDataHub_ptr_pri->executingChecks_f())
    {
        connect(checkDataHub_ptr_pri, &checksDataHub_c::checksExecutionFinished_signal, this, &actionChecksWindow_c::close);
        checkDataHub_ptr_pri->stopExecutingChecks_f();
        executeChecksButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... (exiting after)"));
    }
    askAboutExecutingChecksOnCloseMessageBox_pri->deleteLater();
    askAboutExecutingChecksOnCloseMessageBox_pri = nullptr;
}

void actionChecksWindow_c::executeChecks_f()
{
    std::vector<int> rowsToExecuteTmp;
    clearAllRowsResultColumns_f();

    //FUTURE timer or confirmation (and make it optional on an option)
    //to prevent users from executing unwanted stuff just by missclick

    //this is NOT the amount of rows, this gets CELLS, the same row will appear repeatedly, that's why a std::set is used later
    QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
    //all
    if (selectionTmp.isEmpty())
    {
        rowsToExecuteTmp.reserve(actionChecksTable_pri->rowCount());
        for (auto rowIndex_ite = 0, l = actionChecksTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
        {
            rowsToExecuteTmp.emplace_back(rowIndex_ite);
        }
    }
    else
        //selection
    {
        //default order is always ascending in c++ (uses std::less, AKA less goes first)
        std::set<int> selectedRowsSetTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            selectedRowsSetTmp.emplace(item_ite_con->row());
        }
        rowsToExecuteTmp.reserve(selectedRowsSetTmp.size());
        for (const int row_ite_con : selectedRowsSetTmp)
        {
            rowsToExecuteTmp.emplace_back(row_ite_con);
        }
    }

    if (not rowsToExecuteTmp.empty())
    {
        for (const int checkRow_ite_con : rowsToExecuteTmp)
        {
            int_fast64_t checkIdToRunTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(checkRow_ite_con));

            check_c* checkPtrTmp(checkDataHub_ptr_pri->check_ptr_f(checkIdToRunTmp));
#ifdef DEBUGJOUVEN
            //qDebug() << "checkIdToRunTmp " << checkIdToRunTmp << endl;
            //qDebug() << "checkDataPtrTmp is? " << (checkDataPtrTmp == nullptr ? "null" : "not null") << endl;
#endif
            if (checkPtrTmp not_eq nullptr)
            {
                //it's not tracked if the executions results are already connected... so... delete the results object each time and connect,
                //otherwise repeated connections would happen, to solve this "issue" would require a map of which actions are connected and which aren't
                //also reconnect or undo the initial connection if the check types changes
                checkExecutionResult_c* checkExecutionResultPtr = checkPtrTmp->createGetCheckDataExecutionResult_ptr_f();
                //FUTURE add warning when running an action again if there are results already?
                QObject::connect(checkExecutionResultPtr, &checkExecutionResult_c::executionStateUpdated_signal, this, &actionChecksWindow_c::updateCheckExecutionState_f, Qt::UniqueConnection);
                QObject::connect(checkExecutionResultPtr, &checkExecutionResult_c::errorMessageAdded_signal, this, &actionChecksWindow_c::updateCheckError_f, Qt::UniqueConnection);
                QObject::connect(checkExecutionResultPtr, &checkExecutionResult_c::finished_signal , this, &actionChecksWindow_c::updateCheckResult_f, Qt::UniqueConnection);
                //QObject::connect(checkExecutionResultPtr, &checkExecutionResult_c::resultsCleared_signal, this, &actionChecksWindow_c::checkResultsCleared_f, Qt::UniqueConnection);

                //on the grid only the above are used
                //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);
                //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);
            }
        }

        checkDataHub_ptr_pri->executeCheckDataRows_f(rowsToExecuteTmp);
    }
}

void actionChecksWindow_c::executeChecksButtonClicked_f()
{
    while (true)
    {
        if (checkDataHub_ptr_pri->stoppingChecksExecution_f())
        {
            break;
        }

        //if already running, the button is changed to stop execution
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            executeChecksButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));
            checkDataHub_ptr_pri->stopExecutingChecks_f();
            break;
        }

        executeChecks_f();

        break;
    }
}

void actionChecksWindow_c::showExecutionDetailsButtonClicked_f()
{
    executionResultsWindow_c* executionResultsWindowTmp(nullptr);
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp(actionChecksTable_pri->selectedItems());
        std::set<int> rowIndexSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexSetTmp.emplace(item_ite_con->row());
        }

        //if nothing is selected, filter by all the checks on the table
        if (rowIndexSetTmp.empty())
        {
            for (int index_ite = 0; index_ite < actionChecksTable_pri->rowCount(); ++index_ite)
            {
                rowIndexSetTmp.emplace(index_ite);
            }
        }

        std::vector<check_c*> selectedChecksTmp;
        for (const int row_ite_con : rowIndexSetTmp)
        {
            int_fast64_t checkDataIdTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(row_ite_con));
            check_c* checkPtrTmp(checkDataHub_ptr_pri->check_ptr_f(checkDataIdTmp));
            //no need to check if the checks have results because executionResultsWindow_c can stay empty
            selectedChecksTmp.emplace_back(checkPtrTmp);
        }

        if (not selectedChecksTmp.empty())
        {
            executionResultsWindowTmp = new executionResultsWindow_c(selectedChecksTmp);
        }
        break;
    }

    if (executionResultsWindowTmp not_eq nullptr)
    {
        executionResultsWindowTmp->setWindowFlag(Qt::Window, true);
        executionResultsWindowTmp->setWindowModality(Qt::WindowModal);
        //it is set in the class ctor of executionResultsWindow_c
        //executionResultsWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
        executionResultsWindowTmp->show_f();
    }
}

void actionChecksWindow_c::moveSelectedChecks_f(const int moveOffSet_par_con)
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Check moving disallowed while executing checks")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Empty selection to move")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::unordered_set<int> rowIndexUSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexUSetTmp.emplace(item_ite_con->row());
        }

        std::vector<int> rowIndexVectorTmp(rowIndexUSetTmp.begin(), rowIndexUSetTmp.end());
        //if the offset moves them "up" start from the lower indexes
        if (moveOffSet_par_con < 0)
        {
            std::sort(rowIndexVectorTmp.begin(), rowIndexVectorTmp.end(), std::less<>());
        }
        //if the offset moves them "down" start from the higher indexes
        else
        {
            std::sort(rowIndexVectorTmp.begin(), rowIndexVectorTmp.end(), std::greater<>());
        }

        for (const int index_ite_con : rowIndexVectorTmp)
        {
            auto destinationRow(index_ite_con + moveOffSet_par_con);
            //check if the destination index is out of bounds
            if (destinationRow >= 0 and destinationRow < actionChecksTable_pri->rowCount())
            {
                checkDataHub_ptr_pri->moveCheckRow_f(index_ite_con, destinationRow);
                //change the grid (visual)
                actionChecksTable_pri->removeRow(index_ite_con);
                check_c* checkPtrTmp(checkDataHub_ptr_pri->check_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(destinationRow)));
                insertCheckRow_f(checkPtrTmp, destinationRow);
                //deselects previous stuff, doesn't maintain "old" selection
                //actionsTable_pri->selectRow(destinationRow);
            }
        }
        break;
    }
}

void actionChecksWindow_c::moveSelectedChecksUpByOneButtonClicked_f()
{
    moveSelectedChecks_f(-1);
}

void actionChecksWindow_c::moveSelectedChecksDownByOneButtonClicked_f()
{
    moveSelectedChecks_f(1);
}

void actionChecksWindow_c::inputDialogChangeCheckIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        QList<QTableWidgetItem *> selectionTmp(actionChecksTable_pri->selectedItems());
        int currentIndexTmp(selectionTmp.first()->row());
        int newIndexTmp(changeCheckIndexInputDialog_pri->intValue());
        if (newIndexTmp not_eq currentIndexTmp and (newIndexTmp >= 0 and newIndexTmp < actionChecksTable_pri->rowCount()))
        {
            //change the data
            checkDataHub_ptr_pri->moveCheckRow_f(currentIndexTmp, newIndexTmp);
            //change the grid (visual)
            check_c* checkPtrTmp(checkDataHub_ptr_pri->check_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(newIndexTmp)));
            actionChecksTable_pri->removeRow(currentIndexTmp);
            insertCheckRow_f(checkPtrTmp, newIndexTmp);
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translateAndReplace_f({"Same or wrong index to move the row: {0}", newIndexTmp})
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
        }
    }
    changeCheckIndexInputDialog_pri->deleteLater();
    changeCheckIndexInputDialog_pri = nullptr;
}

void actionChecksWindow_c::openChangeCheckIndexWindowButtonClicked_f()
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Check index change disallowed while executing actions")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp(actionChecksTable_pri->selectedItems());
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No check row selected to change row index")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Check row index change can only be applied to a single row")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        int currentIndexTmp(selectionTmp.first()->row());

        changeCheckIndexInputDialog_pri = new QInputDialog(this);
        changeCheckIndexInputDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Change Check index"));
        changeCheckIndexInputDialog_pri->setLabelText(
                    appConfig_ptr_ext->translateAndReplace_f({"Input a new index to move the check to (current index: {0})", currentIndexTmp}));
        changeCheckIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        changeCheckIndexInputDialog_pri->setIntValue(currentIndexTmp);
        changeCheckIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(changeCheckIndexInputDialog_pri, &QFileDialog::finished, this, &actionChecksWindow_c::inputDialogChangeCheckIndexFinished_f);

        changeCheckIndexInputDialog_pri->show();
        break;
    }
}


