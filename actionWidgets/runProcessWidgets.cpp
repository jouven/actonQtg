#include "runProcessWidgets.hpp"

#include "runProcessExtra/argumentWindow.hpp"
#include "runProcessExtra/environmentPairWindow.hpp"

#include "../optionsWidgets/environmentWindow.hpp"
#include "../optionsWidgets/workingDirectoryWindow.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/actions/runProcess.hpp"
#include "essentialQtgso/messageBox.hpp"
#include "textQtso/text.hpp"

#include <QtWidgets>
#include <QSplitter>
#include <QFileDialog>
#include <QDir>

#include <set>

void runProcessWidgets_c::derivedParentClosing_f()
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + argumentsTable_pri->objectName(), argumentsTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + argumentsTable_pri->objectName() + argumentsTable_pri->horizontalHeader()->objectName(), argumentsTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + environmentToAddTable_pri->objectName(), environmentToAddTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + environmentToAddTable_pri->objectName() + environmentToAddTable_pri->horizontalHeader()->objectName(), environmentToAddTable_pri->horizontalHeader()->saveState());
}

QString runProcessWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>run process widget tips</p>");
}

runProcessData_c runProcessWidgets_c::fieldsToRunProcessDataObject_f() const
{
    //paths aren't checked/valitated on save,
    //but they are on execution
    //otherwise all the paths would have to exist exist and if stuff is created dynamically (string parser)
    //it wouldn't work
    QString processPathTmp(processPathPTE_pri->toPlainText());
    std::vector<argument_c> argumentsTmp;
    argumentsTmp.reserve(argumentsTable_pri->rowCount());
    for (auto rowIndex_ite = 0, l = argumentsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        bool enabledTmp(argumentsTable_pri->item(rowIndex_ite, 2)->checkState() == Qt::CheckState::Checked);
        QString argumentTmp(argumentsTable_pri->item(rowIndex_ite, 1)->text());
        argumentsTmp.emplace_back(argumentTmp, enabledTmp);
    }

    QString workingDirectoryTmp(workingDirectoryPTE_pri->toPlainText());

    QHash<QString, environmentPairConfig_c> environmentToAddPairsTmp;
    environmentToAddPairsTmp.reserve(environmentToAddTable_pri->rowCount());
    for (auto rowIndex_ite = 0, l = environmentToAddTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        bool enabledTmp(environmentToAddTable_pri->item(rowIndex_ite, 2)->checkState() == Qt::CheckState::Checked);
        QString keyTmp(environmentToAddTable_pri->item(rowIndex_ite, 0)->text());
        QString valueTmp(environmentToAddTable_pri->item(rowIndex_ite, 1)->text());
        environmentToAddPairsTmp.insert(keyTmp, environmentPairConfig_c(valueTmp, enabledTmp));
    }
    return runProcessData_c(
                processPathTmp
                , argumentsTmp
                , workingDirectoryTmp
                , useActonEnviroment_pri->isChecked()
                , environmentToAddPairsTmp
                );

}

//bool runProcessWidgets_c::isFieldsDataValid_f() const
//{
//    bool resultTmp(false);
//    while (true)
//    {
//        QString processPathStr(processPathPTE_pri->toPlainText());
//        if (processPathStr.length() > 255)
//        {
//            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Process path is too long (>255): ") + QString::number(processPathStr.length())
//                               , appConfig_ptr_ext->translate_f("Error")
//                               , static_cast<QWidget*>(this->parent()));
//            break;
//        }
//        if (processPathStr.isEmpty())
//        {
//            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Process path is empty")
//                               , appConfig_ptr_ext->translate_f("Error")
//                               , static_cast<QWidget*>(this->parent()));
//            break;
//        }
//        resultTmp = true;
//        break;
//    }
//    return resultTmp;
//}


bool runProcessWidgets_c::derivedSaveNew_f(const actionData_c& actionDataBlock_par_con)
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    runProcessData_c objTmp(fieldsToRunProcessDataObject_f());
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        runProcessAction_ptr_pri =
                    new runProcessAction_c(
                        actionDataBlock_par_con
                        , objTmp
                        )
                    ;
        actionPtr_pro = runProcessAction_ptr_pri;

        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool runProcessWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    runProcessData_c objTmp(fieldsToRunProcessDataObject_f());
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        runProcessAction_ptr_pri->updateRunProcessData_f(objTmp);
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

