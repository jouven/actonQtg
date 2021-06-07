#include "folderChangeReactionWidgets.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/actions/folderChangeReaction.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"
#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
//#include <QSplitter>

#include <QFileDialog>
#include <QDir>

void folderChangeReactionWidgets_c::derivedParentClosing_f()
{
    //no widget geometry to save yet
}

QString folderChangeReactionWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>folder change reaction widget tips</p>");
}

folderChangeReactionData_c folderChangeReactionWidgets_c::fieldsToFolderChangeReactionDataObject_f() const
{
    folderChangeReactionData_c::fileSizeChangeType_ec fileSizeChangeTypeTmp(folderChangeReactionData_c::fileSizeChangeType_ec::any);
    uint_fast64_t fileSizeChangeFixedValueTmp(0);
    int_fast64_t fileSizeChangeThresholdTmp(0);

    std::unordered_set<folderChangeReactionData_c::changeType_ec> changesToMonitor;
    if (fileCreatedCheckBox_pri->isChecked())
    {
        changesToMonitor.emplace(folderChangeReactionData_c::changeType_ec::fileCreated);
    }
    if (fileRemovedCheckBox_pri->isChecked())
    {
        changesToMonitor.emplace(folderChangeReactionData_c::changeType_ec::fileRemoved);
    }
    //this field/combo is loaded and saved despite if file size is monitored because the default value is required
    fileSizeChangeTypeTmp = folderChangeReactionData_c::strToFileSizeChangeTypeMap_sta_con.value(
                    fileSizeChangeTypeToMonitorComboBox_pri->currentData().toString()
    );
    if (fileSizeChangedCheckBox_pri->isChecked())
    {
        changesToMonitor.emplace(folderChangeReactionData_c::changeType_ec::fileSizeChanged);
        if (fileSizeChangeTypeTmp == folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue)
        {
            fileSizeChangeFixedValueTmp = fileSizeChangeFixedLineEdit_pri->text().toULongLong();
        }
        if (fileSizeChangeTypeTmp == folderChangeReactionData_c::fileSizeChangeType_ec::threshold)
        {
            fileSizeChangeThresholdTmp = fileSizeChangeThresholdLineEdit_pri->text().toLongLong();
        }
    }
    if (fileContentsChangedCheckBox_pri->isChecked())
    {
        changesToMonitor.emplace(folderChangeReactionData_c::changeType_ec::fileContentsChanged);
    }
    if (fileModifiedDateTimeChangedCheckBox_pri->isChecked())
    {
        changesToMonitor.emplace(folderChangeReactionData_c::changeType_ec::fileModifiedDateTimeChanged);
    }

    folderChangeReactionData_c::reactionType_ec reactionTypeTmp(
                folderChangeReactionData_c::strToReactionTypeMap_sta_con.value(
                    reactionTypeComboBox_pri->currentData().toString()
                    )
                );

    folderChangeReactionData_c::reactionOrder_ec reactionOrderTmp(
                folderChangeReactionData_c::strToReactionOrderMap_sta_con.value(
                    reactionOrderComboBox_pri->currentData().toString()
                    )
                );
#ifdef DEBUGJOUVEN
    qDebug() << "reactionActionStringIdCombo_pri->currentData().toString() " << reactionActionStringIdCombo_pri->currentData().toString() << Qt::endl;
#endif
    return folderChangeReactionData_c(
                folderPathPTE_pri->toPlainText()
                , recurseFoldersCheckBox_pri->isChecked()
                , useAbsolutePathsCheckbox_pri->isChecked()
                , changesToMonitor
                , fileSizeChangeTypeTmp
                , fileSizeChangeFixedValueTmp
                , fileSizeChangeThresholdTmp
                , reactionTypeTmp
                , argumentPlaceHolderToReplaceWithChangedFilePath_pri->text()
                , sequentialReactionCheckBox_pri->isChecked()
                , reactionOrderTmp
                , reverseReactionOrderCheckBox_pri->isChecked()
                , monitorIntervalMsLineEdit_pri->text().toULongLong()
                , reactionActionStringIdCombo_pri->currentData().toString()
    );
}

