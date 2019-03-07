#include "copyFileWidgets.hpp"

#include "copyFileExtra/fileListWindow.hpp"
#include "copyFileExtra/extensionQLineEditForDelegate.hpp"
#include "copyFileExtra/regexQLineEditForDelegate.hpp"
#include "../optionsWidgets/workingDirectoryWindow.hpp"

#include "appConfig.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QJsonObject>
//#include <QSplitter>

#include <QFileDialog>
#include <QDir>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#include <vector>
#include <set>

void copyFileWidgets_c::parentClosing_f()
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + filenameFullExtensionsTable_pri->objectName() + filenameFullExtensionsTable_pri->horizontalHeader()->objectName(), filenameFullExtensionsTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + filenameFullExtensionsTable_pri->objectName(), filenameFullExtensionsTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + filenameRegexTable_pri->objectName() + filenameRegexTable_pri->horizontalHeader()->objectName(), filenameRegexTable_pri->horizontalHeader()->saveState());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + filenameRegexTable_pri->objectName(), filenameRegexTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
}

bool copyFileWidgets_c::fieldsAreOkToSave_f() const
{
    bool validTmp(false);
    while (true)
    {
        QString sourceDirPathTmp(sourcePathPTE_pri->toPlainText());
        if (sourceDirPathTmp.isEmpty())
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Source path is empty"), appConfig_ptr_ext->translate_f("Error"), qobject_cast<QWidget*>(this->parent()));
            break;
        }
        QString destinationDirPathTmp(destinationPathPTE_pri->toPlainText());
        if (destinationDirPathTmp.isEmpty())
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Destination path is empty"), appConfig_ptr_ext->translate_f("Error"), qobject_cast<QWidget*>(this->parent()));
            break;
        }

        bool goodNumberConversion(false);
        if (bufferSizeLineEdit_pri->text().toLong(&goodNumberConversion) < 0
            or not goodNumberConversion)
        {
            errorQMessageBox_f("Wrong buffer value, value must be between 1 and INT64MAX", "Error", qobject_cast<QWidget*>(this->parent()));
            break;
        }

        validTmp = true;
        break;
    }
    return validTmp;
}

void copyFileWidgets_c::save_f()
{
    if (fieldsAreOkToSave_f())
    {
        saveActionDataJSON_f();
        Q_EMIT JSONSaved_signal();
    }
}


void copyFileWidgets_c::browsePath_f()
{
    browsePathFileDialog_pri = new QFileDialog(qobject_cast<QWidget*>(this->parent()));
    if (browseSource_pri)
    {
        browsePathFileDialog_pri->setObjectName("browseSourcePathFileDialog_");
        browsePathFileDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select a source directory..."));
    }
    if (browseDestination_pri)
    {
        browsePathFileDialog_pri->setObjectName("browseDestinationPathFileDialog_");
        browsePathFileDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select a destination directory..."));
    }
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browsePathFileDialog_pri->setFileMode(QFileDialog::Directory);
    browsePathFileDialog_pri->setViewMode(QFileDialog::Detail);
    browsePathFileDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    browsePathFileDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    browsePathFileDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    browseSourcePathFileDialog_pri.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + browsePathFileDialog_pri->objectName()));
    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        browsePathFileDialog_pri->setDirectory(directoryHistoryTmp.front());
        browsePathFileDialog_pri->setSidebarUrls(directoriesTmp);
    }

    QObject::connect(browsePathFileDialog_pri, &QFileDialog::finished, this, &copyFileWidgets_c::fileDialogBrowseSourceFinished_f);

    browsePathFileDialog_pri->show();
}

void copyFileWidgets_c::browseSourcePath_f()
{
    browseSource_pri = true;
    browsePath_f();
}

void copyFileWidgets_c::browseDestinationPath_f()
{
    browseDestination_pri = true;
    browsePath_f();
}

void copyFileWidgets_c::fileDialogBrowseSourceFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browsePathFileDialog_pri->selectedFiles().isEmpty())
        {
            if (browseSource_pri)
            {
                sourcePathPTE_pri->setPlainText(browsePathFileDialog_pri->selectedFiles().first());
                browseSource_pri = false;
            }
            if (browseDestination_pri)
            {
                destinationPathPTE_pri->setPlainText(browsePathFileDialog_pri->selectedFiles().first());
                browseDestination_pri = false;
            }
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + browsePathFileDialog_pri->objectName(), browsePathFileDialog_pri->directory().path());
        }
    }
    browsePathFileDialog_pri->deleteLater();
    browsePathFileDialog_pri = nullptr;
}

