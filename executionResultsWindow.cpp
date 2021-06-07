#include "executionResultsWindow.hpp"

#include "mainWindow.hpp"
#include "appConfig.hpp"
#include "executionResultWindow.hpp"
#include "stringFormatting.hpp"

#include "../logsWindowWidgets/QDatetimeEditDelegate.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/executionResult.hpp"
#include "actonQtso/actionDataExecutionResult.hpp"
#include "actonQtso/checkDataExecutionResult.hpp"
#include "actonQtso/dateTimeStringFormat.hpp"
#include "actonDataHubGlobal.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QTableWidget>

#include <set>

void executionResultsWindow_c::closeEvent(QCloseEvent* event)
{
#ifdef DEBUGJOUVEN
    //qtOutRef_ext() << "Close log window" << endl;
#endif
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + executionResultsTable_pri->objectName() + executionResultsTable_pri->horizontalHeader()->objectName(), executionResultsTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + executionResultsTable_pri->objectName(), executionResultsTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

void executionResultsWindow_c::keyPressEvent(QKeyEvent* event_par)
{
    if (executionResultsTable_pri->hasFocus() and event_par->matches(QKeySequence::Copy))
    {
        QList<QTableWidgetItem*> selectedItemsTmp(executionResultsTable_pri->selectedItems());
        if (selectedItemsTmp.size() > 1)
        {
            std::set<int> selectedRowsTmp;
            for (const QTableWidgetItem* cell_ite_con : selectedItemsTmp)
            {
               selectedRowsTmp.emplace(cell_ite_con->row());
            }
            //FUTURE now the selection is per row, but if it was per cell, just get the rows, order them like here,
            //and then iterate the SELECTED cells from left to right, iterate the columns checking selected
            //setWindowTitle(appConfig_ptr_ext->translate_f("Logs - items selected " + QString::number(selectedItemsTmp.size())));
            QString textToClipBoardTmp;
            for (const int row_ite_con : selectedRowsTmp)
            {
                 textToClipBoardTmp
                 .append(executionResultsTable_pri->item(row_ite_con, 0)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 1)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 2)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 3)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 4)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 5)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 6)->text() + "\t")
                 //.append(executionResultsTable_pri->item(row_ite_con, 7)->text() + "\t")
                 .append(executionResultsTable_pri->item(row_ite_con, 7)->text() + "\n")
                 .append(executionResultsTable_pri->item(row_ite_con, 8)->text() + "\n")
                 ;
            }
            //remove the last \n
            textToClipBoardTmp.chop(1);
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(textToClipBoardTmp);
        }
        else
        {
            //normal event woks alright for single cell copy
        }
    }
    //qwidgets events default implementation is to ignore
    //but when reimplementing if no accept or ignore is called, accept is the default
    //accept, "accepts" the event and stops event propagation
    //event_par->accept();
    //event_par->ignore();
}