bool folderChangeReactionWidgets_c::isFieldsDataValid_f(textCompilation_c* errors_par) const
{
    bool validTmp(false);
    while (true)
    {
        {
            bool goodNumberConversion(false);
            monitorIntervalMsLineEdit_pri->text().toLongLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong monitor interval value: {0}", monitorIntervalMsLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }
        if (fileSizeChangeFixedLineEdit_pri->isEnabled())
        {
            bool goodNumberConversion(false);
            fileSizeChangeFixedLineEdit_pri->text().toULongLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong file size fixed value: {0}", fileSizeChangeFixedLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        if (fileSizeChangeThresholdLineEdit_pri->isEnabled())
        {
            bool goodNumberConversion(false);
            fileSizeChangeThresholdLineEdit_pri->text().toLongLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong file size threshold value: {0}", fileSizeChangeThresholdLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        validTmp = true;
        break;
    }
    return validTmp;
}

void folderChangeReactionWidgets_c::loadReactionActionStringIdCombo_f()
{
    reactionActionStringIdCombo_pri->clear();
    folderChangeReactionData_c::reactionType_ec reactionTypeTmp(
                folderChangeReactionData_c::strToReactionTypeMap_sta_con.value(
                    reactionTypeComboBox_pri->currentData().toString()
                    )
                );
#ifdef DEBUGJOUVEN
    qDebug() << "folderChangeReactionAction_ptr_pri is nullptr " << QSTRINGBOOL(folderChangeReactionAction_ptr_pri == nullptr) << Qt::endl;
    qDebug() << "folderChangeReactionAction_ptr_pri->actonDataHubParent_f() is nullptr " << QSTRINGBOOL(folderChangeReactionAction_ptr_pri->actonDataHubParent_f() == nullptr) << Qt::endl;
#endif
    for (int_fast32_t index_ite = 0, l = folderChangeReactionAction_ptr_pri->actonDataHubParent_f()->size_f(); index_ite < l; ++index_ite)
    {
        auto actionDataIdTmp(folderChangeReactionAction_ptr_pri->actonDataHubParent_f()->rowToActionDataId_f(index_ite));
        action_c* actionPtrTmp(folderChangeReactionAction_ptr_pri->actonDataHubParent_f()->action_ptr_f(actionDataIdTmp));
#ifdef DEBUGJOUVEN
        //qDebug() << "1 actionPtrTmp is nullptr " << QSTRINGBOOL(actionPtrTmp == nullptr) << endl;
#endif
        if (not actionPtrTmp->enabled_f() and folderChangeReactionData_c::isValidActionForReaction_f(actionPtrTmp, reactionTypeTmp))
        {
            reactionActionStringIdCombo_pri->insertItem(reactionActionStringIdCombo_pri->count(), actionPtrTmp->stringId_f(), actionPtrTmp->stringId_f());
        }
    }
#ifdef DEBUGJOUVEN
    //qDebug() << "2 actionPtrTmp is nullptr " << endl;
#endif
}

bool folderChangeReactionWidgets_c::derivedSaveNew_f(const actionData_c& actionDataBlock_par_con, actonDataHub_c* parentActonDataHub_par)
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        folderChangeReactionData_c objTmp(fieldsToFolderChangeReactionDataObject_f());
        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            folderChangeReactionAction_ptr_pri = new folderChangeReactionAction_c(parentActonDataHub_par, actionDataBlock_par_con, objTmp);
            actionPtr_pro = folderChangeReactionAction_ptr_pri;
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool folderChangeReactionWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        folderChangeReactionData_c objTmp(fieldsToFolderChangeReactionDataObject_f());
        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            folderChangeReactionAction_ptr_pri->updateFolderChangeReactionData_f(objTmp);
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

void folderChangeReactionWidgets_c::browseDirectory_f()
{
    browseFolderDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
    browseFolderDialog_pri->setObjectName("browseFolderChangeReactionDialog_");
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browseFolderDialog_pri->setFileMode(QFileDialog::Directory);
    browseFolderDialog_pri->setDirectory(QDir::currentPath());
    browseFolderDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select directory..."));
    browseFolderDialog_pri->setViewMode(QFileDialog::Detail);
    browseFolderDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    browseFolderDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    browseFolderDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    selectFileDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + browseFolderDialog_pri->objectName()));
    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        browseFolderDialog_pri->setSidebarUrls(directoriesTmp);
    }

    QObject::connect(browseFolderDialog_pri, &QFileDialog::finished, this, &folderChangeReactionWidgets_c::fileDialogBrowseFolderFinished_f);

    browseFolderDialog_pri->show();
}

