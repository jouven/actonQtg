#include "runProcessWidgets.hpp"

#include "runProcessExtra/argumentWindow.hpp"
#include "runProcessExtra/environmentPairWindow.hpp"

#include "optionsWidgets/environmentWindow.hpp"
#include "optionsWidgets/workingDirectoryWindow.hpp"

#include "appConfig.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QJsonObject>
#include <QSplitter>
#include <QFileDialog>
#include <QDir>

#include <unordered_set>

void runProcessWidgets_c::parentClosing_f()
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
}

void runProcessWidgets_c::save_f()
{
    while (true)
    {
        QString processPathStr(processPathPTE_pri->toPlainText());
        if (processPathStr.isEmpty())
        {
            errorQMessageBox_f("Process path is empty", "Error", qobject_cast<QWidget*>(this->parent()));
            break;
        }
        saveActionDataJSON_f();
        Q_EMIT JSONSaved_signal();
        break;
    }
}

void runProcessWidgets_c::insertArgumentRow_f(
        const argument_c& argument_par_con)
{
    QTableWidgetItem *argumentValueCellTmp(new QTableWidgetItem(argument_par_con.argument_f()));
    argumentValueCellTmp->setFlags(argumentValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *argumentEnabledCellTmp(new QTableWidgetItem);
    Qt::CheckState checkValue(argument_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    argumentEnabledCellTmp->setCheckState(checkValue);
    argumentEnabledCellTmp->setFlags(argumentEnabledCellTmp->flags() bitand compl Qt::ItemIsEditable);

    const int_fast32_t indexTmp(argumentsTable_pri->rowCount());

    argumentsTable_pri->insertRow(indexTmp);
    argumentsTable_pri->setItem(indexTmp, 0, argumentValueCellTmp);
    argumentsTable_pri->setItem(indexTmp, 1, argumentEnabledCellTmp);
}

void runProcessWidgets_c::insertEnvironmentPairRow_f(
        const QString& key_par_con
        , const environmentPair_c& environmentPair_par_con)
{
    QTableWidgetItem *environmentKeyCellTmp(new QTableWidgetItem(key_par_con));
    environmentKeyCellTmp->setFlags(environmentKeyCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *environmentValueCellTmp(new QTableWidgetItem(environmentPair_par_con.value_f()));
    environmentValueCellTmp->setFlags(environmentValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *environmentPairEnabledCellTmp(new QTableWidgetItem);
    Qt::CheckState checkValue(environmentPair_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    environmentPairEnabledCellTmp->setCheckState(checkValue);
    environmentPairEnabledCellTmp->setFlags(environmentPairEnabledCellTmp->flags() bitand compl Qt::ItemIsEditable);

    const int_fast32_t indexTmp(environmentToAddTable_pri->rowCount());

    environmentToAddTable_pri->insertRow(indexTmp);
    environmentToAddTable_pri->setItem(indexTmp, 0, environmentKeyCellTmp);
    environmentToAddTable_pri->setItem(indexTmp, 1, environmentValueCellTmp);
    environmentToAddTable_pri->setItem(indexTmp, 2, environmentPairEnabledCellTmp);
}

//index must be valid
void runProcessWidgets_c::updateArgumentRow_f(
        const argument_c& argument_par_con
        , const int row_par_con)
{
    argumentsTable_pri->item(row_par_con, 0)->setText(argument_par_con.argument_f());
    Qt::CheckState checkValue(argument_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    argumentsTable_pri->item(row_par_con, 1)->setCheckState(checkValue);
}

//index must be valid
void runProcessWidgets_c::updateEnvironmentPairRow_f(
        const QString& key_par_con
        , const environmentPair_c& environmentPair_par_con
        , const int row_par_con)
{
    argumentsTable_pri->item(row_par_con, 0)->setText(key_par_con);
    argumentsTable_pri->item(row_par_con, 1)->setText(environmentPair_par_con.value_f());
    Qt::CheckState checkValue(environmentPair_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    argumentsTable_pri->item(row_par_con, 2)->setCheckState(checkValue);
}

void runProcessWidgets_c::loadActionSpecificData_f()
{
    if (not actionData_ptr_pri->actionDataJSON_f().isEmpty())
    {
        runProcessAction_c processActionTmp;
        processActionTmp.read_f(actionData_ptr_pri->actionDataJSON_f());
        processPathPTE_pri->setPlainText(processActionTmp.processPath_f());
        workingDirectoryPTE_pri->setPlainText(processActionTmp.workingDirectory_f());
        useActonEnviroment_pri->setChecked(processActionTmp.useActonEnvironment_f());

        for (const argument_c& argument_ite_con : processActionTmp.arguments_f())
        {
            insertArgumentRow_f(argument_ite_con);
        }

        QHash<QString, environmentPair_c>::const_iterator iteratorTmp = processActionTmp.environmentToAdd_f().constBegin();
        while (iteratorTmp not_eq processActionTmp.environmentToAdd_f().constEnd())
        {
            insertEnvironmentPairRow_f(iteratorTmp.key(), iteratorTmp.value());
            ++iteratorTmp;
        }
    }
}

runProcessWidgets_c::runProcessWidgets_c(
        actionData_c* const actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        , QObject *parent)
    : QObject(parent)
    , actionData_ptr_pri(actionData_ptr_par)
    //, actionDataId_pri(actionDataId_par_con)
{
    this->setObjectName("runProcessWidgets");

    //order
    //process location
    //arguments
    //working directory
    //environment

    //recycle the environment and working directory windows


    //process and browse process path
    QHBoxLayout* firstRowTmp = new QHBoxLayout;

    firstRowTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Process")));
    processPathPTE_pri = new QPlainTextEdit;
    auto minHeightTmp(processPathPTE_pri->fontMetrics().lineSpacing() + 14);
    processPathPTE_pri->setMinimumHeight(minHeightTmp);
    processPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    firstRowTmp->addWidget(processPathPTE_pri);
    QPushButton* browseProcessPathButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    firstRowTmp->addWidget(browseProcessPathButtonTmp);
    connect(browseProcessPathButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::browseProcessFile_f);

    //add-edit argument, remove arguments and argument table
    QHBoxLayout* secondRowTmp = new QHBoxLayout;

    //goes into argumentButtonsAndTableTmp (secondRowTmp)
    QHBoxLayout* argumentButtonsRowTmp = new QHBoxLayout;

    QPushButton* addEditArgumentButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Add/Edit arg"));
    addEditArgumentButtonTmp->setToolTip("Add or edit an argument, select an existing one to edit");
    argumentButtonsRowTmp->addWidget(addEditArgumentButtonTmp);
    connect(addEditArgumentButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::openArgumentEditWindow_f);
    QPushButton* removeArgumentButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Remove args"));
    argumentButtonsRowTmp->addWidget(removeArgumentButtonTmp);
    connect(removeArgumentButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::removeArgument_f);

    //arguments table, goes into buttonsAndTableTmp (secondRowTmp)
    QHBoxLayout* argumentTableRowTmp = new QHBoxLayout;

    argumentsTable_pri = new QTableWidget(0, 2);
    argumentsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);
    argumentsTable_pri->setDragEnabled(true);
    argumentsTable_pri->setDropIndicatorShown(true);
    argumentsTable_pri->setDragDropMode(QAbstractItemView::InternalMove);

    QStringList tablelabels;
    tablelabels << appConfig_ptr_ext->translate_f("Argument") << appConfig_ptr_ext->translate_f("Enabled");
    argumentsTable_pri->setHorizontalHeaderLabels(tablelabels);
    argumentsTable_pri->setShowGrid(true);
    argumentsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    argumentTableRowTmp->addWidget(argumentsTable_pri);

    //goes into secondRowTmp
    QVBoxLayout* argumentButtonsAndTableColumnTmp = new QVBoxLayout;

    argumentButtonsAndTableColumnTmp->addLayout(argumentButtonsRowTmp);
    argumentButtonsAndTableColumnTmp->addLayout(argumentTableRowTmp);

    secondRowTmp->addLayout(argumentButtonsAndTableColumnTmp);

    //working directory + browse working directory button + show current directory button
    QHBoxLayout* thirdRowTmp = new QHBoxLayout;

    thirdRowTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Working directory")));
    workingDirectoryPTE_pri = new QPlainTextEdit;
    workingDirectoryPTE_pri->setMinimumHeight(minHeightTmp);
    workingDirectoryPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    thirdRowTmp->addWidget(workingDirectoryPTE_pri);
    QPushButton* browseWorkingDirectoryButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    thirdRowTmp->addWidget(browseWorkingDirectoryButtonTmp);
    connect(browseWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::browseWorkingDirectory_f);

    QPushButton* showCurrentWorkingDirectoryButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Show current working directory"));
    thirdRowTmp->addWidget(showCurrentWorkingDirectoryButtonTmp);
    connect(showCurrentWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::showCurrentWorkingDirectoryWindow_f);

    //add + remove environment pair buttons and environment table, + show program env button
    //reciclar mostrar sa finestra amb so environment aqui posar un tercer boto
    QHBoxLayout* fourthRowTmp = new QHBoxLayout;

    //goes into environmentButtonsAndTableTmp (fourthRowTmp)
    QHBoxLayout* environmentButtonsRowTmp = new QHBoxLayout;

    QPushButton* addEditEnvironmentPairButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Add/Edit environment pair"));
    addEditEnvironmentPairButtonTmp->setToolTip("Add or edit an argument, select an existing one to edit");
    environmentButtonsRowTmp->addWidget(addEditEnvironmentPairButtonTmp);
    connect(addEditEnvironmentPairButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::openEnvironmentPairEditWindow_f);
    QPushButton* removeEnvironmentPairButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Remove environment pair"));
    environmentButtonsRowTmp->addWidget(removeEnvironmentPairButtonTmp);
    connect(removeEnvironmentPairButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::removeEnvironmentPair_f);

    QPushButton* showCurrentEnvironmentButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Show current enviroment"));
    environmentButtonsRowTmp->addWidget(showCurrentEnvironmentButtonTmp);
    connect(showCurrentEnvironmentButtonTmp, &QPushButton::clicked, this, &runProcessWidgets_c::showCurrentEnvironmentWindow_f);

    useActonEnviroment_pri = new QCheckBox("Use Acton environment as base");
    //this is modified after loadActionSpecificData_f (if something was loaded)
    useActonEnviroment_pri->setChecked(true);
    useActonEnviroment_pri->setToolTip("When running the process use Acton environment, otherwise use an empty one");
    environmentButtonsRowTmp->addWidget(useActonEnviroment_pri);

    //environmentPairs table, goes into environmentButtonsAndTableTmp (fourthRowTmp)
    QHBoxLayout* environmentToAddTableRowTmp = new QHBoxLayout;

    environmentToAddTable_pri = new QTableWidget(0, 3);
    environmentToAddTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);
    environmentToAddTable_pri->setDragEnabled(true);
    environmentToAddTable_pri->setDropIndicatorShown(true);
    //FUTURE implement drag and drops
    environmentToAddTable_pri->setDragDropMode(QAbstractItemView::InternalMove);

    QStringList environmentToAddTablelabels;
    environmentToAddTablelabels << appConfig_ptr_ext->translate_f("Key") << appConfig_ptr_ext->translate_f("Value") << appConfig_ptr_ext->translate_f("Enabled");
    environmentToAddTable_pri->setHorizontalHeaderLabels(environmentToAddTablelabels);
    environmentToAddTable_pri->setShowGrid(true);
    environmentToAddTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    environmentToAddTableRowTmp->addWidget(environmentToAddTable_pri);

    //goes into secondRowTmp
    QVBoxLayout* environmentButtonsAndTableTmp = new QVBoxLayout;

    environmentButtonsAndTableTmp->addLayout(environmentButtonsRowTmp);
    environmentButtonsAndTableTmp->addLayout(environmentToAddTableRowTmp);

    fourthRowTmp->addLayout(environmentButtonsAndTableTmp);

    //WARNING these buttons are on the action window
//    //save and cancel buttons
//    QHBoxLayout* fifthRowTmp = new QHBoxLayout;

//    QPushButton* saveButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Save"));
//    buttonsLayoutTmp->addWidget(saveButtonTmp);
//    connect(saveButtonTmp, &QPushButton::clicked, this, &argumentEditWindow_c::saveButtonPushed_f);
//    QPushButton* cancelButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Cancel"));
//    buttonsLayoutTmp->addWidget(cancelButtonTmp);
//    connect(cancelButtonTmp, &QPushButton::clicked, this, &argumentEditWindow_c::cancelButtonPushed_f);


    QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row2Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;
    //row4Tmp->setContentsMargins(0,0,0,0);

    row1Tmp->setLayout(firstRowTmp);
    row2Tmp->setLayout(secondRowTmp);
    row3Tmp->setLayout(thirdRowTmp);
    row4Tmp->setLayout(fourthRowTmp);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter");

    mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row2Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->addWidget(row4Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setCollapsible(3, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);

    //variableLayout_par_con->setContentsMargins(0,0,0,0);
    //variableLayout_par_con->setSpacing(0);
    //it seems that QVBoxLayout has an innate horizontal margins
    variableLayout_par_con->addWidget(mainSplitter_pri);
    //variableLayout_par_con->addLayout(fifthRowTmp);

    if (appConfig_ptr_ext->configLoaded_f())
    {
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    loadActionSpecificData_f();
}

void runProcessWidgets_c::saveActionDataJSON_f() const
{
    QString processPathTmp(processPathPTE_pri->toPlainText());
    std::vector<argument_c> argumentsTmp;
    argumentsTmp.reserve(argumentsTable_pri->rowCount());
    for (auto rowIndex_ite = 0, l = argumentsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        bool enabledTmp(argumentsTable_pri->item(rowIndex_ite, 1)->checkState() == Qt::CheckState::Checked);
        QString argumentTmp(argumentsTable_pri->item(rowIndex_ite, 0)->text());
        argumentsTmp.emplace_back(argumentTmp, enabledTmp);
    }

    QString workingDirectoryTmp(workingDirectoryPTE_pri->toPlainText());

    QHash<QString, environmentPair_c> environmentToAddPairsTmp;
    environmentToAddPairsTmp.reserve(environmentToAddTable_pri->rowCount());
    for (auto rowIndex_ite = 0, l = environmentToAddTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        bool enabledTmp(environmentToAddTable_pri->item(rowIndex_ite, 2)->checkState() == Qt::CheckState::Checked);
        QString keyTmp(environmentToAddTable_pri->item(rowIndex_ite, 0)->text());
        QString valueTmp(environmentToAddTable_pri->item(rowIndex_ite, 1)->text());
        environmentToAddPairsTmp.insert(keyTmp, {valueTmp, enabledTmp});
    }

    runProcessAction_c processTmp(
                processPathTmp
                , argumentsTmp
                , workingDirectoryTmp
                , useActonEnviroment_pri->isChecked()
                , environmentToAddPairsTmp
    );
    QJsonObject saveValuesJSONObjectTmp;
    processTmp.write_f(saveValuesJSONObjectTmp);
    actionData_ptr_pri->setActionDataJSON_f(saveValuesJSONObjectTmp);
}

void runProcessWidgets_c::browseProcessFile_f()
{
    selectProcessFileDialog_pri = new QFileDialog(qobject_cast<QWidget*>(this->parent()));
    //AcceptOpen is the default
    //selectProcessFileDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    selectProcessFileDialog_pri->setFileMode(QFileDialog::ExistingFile);
    selectProcessFileDialog_pri->setDirectory(QDir::currentPath());
    selectProcessFileDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select process file..."));
    selectProcessFileDialog_pri->setViewMode(QFileDialog::Detail);
    selectProcessFileDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    selectProcessFileDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    selectProcessFileDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    selectProcessFileDialog_pri->setGeometry(QApplication::desktop()->availableGeometry(this));
#endif
    selectProcessFileDialog_pri->setOption(QFileDialog::ReadOnly, true);

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f());
    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryHistoryTmp.size());
    if (not directoryHistoryTmp.empty())
    {
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    selectProcessFileDialog_pri->setSidebarUrls(directoriesTmp);

    QObject::connect(selectProcessFileDialog_pri, &QFileDialog::finished, this, &runProcessWidgets_c::fileDialogProcessFileFinished_f);

    selectProcessFileDialog_pri->show();
}

void runProcessWidgets_c::fileDialogProcessFileFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not selectProcessFileDialog_pri->selectedFiles().isEmpty())
        {
            processPathPTE_pri->setPlainText(selectProcessFileDialog_pri->selectedFiles().first());
        }
    }
    selectProcessFileDialog_pri->deleteLater();
    selectProcessFileDialog_pri = nullptr;
}

void runProcessWidgets_c::openArgumentEditWindow_f()
{
    QString argumentValueTmp;
    int_fast32_t argumentIndexTmp(argumentsTable_pri->rowCount());
    bool argumentEnabledTmp(true);
    if (not argumentsTable_pri->selectedItems().empty())
    {
        auto firstRowSelection = argumentsTable_pri->selectedItems().first()->row();
        argumentValueTmp = argumentsTable_pri->item(firstRowSelection, 0)->text();
        argumentIndexTmp = firstRowSelection;
        argumentEnabledTmp = argumentsTable_pri->item(firstRowSelection, 1)->checkState() == Qt::CheckState::Checked;
    }

    argumentWindow_pri = new argumentEditWindow_c(
                argumentValueTmp
                , argumentIndexTmp
                , argumentEnabledTmp
                , argumentsTable_pri->rowCount()
                , qobject_cast<QWidget*>(this->parent())
    );

    connect(argumentWindow_pri, &argumentEditWindow_c::saveArgumentResult_signal, this, &runProcessWidgets_c::addUpdateArgumentRow_f);
    argumentWindow_pri->show();
}

void runProcessWidgets_c::openEnvironmentPairEditWindow_f()
{
    QString keyTmp;
    QString valueTmp;
    //row index of the edit enviromentPair, this can be an existing one or else the last one hence environmentToAddTable_pri->rowCount()
    int_fast32_t environmentPairToAddIndexTmp(environmentToAddTable_pri->rowCount());
    bool environmentPairEnabledTmp(true);
    if (not environmentToAddTable_pri->selectedItems().empty())
    {
        auto firstRowSelection = environmentToAddTable_pri->selectedItems().first()->row();
        keyTmp = environmentToAddTable_pri->item(firstRowSelection, 0)->text();
        valueTmp = environmentToAddTable_pri->item(firstRowSelection, 1)->text();
        environmentPairToAddIndexTmp = firstRowSelection;
        environmentPairEnabledTmp = environmentToAddTable_pri->item(firstRowSelection, 2)->checkState() == Qt::CheckState::Checked;
    }

    environmentPairWindow_pri = new environmentPairToAddEditWindow_c(
                keyTmp
                , valueTmp
                , environmentPairEnabledTmp
                , environmentPairToAddIndexTmp
                , qobject_cast<QWidget*>(this->parent())
    );

    connect(environmentPairWindow_pri, &environmentPairToAddEditWindow_c::saveEnvironmentPairResult_signal, this, &runProcessWidgets_c::addUpdateEnvironmentPairRow_f);

    environmentPairWindow_pri->show();
}

void runProcessWidgets_c::removeArgument_f()
{
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp = argumentsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No argument rows were selected for removal", "Error", qobject_cast<QWidget*>(this->parent()));
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
            argumentsTable_pri->removeRow(item_ite_con);
        }
        break;
    }
}

void runProcessWidgets_c::removeEnvironmentPair_f()
{
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp = environmentToAddTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No environment pair rows were selected for removal", "Error", qobject_cast<QWidget*>(this->parent()));
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
            environmentToAddTable_pri->removeRow(item_ite_con);
        }
        break;
    }
}