void runProcessWidgets_c::refreshIndexColumn_f(const int index_par_con)
{
    for (auto rowIndex_ite = index_par_con, l = argumentsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        if (rowIndex_ite not_eq argumentsTable_pri->item(rowIndex_ite, 0)->text().toInt())
        {
            argumentsTable_pri->item(rowIndex_ite, 0)->setText(QString::number(rowIndex_ite));
        }
    }
}

void runProcessWidgets_c::insertArgumentRow_f(
        const argument_c& argument_par_con
        , const int row_par_con)
{
    const int_fast32_t indexTmp_con(row_par_con == -1 ? argumentsTable_pri->rowCount() : row_par_con);
    const bool lastRowInsertTmp(row_par_con == argumentsTable_pri->rowCount());

    QTableWidgetItem *indexValueCellTmp(new QTableWidgetItem(QString::number(indexTmp_con)));
    indexValueCellTmp->setFlags(indexValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *argumentValueCellTmp(new QTableWidgetItem(argument_par_con.argument_f()));
    argumentValueCellTmp->setFlags(argumentValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *argumentEnabledCellTmp(new QTableWidgetItem);
    Qt::CheckState checkValue(argument_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    argumentEnabledCellTmp->setCheckState(checkValue);
    argumentEnabledCellTmp->setFlags(argumentEnabledCellTmp->flags() bitand compl Qt::ItemIsEditable);

    argumentsTable_pri->insertRow(indexTmp_con);
    argumentsTable_pri->setItem(indexTmp_con, 0, indexValueCellTmp);
    argumentsTable_pri->setItem(indexTmp_con, 1, argumentValueCellTmp);
    argumentsTable_pri->setItem(indexTmp_con, 2, argumentEnabledCellTmp);


    if (lastRowInsertTmp)
    {
    }
    else
    {
        //if the row is inserted between other rows
        refreshIndexColumn_f(indexTmp_con + 1);
    }
}

void runProcessWidgets_c::insertEnvironmentPairRow_f(
        const QString& key_par_con
        , const environmentPairConfig_c& environmentPair_par_con)
{
    QTableWidgetItem *environmentKeyCellTmp(new QTableWidgetItem(key_par_con));
    environmentKeyCellTmp->setFlags(environmentKeyCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *environmentValueCellTmp(new QTableWidgetItem(environmentPair_par_con.environmentValue()));
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
    argumentsTable_pri->item(row_par_con, 1)->setText(argument_par_con.argument_f());
    Qt::CheckState checkValue(argument_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    argumentsTable_pri->item(row_par_con, 2)->setCheckState(checkValue);
}

//index must be valid
void runProcessWidgets_c::updateEnvironmentPairRow_f(
        const QString& key_par_con
        , const environmentPairConfig_c& environmentPair_par_con
        , const int row_par_con)
{
    environmentToAddTable_pri->item(row_par_con, 0)->setText(key_par_con);
    environmentToAddTable_pri->item(row_par_con, 1)->setText(environmentPair_par_con.environmentValue());
    Qt::CheckState checkValueTmp(environmentPair_par_con.enabled_f() ? Qt::Checked : Qt::Unchecked);
    environmentToAddTable_pri->item(row_par_con, 2)->setCheckState(checkValueTmp);
}

void runProcessWidgets_c::loadActionSpecificData_f()
{
    runProcessData_c valuesToLoadTmp(runProcessAction_ptr_pri not_eq nullptr ? *runProcessAction_ptr_pri : runProcessData_c());

    processPathPTE_pri->setPlainText(valuesToLoadTmp.processPath_f());
    workingDirectoryPTE_pri->setPlainText(valuesToLoadTmp.workingDirectory_f());
    useActonEnviroment_pri->setChecked(valuesToLoadTmp.useActonEnvironment_f());

    for (const argument_c& argument_ite_con : valuesToLoadTmp.arguments_f())
    {
        insertArgumentRow_f(argument_ite_con);
    }

    QHash<QString, environmentPairConfig_c>::const_iterator iteratorTmp = valuesToLoadTmp.environmentToAdd_f().constBegin();
    while (iteratorTmp not_eq valuesToLoadTmp.environmentToAdd_f().constEnd())
    {
        insertEnvironmentPairRow_f(iteratorTmp.key(), iteratorTmp.value());
        ++iteratorTmp;
    }
}

runProcessWidgets_c::runProcessWidgets_c(
        action_c*& actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        )
    : baseClassActionTypeWidgets_c(actionData_ptr_par, variableLayout_par_con->parentWidget())
    , runProcessAction_ptr_pri(actionData_ptr_par == nullptr ? nullptr : static_cast<runProcessAction_c*>(actionData_ptr_par))
{
    this->setObjectName("runProcessWidgets_");

    //process and browse process path
    QHBoxLayout* firstRowTmp = new QHBoxLayout;

    firstRowTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Process")));
    processPathPTE_pri = new QPlainTextEdit;
    auto minHeightTmp(processPathPTE_pri->fontMetrics().lineSpacing() + 14);
    processPathPTE_pri->setMinimumHeight(minHeightTmp);
    processPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    processPathPTE_pri->setToolTip(appConfig_ptr_ext->translate_f(
"Path isn't validated/checked until execution, DO NOT put arguments here because this is a path and \"process path not found\" will happen"));

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

    argumentsTable_pri = new QTableWidget(0, 3);
    argumentsTable_pri->setObjectName("argumentsTable_");
    argumentsTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    argumentsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);
    argumentsTable_pri->setDragEnabled(true);
    argumentsTable_pri->setDropIndicatorShown(true);
    argumentsTable_pri->setDragDropMode(QAbstractItemView::InternalMove);

    QStringList tablelabels;
    tablelabels << appConfig_ptr_ext->translate_f("Index") << appConfig_ptr_ext->translate_f("Argument") << appConfig_ptr_ext->translate_f("Enabled");
    argumentsTable_pri->setHorizontalHeaderLabels(tablelabels);
    argumentsTable_pri->setShowGrid(true);
    argumentsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    argumentsTable_pri->verticalHeader()->setVisible(false);
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
    workingDirectoryPTE_pri->setToolTip(appConfig_ptr_ext->translate_f("Path isn't validated/checked until execution"));
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
    useActonEnviroment_pri->setToolTip("When running the process use Acton environment, otherwise use an empty one");
    environmentButtonsRowTmp->addWidget(useActonEnviroment_pri);

    //environmentPairs table, goes into environmentButtonsAndTableTmp (fourthRowTmp)
    QHBoxLayout* environmentToAddTableRowTmp = new QHBoxLayout;

    environmentToAddTable_pri = new QTableWidget(0, 3);
    environmentToAddTable_pri->setObjectName("environmentToAddTable_");
    environmentToAddTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
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
    mainSplitter_pri->setObjectName("QSplitter_");

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
         argumentsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + argumentsTable_pri->objectName()));
         argumentsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + argumentsTable_pri->objectName() + argumentsTable_pri->horizontalHeader()->objectName()));
         environmentToAddTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + environmentToAddTable_pri->objectName()));
         environmentToAddTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + environmentToAddTable_pri->objectName() + environmentToAddTable_pri->horizontalHeader()->objectName()));
    }

    loadActionSpecificData_f();
}