void folderChangeReactionWidgets_c::fileDialogBrowseFolderFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browseFolderDialog_pri->selectedFiles().isEmpty())
        {
            folderPathPTE_pri->setPlainText(browseFolderDialog_pri->selectedFiles().first());
            appConfig_ptr_ext->addDirectoryHistory_f(
                        this->objectName() + browseFolderDialog_pri->objectName()
                        , browseFolderDialog_pri->directory().path()
            );
        }
    }
    browseFolderDialog_pri->deleteLater();
    browseFolderDialog_pri = nullptr;
}

void folderChangeReactionWidgets_c::fileSizeChangeTypeToMonitorComboChanged_f(int )
{
    while (true)
    {
        if (fileSizeChangeTypeToMonitorComboBox_pri->currentData().toString()
            == folderChangeReactionData_c::fileSizeChangeTypeToStrUMap_sta_con.at(folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue).toLower())
        {
            fileSizeChangeFixedLineEdit_pri->setEnabled(true);
            fileSizeChangeThresholdLineEdit_pri->setEnabled(false);
            break;
        }
        if (fileSizeChangeTypeToMonitorComboBox_pri->currentData().toString()
            == folderChangeReactionData_c::fileSizeChangeTypeToStrUMap_sta_con.at(folderChangeReactionData_c::fileSizeChangeType_ec::threshold).toLower())
        {
            fileSizeChangeFixedLineEdit_pri->setEnabled(false);
            fileSizeChangeThresholdLineEdit_pri->setEnabled(true);
            break;
        }

        fileSizeChangeThresholdLineEdit_pri->setEnabled(false);
        fileSizeChangeFixedLineEdit_pri->setEnabled(false);
        break;
    }
}

void folderChangeReactionWidgets_c::fileSizeChangedCheckBoxChanged_f()
{
    if (fileSizeChangedCheckBox_pri->isChecked())
    {
        fileSizeChangeTypeToMonitorComboBox_pri->setEnabled(true);
        fileSizeChangeTypeToMonitorComboChanged_f(0);
    }
    else
    {
        fileSizeChangeTypeToMonitorComboBox_pri->setEnabled(false);
        fileSizeChangeFixedLineEdit_pri->setEnabled(false);
        fileSizeChangeThresholdLineEdit_pri->setEnabled(false);
    }
}

void folderChangeReactionWidgets_c::reactionTypeComboChanged_f(int)
{
    if (reactionTypeComboBox_pri->currentData().toString()
        == folderChangeReactionData_c::reactionTypeToStrUMap_sta_con.at(folderChangeReactionData_c::reactionType_ec::runProcess).toLower())
    {
        argumentPlaceHolderToReplaceWithChangedFilePath_pri->setEnabled(true);
    }
    else
    {
        argumentPlaceHolderToReplaceWithChangedFilePath_pri->setEnabled(false);
    }
    loadReactionActionStringIdCombo_f();
}

