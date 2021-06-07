#include "fileListWindow.hpp"

#include "../../mainWindow.hpp"
#include "../../appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "sizeConversionso/byte.hpp"

#include <QtWidgets>
#include <QTableWidget>
#include <QFileInfo>

void fileListWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + fileListTable_pri->objectName() + fileListTable_pri->horizontalHeader()->objectName(), fileListTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + fileListTable_pri->objectName(), fileListTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

void fileListWindow_c::addFileEntry_f(
        const QString& filename_par_con
        , const QString& lastModDatetimeStr_par_con
        , const uint_fast64_t size_par_con
)
{
    int indexTmp(fileListTable_pri->rowCount());
    fileListTable_pri->insertRow(indexTmp);
    QTableWidgetItem* nameCellTmp(new QTableWidgetItem);
    nameCellTmp->setData(Qt::DisplayRole, filename_par_con);
    nameCellTmp->setFlags(nameCellTmp->flags() bitand compl Qt::ItemIsEditable);
    nameCellTmp->setToolTip(filename_par_con);

    QTableWidgetItem* sizeCellTmp(new QTableWidgetItem);
    QString sizeStrTmp(QString::fromStdString(formatByteSizeValue_f(size_par_con)));
    sizeCellTmp->setData(Qt::DisplayRole, sizeStrTmp);
    sizeCellTmp->setFlags(sizeCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem* sizeBytesCellTmp(new QTableWidgetItem);
    sizeBytesCellTmp->setData(Qt::DisplayRole, QVariant::fromValue(size_par_con));
    sizeBytesCellTmp->setFlags(sizeBytesCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem* lastModDatetimeCellTmp(new QTableWidgetItem);
    lastModDatetimeCellTmp->setData(Qt::DisplayRole, lastModDatetimeStr_par_con);
    lastModDatetimeCellTmp->setFlags(lastModDatetimeCellTmp->flags() bitand compl Qt::ItemIsEditable);

//    QTableWidgetItem *dateTimeCellTmp(new QTableWidgetItem(logDateTime_par_con->toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
//    dateTimeCellTmp->setFlags(dateTimeCellTmp->flags() bitand compl Qt::ItemIsEditable);

//    QTableWidgetItem *fileCellTmp(new QTableWidgetItem(logItem_par_con->sourceFileName_f()));
//    fileCellTmp->setFlags(fileCellTmp->flags() bitand compl Qt::ItemIsEditable);

//    QTableWidgetItem *functionCellTmp(new QTableWidgetItem(logItem_par_con->sourceFunctionName_f()));
//    functionCellTmp->setFlags(functionCellTmp->flags() bitand compl Qt::ItemIsEditable);

//    QTableWidgetItem *lineNumberCellTmp(new QTableWidgetItem(QString::number(logItem_par_con->sourceLineNumber_f())));
//    lineNumberCellTmp->setFlags(lineNumberCellTmp->flags() bitand compl Qt::ItemIsEditable);

    fileListTable_pri->setItem(indexTmp, 0, nameCellTmp);
    fileListTable_pri->setItem(indexTmp, 1, sizeCellTmp);
    fileListTable_pri->setItem(indexTmp, 2, sizeBytesCellTmp);
    fileListTable_pri->setItem(indexTmp, 3, lastModDatetimeCellTmp);
//#ifdef DEBUGJOUVEN
////    qtOutRef_ext() << "Log entry added, index " << index_par << endl;
////    qtOutRef_ext() << "Log entry added, message_f " << logItem_par->message_f() << endl;
////    qtOutRef_ext() << "Log entry added, datetime " << logDateTime_par->toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << endl;
//#endif

}

//void fileListWindow_c::loadLogs_f()
//{
//    std::vector<std::pair<const logItem_c* const, const QDateTime* const>> logsTmp(appConfig_ptr_ext->getLogs_f());

//    int indexTmp(0);
//    for (const std::pair<const logItem_c* const, const QDateTime* const>& logItem_ite_con : logsTmp)
//    {
//        addLoadLogEntry_f(indexTmp, logItem_ite_con.first, logItem_ite_con.second);
//        indexTmp = indexTmp + 1;
//    }
//}

void fileListWindow_c::loadFileVectorToTableGrid_f(const std::vector<QString>& fileVector_par_con)
{
    fileListTable_pri->setSortingEnabled(false);
    fileListTable_pri->setUpdatesEnabled(false);
    for (const QString& filename_ite_con : fileVector_par_con)
    {
        QFileInfo fileInfoTmp(filename_ite_con);
        QString datetimeModStrTmp(fileInfoTmp.lastModified().toString("yyyy-MM-dd hh:mm:ss.zzz"));

        addFileEntry_f(filename_ite_con, datetimeModStrTmp, fileInfoTmp.size());
    }
    fileListTable_pri->setUpdatesEnabled(true);
    fileListTable_pri->setSortingEnabled(true);
}

fileListWindow_c::fileListWindow_c(
        const std::vector<QString>& fileVector_par_con
        , QWidget* parent_par
)
    : QWidget(parent_par)
{
    this->setObjectName("fileListWindow_");

    //connect(mainWindow_ptr_ext, &mainWindow_c::closeWindow_signal, this, &QWidget::close);

    //FUTURE filters
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    QHBoxLayout* tableRowLayoutTmp = new QHBoxLayout;

    fileListTable_pri = new QTableWidget(0, 4);
    fileListTable_pri->verticalHeader()->setVisible(false);
    fileListTable_pri->setObjectName("QTableWidget_");
    fileListTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labelsTmp({
            appConfig_ptr_ext->translate_f("Filename")
            , appConfig_ptr_ext->translate_f("Size")
            //with this one it's easy to sort by size
            , appConfig_ptr_ext->translate_f("Size bytes")
            , appConfig_ptr_ext->translate_f("Modification datetime")
    });
    fileListTable_pri->setHorizontalHeaderLabels(labelsTmp);
    fileListTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    fileListTable_pri->setShowGrid(true);
    fileListTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    logsTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif

    tableRowLayoutTmp->addWidget(fileListTable_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &fileListWindow_c::cancelButtonClicked_f);

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

    loadFileVectorToTableGrid_f(fileVector_par_con);

    setWindowTitle(appConfig_ptr_ext->translateAndReplace_f({"File list ({0} items)", fileVector_par_con.size()}));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         fileListTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + fileListTable_pri->objectName()));
         fileListTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + fileListTable_pri->objectName() + fileListTable_pri->horizontalHeader()->objectName()));
    }

}

void fileListWindow_c::cancelButtonClicked_f()
{
    close();
}


//no need for now
//void optionsWindow_c::tipsButtonClicked_f()
//{
//
//}