//this is called from an emit in the argument class window function
void runProcessWidgets_c::addUpdateArgumentRow_f(
        const argument_c& saveResult_par_con
        , const int index_par_con
)
{
    if (index_par_con == argumentsTable_pri->rowCount())
    {
        insertArgumentRow_f(saveResult_par_con);
    }
    else
    {
        updateArgumentRow_f(saveResult_par_con, index_par_con);
    }
}

//this is called from an emit in the environment pair edit class window function
void runProcessWidgets_c::addUpdateEnvironmentPairRow_f(
        const QString& savedKey_par_con
        , const environmentPair_c& savedEnvironmentPair_par_con
        , const int index_par_con
)
{
    if (index_par_con == environmentToAddTable_pri->rowCount())
    {
        insertEnvironmentPairRow_f(savedKey_par_con, savedEnvironmentPair_par_con);
    }
    else
    {
        updateEnvironmentPairRow_f(savedKey_par_con, savedEnvironmentPair_par_con, index_par_con);
    }
}

void runProcessWidgets_c::browseWorkingDirectory_f()
{
    selectWorkingDirectoryDialog_pri = new QFileDialog(qobject_cast<QWidget*>(this->parent()));
    selectWorkingDirectoryDialog_pri->setFileMode(QFileDialog::Directory);
    selectWorkingDirectoryDialog_pri->setDirectory(QDir::currentPath());
    selectWorkingDirectoryDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select directory..."));
    selectWorkingDirectoryDialog_pri->setViewMode(QFileDialog::Detail);
    selectWorkingDirectoryDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    selectWorkingDirectoryDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    selectWorkingDirectoryDialog_pri->setWindowModality(Qt::WindowModal);
    //don't delete on close because it must be queried after
    //selectFolderDialog_pri->setAttribute(Qt::WA_DeleteOnClose);
#ifdef __ANDROID__
    selectWorkingDirectoryDialog_pri->setGeometry(QApplication::desktop()->availableGeometry(this));
#endif
    selectWorkingDirectoryDialog_pri->setOption(QFileDialog::ReadOnly, true);

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f());
    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryHistoryTmp.size());
    if (not directoryHistoryTmp.empty())
    {
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    selectWorkingDirectoryDialog_pri->setSidebarUrls(directoriesTmp);

    QObject::connect(selectWorkingDirectoryDialog_pri, &QFileDialog::finished, this, &runProcessWidgets_c::fileDialogWorkingDirectoryFinished_f);
    selectWorkingDirectoryDialog_pri->show();
}

void runProcessWidgets_c::fileDialogWorkingDirectoryFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        QString directoryTmp;
        if (not selectWorkingDirectoryDialog_pri->selectedFiles().isEmpty())
        {
            directoryTmp = selectWorkingDirectoryDialog_pri->selectedFiles().first();
        }

        if (not directoryTmp.isEmpty())
        {
            workingDirectoryPTE_pri->setPlainText(directoryTmp);
            appConfig_ptr_ext->addDirectoryHistory_f(selectWorkingDirectoryDialog_pri->directory().path());
        }
    }
    selectWorkingDirectoryDialog_pri->deleteLater();
    selectWorkingDirectoryDialog_pri = nullptr;
}

void runProcessWidgets_c::showCurrentWorkingDirectoryWindow_f()
{
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(qobject_cast<QWidget*>(this->parent()));
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

void runProcessWidgets_c::showCurrentEnvironmentWindow_f()
{
    environmentWindow_c* environmentWindowTmp = new environmentWindow_c(qobject_cast<QWidget*>(this->parent()));
    environmentWindowTmp->setWindowFlag(Qt::Window, true);
    environmentWindowTmp->setWindowModality(Qt::WindowModal);
    environmentWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    environmentWindowTmp->show();
}