void folderChangeReactionWidgets_c::loadActionSpecificData_f()
{
    folderChangeReactionData_c valuesToLoadTmp(folderChangeReactionAction_ptr_pri not_eq nullptr ? *folderChangeReactionAction_ptr_pri : folderChangeReactionData_c());

    folderPathPTE_pri->setPlainText(valuesToLoadTmp.folderPath_f());
    recurseFoldersCheckBox_pri->setChecked(valuesToLoadTmp.recurseFolders_f());
    useAbsolutePathsCheckbox_pri->setChecked(valuesToLoadTmp.useAbsolutePaths_f());

    fileCreatedCheckBox_pri->setChecked(valuesToLoadTmp.changesToMonitor_f().count(folderChangeReactionData_c::changeType_ec::fileCreated) == 1);
    fileModifiedDateTimeChangedCheckBox_pri->setChecked(valuesToLoadTmp.changesToMonitor_f().count(folderChangeReactionData_c::changeType_ec::fileModifiedDateTimeChanged) == 1);
    fileRemovedCheckBox_pri->setChecked(valuesToLoadTmp.changesToMonitor_f().count(folderChangeReactionData_c::changeType_ec::fileRemoved) == 1);
    fileSizeChangedCheckBox_pri->setChecked(valuesToLoadTmp.changesToMonitor_f().count(folderChangeReactionData_c::changeType_ec::fileSizeChanged) == 1);

    fileContentsChangedCheckBox_pri->setChecked(valuesToLoadTmp.changesToMonitor_f().count(folderChangeReactionData_c::changeType_ec::fileContentsChanged) == 1);

    //this field/combo is loaded and saved despite if file size is monitored because the default value is required
    QString fileSizeChangeTypeToMonitorStrTmp(folderChangeReactionData_c::fileSizeChangeTypeToStrUMap_sta_con.at(valuesToLoadTmp.fileSizeChangeTypeToMonitor_f()));
    int loadedIndexTmp(fileSizeChangeTypeToMonitorComboBox_pri->findData(fileSizeChangeTypeToMonitorStrTmp.toLower()));
    fileSizeChangeTypeToMonitorComboBox_pri->setCurrentIndex(loadedIndexTmp);

    if (fileSizeChangedCheckBox_pri->isChecked())
    {
        if (valuesToLoadTmp.fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue)
        {
            fileSizeChangeFixedLineEdit_pri->setText(QString::number(valuesToLoadTmp.fileSizeChangeFixedValue_f()));
        }
        if (valuesToLoadTmp.fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::threshold)
        {
            fileSizeChangeThresholdLineEdit_pri->setText(QString::number(valuesToLoadTmp.fileSizeChangeThreshold_f()));
        }
    }
    else
    {
        //seems setting the same value doesn't trigger the checkbox change state signal
        //so if it's left unchecked call this function manually
        fileSizeChangedCheckBoxChanged_f();
    }

    {
        QString reactionTypeStrTmp(folderChangeReactionData_c::reactionTypeToStrUMap_sta_con.at(valuesToLoadTmp.reactionType_f()));
        int loadedIndexTmp(reactionTypeComboBox_pri->findData(reactionTypeStrTmp.toLower()));
        reactionTypeComboBox_pri->setCurrentIndex(loadedIndexTmp);
        //default combo value might or might not trigger a change, call the signal slot function anyway
        reactionTypeComboChanged_f(0);
    }

    argumentPlaceHolderToReplaceWithChangedFilePath_pri->setText(valuesToLoadTmp.argumentPlaceHolderToReplaceWithChangedFilePath_f());
    sequentialReactionCheckBox_pri->setChecked(valuesToLoadTmp.sequentialReaction_f());

    {
        QString reactionOrderStrTmp(folderChangeReactionData_c::reactionOrderToStrUMap_sta_con.at(valuesToLoadTmp.reactionOrder_f()));
        int loadedIndexTmp(reactionOrderComboBox_pri->findData(reactionOrderStrTmp.toLower()));
        reactionOrderComboBox_pri->setCurrentIndex(loadedIndexTmp);
    }

    reverseReactionOrderCheckBox_pri->setChecked(valuesToLoadTmp.reverseReactionOrder_f());
    monitorIntervalMsLineEdit_pri->setText(QString::number(valuesToLoadTmp.monitorIntervalMs_f()));

    {
        QString reactionStringIdTmp(valuesToLoadTmp.reactionActionStringId_f());
        int loadedIndexTmp(reactionActionStringIdCombo_pri->findData(reactionStringIdTmp));
        reactionActionStringIdCombo_pri->setCurrentIndex(loadedIndexTmp);
    }
}

