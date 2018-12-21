#include "actionChecksWindow.hpp"

#include "checkWindow.hpp"
#include "appConfig.hpp"
#include "checkExecutionDetailsWindow.hpp"

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
            MACRO_ADDACTONQTGLOG(R"(Action, stringId)" + checkDataHub_ptr_pri->parentAction_f()->stringId_f() + R"(, "checks window" close while executing checks)", logItem_c::type_ec::info);
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
    this->setObjectName("actionChecksWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

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
    executeChecksButton_pri->setToolTip("No selection executes all checks, selection executes selected checks");
    secondRowLayoutTmp->addWidget(executeChecksButton_pri);
    QObject::connect(executeChecksButton_pri, &QPushButton::clicked, this, &actionChecksWindow_c::executeChecksButtonClicked_f);

    QPushButton* executionDetailsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Execution &details"));
    secondRowLayoutTmp->addWidget(executionDetailsButtonTmp);
    QObject::connect(executionDetailsButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::showExecutionDetailsButtonClicked_f);


    actionChecksTable_pri = new QTableWidget(0, 5);
    actionChecksTable_pri->setObjectName("QTableWidget");
    actionChecksTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    //0 check (type) | 1 description | 2 Execution state | 3 Last error | 4 Result
    labels << appConfig_ptr_ext->translate_f("Check") << appConfig_ptr_ext->translate_f("Description") << appConfig_ptr_ext->translate_f("Execution state")  << appConfig_ptr_ext->translate_f("Last error") << appConfig_ptr_ext->translate_f("Result");
    actionChecksTable_pri->setHorizontalHeaderLabels(labels);
    actionChecksTable_pri->horizontalHeader()->setObjectName("QHeaderView");
    actionChecksTable_pri->setShowGrid(true);
    //TODO on android there is no right mouse button by default
    //FUTURE implement context on holding for x seconds?
    actionChecksTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    actionChecksTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif
//    connect(commandsTable_pri, &QTableWidget::customContextMenuRequested,
//            this, &Window_c::contextMenu);
#ifdef __ANDROID__
    baseWidget_pri = new QWidget(this);
    baseWidget_pri->setAcceptDrops(true);
#else
    this->setAcceptDrops(true);
#endif
    //grid and index changing widgwets
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(actionChecksTable_pri);

    QVBoxLayout* indexChangingLayoutTmp = new QVBoxLayout;

    //move checks up button
    QPushButton* moveCheckUpButtonTmp = new QPushButton(QIcon(":/images/upArrow.png"), "");
    moveCheckUpButtonTmp->setToolTip("Move the selected checks up one index");
    indexChangingLayoutTmp->addWidget(moveCheckUpButtonTmp);
    QObject::connect(moveCheckUpButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::moveSelectedChecksUpByOneButtonClicked_f);

    //change the index of the selected check button
    QPushButton* changeIndexButtonTmp = new QPushButton("Change\ncheck\nindex");
    changeIndexButtonTmp->setToolTip("Changes the first selected check index to a new one moving the destination and all the checks below one index down (except if it's the last index)");
    indexChangingLayoutTmp->addWidget(changeIndexButtonTmp);
    QObject::connect(changeIndexButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::openChangeCheckIndexWindowButtonClicked_f);

    //move checks down button
    QPushButton* moveCheckDownButtonTmp = new QPushButton(QIcon(":/images/downArrow.png"), "");
    moveCheckDownButtonTmp->setToolTip("Move the selected checks down one index");
    indexChangingLayoutTmp->addWidget(moveCheckDownButtonTmp);
    QObject::connect(moveCheckDownButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::moveSelectedChecksDownByOneButtonClicked_f);

    thirdRowLayoutTmp->addLayout(indexChangingLayoutTmp);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* okButtonTmp = new QPushButton("&Ok");
    lastRowLayoutTmp->addWidget(okButtonTmp);
    connect(okButtonTmp, &QPushButton::clicked, this, &actionChecksWindow_c::okButtonClicked_f);

    //tips button
    QPushButton* tipsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
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

    if (checkDataHub_ptr_par->executingChecks_f())
    {
        executionStarted_f();
    }

    QObject::connect(checkDataHub_ptr_pri->proxyQObj_f(), &checksDataHubProxyQObj_c::checksExecutionStarted_signal, this, &actionChecksWindow_c::executionStarted_f);
    QObject::connect(checkDataHub_ptr_pri->proxyQObj_f(), &checksDataHubProxyQObj_c::checksExecutionFinished_signal, this, &actionChecksWindow_c::executionFinished_f);
    QObject::connect(checkDataHub_ptr_pri->proxyQObj_f(), &checksDataHubProxyQObj_c::stoppingChecksExecution_signal, this, &actionChecksWindow_c::stoppingExecution_f);
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
            errorQMessageBox_f("Add/edit check disallowed while executing checks", "Error", this);
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
        const checkType_ec& checkType_par_con
        , const QString& description_par_con
        , const int row_par_con)
{
    QString checkTypeStr(checkTypeToStrUMap_glo_sta_con.at(checkType_par_con));

    QTableWidgetItem *checkValueCellTmp(new QTableWidgetItem(checkTypeStr));
    checkValueCellTmp->setFlags(checkValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *descriptionCellTmp(new QTableWidgetItem(description_par_con));
    descriptionCellTmp->setFlags(descriptionCellTmp->flags() bitand compl Qt::ItemIsEditable);

    int newIndexTmp(row_par_con);
    if (newIndexTmp == -1)
    {
        newIndexTmp = actionChecksTable_pri->rowCount();
    }

    actionChecksTable_pri->insertRow(newIndexTmp);
    actionChecksTable_pri->setItem(newIndexTmp, 0, checkValueCellTmp);
    actionChecksTable_pri->setItem(newIndexTmp, 1, descriptionCellTmp);
    //2 execution state, 3 last error, 5 last result
    actionChecksTable_pri->setItem(newIndexTmp, 2, new QTableWidgetItem);
    actionChecksTable_pri->setItem(newIndexTmp, 3, new QTableWidgetItem);
    actionChecksTable_pri->setItem(newIndexTmp, 4, new QTableWidgetItem);
}

void actionChecksWindow_c::updateExistingCheckRow_f(
        const checkType_ec& checkType_par_con
        , const QString& description_par_con
        , const int row_par_con)
{
    QString checkTypeStr(checkTypeToStrUMap_glo_sta_con.at(checkType_par_con));

    actionChecksTable_pri->item(row_par_con, 0)->setText(checkTypeStr);
    actionChecksTable_pri->item(row_par_con, 1)->setText(description_par_con);
}

void actionChecksWindow_c::clearAllRowsResultColumns_f()
{
    for (auto rowIndex_ite = 0, l = actionChecksTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        //2 execution state, 3 last error, 4 last result
        actionChecksTable_pri->item(rowIndex_ite, 2)->setText("");
        actionChecksTable_pri->item(rowIndex_ite, 3)->setText("");
        actionChecksTable_pri->item(rowIndex_ite, 4)->setText("");
    }
}

void actionChecksWindow_c::updateCheckRow_f(const int row_par_con)
{
    int_fast32_t checkDataIdTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(row_par_con));
    checkData_c* checkDataPtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkDataIdTmp));
    //qInfo() << "3 actionDataTmp.id_f() " << actionDataTmp.id_f() << endl;
    if (row_par_con == actionChecksTable_pri->rowCount())
    {
        insertCheckRow_f(checkDataPtrTmp->type_f(), checkDataPtrTmp->description_f());
        if (checkDataPtrTmp->checkDataExecutionResult_ptr_f() not_eq nullptr)
        {
            updateCheckExecutionState_f(checkDataPtrTmp);
            updateCheckError_f(checkDataPtrTmp);
            if (checkDataPtrTmp->checkDataExecutionResult_ptr_f()->finished_f())
            {
                updateCheckResult_f(checkDataPtrTmp);
            }
            //and connect
            QObject::connect(checkDataPtrTmp->checkDataExecutionResult_ptr_f(), &checkDataExecutionResult_c::executionStateUpdated_signal, this, &actionChecksWindow_c::updateCheckExecutionState_f, Qt::UniqueConnection);
            QObject::connect(checkDataPtrTmp->checkDataExecutionResult_ptr_f(), &checkDataExecutionResult_c::error_signal, this, &actionChecksWindow_c::updateCheckError_f, Qt::UniqueConnection);
            QObject::connect(checkDataPtrTmp->checkDataExecutionResult_ptr_f(), &checkDataExecutionResult_c::finished_signal , this, &actionChecksWindow_c::updateCheckResult_f, Qt::UniqueConnection);
            QObject::connect(checkDataPtrTmp->checkDataExecutionResult_ptr_f(), &checkDataExecutionResult_c::resultsCleared_signal, this, &actionChecksWindow_c::checkResultsCleared_f, Qt::UniqueConnection);
        }
    }
    else
    {
        updateExistingCheckRow_f(checkDataPtrTmp->type_f(), checkDataPtrTmp->description_f(), row_par_con);
    }
}

void actionChecksWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>TODO</p>"
                    )
            , "Tips"
            , this
    );
}

void actionChecksWindow_c::removeChecksButtonClicked_f()
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f("Check removal disallowed while executing checks", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No check rows selected for removal", "Error", this);
            break;
        }

        std::unordered_set<int> rowIndexUSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexUSetTmp.emplace(item_ite_con->row());
        }

        for (const int item_ite_con : rowIndexUSetTmp)
        {
            actionChecksTable_pri->removeRow(item_ite_con);
            checkDataHub_ptr_pri->removeCheckDataUsingRow_f(item_ite_con);
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
            checkData_c* checkDataSourcePtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(selectedRowTmp)));
            checkData_c copyTmp;
            copyTmp.setType_f(checkDataSourcePtrTmp->type_f());
            copyTmp.setDescription_f(checkDataSourcePtrTmp->description_f());
            copyTmp.setCheckDataJSON_f(checkDataSourcePtrTmp->checkDataJSON_f());

            checkDataHub_ptr_pri->insertCheckData_f(copyTmp, newIndexTmp);
            insertCheckRow_f(copyTmp.type_f(), copyTmp.description_f(), newIndexTmp);
            //actionChecksTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f("Wrong index to copy the row: " + QString::number(newIndexTmp), "Error", this);
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
            errorQMessageBox_f("Copy Check disallowed while checks are executing", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No check row selected to copy", "Error", this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f("Check row copy can only be applied to a single row", "Error", this);
            break;
        }

        int selectedRowTmp(selectionTmp.first()->row());

        checkData_c* checkDataSourcePtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(selectedRowTmp)));

        copyCheckIndexInputDialog_pri = new QInputDialog(this);
        copyCheckIndexInputDialog_pri->setWindowTitle("Copy Check");
        copyCheckIndexInputDialog_pri->setLabelText(
                    "Input a new index to copy the check to.\nCheck source:"
                    "\nType: " + checkTypeToStrUMap_glo_sta_con.at(checkDataSourcePtrTmp->type_f()) +
                    "\nDescription: " + checkDataSourcePtrTmp->description_f().left(80) +
                    "\nRow index: " + QString::number(selectedRowTmp)
                    );
        copyCheckIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        copyCheckIndexInputDialog_pri->setIntValue(selectedRowTmp);
        copyCheckIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(copyCheckIndexInputDialog_pri, &QFileDialog::finished, this, &actionChecksWindow_c::inputDialogCopyCheckIndexFinished_f);

        copyCheckIndexInputDialog_pri->show();
        break;
    }
}