copyFileAction_c copyFileWidgets_c::fieldsToCopyFileActionObject_f() const
{
    QString sourcePathTmp(sourcePathPTE_pri->toPlainText());
    QString destinationPathTmp(destinationPathPTE_pri->toPlainText());

    copyFileAction_c::transferType_ec transferTypeTmp(copyFileAction_c::strToTransferTypeMap_sta_con.value(transferTypeCombobox_pri->currentData().toString()));
    copyFileAction_c::destinationTreatment_ec destinationTreatmentTmp(copyFileAction_c::strToDestinationTreatmentMap_sta_con.value(destinationTreatmentCombobox_pri->currentData().toString()));

    QStringList sourceFilenameRegexStringListTmp;
    sourceFilenameRegexStringListTmp.reserve(filenameRegexTable_pri->rowCount());
    for (auto rowIndex_ite = 0, l = filenameRegexTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        QString sourceFilenameRegexTmp(filenameRegexTable_pri->item(rowIndex_ite, 0)->text());
        if (sourceFilenameRegexTmp.isEmpty())
        {
        }
        else
        {
            sourceFilenameRegexStringListTmp.append(sourceFilenameRegexTmp);
        }

    }

    QStringList sourceFileFullExtensionStringListTmp;
    sourceFileFullExtensionStringListTmp.reserve(filenameFullExtensionsTable_pri->rowCount());
    for (auto rowIndex_ite = 0, l = filenameFullExtensionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        QString sourceFileFullExtensionTmp(filenameFullExtensionsTable_pri->item(rowIndex_ite, 0)->text());
        if (sourceFileFullExtensionTmp.isEmpty())
        {
        }
        else
        {
            sourceFileFullExtensionStringListTmp.append(sourceFileFullExtensionTmp);
        }
    }

    copyFileAction_c copyFileActionTmp(
                sourcePathTmp
                , destinationPathTmp
                , transferTypeTmp
                , destinationTreatmentTmp
                , copyHiddenCheckbox_pri->isChecked()
                , sourceFilenameRegexStringListTmp
                , sourceFileFullExtensionStringListTmp
                , navigateSubdirectoriesCheckbox_pri->isChecked()
                , navigateHiddenCheckbox_pri->isChecked()
                , copyEmptyDirectoriesCheckbox_pri->isChecked()
                , createDestinationAndParentsCheckbox_pri->isChecked()
                , noFilesCopiedIsErrorCheckbox_pri->isChecked()
                , stopAllCopyOnFileCopyErrorCheckbox_pri->isChecked()
                , bufferSizeLineEdit_pri->text().toLong()
    );
    return copyFileActionTmp;
}

void copyFileWidgets_c::tryGenerateFileList_f() const
{
    if (fieldsAreOkToSave_f())
    {
        copyFileAction_c copyFileAcitonTmp(fieldsToCopyFileActionObject_f());
        std::vector<QString> fileListTmp(copyFileAcitonTmp.testSourceFileList_f());

        fileListWindow_c* fileListWindowTmp = new fileListWindow_c(fileListTmp, qobject_cast<QWidget*>(this->parent()));
        fileListWindowTmp->setWindowFlag(Qt::Window, true);
        fileListWindowTmp->setWindowModality(Qt::WindowModal);
        fileListWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
        fileListWindowTmp->show();
    }
}

void copyFileWidgets_c::insertFullExtensionRow_f(
        const QString& fullExtrension_par_con)
{
    QTableWidgetItem* fullExtensionValueCellTmp(new QTableWidgetItem(fullExtrension_par_con));
    fullExtensionValueCellTmp->setFlags(fullExtensionValueCellTmp->flags() bitor Qt::ItemIsEditable);

    const int_fast32_t indexTmp(filenameFullExtensionsTable_pri->rowCount());
    filenameFullExtensionsTable_pri->insertRow(indexTmp);
    filenameFullExtensionsTable_pri->setItem(indexTmp, 0, fullExtensionValueCellTmp);
}