void executionResultsWindow_c::addExecutionResultEntry_f(const executionResult_c* executionResult_par_con)
{
    while (true)
    {
        if (not actionsToFilter_pri.empty())
        {
            bool filterMatchTmp(false);
            if (executionResult_par_con->type_f() == executionResult_c::type_ec::action)
            {
                const actionExecutionResult_c* actionExecutionResultTmp(static_cast<const actionExecutionResult_c*>(executionResult_par_con));
                for (action_c* action_ite : actionsToFilter_pri)
                {
                    if (actionExecutionResultTmp->action_ptr_f() == action_ite)
                    {
                        filterMatchTmp = true;
                        break;
                    }
                }
            }
            //allow checks if their parent action matches
            if (executionResult_par_con->type_f() == executionResult_c::type_ec::check)
            {
                const checkExecutionResult_c* actionExecutionResultTmp(static_cast<const checkExecutionResult_c*>(executionResult_par_con));
                for (action_c* action_ite : actionsToFilter_pri)
                {
                    if (actionExecutionResultTmp->check_ptr_f()->parentAction_f() == action_ite)
                    {
                        filterMatchTmp = true;
                        break;
                    }
                }
            }
            if (not filterMatchTmp)
            {
                break;
            }
        }

        if ((not checksToFilter_pri.empty()) and (executionResult_par_con->type_f() == executionResult_c::type_ec::check))
        {
            bool filterMatchTmp(false);
            const checkExecutionResult_c* checkExecutionResultTmp(static_cast<const checkExecutionResult_c*>(executionResult_par_con));
            for (check_c* check_ite : checksToFilter_pri)
            {
                if (checkExecutionResultTmp->check_ptr_f() == check_ite)
                {
                    filterMatchTmp = true;
                    break;
                }
            }
            if (not filterMatchTmp)
            {
                break;
            }
        }

#ifdef DEBUGJOUVEN
//    qtOutRef_ext() << "index_par_con " << index_par_con << "\n"
//    //<< "indexTmp " << indexTmp << "\n"
//    << "logsTable_pri->rowCount() " << logsTable_pri->rowCount() << "\n"
//    << "message " << logItem_par_con->message_f()
//    << endl;
#endif

        int newRowIndexTmp(executionResultsTable_pri->rowCount());
        executionResultsTable_pri->insertRow(newRowIndexTmp);
        //QTableWidgetItem *indexCellTmp(new QTableWidgetItem(QString::number(indexTmp)));
        QTableWidgetItem* idCellTmp(new QTableWidgetItem);
        //Note: The default implementation treats Qt::EditRole and Qt::DisplayRole as referring to the same data.
        idCellTmp->setData(Qt::DisplayRole, QVariant::fromValue(executionResult_par_con->id_f()));
        idCellTmp->setFlags(idCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* typeCellTmp(new QTableWidgetItem);
        typeCellTmp->setData(Qt::DisplayRole, executionResult_c::executionResultTypeToString_f(executionResult_par_con->type_f()));
        typeCellTmp->setFlags(typeCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* derivedElementTypeCellTmp(new QTableWidgetItem);
        derivedElementTypeCellTmp->setData(Qt::DisplayRole, executionResult_par_con->derivedElementTypeString_f());
        derivedElementTypeCellTmp->setFlags(derivedElementTypeCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* derivedElementDescriptionCellTmp(new QTableWidgetItem);
        derivedElementDescriptionCellTmp->setData(Qt::DisplayRole, executionResult_par_con->derivedElementDescription_f());
        derivedElementDescriptionCellTmp->setFlags(typeCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* stateCellTmp(new QTableWidgetItem);
        stateCellTmp->setData(Qt::DisplayRole, appConfig_ptr_ext->translateAndReplace_f(executionResult_par_con->stateString_f()));
        stateCellTmp->setFlags(stateCellTmp->flags() bitand compl Qt::ItemIsEditable);
        //messageCellTmp->setToolTip(appConfig_ptr_ext->translateAndReplace_f(logItem_par_con->stateString_f()));

        QTableWidgetItem* startedCellTmp(new QTableWidgetItem);
        startedCellTmp->setData(Qt::DisplayRole, QDateTime::fromMSecsSinceEpoch(executionResult_par_con->startTime_f()).toLocalTime().toString(dateTimeFormat_con));
        startedCellTmp->setFlags(startedCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* finishedCellTmp(new QTableWidgetItem);
        finishedCellTmp->setData(Qt::DisplayRole, QDateTime::fromMSecsSinceEpoch(executionResult_par_con->finishedTime_f()).toLocalTime().toString(dateTimeFormat_con));
        finishedCellTmp->setFlags(finishedCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* lastMessageCellTmp(new QTableWidgetItem);
        executionMessage_c* lastMessagePtrTmp(executionResult_par_con->lastMessage_f());
        if (lastMessagePtrTmp not_eq nullptr)
        {
            QString lastMessageStringTmp(appConfig_ptr_ext->translateAndReplace_f(lastMessagePtrTmp->text_f()));
            lastMessageCellTmp->setData(Qt::DisplayRole, truncateString_f(lastMessageStringTmp, 32));
        }
        lastMessageCellTmp->setFlags(lastMessageCellTmp->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem* messageCountCellTmp(new QTableWidgetItem);
        messageCountCellTmp->setData(Qt::DisplayRole, QVariant::fromValue(executionResult_par_con->messageCount_f()));
        messageCountCellTmp->setFlags(messageCountCellTmp->flags() bitand compl Qt::ItemIsEditable);

        executionResultsTable_pri->setItem(newRowIndexTmp, 0, idCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 1, typeCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 2, derivedElementTypeCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 3, derivedElementDescriptionCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 4, stateCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 5, startedCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 6, finishedCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 7, lastMessageCellTmp);
        executionResultsTable_pri->setItem(newRowIndexTmp, 8, messageCountCellTmp);
#ifdef DEBUGJOUVEN
        //    qtOutRef_ext() << "Log entry added, index " << index_par << endl;
        //    qtOutRef_ext() << "Log entry added, message_f " << logItem_par->message_f() << endl;
        //    qtOutRef_ext() << "Log entry added, datetime " << logDateTime_par->toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << endl;
#endif
        QObject::connect(executionResult_par_con, &executionResult_c::anyChange_signal, this, &executionResultsWindow_c::updateTableData_f);

        break;
    }
}

void executionResultsWindow_c::addSingleExecutionResultEntry_f(const executionResult_c* executionResult_par_con)
{
    executionResultsTable_pri->setSortingEnabled(false);
    addExecutionResultEntry_f(executionResult_par_con);
    executionResultsTable_pri->setSortingEnabled(true);
    executionResultsTable_pri->scrollToBottom();
}

void executionResultsWindow_c::updateTableData_f(const executionResult_c* executionResultItem_par_con)
{
    for (int rowIndex_ite = 0; rowIndex_ite < executionResultsTable_pri->rowCount(); ++rowIndex_ite)
    {
        if (executionResultsTable_pri->item(rowIndex_ite, 0)->data(Qt::DisplayRole).toULongLong() == executionResultItem_par_con->id_f())
        {
            executionResultsTable_pri->item(rowIndex_ite, 4)->setData(Qt::DisplayRole, appConfig_ptr_ext->translateAndReplace_f(executionResultItem_par_con->stateString_f()));
            executionResultsTable_pri->item(rowIndex_ite, 5)->setData(Qt::DisplayRole, QDateTime::fromMSecsSinceEpoch(executionResultItem_par_con->startTime_f()).toLocalTime().toString(dateTimeFormat_con));
            executionResultsTable_pri->item(rowIndex_ite, 6)->setData(Qt::DisplayRole, QDateTime::fromMSecsSinceEpoch(executionResultItem_par_con->finishedTime_f()).toLocalTime().toString(dateTimeFormat_con));
            executionMessage_c* lastMessagePtrTmp(executionResultItem_par_con->lastMessage_f());
            if (lastMessagePtrTmp not_eq nullptr)
            {
                executionResultsTable_pri->item(rowIndex_ite, 7)->setData(Qt::DisplayRole, appConfig_ptr_ext->translateAndReplace_f(executionResultItem_par_con->lastMessage_f()->text_f()));
            }
            executionResultsTable_pri->item(rowIndex_ite, 8)->setData(Qt::DisplayRole, QVariant::fromValue(executionResultItem_par_con->messageCount_f()));
            break;
        }
    }
}

void executionResultsWindow_c::viewExecutionResultButtonClicked_f()
{
    while (true)
    {
        //when clicking "viewExecutionResultButtonClicked_f" it can go:
        //1 nothing is selected show all the execution messages (no executionResult info)
        //2 multiselect, same as above but narrow
        //3 single row
        //  the row is from an action execution result, show it's execution result details and the execution messages from that execution results
        //  the row is from a check execution result, show it's execution result details and the execution messages from that execution results
        //4 actonDataHub_ptr_ext->executionResults_f() is empty, can't happen because this window would not open to begin with
        //5 no execution messages, this applies to 1 and 2, show an error

        if (actonDataHub_ptr_ext->executionResults_f().empty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No executions results have been generated")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        QList<QTableWidgetItem *> selectionTmp(executionResultsTable_pri->selectedItems());
        //1 nothing is selected show all the execution messages (no executionResult info)
        if (selectionTmp.isEmpty())
        {
            executionResultWindow_c* allExecutionResultsWindow(new executionResultWindow_c(std::vector<executionResult_c*>(), this));
            allExecutionResultsWindow->show_f();
            break;
        }

        std::set<size_t> rowIndexSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexSetTmp.emplace(item_ite_con->row());
        }

        //only one 1 row selected
        if (rowIndexSetTmp.size() == 1)
        {
            size_t indexTmp(*(rowIndexSetTmp.begin()));
            if (indexTmp > actonDataHub_ptr_ext->executionResults_f().size())
            {
                errorQMessageBox_f(
                            appConfig_ptr_ext->translate_f("Invalid execution result index")
                            , appConfig_ptr_ext->translate_f("Error")
                            , this);
                break;
            }
            executionResult_c* executionResultSelected(actonDataHub_ptr_ext->executionResults_f().at(indexTmp));
            executionResultWindow_c* allExecutionResultsWindow(new executionResultWindow_c({executionResultSelected}, this));
            allExecutionResultsWindow->show_f();
            break;
        }

        std::vector<executionResult_c*> executionResultsSelected;
        executionResultsSelected.reserve(rowIndexSetTmp.size());
        for (const auto index_ite_con : rowIndexSetTmp)
        {
            if (index_ite_con > actonDataHub_ptr_ext->executionResults_f().size())
            {
                errorQMessageBox_f(
                            appConfig_ptr_ext->translate_f("Invalid execution result index")
                            , appConfig_ptr_ext->translate_f("Error")
                            , this);
                break;
            }
            else
            {
                executionResult_c* executionResultSelected(actonDataHub_ptr_ext->executionResults_f().at(index_ite_con));
                executionResultsSelected.emplace_back(executionResultSelected);
            }
        }

        executionResultWindow_c* allExecutionResultsWindow(new executionResultWindow_c(executionResultsSelected, this));
        allExecutionResultsWindow->show_f();
        break;
    }
}

void executionResultsWindow_c::loadExecutionResults_f()
{
    executionResultsTable_pri->setSortingEnabled(false);
    for (const executionResult_c* const executionResultPtr_ite : actonDataHub_ptr_ext->executionResults_f())
    {
        addExecutionResultEntry_f(executionResultPtr_ite);
    }
    executionResultsTable_pri->setSortingEnabled(true);
    executionResultsTable_pri->scrollToBottom();
}

executionResultsWindow_c::executionResultsWindow_c()
    : QWidget(mainWindow_ptr_ext)
{
    this->setObjectName("executionResultsWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);
}
executionResultsWindow_c::executionResultsWindow_c(const std::vector<action_c*>& actionsToFilter_par)
    : actionsToFilter_pri(actionsToFilter_par)
{
    executionResultsWindow_c();
}
executionResultsWindow_c::executionResultsWindow_c(const std::vector<check_c*>& checksToFilter_par)
    : checksToFilter_pri(checksToFilter_par)
{
    executionResultsWindow_c();
}

void executionResultsWindow_c::show_f()
{
    connect(mainWindow_ptr_ext, &mainWindow_c::closeWindow_signal, this, &QWidget::close);

    //buttons, view result details
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //view execution result details (and execution messages)
    QPushButton* viewExecutionResultDetailsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("View execution result details"));
    firstRowLayoutTmp->addWidget(viewExecutionResultDetailsButtonTmp);
    QObject::connect(viewExecutionResultDetailsButtonTmp, &QPushButton::clicked, this, &executionResultsWindow_c::viewExecutionResultButtonClicked_f);

    QHBoxLayout* tableRowLayoutTmp = new QHBoxLayout;

    executionResultsTable_pri = new QTableWidget(0, 9);
    executionResultsTable_pri->verticalHeader()->setVisible(false);
    executionResultsTable_pri->setObjectName("QTableWidget_");
    executionResultsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labelsTmp({
            appConfig_ptr_ext->translate_f("Id") //execution result Id
            , appConfig_ptr_ext->translate_f("Type") //action or check
            , appConfig_ptr_ext->translate_f("SubType") //action/check type
            , appConfig_ptr_ext->translate_f("Description") //check/action description
            , appConfig_ptr_ext->translate_f("State") //execution state
            , appConfig_ptr_ext->translate_f("Started") //datetime
            , appConfig_ptr_ext->translate_f("Finished") //datetime
            , appConfig_ptr_ext->translate_f("Last Message") //last message text
            , appConfig_ptr_ext->translate_f("Message count")
    });
    executionResultsTable_pri->setHorizontalHeaderLabels(labelsTmp);
    executionResultsTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    executionResultsTable_pri->setShowGrid(true);
    executionResultsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionResultsTable_pri->sortItems(0);
    executionResultsTable_pri->setSortingEnabled(true);

    QDateTimeStyledDelegate_c* dateTimeCellDelegate(new QDateTimeStyledDelegate_c(this));
    executionResultsTable_pri->setItemDelegateForColumn(5, dateTimeCellDelegate);
    executionResultsTable_pri->setItemDelegateForColumn(6, dateTimeCellDelegate);

#ifdef __ANDROID__
    logsTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif

    tableRowLayoutTmp->addWidget(executionResultsTable_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* closeButtonTmp = new QPushButton("&Close");
    lastRowLayoutTmp->addWidget(closeButtonTmp);
    connect(closeButtonTmp, &QPushButton::clicked, this, &executionResultsWindow_c::closeButtonClicked_f);

//    QPushButton* tipsButtonTmp = new QPushButton("Tips");
//    lastRowLayoutTmp->addWidget(tipsButtonTmp);
//    connect(tipsButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::tipsButtonClicked_f);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(tableRowLayoutTmp);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
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

    setWindowTitle(appConfig_ptr_ext->translate_f("Execution results"));

    loadExecutionResults_f();

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         executionResultsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + executionResultsTable_pri->objectName()));
         executionResultsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + executionResultsTable_pri->objectName() + executionResultsTable_pri->horizontalHeader()->objectName()));
    }

    connect(actonDataHub_ptr_ext, &actonDataHub_c::executionResultAdded_signal, this, &executionResultsWindow_c::addSingleExecutionResultEntry_f);

    show();
}

void executionResultsWindow_c::closeButtonClicked_f()
{
    close();
}
//TODO start commiting stuff

//no need for now
//void optionsWindow_c::tipsButtonClicked_f()
//{
//
//}