void actionChecksWindow_c::updateCheckError_f(checkData_c* const checkData_par_ptr_con)
{
    int rowTmp(checkDataHub_ptr_pri->checkDataIdToRow_f(checkData_par_ptr_con->id_f()));
    //0 check (type) | 1 description | 2 Execution state | 3 Last error | 4 Result
    actionChecksTable_pri->item(rowTmp, 3)->setText(checkData_par_ptr_con->checkDataExecutionResult_ptr_f()->error_f().left(32));
}

void actionChecksWindow_c::updateCheckExecutionState_f(checkData_c* const checkData_par_ptr_con)
{
    int rowTmp(checkDataHub_ptr_pri->checkDataIdToRow_f(checkData_par_ptr_con->id_f()));
    QString checkExecutionStateStrTmp(checkExecutionStateToStrUMap_glo_sta_con.at(checkData_par_ptr_con->checkDataExecutionResult_ptr_f()->lastState_f()));
    //0 check (type) | 1 description | 2 Execution state | 3 Last error | 4 Result
    actionChecksTable_pri->item(rowTmp, 2)->setText(checkExecutionStateStrTmp.left(32));
}

void actionChecksWindow_c::updateCheckResult_f(checkData_c* const checkData_par_ptr_con)
{
    int rowTmp(checkDataHub_ptr_pri->checkDataIdToRow_f(checkData_par_ptr_con->id_f()));
    if (checkData_par_ptr_con->checkDataExecutionResult_ptr_f() not_eq nullptr and checkData_par_ptr_con->checkDataExecutionResult_ptr_f()->finished_f())
    {
        //0 check (type) | 1 description | 2 Execution state | 3 Last error | 4 Result
        actionChecksTable_pri->item(rowTmp, 4)->setText(checkData_par_ptr_con->checkDataExecutionResult_ptr_f()->result_f() ? appConfig_ptr_ext->translate_f("True") : appConfig_ptr_ext->translate_f("False"));
    }
    else
    {
        //0 check (type) | 1 description | 2 Execution state | 3 Last error | 4 Result
        actionChecksTable_pri->item(rowTmp, 4)->setText("");
    }
}