////index must be valid
//void copyFileWidgets_c::updateFullExtensionRow_f(
//        const QString& fullExtrension_par_con
//        , const int row_par_con)
//{
//    filenameFullExtensionsTable_pri->item(row_par_con, 0)->setText(fullExtrension_par_con);
//}

void copyFileWidgets_c::insertRegexPatternRow_f(
        const QString& regexPattern_par_con)
{
    QTableWidgetItem* regexPatternValueCellTmp(new QTableWidgetItem(regexPattern_par_con));
    regexPatternValueCellTmp->setFlags(regexPatternValueCellTmp->flags() bitor Qt::ItemIsEditable);

    const int_fast32_t indexTmp(filenameRegexTable_pri->rowCount());
    filenameRegexTable_pri->insertRow(indexTmp);
    filenameRegexTable_pri->setItem(indexTmp, 0, regexPatternValueCellTmp);
}

void copyFileWidgets_c::removeSelectedRegexPatternRow_f()
{
    if (filenameRegexTable_pri->selectedItems().isEmpty())
    {
        //nothing to do
    }
    else
    {
        std::set<int> uniqueRowsSetTmp;
        for (QTableWidgetItem* cellPtr_ite : filenameRegexTable_pri->selectedItems())
        {
            uniqueRowsSetTmp.emplace(cellPtr_ite->row());
        }
        std::vector<int> rowsToRemoveTmp(uniqueRowsSetTmp.begin(), uniqueRowsSetTmp.end());
        std::reverse(rowsToRemoveTmp.begin(), rowsToRemoveTmp.end());
        //remove from the larger index to the smaller because the table will reposition the index on remove
        for (const int row_ite_con : rowsToRemoveTmp)
        {
            filenameRegexTable_pri->removeRow(row_ite_con);
        }
    }
}

////index must be valid
//void copyFileWidgets_c::updateRegexPatternRow_f(
//        const QString& regexPattern_par_con
//        , const int row_par_con)
//{
//    filenameRegexTable_pri->item(row_par_con, 0)->setText(regexPattern_par_con);
//}

void copyFileWidgets_c::loadActionSpecificData_f()
{
    if (not actionData_ptr_pri->actionDataJSON_f().isEmpty())
    {
        copyFileAction_c copyFileActionTmp;
        copyFileActionTmp.read_f(actionData_ptr_pri->actionDataJSON_f());

        sourcePathPTE_pri->setPlainText(copyFileActionTmp.sourcePath_f());
        destinationPathPTE_pri->setPlainText(copyFileActionTmp.destinationPath_f());

        {
            QString transferTypeStrTmp(copyFileAction_c::transferTypeToStrUMap_sta_con.at(copyFileActionTmp.transferType_f()));
            int loadedIndexTmp(transferTypeCombobox_pri->findData(transferTypeStrTmp.toLower()));
            transferTypeCombobox_pri->setCurrentIndex(loadedIndexTmp);
        }

        {
            QString destinationTreatmentStrTmp(copyFileAction_c::destinationTreatmentToStrUMap_sta_con.at(copyFileActionTmp.destinationTreatment_f()));
            int loadedIndexTmp(destinationTreatmentCombobox_pri->findData(destinationTreatmentStrTmp.toLower()));
            destinationTreatmentCombobox_pri->setCurrentIndex(loadedIndexTmp);
        }


        for (const QString& filenameFullExtension_ite_con : copyFileActionTmp.sourceFilenameFullExtensions_f())
        {
            insertFullExtensionRow_f(filenameFullExtension_ite_con);
        }

        for (const QString& sourceFilenameRegexFilter_ite_con : copyFileActionTmp.sourceFilenameRegexFilters_f())
        {
            insertRegexPatternRow_f(sourceFilenameRegexFilter_ite_con);
        }

        copyHiddenCheckbox_pri->setChecked(copyFileActionTmp.copyHidden_f());
        navigateSubdirectoriesCheckbox_pri->setChecked(copyFileActionTmp.navigateSubdirectories_f());
        navigateHiddenCheckbox_pri->setChecked(copyFileActionTmp.navigateHidden_f());
        copyEmptyDirectoriesCheckbox_pri->setChecked(copyFileActionTmp.copyEmptyDirectories_f());
        createDestinationAndParentsCheckbox_pri->setChecked(copyFileActionTmp.createDestinationAndParents_f());
        noFilesCopiedIsErrorCheckbox_pri->setChecked(copyFileActionTmp.noFilesCopiedIsError_f());

        bufferSizeLineEdit_pri->setText(QString::number(copyFileActionTmp.bufferSize_f()));
    }
}