folderChangeReactionWidgets_c::folderChangeReactionWidgets_c(
        action_c*& actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        )
    : baseClassActionTypeWidgets_c(actionData_ptr_par, variableLayout_par_con->parentWidget())
    , folderChangeReactionAction_ptr_pri(actionData_ptr_par == nullptr ? nullptr : static_cast<folderChangeReactionAction_c*>(actionData_ptr_par))
{
    this->setObjectName("folderChangeReactionWidgets_");

    //1st row: folder to monitor field, monitor intervalfield, recurse folder check, use absolute path check
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    folderPathPTE_pri = new QPlainTextEdit;
    folderPathPTE_pri->setMinimumHeight(28);
    folderPathPTE_pri->setMaximumHeight(56);
    folderPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Directory path to monitor")));
    firstRowLayoutTmp->addWidget(folderPathPTE_pri);
    QPushButton* browsePathButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    firstRowLayoutTmp->addWidget(browsePathButtonTmp);

    connect(browsePathButtonTmp, &QPushButton::clicked, this, &folderChangeReactionWidgets_c::browseDirectory_f);

    monitorIntervalMsLineEdit_pri = new QLineEdit;
    const QString monitorIntervalExplanationStrTmp(appConfig_ptr_ext->translate_f(
R"(Interval in milliseconds in which the directory is monitored for changes, 0 is allowed but it will probably max the CPU or the I/O of the monitored storage device
It's important to understand that monitoring works detecting changes between two time states: "previous" and "current"
If something happens in-between these time states that doesn't leave an element to be detected, like a file is created and then removed, it will be ignored
Also for now the last state is not saved for actonQtg to resume if the program quits/stops for some reason)"));
    monitorIntervalMsLineEdit_pri->setToolTip(monitorIntervalExplanationStrTmp);
    //that's the "string" length of max 64bit signed number
    /////////////////////////////////////"9223372036854775808 (length 19)
    QRegExpValidator *uint64bitValidatorTmp = new QRegExpValidator(QRegExp("[0-9]{19}"), this);
    QRegExpValidator *int64bitValidatorTmp = new QRegExpValidator(QRegExp("^-?[0-9]{18}"), this);
    monitorIntervalMsLineEdit_pri->setValidator(uint64bitValidatorTmp);
    //timerValueLineEdit_pri->setInputMask("0000000000000000009");

    QLabel* monitorIntervalMsLabelTmp(new QLabel(appConfig_ptr_ext->translate_f("Monitor interval Ms")));
    monitorIntervalMsLabelTmp->setToolTip(monitorIntervalExplanationStrTmp);
    firstRowLayoutTmp->addWidget(monitorIntervalMsLabelTmp);
    firstRowLayoutTmp->addWidget(monitorIntervalMsLineEdit_pri);

    recurseFoldersCheckBox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Recurse subdirectories"));
    firstRowLayoutTmp->addWidget(recurseFoldersCheckBox_pri);

    useAbsolutePathsCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Use absolute paths"));
    useAbsolutePathsCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Reactions will use absolute paths, otherwise paths are relative to the monitored directory"));
    firstRowLayoutTmp->addWidget(useAbsolutePathsCheckbox_pri);

    //2nd row: changes to monitor checks + which filesize change to monitor combo + fileSizeChangeFixedLineEdit_pri + fileSizeChangeThresholdLineEdit_pri
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    fileContentsChangedCheckBox_pri = new QCheckBox("File contents changed");
    fileContentsChangedCheckBox_pri->setToolTip(
R"(It's file size changes plus a hash to check, when the size is equal, if the contents has changed)"
                );
    secondRowLayoutTmp->addWidget(fileContentsChangedCheckBox_pri);

    fileCreatedCheckBox_pri = new QCheckBox("File created");
    fileCreatedCheckBox_pri->setToolTip(
R"(Created files will be reacted)"
                );
    secondRowLayoutTmp->addWidget(fileCreatedCheckBox_pri);

    fileModifiedDateTimeChangedCheckBox_pri = new QCheckBox("File modified datetime changed");
    fileModifiedDateTimeChangedCheckBox_pri->setToolTip(
R"(Files the last modified datetime changes will be reacted)");
    secondRowLayoutTmp->addWidget(fileModifiedDateTimeChangedCheckBox_pri);

    fileRemovedCheckBox_pri = new QCheckBox("File removed");
    fileRemovedCheckBox_pri->setToolTip(
R"(Removed files will be reacted)");
    secondRowLayoutTmp->addWidget(fileRemovedCheckBox_pri);

    fileSizeChangedCheckBox_pri = new QCheckBox("File size changed");
    fileSizeChangedCheckBox_pri->setToolTip(
R"(Files the size changes will be reacted)");
    secondRowLayoutTmp->addWidget(fileSizeChangedCheckBox_pri);

    QObject::connect(fileSizeChangedCheckBox_pri, &QCheckBox::stateChanged, this, &folderChangeReactionWidgets_c::fileSizeChangedCheckBoxChanged_f);

    fileSizeChangeTypeToMonitorComboBox_pri = new QComboBox;
    fileSizeChangeTypeToMonitorComboBox_pri->setEditable(true);
    fileSizeChangeTypeToMonitorComboBox_pri->setInsertPolicy(QComboBox::NoInsert);
    fileSizeChangeTypeToMonitorComboBox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    fileSizeChangeTypeToMonitorComboBox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("File size change type")));
    secondRowLayoutTmp->addWidget(fileSizeChangeTypeToMonitorComboBox_pri);

    for (const QString& actionStr_ite_con : folderChangeReactionData_c::strToFileSizeChangeTypeMap_sta_con.keys())
    {
        fileSizeChangeTypeToMonitorComboBox_pri->insertItem(
                    fileSizeChangeTypeToMonitorComboBox_pri->count()
                    , appConfig_ptr_ext->translate_f(actionStr_ite_con)
                    , actionStr_ite_con
        );
    }

    QObject::connect(fileSizeChangeTypeToMonitorComboBox_pri
            , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
            , this, &folderChangeReactionWidgets_c::fileSizeChangeTypeToMonitorComboChanged_f);

    fileSizeChangeFixedLineEdit_pri = new QLineEdit;
    fileSizeChangeFixedLineEdit_pri->setToolTip(
R"(In bytes. Will trigger a reaction if a file size matches this value)");
    fileSizeChangeFixedLineEdit_pri->setValidator(uint64bitValidatorTmp);
    fileSizeChangeThresholdLineEdit_pri = new QLineEdit;
    fileSizeChangeThresholdLineEdit_pri->setToolTip(
R"(In bytes. Will trigger a reaction if the file size is past, for positive values, or below-equal, for negative values, the value threshold
e.g., 1024 will trigger a reaction if a file size is past the threshold value
-1024 will trigger a reaction if a file size is bellow-equal the threshold value)");
    fileSizeChangeThresholdLineEdit_pri->setValidator(int64bitValidatorTmp);

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Fixed value")));
    secondRowLayoutTmp->addWidget(fileSizeChangeFixedLineEdit_pri);
    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Threshold value")));
    secondRowLayoutTmp->addWidget(fileSizeChangeThresholdLineEdit_pri);

    //3rd row: reaction type combo + sequentialReactionCheckBox_pri + argumentPlaceHolderToReplaceWithChangedFilePath_pri + reactionActionStringIdCombo_pri
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    reactionTypeComboBox_pri = new QComboBox();
    reactionTypeComboBox_pri->setEditable(true);
    reactionTypeComboBox_pri->setInsertPolicy(QComboBox::NoInsert);
    reactionTypeComboBox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    reactionTypeComboBox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Reaction type")));
    thirdRowLayoutTmp->addWidget(reactionTypeComboBox_pri);

    for (const QString& reactionTypeStr_ite_con : folderChangeReactionData_c::strToReactionTypeMap_sta_con.keys())
    {
        reactionTypeComboBox_pri->insertItem(
                    reactionTypeComboBox_pri->count()
                    , appConfig_ptr_ext->translate_f(reactionTypeStr_ite_con)
                    , reactionTypeStr_ite_con
        );
    }

    QObject::connect(reactionTypeComboBox_pri
            , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
            , this, &folderChangeReactionWidgets_c::reactionTypeComboChanged_f);

    sequentialReactionCheckBox_pri = new QCheckBox("Sequential reaction");
    sequentialReactionCheckBox_pri->setToolTip(
R"(React one file at a time, otherwise the reactions are launched in order but happend at the same time (this option is affected by the extra threads configuration)");
    thirdRowLayoutTmp->addWidget(sequentialReactionCheckBox_pri);

    argumentPlaceHolderToReplaceWithChangedFilePath_pri = new QLineEdit;
    argumentPlaceHolderToReplaceWithChangedFilePath_pri->setToolTip(appConfig_ptr_ext->translate_f("This applies only for the runProcess reaction, if not empty, will replace the runProcess reaction arguments that have this placeholder with the reacted file path"));

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Argument placeholder to be replaced")));
    thirdRowLayoutTmp->addWidget(argumentPlaceHolderToReplaceWithChangedFilePath_pri);

    reactionActionStringIdCombo_pri = new QComboBox();
    reactionActionStringIdCombo_pri->setEditable(true);
    reactionActionStringIdCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    reactionActionStringIdCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    reactionActionStringIdCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Reaction action stringId")));
    thirdRowLayoutTmp->addWidget(reactionActionStringIdCombo_pri);

    //4th row: reactionOrderComboBox_pri + reverse check
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    reactionOrderComboBox_pri = new QComboBox();
    reactionOrderComboBox_pri->setEditable(true);
    reactionOrderComboBox_pri->setInsertPolicy(QComboBox::NoInsert);
    reactionOrderComboBox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    reactionOrderComboBox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    reactionOrderComboBox_pri->setToolTip(
R"(In which order the files are reacted when multiple files are going to be reacted)");
    fourthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Reaction order")));
    fourthRowLayoutTmp->addWidget(reactionOrderComboBox_pri);

    for (const QString& reactionOrderStr_ite_con : folderChangeReactionData_c::strToReactionOrderMap_sta_con.keys())
    {
        reactionOrderComboBox_pri->insertItem(
                    reactionOrderComboBox_pri->count()
                    , appConfig_ptr_ext->translate_f(reactionOrderStr_ite_con)
                    , reactionOrderStr_ite_con
        );
    }

    reverseReactionOrderCheckBox_pri = new QCheckBox("Reverse reaction order");
    reverseReactionOrderCheckBox_pri->setToolTip(
R"(Files will be reacted in reverse order e.g., newest first becomes oldest first)");
    fourthRowLayoutTmp->addWidget(reverseReactionOrderCheckBox_pri);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);
    variableLayout_par_con->addLayout(fourthRowLayoutTmp);

    loadActionSpecificData_f();
}