void actionChecksWindow_c::executionFinished_f()
{
    //FUTURE this might need more details like duration, if errors or not, x/y successfull
    statusBarLabel_pri->setText("Check/s execution finished");
    //restore execute button text values
    executeChecksButton_pri->setText("&Execute Checks");
    executeChecksButton_pri->setToolTip("No selection executes all actions, selection executes selected actions");
}

void actionChecksWindow_c::executionStarted_f()
{
    statusBarLabel_pri->setText("Action/s execution started");

    executeChecksButton_pri->setText("Executing... Press again to Stop");
    executeChecksButton_pri->setToolTip("Will stop after finishing already executing check/s");
}

void actionChecksWindow_c::stoppingExecution_f()
{
    statusBarLabel_pri->setText("Trying to stop execution...");
}

void actionChecksWindow_c::checkResultsCleared_f(checkData_c* const checkData_par_ptr_con)
{
    updateCheckResult_f(checkData_par_ptr_con);
    updateCheckError_f(checkData_par_ptr_con);
    updateCheckExecutionState_f(checkData_par_ptr_con);
}

void actionChecksWindow_c::createMessageBoxAskAboutExecutingChecksOnClose_f()
{
    askAboutExecutingChecksOnCloseMessageBox_pri = new QMessageBox(this);
    askAboutExecutingChecksOnCloseMessageBox_pri->setText(R"(Check/s are executing. What to do?)");
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
        connect(checkDataHub_ptr_pri->proxyQObj_f(), &checksDataHubProxyQObj_c::checksExecutionFinished_signal, this, &actionChecksWindow_c::close);
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

            checkData_c* checkDataPtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkIdToRunTmp));
#ifdef DEBUGJOUVEN
            //qDebug() << "checkIdToRunTmp " << checkIdToRunTmp << endl;
            //qDebug() << "checkDataPtrTmp is? " << (checkDataPtrTmp == nullptr ? "null" : "not null") << endl;
#endif
            if (checkDataPtrTmp not_eq nullptr)
            {
                //it's not tracked if the executions results are already connected... so... delete the results object each time and connect,
                //otherwise repeated connections would happen, to solve this "issue" would require a map of which actions are connected and which aren't
                //also reconnect or undo the initial connection if the check types changes
                checkDataExecutionResult_c* checkExecutionResultPtr = checkDataPtrTmp->createGetCheckDataExecutionResult_ptr_f();
                //FUTURE add warning when running an action again if there are results already?
                QObject::connect(checkExecutionResultPtr, &checkDataExecutionResult_c::executionStateUpdated_signal, this, &actionChecksWindow_c::updateCheckExecutionState_f, Qt::UniqueConnection);
                QObject::connect(checkExecutionResultPtr, &checkDataExecutionResult_c::error_signal, this, &actionChecksWindow_c::updateCheckError_f, Qt::UniqueConnection);
                QObject::connect(checkExecutionResultPtr, &checkDataExecutionResult_c::finished_signal , this, &actionChecksWindow_c::updateCheckResult_f, Qt::UniqueConnection);
                QObject::connect(checkExecutionResultPtr, &checkDataExecutionResult_c::resultsCleared_signal, this, &actionChecksWindow_c::checkResultsCleared_f, Qt::UniqueConnection);

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
            executeChecksButton_pri->setText("Stopping...");
            checkDataHub_ptr_pri->stopExecutingChecks_f();
            break;

        }

        executeChecks_f();

        break;
    }
}