void copyFileWidgets_c::showCurrentWorkingDirectoryWindow_f()
{
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(qobject_cast<QWidget*>(this->parent()));
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

void copyFileWidgets_c::addFilenameFullExtensionRow_f()
{
    insertFullExtensionRow_f();
}

void copyFileWidgets_c::removeSelectedFilenameFullExtensionRow_f()
{
    if (filenameFullExtensionsTable_pri->selectedItems().isEmpty())
    {
        //nothing to do
    }
    else
    {
        std::set<int> uniqueRowsSetTmp;
        for (QTableWidgetItem* cellPtr_ite : filenameFullExtensionsTable_pri->selectedItems())
        {
            uniqueRowsSetTmp.emplace(cellPtr_ite->row());
        }
        std::vector<int> rowsToRemoveTmp(uniqueRowsSetTmp.begin(), uniqueRowsSetTmp.end());
        std::reverse(rowsToRemoveTmp.begin(), rowsToRemoveTmp.end());
        for (const int row_ite_con : rowsToRemoveTmp)
        {
            filenameFullExtensionsTable_pri->removeRow(row_ite_con);
        }
    }
}

void copyFileWidgets_c::addFilenameRegexPatternRow_f()
{
    insertRegexPatternRow_f();
}

copyFileWidgets_c::copyFileWidgets_c(
        actionData_c* const actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        , QObject *parent)
    : QObject(parent)
    , actionData_ptr_pri(actionData_ptr_par)
{
    this->setObjectName("copyFileWidgets_");

    //source path PTE
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Source path")));
    sourcePathPTE_pri = new QPlainTextEdit;
    auto minHeightTmp(sourcePathPTE_pri->fontMetrics().lineSpacing() + 14);
    sourcePathPTE_pri->setMinimumHeight(minHeightTmp);
    sourcePathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    sourcePathPTE_pri->setToolTip(appConfig_ptr_ext->translate_f("Doesn't have to exist, but it must exist when the action execution happens"));
    firstRowLayoutTmp->addWidget(sourcePathPTE_pri);
    QPushButton* browseSourcePathButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    firstRowLayoutTmp->addWidget(browseSourcePathButtonTmp);
    connect(browseSourcePathButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::browseSourcePath_f);

    //destination path PTE
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Destination path")));
    destinationPathPTE_pri = new QPlainTextEdit;
    destinationPathPTE_pri->setMinimumHeight(minHeightTmp);
    destinationPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    destinationPathPTE_pri->setToolTip(appConfig_ptr_ext->translate_f("Doesn't have to exist, but it must exist when the action execution happens or \"create destination parents\" must be checked"));
    secondRowLayoutTmp->addWidget(destinationPathPTE_pri);
    QPushButton* browseDestinationPathButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    secondRowLayoutTmp->addWidget(browseDestinationPathButtonTmp);
    connect(browseDestinationPathButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::browseDestinationPath_f);

    //transfer type combo
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    transferTypeCombobox_pri = new QComboBox();
    transferTypeCombobox_pri->setEditable(true);
    transferTypeCombobox_pri->setInsertPolicy(QComboBox::NoInsert);
    transferTypeCombobox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    transferTypeCombobox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    transferTypeCombobox_pri->setToolTip(appConfig_ptr_ext->translate_f(
    R"(Copy and move work like the OS, except they are done in blocks so they can be cancelled/resumed.
"TrueMove" reads the source from the end and writes the destination at the begining and then
shifts the destination if there is more data to be moved,
each moved block iteration is read(source)-write(destination)-delete(source)-shift(destination, if there is another block to move),
plus an initial shift is resuming,
this method is really slow but barely uses no extra space, regular move uses 2x"the file size" during the move, this only uses the buffer size
increasing the buffer can increase the speed since less shifting will be done in total.
Also resumes by default, it will prepend the source to the destination if the destination already exists)"
    ));
    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Transfer type")));
    thirdRowLayoutTmp->addWidget(transferTypeCombobox_pri);

    for (const QString& transferTypeStr_ite_con : copyFileAction_c::strToTransferTypeMap_sta_con.keys())
    {
        transferTypeCombobox_pri->insertItem(transferTypeCombobox_pri->count(), appConfig_ptr_ext->translate_f(transferTypeStr_ite_con), transferTypeStr_ite_con);
    }

    //destination treatment
    //QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    destinationTreatmentCombobox_pri = new QComboBox();
    destinationTreatmentCombobox_pri->setEditable(true);
    destinationTreatmentCombobox_pri->setInsertPolicy(QComboBox::NoInsert);
    destinationTreatmentCombobox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    destinationTreatmentCombobox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    destinationTreatmentCombobox_pri->setToolTip(appConfig_ptr_ext->translate_f(
    R"("Overwrite is error only if different" or how to detect diferent files.
"Overwrite only if different" check if it's the same file first, depending on the situation this might be faster than regular overwrite.
"No overwrite" also known as don't modify existing files
"Try stupid resume" resume based on file size, if destination is smaller than source, read source starting from "destination size index"
and write starting at the end of the destination. If between the first transfer and the resume transfer any, source or destination is modified
this method will go wrong.
"Lazy resume" first compares the files and detects the first block where the files are different and starts resuming from there,
"accuracy" depends on the buffer size, and it doesn't check if the the other parts of the file are the same, it's not perfect
but it does the job.)"
    ));
    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Destination treatment")));
    thirdRowLayoutTmp->addWidget(destinationTreatmentCombobox_pri);

    for (const QString& destinationTreatmentStr_ite_con : copyFileAction_c::strToDestinationTreatmentMap_sta_con.keys())
    {
        destinationTreatmentCombobox_pri->insertItem(destinationTreatmentCombobox_pri->count(), appConfig_ptr_ext->translate_f(destinationTreatmentStr_ite_con), destinationTreatmentStr_ite_con);
    }

    //fullExtensions
    QHBoxLayout* fifthRowLayoutTmp = new QHBoxLayout;

    filenameFullExtensionsTable_pri = new QTableWidget(0, 1);
    filenameFullExtensionsTable_pri->setObjectName("filenameFullExtensionsTable_");
    filenameFullExtensionsTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    filenameFullExtensionsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);
    filenameFullExtensionsTable_pri->setDragEnabled(true);
    filenameFullExtensionsTable_pri->setDropIndicatorShown(true);
    filenameFullExtensionsTable_pri->setDragDropMode(QAbstractItemView::InternalMove);

    {
        QItemEditorCreatorBase* itemEditorCreatorBase(new QStandardItemEditorCreator<extensionQLineEdit_c>());
        QItemEditorFactory* itemEditorFactory(new QItemEditorFactory);
        itemEditorFactory->registerEditor(QVariant::String, itemEditorCreatorBase);

        QStyledItemDelegate* cellTextDelegate(new QStyledItemDelegate(this));
        cellTextDelegate->setItemEditorFactory(itemEditorFactory);
        filenameFullExtensionsTable_pri->setItemDelegate(cellTextDelegate);
    }

    filenameFullExtensionsTable_pri->setToolTip(appConfig_ptr_ext->translate_f(
    R"(The "full extension" or "complete suffix" consists of all characters in the filename after, but not including, the first '.'.
Filter is applied "or" wise)"
    ));

    {
        QStringList tablelabels;
        tablelabels << appConfig_ptr_ext->translate_f("Full extension");
        filenameFullExtensionsTable_pri->setHorizontalHeaderLabels(tablelabels);
    }

    filenameFullExtensionsTable_pri->setShowGrid(true);
    filenameFullExtensionsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    fifthRowLayoutTmp->addWidget(filenameFullExtensionsTable_pri);

    {
        //fullExtensions buttons
        QVBoxLayout* fullExtensionsButtonsVLayoutTmp = new QVBoxLayout;

        QPushButton* addFilenameFullExtensionRowButtonTmp(new QPushButton(appConfig_ptr_ext->translate_f("Add")));
        addFilenameFullExtensionRowButtonTmp->setToolTip("Add a row, empty rows will be ignored when saving");
        connect(addFilenameFullExtensionRowButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::addFilenameFullExtensionRow_f);
        fullExtensionsButtonsVLayoutTmp->addWidget(addFilenameFullExtensionRowButtonTmp);

        QPushButton* removeFlenameFullExtensionRowButtonTmp(new QPushButton(appConfig_ptr_ext->translate_f("Remove")));
        removeFlenameFullExtensionRowButtonTmp->setToolTip("Remove selected rows");
        connect(removeFlenameFullExtensionRowButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::removeSelectedFilenameFullExtensionRow_f);
        fullExtensionsButtonsVLayoutTmp->addWidget(removeFlenameFullExtensionRowButtonTmp);

        fifthRowLayoutTmp->addLayout(fullExtensionsButtonsVLayoutTmp);
    }