void runProcessWidgets_c::browseProcessFile_f()
{
    selectProcessFileDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
    selectProcessFileDialog_pri->setObjectName("selectProcessFileDialog_");
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

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + selectProcessFileDialog_pri->objectName()));

    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        selectProcessFileDialog_pri->setSidebarUrls(directoriesTmp);
    }

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
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + selectProcessFileDialog_pri->objectName(), selectProcessFileDialog_pri->directory().path());
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
        argumentValueTmp = argumentsTable_pri->item(firstRowSelection, 1)->text();
        argumentEnabledTmp = argumentsTable_pri->item(firstRowSelection, 2)->checkState() == Qt::CheckState::Checked;
        argumentIndexTmp = firstRowSelection;
    }

    argumentWindow_pri = new argumentEditWindow_c(
                argumentValueTmp
                , argumentIndexTmp
                , argumentEnabledTmp
                , argumentsTable_pri->rowCount()
                , static_cast<QWidget*>(this->parent())
    );

    connect(argumentWindow_pri, &argumentEditWindow_c::saveArgumentResult_signal, this, &runProcessWidgets_c::addUpdateArgumentRow_f);
    connect(argumentWindow_pri, &argumentEditWindow_c::deleteArgument_signal, this, &runProcessWidgets_c::removeArgumentRow_f);
    connect(argumentWindow_pri, &argumentEditWindow_c::refreshArgumentIndexColumn_signal, this, &runProcessWidgets_c::refreshIndexColumn_f);
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
        environmentPairEnabledTmp = environmentToAddTable_pri->item(firstRowSelection, 2)->checkState() == Qt::CheckState::Checked;
        environmentPairToAddIndexTmp = firstRowSelection;
    }

    environmentPairWindow_pri = new environmentPairToAddEditWindow_c(
                keyTmp
                , valueTmp
                , environmentPairEnabledTmp
                , environmentPairToAddIndexTmp
                , static_cast<QWidget*>(this->parent())
    );

    connect(environmentPairWindow_pri, &environmentPairToAddEditWindow_c::saveEnvironmentPairResult_signal, this, &runProcessWidgets_c::addUpdateEnvironmentPairRow_f);

    environmentPairWindow_pri->show();
}