void actionChecksWindow_c::showExecutionDetailsButtonClicked_f()
{
    while (true)
    {
        checkDataExecutionResult_c* checkDataExecutionResultTmp_ptr(nullptr);
        if (actionChecksTable_pri->selectedItems().isEmpty())
        {
            errorQMessageBox_f("No check row selected", "Error", this);
            break;
        }

        int rowTmp(actionChecksTable_pri->selectedItems().first()->row());
        int_fast32_t checkDataIdTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(rowTmp));
        checkData_c* checkDataPtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkDataIdTmp));
        if (checkDataPtrTmp not_eq nullptr)
        {
            if (checkDataPtrTmp->checkDataExecutionResult_ptr_f() == nullptr)
            {
                errorQMessageBox_f("Check has no execution results", "Error", this);
                break;
            }
            else
            {
                checkDataExecutionResultTmp_ptr = checkDataPtrTmp->checkDataExecutionResult_ptr_f();
            }
        }
        else
        {
            //this shouldn't happen (see the "same" code in mainWindow for an explanation)
        }

        checkExecutionDetailsWindow_c* checkExecutionDetailsWindowTmp = new checkExecutionDetailsWindow_c(checkDataExecutionResultTmp_ptr, this);
        //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
        checkExecutionDetailsWindowTmp->setWindowFlag(Qt::Window, true);
        checkExecutionDetailsWindowTmp->setWindowModality(Qt::WindowModal);
        checkExecutionDetailsWindowTmp->show();
        break;
    }
}

void actionChecksWindow_c::moveSelectedChecks_f(const int moveOffSet_par_con)
{
    while (true)
    {
        if (checkDataHub_ptr_pri->executingChecks_f())
        {
            errorQMessageBox_f("Check moving disallowed while executing checks", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionChecksTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("Empty selection to move", "Error", this);
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
            std::sort(rowIndexVectorTmp.begin(), rowIndexVectorTmp.end(), std::less<int>());
        }
        //if the offset moves them "down" start from the higher indexes
        else
        {
            std::sort(rowIndexVectorTmp.begin(), rowIndexVectorTmp.end(), std::greater<int>());
        }

        for (const int index_ite_con : rowIndexVectorTmp)
        {
            auto destinationRow(index_ite_con + moveOffSet_par_con);
            //check if the destination index is out of bounds
            if (destinationRow >= 0 and destinationRow < actionChecksTable_pri->rowCount())
            {
                checkDataHub_ptr_pri->moveRowCheckData_f(index_ite_con, destinationRow);
                //change the grid (visual)
                actionChecksTable_pri->removeRow(index_ite_con);
                checkData_c* actionDataPtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(destinationRow)));
                insertCheckRow_f(actionDataPtrTmp->type_f(), actionDataPtrTmp->description_f(), destinationRow);
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
            checkDataHub_ptr_pri->moveRowCheckData_f(currentIndexTmp, newIndexTmp);
            //change the grid (visual)
            checkData_c* checkDataPtrTmp(checkDataHub_ptr_pri->checkData_ptr_f(checkDataHub_ptr_pri->rowToCheckDataId_f(newIndexTmp)));
            actionChecksTable_pri->removeRow(currentIndexTmp);
            insertCheckRow_f(checkDataPtrTmp->type_f(), checkDataPtrTmp->description_f(), newIndexTmp);
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f("Same or wrong index to move the row: " + QString::number(newIndexTmp), "Error", this);
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
            errorQMessageBox_f("Check index change disallowed while executing actions", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp(actionChecksTable_pri->selectedItems());
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No check row selected to change row index", "Error", this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f("Check row index change can only be applied to a single row", "Error", this);
            break;
        }

        int currentIndexTmp(selectionTmp.first()->row());

        changeCheckIndexInputDialog_pri = new QInputDialog(this);
        changeCheckIndexInputDialog_pri->setWindowTitle("Change Check index");
        changeCheckIndexInputDialog_pri->setLabelText("Input a new index to move the check to (current index: " + QString::number(currentIndexTmp) + ")");
        changeCheckIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        changeCheckIndexInputDialog_pri->setIntValue(currentIndexTmp);
        changeCheckIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(changeCheckIndexInputDialog_pri, &QFileDialog::finished, this, &actionChecksWindow_c::inputDialogChangeCheckIndexFinished_f);

        changeCheckIndexInputDialog_pri->show();
        break;
    }
}