#ifdef DEBUGJOUVEN
    //qDebug() << "doubleClicked_f";
#endif
    //regex
    QHBoxLayout* sixthRowLayoutTmp = new QHBoxLayout;

    filenameRegexTable_pri = new QTableWidget(0, 1);
    filenameRegexTable_pri->setObjectName("filenameRegexTable_");
    filenameRegexTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    filenameRegexTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);
    filenameRegexTable_pri->setDragEnabled(true);
    filenameRegexTable_pri->setDropIndicatorShown(true);
    filenameRegexTable_pri->setDragDropMode(QAbstractItemView::InternalMove);

    {
        QItemEditorCreatorBase* itemEditorCreatorBase(new QStandardItemEditorCreator<regexQLineEdit_c>());
        QItemEditorFactory* itemEditorFactory(new QItemEditorFactory);
        itemEditorFactory->registerEditor(QVariant::String, itemEditorCreatorBase);

        QStyledItemDelegate* cellTextDelegate(new QStyledItemDelegate(this));
        cellTextDelegate->setItemEditorFactory(itemEditorFactory);
        filenameRegexTable_pri->setItemDelegate(cellTextDelegate);
    }

    filenameRegexTable_pri->setToolTip(appConfig_ptr_ext->translate_f(
    R"(Regex that filenames need to match to get copied.
Filter is applied "or" wise)"
    ));

    {
        QStringList tablelabels;
        tablelabels << appConfig_ptr_ext->translate_f("Regex pattern");
        filenameRegexTable_pri->setHorizontalHeaderLabels(tablelabels);
    }

    filenameRegexTable_pri->setShowGrid(true);
    filenameRegexTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    sixthRowLayoutTmp->addWidget(filenameRegexTable_pri);

    {
        //regex buttons
        QVBoxLayout* regexButtonsVLayoutTmp = new QVBoxLayout;

        QPushButton* addFilenameRegexRowButtonTmp(new QPushButton(appConfig_ptr_ext->translate_f("Add")));
        addFilenameRegexRowButtonTmp->setToolTip("Add a row, empty rows will be ignored when saving");
        connect(addFilenameRegexRowButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::addFilenameRegexPatternRow_f);
        regexButtonsVLayoutTmp->addWidget(addFilenameRegexRowButtonTmp);

        QPushButton* removeFlenameFullExtensionRowButtonTmp(new QPushButton(appConfig_ptr_ext->translate_f("Remove")));
        removeFlenameFullExtensionRowButtonTmp->setToolTip("Remove selected rows");
        connect(removeFlenameFullExtensionRowButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::removeSelectedRegexPatternRow_f);
        regexButtonsVLayoutTmp->addWidget(removeFlenameFullExtensionRowButtonTmp);

        sixthRowLayoutTmp->addLayout(regexButtonsVLayoutTmp);
    }
    //checks and buffer
    QHBoxLayout* seventhRowLayoutTmp = new QHBoxLayout;

    QVBoxLayout* firstChecksColumnTmp = new QVBoxLayout;

    navigateSubdirectoriesCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Navigate subdirectories"));
    navigateSubdirectoriesCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Navigate or ignore directories in the source"));
    navigateSubdirectoriesCheckbox_pri->setMinimumHeight(minHeightTmp);
    navigateSubdirectoriesCheckbox_pri->setChecked(true);
    firstChecksColumnTmp->addWidget(navigateSubdirectoriesCheckbox_pri);

    navigateHiddenCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Navigate hidden directories"));
    navigateHiddenCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("When copying subdirectories of the source path, navigate or ignore hidden directories"));
    navigateHiddenCheckbox_pri->setMinimumHeight(minHeightTmp);
    navigateHiddenCheckbox_pri->setChecked(true);
    firstChecksColumnTmp->addWidget(navigateHiddenCheckbox_pri);

    copyHiddenCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Copy hidden files"));
    copyHiddenCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Copy hidden files, directories are not factored"));
    copyHiddenCheckbox_pri->setMinimumHeight(minHeightTmp);
    copyHiddenCheckbox_pri->setChecked(true);
    firstChecksColumnTmp->addWidget(copyHiddenCheckbox_pri);

    copyEmptyDirectoriesCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Copy empty directories"));
    copyEmptyDirectoriesCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Copy directories with no files, will still copy directories that have directories even if they are empty"));
    copyEmptyDirectoriesCheckbox_pri->setMinimumHeight(minHeightTmp);
    copyEmptyDirectoriesCheckbox_pri->setChecked(true);
    firstChecksColumnTmp->addWidget(copyEmptyDirectoriesCheckbox_pri);

    QVBoxLayout* secondChecksColumnTmp = new QVBoxLayout;

    createDestinationAndParentsCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Create destination parents"));
    createDestinationAndParentsCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Create destination and all the parent directories, otherwise, if destination doesn't exist, an error will happen"));
    createDestinationAndParentsCheckbox_pri->setMinimumHeight(minHeightTmp);
    createDestinationAndParentsCheckbox_pri->setChecked(true);
    secondChecksColumnTmp->addWidget(createDestinationAndParentsCheckbox_pri);

    noFilesCopiedIsErrorCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("No files copied is error"));
    noFilesCopiedIsErrorCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("If no files are coppied throw an error"));
    noFilesCopiedIsErrorCheckbox_pri->setMinimumHeight(minHeightTmp);
    noFilesCopiedIsErrorCheckbox_pri->setChecked(true);
    secondChecksColumnTmp->addWidget(noFilesCopiedIsErrorCheckbox_pri);

    stopAllCopyOnFileCopyErrorCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Stop copy on error"));
    stopAllCopyOnFileCopyErrorCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f(R"(Stop the copy operation on the first error, or when unchecked,
    continue copying, saving the errors, and showing them at the end)"));
    stopAllCopyOnFileCopyErrorCheckbox_pri->setMinimumHeight(minHeightTmp);
    stopAllCopyOnFileCopyErrorCheckbox_pri->setChecked(true);
    secondChecksColumnTmp->addWidget(stopAllCopyOnFileCopyErrorCheckbox_pri);

    QVBoxLayout* thirdColumnTmp = new QVBoxLayout;

    QHBoxLayout* bufferRowTmp = new QHBoxLayout;

    QLabel* bufferLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Buffer size"));
    bufferLabelTmp->setToolTip(
"<p>Minimum 1 byte, maximum INT64MAX</p>"
"<p>Mind the free available ram size, when using trueMove it can be \"tweaked\" to transfer a file faster, but using more extra space/p>"
    );
    bufferRowTmp->addWidget(bufferLabelTmp);

    bufferSizeLineEdit_pri = new QLineEdit;

    QRegExp reIntegersTmp(R"(^[0-9]*$)");
    QValidator *validatorTmp = new QRegExpValidator(reIntegersTmp, this);
    bufferSizeLineEdit_pri->setValidator(validatorTmp);
    //enough for 64bit
    bufferSizeLineEdit_pri->setMaxLength(20);
    bufferSizeLineEdit_pri->setToolTip(
                "<p>Mind the free available ram size, when using trueMove this can be \"abused\" to read a file in one-shot, and if nothing goes wrong electric power wise, write it in one-shot using 0 extra space</p>"
    );
    bufferRowTmp->addWidget(bufferSizeLineEdit_pri);

    thirdColumnTmp->addLayout(bufferRowTmp);

    seventhRowLayoutTmp->addLayout(firstChecksColumnTmp);
    seventhRowLayoutTmp->addLayout(secondChecksColumnTmp);
    seventhRowLayoutTmp->addLayout(thirdColumnTmp);

    QWidget* row1Tmp = new QWidget;
    QWidget* row2Tmp = new QWidget;
    QWidget* row3Tmp = new QWidget;
    //QWidget* row4Tmp = new QWidget;
    QWidget* row5Tmp = new QWidget;
    QWidget* row6Tmp = new QWidget;
    QWidget* row7Tmp = new QWidget;


    row1Tmp->setLayout(firstRowLayoutTmp);
    row2Tmp->setLayout(secondRowLayoutTmp);
    row3Tmp->setLayout(thirdRowLayoutTmp);
    //row4Tmp->setLayout(fourthRowLayoutTmp);
    row5Tmp->setLayout(fifthRowLayoutTmp);
    row6Tmp->setLayout(sixthRowLayoutTmp);
    row7Tmp->setLayout(seventhRowLayoutTmp);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter_");

    mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row2Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    //mainSplitter_pri->addWidget(row4Tmp);
    mainSplitter_pri->addWidget(row5Tmp);
    mainSplitter_pri->addWidget(row6Tmp);
    mainSplitter_pri->addWidget(row7Tmp);
    //this is to prevent moving the splitter separator to the point of hiding completly the widgets
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setCollapsible(3, false);
    mainSplitter_pri->setCollapsible(4, false);
    mainSplitter_pri->setCollapsible(5, false);
    //mainSplitter_pri->setCollapsible(6, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);

    variableLayout_par_con->addWidget(mainSplitter_pri);

    QHBoxLayout* bottomButtonsLayoutTmp = new QHBoxLayout;

    QPushButton* tryGenerateFileListTmp = new QPushButton(appConfig_ptr_ext->translate_f("Try generate file list"));
    tryGenerateFileListTmp->setToolTip("Show a new window with a file list filtered by the current settings");
    connect(tryGenerateFileListTmp, &QPushButton::clicked, this, &copyFileWidgets_c::tryGenerateFileList_f);
    bottomButtonsLayoutTmp->addWidget(tryGenerateFileListTmp);

    QPushButton* showCurrentWorkingDirectoryButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Show current working directory"));
    showCurrentWorkingDirectoryButtonTmp->setToolTip("A little help to work with relative paths");
    bottomButtonsLayoutTmp->addWidget(showCurrentWorkingDirectoryButtonTmp);
    connect(showCurrentWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &copyFileWidgets_c::showCurrentWorkingDirectoryWindow_f);
    bottomButtonsLayoutTmp->addWidget(tryGenerateFileListTmp);

    variableLayout_par_con->addLayout(bottomButtonsLayoutTmp);