void runProcessWidgets_c::removeArgumentRow_f(const int index_par_con)
{
    argumentsTable_pri->removeRow(index_par_con);
}

void runProcessWidgets_c::removeArgument_f()
{
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp = argumentsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("No argument rows were selected for removal")
                               , appConfig_ptr_ext->translate_f("Error")
                               , static_cast<QWidget*>(this->parent()));
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
            removeArgumentRow_f(item_ite_con);
        }

        refreshIndexColumn_f(rowsToRemoveTmp.back());
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
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No environment pair rows were selected for removal")
                        , appConfig_ptr_ext->translate_f("Error")
                        , static_cast<QWidget*>(this->parent()));
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
            environmentToAddTable_pri->removeRow(item_ite_con);
        }
        break;
    }
}

//this is called from an emit in the argument class window function
void runProcessWidgets_c::addUpdateArgumentRow_f(
        const argument_c& saveResult_par_con
        , const int index_par_con
        , const bool update_par_con
)
{
    if (update_par_con
        and index_par_con < argumentsTable_pri->rowCount())
    {
        updateArgumentRow_f(saveResult_par_con, index_par_con);
    }
    else
    {
        insertArgumentRow_f(saveResult_par_con, index_par_con);
    }
}

//this is called from an emit in the environment pair edit class window function
void runProcessWidgets_c::addUpdateEnvironmentPairRow_f(
        const QString& savedKey_par_con
        , const environmentPairConfig_c& savedEnvironmentPair_par_con
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
    selectWorkingDirectoryDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
    selectWorkingDirectoryDialog_pri->setObjectName("selectWorkingDirectoryDialog_");
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

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + selectWorkingDirectoryDialog_pri->objectName()));
    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        selectWorkingDirectoryDialog_pri->setSidebarUrls(directoriesTmp);
    }

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
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + selectWorkingDirectoryDialog_pri->objectName(), selectWorkingDirectoryDialog_pri->directory().path());
        }
    }
    selectWorkingDirectoryDialog_pri->deleteLater();
    selectWorkingDirectoryDialog_pri = nullptr;
}

void runProcessWidgets_c::showCurrentWorkingDirectoryWindow_f()
{
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(static_cast<QWidget*>(this->parent()));
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

void runProcessWidgets_c::showCurrentEnvironmentWindow_f()
{
    environmentWindow_c* environmentWindowTmp = new environmentWindow_c(static_cast<QWidget*>(this->parent()));
    environmentWindowTmp->setWindowFlag(Qt::Window, true);
    environmentWindowTmp->setWindowModality(Qt::WindowModal);
    environmentWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    environmentWindowTmp->show();
}

