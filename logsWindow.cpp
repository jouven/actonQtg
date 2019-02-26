#include "logsWindow.hpp"

#include "mainWindow.hpp"
#include "appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "logsinJSONQtso/logItemStrMapping.hpp"

#include <QtWidgets>
#include <QTableWidget>

#include <set>

void logsWindow_c::closeEvent(QCloseEvent* event)
{
#ifdef DEBUGJOUVEN
    qtOutRef_ext() << "Close log window" << endl;
#endif
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + logsTable_pri->objectName() + logsTable_pri->horizontalHeader()->objectName(), logsTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + logsTable_pri->objectName(), logsTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

void logsWindow_c::keyPressEvent(QKeyEvent* event_par)
{
    if (logsTable_pri->hasFocus() and event_par->matches(QKeySequence::Copy))
    {
        QList<QTableWidgetItem*> selectedItemsTmp(logsTable_pri->selectedItems());
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
                 .append(logsTable_pri->item(row_ite_con, 0)->text() + "\t")
                 .append(logsTable_pri->item(row_ite_con, 1)->text() + "\t")
                 .append(logsTable_pri->item(row_ite_con, 2)->text() + "\t")
                 .append(logsTable_pri->item(row_ite_con, 3)->text() + "\t")
                 .append(logsTable_pri->item(row_ite_con, 4)->text() + "\t")
                 .append(logsTable_pri->item(row_ite_con, 5)->text() + "\t")
                 .append(logsTable_pri->item(row_ite_con, 6)->text() + "\n")
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

void logsWindow_c::addLogEntry_f(const int index_par_con, const logItem_c* const logItem_par_con, const QDateTime* const logDateTime_par_con)
{
    int indexTmp(index_par_con);
#ifdef DEBUGJOUVEN
    //qtOutRef_ext() << "index_par_con " << index_par_con << endl;
    //qtOutRef_ext() << "logsTable_pri->rowCount() " << logsTable_pri->rowCount() << endl;
#endif
    if (indexTmp > logsTable_pri->rowCount())
    {
        indexTmp = logsTable_pri->rowCount();
    }

    logsTable_pri->insertRow(indexTmp);
    QTableWidgetItem *indexCellTmp(new QTableWidgetItem(QString::number(indexTmp)));
    indexCellTmp->setFlags(indexCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *typeCellTmp(new QTableWidgetItem(logTypeToStrUMap_glo_sta_con.at(logItem_par_con->type_f())));
    typeCellTmp->setFlags(typeCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *messageCellTmp(new QTableWidgetItem(logItem_par_con->message_f()));
    messageCellTmp->setFlags(messageCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *dateTimeCellTmp(new QTableWidgetItem(logDateTime_par_con->toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
    dateTimeCellTmp->setFlags(dateTimeCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *fileCellTmp(new QTableWidgetItem(logItem_par_con->sourceFileName_f()));
    fileCellTmp->setFlags(fileCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *functionCellTmp(new QTableWidgetItem(logItem_par_con->sourceFunctionName_f()));
    functionCellTmp->setFlags(functionCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *lineNumberCellTmp(new QTableWidgetItem(QString::number(logItem_par_con->sourceLineNumber_f())));
    lineNumberCellTmp->setFlags(lineNumberCellTmp->flags() bitand compl Qt::ItemIsEditable);

    logsTable_pri->setItem(indexTmp, 0, indexCellTmp);
    logsTable_pri->setItem(indexTmp, 1, typeCellTmp);
    logsTable_pri->setItem(indexTmp, 2, messageCellTmp);
    logsTable_pri->setItem(indexTmp, 3, dateTimeCellTmp);
    logsTable_pri->setItem(indexTmp, 4, fileCellTmp);
    logsTable_pri->setItem(indexTmp, 5, functionCellTmp);
    logsTable_pri->setItem(indexTmp, 6, lineNumberCellTmp);
#ifdef DEBUGJOUVEN
//    qtOutRef_ext() << "Log entry added, index " << index_par << endl;
//    qtOutRef_ext() << "Log entry added, message_f " << logItem_par->message_f() << endl;
//    qtOutRef_ext() << "Log entry added, datetime " << logDateTime_par->toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << endl;
#endif

}

void logsWindow_c::addSingleLogEntry_f(const int index_par_con, const logItem_c* const logItem_par_con, const QDateTime* const logDateTime_par_con)
{
    addLogEntry_f(index_par_con, logItem_par_con, logDateTime_par_con);
    logsTable_pri->scrollToBottom();
}

void logsWindow_c::loadLogs_f()
{
    std::vector<std::pair<const logItem_c* const, const QDateTime* const>> logsTmp(appConfig_ptr_ext->getLogs_f());

    int indexTmp(0);
    for (const std::pair<const logItem_c* const, const QDateTime* const>& logItem_ite_con : logsTmp)
    {
        addLogEntry_f(indexTmp, logItem_ite_con.first, logItem_ite_con.second);
        indexTmp = indexTmp + 1;
    }
    logsTable_pri->scrollToBottom();
}

logsWindow_c::logsWindow_c()
    //: QWidget(mainWindow_ptr_ext)
{
    this->setObjectName("logsWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(mainWindow_ptr_ext, &mainWindow_c::close_signal, this, &QWidget::close);

    //FUTURE filters
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    QHBoxLayout* tableRowLayoutTmp = new QHBoxLayout;

    logsTable_pri = new QTableWidget(0, 7);
    logsTable_pri->verticalHeader()->setVisible(false);
    logsTable_pri->setObjectName("QTableWidget_");
    logsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labelsTmp({
            appConfig_ptr_ext->translate_f("Index")
            , appConfig_ptr_ext->translate_f("Type")
            , appConfig_ptr_ext->translate_f("Message")
            , appConfig_ptr_ext->translate_f("DateTime")
            , appConfig_ptr_ext->translate_f("File")
            , appConfig_ptr_ext->translate_f("Function")
            , appConfig_ptr_ext->translate_f("Line")
    });
    logsTable_pri->setHorizontalHeaderLabels(labelsTmp);
    logsTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    logsTable_pri->setShowGrid(true);
    logsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    logsTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif

    tableRowLayoutTmp->addWidget(logsTable_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &logsWindow_c::cancelButtonClicked_f);

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

    setWindowTitle(appConfig_ptr_ext->translate_f("Logs"));

    loadLogs_f();

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         logsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + logsTable_pri->objectName()));
         logsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + logsTable_pri->objectName() + logsTable_pri->horizontalHeader()->objectName()));
    }

    connect(appConfig_ptr_ext->logDataHub_f(), &logDataHub_c::messageAdded_signal, this, &logsWindow_c::addSingleLogEntry_f);
}

void logsWindow_c::cancelButtonClicked_f()
{
    close();
}

//no need for now
//void optionsWindow_c::tipsButtonClicked_f()
//{
//
//}