//    variableLayout_par_con->addLayout(secondRowLayoutTmp);
//    variableLayout_par_con->addLayout(thirdRowLayoutTmp);
//    variableLayout_par_con->addLayout(fourthRowLayoutTmp);
//    variableLayout_par_con->addLayout(fifthRowLayoutTmp);
//    variableLayout_par_con->addLayout(sixthRowLayoutTmp);
//    variableLayout_par_con->addLayout(seventhRowLayoutTmp);

    if (appConfig_ptr_ext->configLoaded_f())
    {
        filenameFullExtensionsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + filenameFullExtensionsTable_pri->objectName()));filenameFullExtensionsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + filenameFullExtensionsTable_pri->objectName()));
        filenameFullExtensionsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + filenameFullExtensionsTable_pri->objectName() + filenameFullExtensionsTable_pri->horizontalHeader()->objectName()));
        filenameRegexTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + filenameRegexTable_pri->objectName()));
        filenameRegexTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + filenameRegexTable_pri->objectName() + filenameRegexTable_pri->horizontalHeader()->objectName()));
    }

    loadActionSpecificData_f();
}

void copyFileWidgets_c::saveActionDataJSON_f() const
{
    MACRO_ADDACTONQTGLOG("Create copyFileAction from fields", logItem_c::type_ec::debug);
    copyFileAction_c copyFileActionTmp(fieldsToCopyFileActionObject_f());

    QJsonObject saveValuesJSONObjectTmp;
    MACRO_ADDACTONQTGLOG("Write object to JSON", logItem_c::type_ec::debug);
    copyFileActionTmp.write_f(saveValuesJSONObjectTmp);
    MACRO_ADDACTONQTGLOG("Save JSON object into the action", logItem_c::type_ec::debug);
    actionData_ptr_pri->setActionDataJSON_f(saveValuesJSONObjectTmp);
}

