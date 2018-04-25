//1 is QT_NO_* (like QT_NO_CLIPBOARD) neccessary anymore?, the define doesn't lead anywhere
//searching in google/qt site doesn't provide any platform that needs it
//2 add icons, right now in *nix it uses the current loaded theme ones, but in windows
//this will not fly
#include "mainWindow.hpp"

#include "actionWindow.hpp"
#include "actionExecutionDetailsWindow.hpp"
#include "actonQtso/actionData.hpp"
#include "actonQtso/mappings/actionStrMapping.hpp"
#include "actonQtso/mappings/actionExecutionStateStrMapping.hpp"
#include "appConfig.hpp"
#include "optionsWindow.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "signalso/signal.hpp"
#include "sizeConversionso/byte.hpp"

#include <QtWidgets>
#include <QSet>

//#include <chrono>
#include <unordered_set>
#include <set>

void mainWindow_c::closeEvent(QCloseEvent* event)
{
    if (eines::signal::isRunning_f())
    {
        eines::signal::stopRunning_f();
    }
    if (finalCounterSeconds_pri == 0 and qThreadCount_f() == 0)
    {
        appConfig_f().setWidgetGeometry_f(this->objectName() + actionsTable_pri->objectName() + actionsTable_pri->horizontalHeader()->objectName(), actionsTable_pri->horizontalHeader()->saveState());
        appConfig_f().setWidgetGeometry_f(this->objectName() + actionsTable_pri->objectName(), actionsTable_pri->saveGeometry());
        appConfig_f().setWidgetGeometry_f(this->objectName(), saveGeometry());
        appConfig_f().saveConfigFile_f();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

//void mainWindow_c::resizeFileTable_f()
//{
//    actionsTable_pri->resizeColumnsToContents();
//    actionsTable_pri->adjustSize();

//    QTimer::singleShot(100,[=]
//    {
//#ifdef __ANDROID__
//        baseWidget_pri->adjustSize();
//#else
//        adjustSize();
//#endif
//    });
//}

mainWindow_c::mainWindow_c()
{
    this->setObjectName("mainWindow");
    const QRect screenGeometry = QApplication::desktop()->availableGeometry(this);
    statusBarLabel_pri = new QLabel;

    //statusBarLabel_pri->setText(tr("Files/folders can be drop inside the window to add them to the list"));

    //new, load and save buttons
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //use QTabWidget? FUTURE?
    //new config file
    QIcon newIcon(QIcon::fromTheme("document-new", QIcon(":/images/new.png")));
    QPushButton* newConfigFileButtonTmp = new QPushButton(newIcon, tr("New"));
    newConfigFileButtonTmp->setToolTip("Clears all loaded actions and resets action execution settings");
    firstRowLayoutTmp->addWidget(newConfigFileButtonTmp);
    QObject::connect(newConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::clearAllActions_f);

    //load config file
    QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/browse.png"));
    QPushButton* loadConfigFileButtonTmp = new QPushButton(loadIcon, tr("Load"));
    firstRowLayoutTmp->addWidget(loadConfigFileButtonTmp);
    QObject::connect(loadConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::loadActionsFileButtonPushed_f);

    //save config file
    QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QPushButton* saveConfigFileButtonTmp = new QPushButton(saveIcon, tr("Save"));
    firstRowLayoutTmp->addWidget(saveConfigFileButtonTmp);
    QObject::connect(saveConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::saveActionFileButtonPushed_f);

    //add/edit action, remove actions and test actions
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    //add/edit action
    //QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/hash.png"));
    QPushButton* addEditActionButtonTmp = new QPushButton(tr("&Add/Edit Action"));
    secondRowLayoutTmp->addWidget(addEditActionButtonTmp);
    QObject::connect(addEditActionButtonTmp, &QPushButton::clicked, this, &mainWindow_c::addUpdateAction_f);

    //remove actions
    QPushButton* removeActionsButtonTmp = new QPushButton(tr("Remove Actions"));
    secondRowLayoutTmp->addWidget(removeActionsButtonTmp);
    QObject::connect(removeActionsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::removeActions_f);

    //copy action
    QPushButton* copyActionButtonTmp = new QPushButton(tr("Copy Action"));
    secondRowLayoutTmp->addWidget(copyActionButtonTmp);
    QObject::connect(copyActionButtonTmp, &QPushButton::clicked, this, &mainWindow_c::copyAction_f);

    //execute actions and execution details
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    //execute/stop actions
    executeActionsButton_pri = new QPushButton(tr("Execute Actions"));
    executeActionsButton_pri->setToolTip("No selection executes all actions, selection executes selected actions");
    thirdRowLayoutTmp->addWidget(executeActionsButton_pri);
    QObject::connect(executeActionsButton_pri, &QPushButton::clicked, this, &mainWindow_c::executeActionsButtonPushed_f);

    QPushButton* executionDetailsButtonTmp = new QPushButton(tr("Execution details"));
    thirdRowLayoutTmp->addWidget(executionDetailsButtonTmp);
    QObject::connect(executionDetailsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showExecutionDetails_f);

    //tips, about buttons and program options
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    //tips button
    QPushButton* optionsButtonTmp = new QPushButton(tr("Options"));
    fourthRowLayoutTmp->addWidget(optionsButtonTmp);
    QObject::connect(optionsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showOptions_f);

    //tips button
    QPushButton* tipsButtonTmp = new QPushButton(tr("Tips"));
    fourthRowLayoutTmp->addWidget(tipsButtonTmp);
    QObject::connect(tipsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showTips_f);

    //about button
    QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/images/about.png"));
    QPushButton* aboutButtonTmp = new QPushButton(aboutIcon, tr("About"));
    fourthRowLayoutTmp->addWidget(aboutButtonTmp);
    QObject::connect(aboutButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showAboutMessage_f);

    actionsTable_pri = new QTableWidget(0, 6);
    actionsTable_pri->setObjectName("QTableWidget");
    actionsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    //action (type) 0 | description 1 | halt on fail 2 | Execution state 3 | Last output 4 | Last error 5
    labels << tr("Action") << tr("Description") << tr("Halt on fail") << tr("Execution state") << tr("Last output") << tr("Last error");
    actionsTable_pri->setHorizontalHeaderLabels(labels);
    actionsTable_pri->horizontalHeader()->setObjectName("QHeaderView");
    actionsTable_pri->setShowGrid(true);
    //TODO on android there is no right mouse button (touch holding or doing gestures doesn't make it) by default
    //so no context menus
    //commandsTable_pri->setContextMenuPolicy(Qt::CustomContextMenu);
    actionsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    fileTable_pri->setMinimumHeight(screenGeometry.height() / 3);
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
    QHBoxLayout* fifthRowLayoutTmp = new QHBoxLayout;

    fifthRowLayoutTmp->addWidget(actionsTable_pri);

    QVBoxLayout* indexChangingLayoutTmp = new QVBoxLayout;

    //move action up button
    QPushButton* moveActionUpButtonTmp = new QPushButton(QIcon(":/images/upArrow.png"), "");
    moveActionUpButtonTmp->setToolTip("Move the selected actions up one index");
    indexChangingLayoutTmp->addWidget(moveActionUpButtonTmp);
    QObject::connect(moveActionUpButtonTmp, &QPushButton::clicked, this, &mainWindow_c::moveSelectedActionsUpByOne_f);

    //change the index of the selected action button
    QPushButton* changeIndexButtonTmp = new QPushButton("Change\naction\nindex");
    changeIndexButtonTmp->setToolTip("Changes the first selected action index to a new one moving the destination and all the actions below one index down (except if it's the last index)");
    indexChangingLayoutTmp->addWidget(changeIndexButtonTmp);
    QObject::connect(changeIndexButtonTmp, &QPushButton::clicked, this, &mainWindow_c::openChangeActionIndexWindow_f);

    //move action down button
    QPushButton* moveActionDownButtonTmp = new QPushButton(QIcon(":/images/downArrow.png"), "");
    moveActionDownButtonTmp->setToolTip("Move the selected actions down one index");
    indexChangingLayoutTmp->addWidget(moveActionDownButtonTmp);
    QObject::connect(moveActionDownButtonTmp, &QPushButton::clicked, this, &mainWindow_c::moveSelectedActionsDownByOne_f);

    fifthRowLayoutTmp->addLayout(indexChangingLayoutTmp);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addLayout(thirdRowLayoutTmp);
    mainLayout_pri->addLayout(fourthRowLayoutTmp);
    mainLayout_pri->addLayout(fifthRowLayoutTmp);
    mainLayout_pri->addWidget(statusBarLabel_pri);
#ifdef __ANDROID__
    baseWidget_pri->setLayout(mainLayout_pri);

#else
    this->setLayout(mainLayout_pri);
#endif

    //qInfo() << "moveActionDownButton_pri->parent()->objectName() " << moveActionDownButton_pri->parent()->objectName() << endl;

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

    setWindowTitle(tr("actonQtg"));

    //TODO android has bugs, scrollto has no effect
#ifdef __ANDROID__
    baseWidget_pri->resize(scrollArea_pri->width() * 0.96, scrollArea_pri->height() * 0.95);
#endif
    //resize(screenGeometry.width(), screenGeometry.height() * 0.99);

    QTimer* mainLoopTimer = new QTimer(qApp);
    QObject::connect(mainLoopTimer, &QTimer::timeout, this, &mainWindow_c::mainLoop_f);
    mainLoopTimer->start(500);

    if (appConfig_f().configLoaded_f())
    {
        //qInfo() << "configFile_f().second" << endl;
        restoreGeometry(appConfig_f().widgetGeometry_f(this->objectName()));
        actionsTable_pri->restoreGeometry(appConfig_f().widgetGeometry_f(this->objectName() + actionsTable_pri->objectName()));
        actionsTable_pri->horizontalHeader()->restoreState(appConfig_f().widgetGeometry_f(this->objectName() + actionsTable_pri->objectName() + actionsTable_pri->horizontalHeader()->objectName()));
    }

    //QObject::connect(this, &mainWindow_c::executeNextAction_signal, this, &mainWindow_c::executeNextAction_f);

    QObject::connect(this, &mainWindow_c::setStatusBarText_signal, statusBarLabel_pri, &QLabel::setText);
    QObject::connect(this, &mainWindow_c::scrollToItem_signal, this, &mainWindow_c::scrollToItem_f);
    QObject::connect(this, &mainWindow_c::setRowCellField_signal, this, &mainWindow_c::setRowCellField_f);
    QObject::connect(this, &mainWindow_c::setHashRowCellField_signal, this, &mainWindow_c::setHashRowCellField_f);
    //QObject::connect(this, &Window_c::saveAfterHash_signal_f, this, &Window_c::dialogSaveFileList_f);
    //QObject::connect(this, &mainWindow_c::resizeFileTable_signal, this, &mainWindow_c::resizeFileTable_f);

}

void mainWindow_c::processPositionalArguments_f(const QStringList& positionalArguments_par_con)
{
    QTimer::singleShot(15,[=]
    {
        if (not positionalArguments_par_con.isEmpty())
        {
            loadFileList_f(positionalArguments_par_con);
        }
    });
}

void mainWindow_c::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls() and not actionsRunning_f())
    {
        event->acceptProposedAction();
    }
}

void mainWindow_c::dropEvent(QDropEvent* event)
{
    QStringList filesToLoadTmp;
    for (const QUrl& url_ite_con : event->mimeData()->urls())
    {
        if (url_ite_con.isLocalFile())
        {
            QString someLocalUrl(url_ite_con.toLocalFile());
            QFileInfo localItemInfoTmp(someLocalUrl);
            if (localItemInfoTmp.exists())
            {
                filesToLoadTmp.append(localItemInfoTmp.filePath());
            }
        }
    }
    if (not filesToLoadTmp.isEmpty())
    {
        loadFileList_f(filesToLoadTmp);
        //resizeFileTable_f();
    }
}

void mainWindow_c::mainLoop_f()
{
    if (finalCounterSeconds_pri == 0 and qThreadCount_f() == 0)
    {
        this->close();
    }
    if (not eines::signal::isRunning_f())
    {
        statusBarLabel_pri->setText(tr("Exiting..."));
        finalCounterSeconds_pri = finalCounterSeconds_pri - 1;
    }
//    QTimer::singleShot(0,[=]
//    {
//        //directoryComboBox_pri->adjustSize();
//        adjustSize();
    //    });
}

void mainWindow_c::showOptions_f()
{
    optionsWindow_c* optionsWindowTmp = new optionsWindow_c(this);
    optionsWindowTmp->setWindowFlag(Qt::Window, true);
    optionsWindowTmp->setWindowModality(Qt::WindowModal);
    optionsWindowTmp->show();
}

//NOT IN USE
void mainWindow_c::setRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con)
{
    actionsTable_pri->item(row_par_con, column_par_con)->setText(text_par_con);
}

//NOT IN USE
void mainWindow_c::setHashRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con)
{
    actionsTable_pri->item(row_par_con, column_par_con)->setText(text_par_con);
    if (not actionsTable_pri->item(row_par_con, column_par_con + 1)->text().isEmpty())
    {
           if (actionsTable_pri->item(row_par_con, column_par_con)->text() == actionsTable_pri->item(row_par_con, column_par_con + 1)->text())
        {
            actionsTable_pri->item(row_par_con, column_par_con+ 2)->setText("yes");
        }
        else
        {
            actionsTable_pri->item(row_par_con, column_par_con+ 2)->setText("no");
        }
    }
}

//NOT IN USE
void mainWindow_c::scrollToItem_f(const int row_par_con)
{
    actionsTable_pri->scrollToItem(actionsTable_pri->item(row_par_con, 0));
}

void mainWindow_c::showTips_f()
{
    plainQMessageBox_f
    (
                tr(
                    "<p>1 Action saved file/s can be dropped on the ActionQtg executable file and will be loaded at the start</p>"
                    "<p>2 Action saved file/s can be dropped on the ActionQtg windows and will be loaded</p>"
                    R"(<p>3 Quitting ActonQtg "normally" during an operation won't instantly exit, during execution, it will have to finish executing the current action, during saving, it will have to finish saving</p>)")
            , "Tips"
            , this
    );
}

void mainWindow_c::clearAllActions_f()
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Clear actions disallowed while actions are executing", "Error", this);
        return;
    }
    lastLoadedFilePath_pri.clear();
    actionsTable_pri->setRowCount(0);
    dataHub_f().clearAllActionData_f();
}

//FUTURE when I implement the execution dependency order
//allow loading multiple files but check if they use the same exeuction "orders" (sequential or dependency)
//and if dependecy check for nonexisting ones
void mainWindow_c::loadFileList_f(const QStringList& fileList_par_con)
{
    if (not fileList_par_con.isEmpty())
    {
        bool errorTmp(false);
        bool somethingLoaded(false);
        for (const QString& fileStr_ite_con : fileList_par_con)
        {
            QFile savedFile(fileStr_ite_con);
            if (not savedFile.exists())
            {
                statusBarLabel_pri->setText(tr("Save filed does not exist"));
                errorTmp = true;
                break;
            }

            QByteArray jsonByteArray;
            if (savedFile.open(QIODevice::ReadOnly))
            {
                jsonByteArray = savedFile.readAll();
            }
            else
            {
                statusBarLabel_pri->setText(tr("Could not open config file ") + savedFile.fileName());
                errorTmp = true;
                break;
            }

            auto jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
            bool thisFileLoadedSomething(false);
            if (jsonDocObj.isNull())
            {
                statusBarLabel_pri->setText(tr("Could not parse json from the save file"));
                errorTmp = true;
                break;
            }
            else
            {
                actionDataRoot_c actionDataRootObj;
                actionDataRootObj.read_f(jsonDocObj.object());
                actionDataRootObj.moveToDataHub_f();

                for (int rowIndex_ite = 0, l = dataHub_f().size_f(); rowIndex_ite < l; ++rowIndex_ite)
                {
                    updateActionRow_f(rowIndex_ite);
                    if (not eines::signal::isRunning_f())
                    {
                        break;
                    }
                }
                thisFileLoadedSomething = true;
            }
            if (thisFileLoadedSomething)
            {
                statusBarLabel_pri->setText(tr("Save file loaded " + fileStr_ite_con.toUtf8()));
                somethingLoaded = true;
                if (fileList_par_con.size() == 1)
                {
                    lastLoadedFilePath_pri = fileList_par_con.at(0);
                }
            }
            if (not eines::signal::isRunning_f())
            {
                break;
            }
        }
        if (not errorTmp and somethingLoaded)
        {
            //resizeFileTable_f();
            //statusBarLabel_pri->setText(tr("All save files loaded"));
        }
    }
    else
    {
        statusBarLabel_pri->setText(tr("No save files selected"));
    }
}

void mainWindow_c::loadActionsFileButtonPushed_f()
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Load disallowed while executing actions", "Error", this);
        return;
    }

    selectActionFilesToLoadDialog_pri = new QFileDialog(this);
    selectActionFilesToLoadDialog_pri->setFileMode(QFileDialog::ExistingFiles);
    selectActionFilesToLoadDialog_pri->setDirectory(QDir::currentPath());
    selectActionFilesToLoadDialog_pri->setWindowTitle(tr("Select action files..."));
    selectActionFilesToLoadDialog_pri->setViewMode(QFileDialog::Detail);
    selectActionFilesToLoadDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    selectActionFilesToLoadDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    selectActionFilesToLoadDialog_pri->setOption(QFileDialog::ReadOnly, true);
    selectActionFilesToLoadDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    selectActionFilesToLoad_pri->setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_f().directoryHistory_f());
    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryHistoryTmp.size());
    if (not directoryHistoryTmp.empty())
    {
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    selectActionFilesToLoadDialog_pri->setSidebarUrls(directoriesTmp);

    QObject::connect(selectActionFilesToLoadDialog_pri, &QFileDialog::finished, this, &mainWindow_c::fileDialogActionFilesToLoadFinished_f);

    selectActionFilesToLoadDialog_pri->show();
}

void mainWindow_c::fileDialogActionFilesToLoadFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not selectActionFilesToLoadDialog_pri->selectedFiles().isEmpty())
        {
            loadFileList_f(selectActionFilesToLoadDialog_pri->selectedFiles());
        }
    }
    selectActionFilesToLoadDialog_pri->deleteLater();
    selectActionFilesToLoadDialog_pri = Q_NULLPTR;
}

void mainWindow_c::saveActionFileButtonPushed_f()
{
    //QMessageBox::No --> don't overwrite (the last loaded file if there is any)
    //QMessageBox::Cancel --> cancel
    //QMessageBox::Yes --> overwrite

    //if no file was loaded previously don't ask and go straight to browse a file to save
    if (not lastLoadedFilePath_pri.isEmpty())
    {
        overwriteLastActionLoadedMessageBox_pri = new QMessageBox(this);
        overwriteLastActionLoadedMessageBox_pri->setText(R"(Overwrite loaded file?<br>Choosing "no" will save open a dialog to save a new file)");
        overwriteLastActionLoadedMessageBox_pri->setTextFormat(Qt::RichText);
        overwriteLastActionLoadedMessageBox_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        overwriteLastActionLoadedMessageBox_pri->setDefaultButton(QMessageBox::Yes);
        overwriteLastActionLoadedMessageBox_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(overwriteLastActionLoadedMessageBox_pri, &QFileDialog::finished, this, &mainWindow_c::messageBoxOverwriteLastActionLoadedFileFinished_f);

        overwriteLastActionLoadedMessageBox_pri->show();
    }
    else
    {
        messageBoxOverwriteLastActionLoadedFileFinished_f(QMessageBox::No);
    }
}

void mainWindow_c::messageBoxOverwriteLastActionLoadedFileFinished_f(const int result_par)
{
    //browse a file to save
    if (result_par == QMessageBox::No)
    {
        saveActionFileDialog_pri = new QFileDialog(this);
        saveActionFileDialog_pri->setAcceptMode(QFileDialog::AcceptSave);
        saveActionFileDialog_pri->setFileMode(QFileDialog::AnyFile);
        saveActionFileDialog_pri->setDirectory(QDir::currentPath());
        saveActionFileDialog_pri->selectFile("actions.json");
        saveActionFileDialog_pri->setWindowTitle(tr("Save action file..."));
        saveActionFileDialog_pri->setViewMode(QFileDialog::Detail);
        saveActionFileDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
        saveActionFileDialog_pri->setDefaultSuffix("json");
        saveActionFileDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
        saveActionFileDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
        saveDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

        std::vector<QString> directoryHistoryTmp(appConfig_f().directoryHistory_f());
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        if (not directoryHistoryTmp.empty())
        {
            for (const QString& directory_par_con : directoryHistoryTmp)
            {
                directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
            }
        }
        saveActionFileDialog_pri->setSidebarUrls(directoriesTmp);

        QObject::connect(saveActionFileDialog_pri, &QFileDialog::finished, this, &mainWindow_c::fileDialogSelectSaveActionFileFinished_f);

        saveActionFileDialog_pri->show();
    }
    if (result_par == QMessageBox::Yes)
    {
        saveActionFile_f(lastLoadedFilePath_pri);
    }
    if (overwriteLastActionLoadedMessageBox_pri != Q_NULLPTR)
    {
        overwriteLastActionLoadedMessageBox_pri->deleteLater();
        overwriteLastActionLoadedMessageBox_pri = Q_NULLPTR;
    }
}

void mainWindow_c::fileDialogSelectSaveActionFileFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        QString savePathTmp;
        if (not saveActionFileDialog_pri->selectedFiles().isEmpty())
        {
            savePathTmp = saveActionFileDialog_pri->selectedFiles().first();
            saveActionFile_f(savePathTmp);
        }
        else
        {
            //I don't think it will ever enter here...
            errorQMessageBox_f("Empty save filename", "Error", this);
        }
    }

    saveActionFileDialog_pri->deleteLater();
    saveActionFileDialog_pri = Q_NULLPTR;
}

void mainWindow_c::saveActionFile_f(const QString& savePath_par_con)
{
    //save using lastLoadedFilePath_pri
    QFile saveFile(savePath_par_con);
    if (saveFile.open(QIODevice::WriteOnly))
    {
        statusBarLabel_pri->setText(tr("Creating JSON save file..."));
        QJsonObject somethingJson;
        {
            std::vector<actionData_c> actionDataVectorTmp;
            actionDataVectorTmp.reserve(actionsTable_pri->rowCount());
            for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
            {
                auto actionDataIdTmp(dataHub_f().rowToActionDataId_f(rowIndex_ite));
                actionDataVectorTmp.emplace_back(dataHub_f().actionData_f(actionDataIdTmp).first);
            }
            actionDataRoot_c tmp(actionDataVectorTmp);
            tmp.write_f(somethingJson);
        }
        QJsonDocument somethingJsonD(somethingJson);
        saveFile.write(somethingJsonD.toJson(QJsonDocument::Indented));

        lastLoadedFilePath_pri = savePath_par_con;
        statusBarLabel_pri->setText(tr("File saved successfully"));
    }
    else
    {
        errorQMessageBox_f("Failed to save file: " + saveFile.errorString(), "Error", this);
    }
}

void mainWindow_c::addUpdateAction_f()
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Add/edit action disallowed while executing actions", "Error", this);
        return;
    }
    int rowTmp(-1);
    int_fast32_t actionDataId(0);
    if (not actionsTable_pri->selectedItems().isEmpty())
    {
        rowTmp = actionsTable_pri->selectedItems().first()->row();
        actionDataId = dataHub_f().rowToActionDataId_f(rowTmp);
    }
    else
    {
        rowTmp = actionsTable_pri->rowCount();
    }

    actionWindow_pri = new actionWindow_c(rowTmp, actionDataId, this);
    //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
    actionWindow_pri->setWindowFlag(Qt::Window, true);
    actionWindow_pri->setWindowModality(Qt::WindowModal);

    connect(actionWindow_pri, &actionWindow_c::updateRow_Signal, this, &mainWindow_c::updateActionRow_f);

    actionWindow_pri->show();
    //Q_EMIT setStatusBarText_signal_f("creating new window...");
}

void mainWindow_c::copyAction_f()
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Copy Action disallowed while actions are executing", "Error", this);
        return;
    }
    QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
    if (selectionTmp.isEmpty())
    {
        errorQMessageBox_f("No action row selected to copy", "Error", this);
        return;
    }

    std::unordered_set<int> rowIndexesTmp;
    for (const QTableWidgetItem* item_ite_con : selectionTmp)
    {
        rowIndexesTmp.emplace(item_ite_con->row());
    }

    if (rowIndexesTmp.size() > 1)
    {
        errorQMessageBox_f("Action row copy can only be applied to a single row", "Error", this);
        return;
    }

    int selectedRowTmp(selectionTmp.first()->row());

    actionData_c& actionDataSourceTmp(dataHub_f().actionData_f(dataHub_f().rowToActionDataId_f(selectedRowTmp)).first);

    copyActionIndexInputDialog_pri = new QInputDialog(this);
    copyActionIndexInputDialog_pri->setWindowTitle("Copy Action");
    copyActionIndexInputDialog_pri->setLabelText(
                "Input a new index to copy the action to.\nAction source:"
                "\nType: " + actionToStrUMap_glo_sta_con.at(actionDataSourceTmp.type_f()) +
                "\nDescription: " + actionDataSourceTmp.description_f().left(80) +
                "\nRow index: " + QString::number(selectedRowTmp)
    );
    copyActionIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
    copyActionIndexInputDialog_pri->setIntValue(selectedRowTmp);
    copyActionIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

    QObject::connect(copyActionIndexInputDialog_pri, &QFileDialog::finished, this, &mainWindow_c::inputDialogCopyActionIndexFinished_f);

    copyActionIndexInputDialog_pri->show();
}

void mainWindow_c::inputDialogCopyActionIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        int newIndexTmp(copyActionIndexInputDialog_pri->intValue());
        if (newIndexTmp >= 0 and newIndexTmp < actionsTable_pri->rowCount())
        {
            QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
            int selectedRowTmp(selectionTmp.first()->row());
            actionData_c& actionDataSourceTmp(dataHub_f().actionData_f(dataHub_f().rowToActionDataId_f(selectedRowTmp)).first);
            actionData_c copyTmp;
            copyTmp.setType_f(actionDataSourceTmp.type_f());
            copyTmp.setDescription_f(actionDataSourceTmp.description_f());
            copyTmp.setHaltOnFail_f(actionDataSourceTmp.haltOnFail_f());
            copyTmp.setActionDataJSON_f(actionDataSourceTmp.actionDataJSON_f());

            dataHub_f().insertActionData_f(copyTmp, newIndexTmp);
            insertActionRow_f(copyTmp.type_f(), copyTmp.description_f(), copyTmp.haltOnFail_f(), newIndexTmp);
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f("Wrong index to copy the row: " + QString::number(newIndexTmp), "Error", this);
        }
    }
    copyActionIndexInputDialog_pri->deleteLater();
    copyActionIndexInputDialog_pri = Q_NULLPTR;
}

void mainWindow_c::insertActionRow_f(
        const action_ec& actionType_par_con
        , const QString& description_par_con
        , const bool haltOnFail_par_con
        , const int row_par_con)
{
    QString actionTypeStr(actionToStrUMap_glo_sta_con.at(actionType_par_con));

    QTableWidgetItem *actionValueCellTmp;
    actionValueCellTmp = new QTableWidgetItem(actionTypeStr);
    actionValueCellTmp->setFlags(actionValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *descriptionCellTmp;
    descriptionCellTmp = new QTableWidgetItem(description_par_con);
    descriptionCellTmp->setFlags(descriptionCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *haltOnFailCellTmp;
    haltOnFailCellTmp = new QTableWidgetItem;
    Qt::CheckState checkValue(haltOnFail_par_con ? Qt::Checked : Qt::Unchecked);
    haltOnFailCellTmp->setCheckState(checkValue);
    haltOnFailCellTmp->setFlags(haltOnFailCellTmp->flags() bitand compl Qt::ItemIsEditable);


    int newIndexTmp(row_par_con);
    if (newIndexTmp == -1)
    {
        newIndexTmp = actionsTable_pri->rowCount();
    }

    actionsTable_pri->insertRow(newIndexTmp);
    actionsTable_pri->setItem(newIndexTmp, 0, actionValueCellTmp);
    actionsTable_pri->setItem(newIndexTmp, 1, descriptionCellTmp);
    actionsTable_pri->setItem(newIndexTmp, 2, haltOnFailCellTmp);
    //3 execution state, 4 last output, 5 last error
    actionsTable_pri->setItem(newIndexTmp, 3, new QTableWidgetItem);
    actionsTable_pri->setItem(newIndexTmp, 4, new QTableWidgetItem);
    actionsTable_pri->setItem(newIndexTmp, 5, new QTableWidgetItem);
}

void mainWindow_c::updateExistingActionRow_f(
        const action_ec& actionType_par_con
        , const QString& description_par_con
        , const bool haltOnFail_par_con
        , const int row_par_con)
{
    QString actionTypeStr(actionToStrUMap_glo_sta_con.at(actionType_par_con));

    actionsTable_pri->item(row_par_con, 0)->setText(actionTypeStr);
    actionsTable_pri->item(row_par_con, 1)->setText(description_par_con);
    Qt::CheckState checkValue(haltOnFail_par_con ? Qt::Checked : Qt::Unchecked);
    actionsTable_pri->item(row_par_con, 2)->setCheckState(checkValue);
}

void mainWindow_c::clearAllRowsResultColumns_f()
{
    for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        //3 execution state, 4 last output, 5 last error
        actionsTable_pri->item(rowIndex_ite, 3)->setText("");
        actionsTable_pri->item(rowIndex_ite, 4)->setText("");
        actionsTable_pri->item(rowIndex_ite, 5)->setText("");
    }
}


void mainWindow_c::updateActionRow_f(const int row_par_con)
{
    int_fast32_t actionDataId(dataHub_f().rowToActionDataId_f(row_par_con));
    actionData_c& actionDataTmp(dataHub_f().actionData_f(actionDataId).first);
    //qInfo() << "3 actionDataTmp.id_f() " << actionDataTmp.id_f() << endl;
    if (row_par_con == actionsTable_pri->rowCount())
    {
        insertActionRow_f(actionDataTmp.type_f(), actionDataTmp.description_f(), actionDataTmp.haltOnFail_f());
    }
    else
    {
        updateExistingActionRow_f(actionDataTmp.type_f(), actionDataTmp.description_f(), actionDataTmp.haltOnFail_f(), row_par_con);
    }
}

void mainWindow_c::moveSelectedActions_f(const int moveOffSet_par_con)
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Action moving disallowed while executing actions", "Error", this);
        return;
    }
    QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
    if (selectionTmp.isEmpty())
    {
        errorQMessageBox_f("Empty selection to move", "Error", this);
        return;
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
        if (destinationRow >= 0 and destinationRow < actionsTable_pri->rowCount())
        {
            dataHub_f().moveRowActionData(index_ite_con, destinationRow);
            //change the grid (visual)
            actionsTable_pri->removeRow(index_ite_con);
            actionData_c& actionDataTmp(dataHub_f().actionData_f(dataHub_f().rowToActionDataId_f(destinationRow)).first);
            insertActionRow_f(actionDataTmp.type_f(), actionDataTmp.description_f(), actionDataTmp.haltOnFail_f(), destinationRow);
            //deselects previous stuff, doesn't maintain "old" selection
            //actionsTable_pri->selectRow(destinationRow);
        }
    }
}

bool mainWindow_c::actionsRunning_f() const
{
    return runningActions_pri;
}

void mainWindow_c::moveSelectedActionsUpByOne_f()
{
    moveSelectedActions_f(-1);
}

void mainWindow_c::moveSelectedActionsDownByOne_f()
{
    moveSelectedActions_f(1);
}

void mainWindow_c::openChangeActionIndexWindow_f()
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Action index change disallowed while executing actions", "Error", this);
        return;
    }
    QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
    if (selectionTmp.isEmpty())
    {
        errorQMessageBox_f("No action row selected to change row index", "Error", this);
        return;
    }

    std::unordered_set<int> rowIndexesTmp;
    for (const QTableWidgetItem* item_ite_con : selectionTmp)
    {
        rowIndexesTmp.emplace(item_ite_con->row());
    }

    if (rowIndexesTmp.size() > 1)
    {
        errorQMessageBox_f("Action row index change can only be applied to a single row", "Error", this);
        return;
    }

    int currentIndexTmp(selectionTmp.first()->row());

    changeActionIndexInputDialog_pri = new QInputDialog(this);
    changeActionIndexInputDialog_pri->setWindowTitle("Change Action index");
    changeActionIndexInputDialog_pri->setLabelText("Input a new index to move the action to (current index: " + QString::number(currentIndexTmp) + ")");
    changeActionIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
    changeActionIndexInputDialog_pri->setIntValue(currentIndexTmp);
    changeActionIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

    QObject::connect(copyActionIndexInputDialog_pri, &QFileDialog::finished, this, &mainWindow_c::inputDialogCopyActionIndexFinished_f);

    changeActionIndexInputDialog_pri->show();
}

void mainWindow_c::inputDialogChangeActionIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
        int currentIndexTmp(selectionTmp.first()->row());
        int newIndexTmp(changeActionIndexInputDialog_pri->intValue());
        if (newIndexTmp != currentIndexTmp and (newIndexTmp >= 0 and newIndexTmp < actionsTable_pri->rowCount()))
        {
            //change the data
            dataHub_f().moveRowActionData(currentIndexTmp, newIndexTmp);
            //change the grid (visual)
            actionData_c& actionDataTmp(dataHub_f().actionData_f(dataHub_f().rowToActionDataId_f(newIndexTmp)).first);
            actionsTable_pri->removeRow(currentIndexTmp);
            insertActionRow_f(actionDataTmp.type_f(), actionDataTmp.description_f(), actionDataTmp.haltOnFail_f(), newIndexTmp);
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f("Same or wrong index to move the row: " + QString::number(newIndexTmp), "Error", this);
        }
    }
    changeActionIndexInputDialog_pri->deleteLater();
    changeActionIndexInputDialog_pri = Q_NULLPTR;
}

void mainWindow_c::updateActionOutput_f(const int_fast32_t actionId_par_con)
{
    std::pair<actionData_c&, bool> actionDataPairTmp(dataHub_f().actionData_f(actionId_par_con));
    if (not actionDataPairTmp.second)
    {
        return;
    }
    actionData_c& actionDataRef(actionDataPairTmp.first);
    //get the row, get the acionData Obj for the id (to get the executionResult Obj)
    //update the cell of the row with the last part of the str
    int rowTmp(dataHub_f().actionDataIdToRow_f(actionId_par_con));
    //action (type) 0 | description 1 | halt on fail 2 | Execution state 3 | Last output 4 | Last error 5
    actionsTable_pri->item(rowTmp, 4)->setText(actionDataRef.actionDataExecutionResultPtr_f()->externalOutput_f().right(32));
}

void mainWindow_c::updateActionError_f(const int_fast32_t actionId_par_con)
{
    std::pair<actionData_c&, bool> actionDataPairTmp(dataHub_f().actionData_f(actionId_par_con));
    if (not actionDataPairTmp.second)
    {
        return;
    }
    actionData_c& actionDataRef(actionDataPairTmp.first);
    int rowTmp(dataHub_f().actionDataIdToRow_f(actionId_par_con));
    //action (type) 0 | description 1 | halt on fail 2 | Execution state 3 | Last output 4 | Last error 5
    actionsTable_pri->item(rowTmp, 5)->setText(actionDataRef.actionDataExecutionResultPtr_f()->externalErrorOutput_f().right(32));
}

void mainWindow_c::updateActionExecutionState_f(const int_fast32_t actionId_par_con)
{
    std::pair<actionData_c&, bool> actionDataPairTmp(dataHub_f().actionData_f(actionId_par_con));
    if (not actionDataPairTmp.second)
    {
        return;
    }
    actionData_c& actionDataRef(actionDataPairTmp.first);
    QString actionExecutionStateStrTmp(actionExecutionStateToStrUMap_glo_sta_con.at(actionDataRef.actionDataExecutionResultPtr_f()->actionState_f()));
    int rowTmp(dataHub_f().actionDataIdToRow_f(actionId_par_con));
    //action (type) 0 | description 1 | halt on fail 2 | Execution state 3 | Last output 4 | Last error 5
    actionsTable_pri->item(rowTmp, 3)->setText(actionExecutionStateStrTmp.right(32));
}

void mainWindow_c::executeNextAction_f()
{
    if (not runningActionIds_pri.empty() and not haltRunningActions_pri)
    {
        const int_fast32_t actionIdToRunTmp(runningActionIds_pri.front());

        std::pair<actionData_c&, bool> actionDataRefPairTmp(dataHub_f().actionData_f(actionIdToRunTmp));
#ifdef DEBUGJOUVEN
        qDebug() << "actionIdToRunTmp " << actionIdToRunTmp << endl;
        qDebug() << "actionDataRefPairTmp.second " << actionDataRefPairTmp.second << endl;
#endif
        if (actionDataRefPairTmp.second)
        {
            actionData_c& actionDataRefTmp(actionDataRefPairTmp.first);

            actionDataExecutionResult_c* actionExecutionResultPtr = new actionDataExecutionResult_c(actionDataRefTmp);
            //FUTURE add warning when running an action again if there are results already?
            QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::executionStateUpdated_signal, this, &mainWindow_c::updateActionExecutionState_f);
            QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::outputUpdated_signal, this, &mainWindow_c::updateActionOutput_f);
            QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::errorUpdated_signal, this, &mainWindow_c::updateActionError_f);

            //on the grid only the above are used
            //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);
            //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);

            //when hitting any finish, order is important here:
            //if a halt happens stop execution, without allowing what's next
            //remove the action from the runningActionIds_pri (front)
            //signal to try to execute the next one (front)
            QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::halt_signal, this, &mainWindow_c::haltExecution_f);
            QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::anyFinish_signal, this, [this]()
            {
                if (not runningActionIds_pri.empty() and not haltRunningActions_pri)
                {
                    runningActionIds_pri.erase(runningActionIds_pri.begin());
                }
            });
            QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::anyFinish_signal, this, &mainWindow_c::executeNextAction_f);

            actionDataRefTmp.createExecution_f(actionExecutionResultPtr);
        }
    }
    else
    {
        executeActionsButton_pri->setToolTip("No selection executes all actions, selection executes selected actions");
        executeActionsButton_pri->setText("Execute Actions");
        runningActions_pri = false;
    }
}

void mainWindow_c::haltExecution_f()
{
    haltRunningActions_pri = true;
}

void mainWindow_c::removeActions_f()
{
    if (actionsRunning_f())
    {
        errorQMessageBox_f("Action removal disallowed while executing actions", "Error", this);
        return;
    }
    QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
    if (selectionTmp.isEmpty())
    {
        errorQMessageBox_f("No action rows selected for removal", "Error", this);
        return;
    }

    std::unordered_set<int> rowIndexUSetTmp;
    //get the selected row (indexes)
    for (const QTableWidgetItem* item_ite_con : selectionTmp)
    {
        rowIndexUSetTmp.emplace(item_ite_con->row());
    }

    for (const int item_ite_con : rowIndexUSetTmp)
    {
        actionsTable_pri->removeRow(item_ite_con);
        dataHub_f().removeActionDataUsingRow_f(item_ite_con);
    }
}

void mainWindow_c::executeActionsButtonPushed_f()
{
    if (haltRunningActions_pri)
    {
        runFromHaltedActionMessageBox_pri = new QMessageBox(this);
        runFromHaltedActionMessageBox_pri->setText(R"(Run from last unsuccessful action?)");
        //runFromHaltedActionMessageBox_pri->setTextFormat(Qt::RichText);
        runFromHaltedActionMessageBox_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        runFromHaltedActionMessageBox_pri->setDefaultButton(QMessageBox::Yes);
        runFromHaltedActionMessageBox_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(runFromHaltedActionMessageBox_pri, &QFileDialog::finished, this, &mainWindow_c::runFromHaltedActionsMessageBoxResult_f);

        runFromHaltedActionMessageBox_pri->show();
    }
    else
    {
        executeActions_f();
    }
}

void mainWindow_c::runFromHaltedActionsMessageBoxResult_f(const int result_par)
{
    if (result_par == QMessageBox::Cancel)
    {
        return;
    }
    if (result_par == QMessageBox::No)
    {
        runningActionIds_pri.clear();
    }
    runFromHaltedActionMessageBox_pri->deleteLater();
    runFromHaltedActionMessageBox_pri = Q_NULLPTR;
    executeActions_f();
}


void mainWindow_c::executeActions_f()
{
    //if already running, since the button changed to stop execution
    //clear the runnning queue and let the already running actions finish
    if (actionsRunning_f())
    {
        executeActionsButton_pri->setText("Stopping...");
        runningActionIds_pri.clear();
        return;
    }

    clearAllRowsResultColumns_f();

    //FUTURE timer or confirmation (and make it optional on an option)
    //to prevent users from executing unwanted stuff just by missclick
    QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
    //all
    if (selectionTmp.isEmpty())
    {
        for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
        {
            runningActionIds_pri.emplace_back(dataHub_f().rowToActionDataId_f(rowIndex_ite));
        }
    }
    else
        //selection
    {
        std::set<int> selectedRowsSetTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            selectedRowsSetTmp.emplace(item_ite_con->row());
            //runningActionIds_pri.emplace_back(dataHub_f().rowToActionDataId_f());
            //qDebug() << "dataHub_f().rowToActionDataId_f(item_ite_con->row()) " << dataHub_f().rowToActionDataId_f(item_ite_con->row()) << endl;
        }
        for (const int row_ite_con : selectedRowsSetTmp)
        {
            runningActionIds_pri.emplace_back(dataHub_f().rowToActionDataId_f(row_ite_con));
        }
    }

    //qDebug() << "runningActionIds_pri.size() " << runningActionIds_pri.size() << endl;
    if (not runningActionIds_pri.empty())
    {
        runningActions_pri = true;
        haltRunningActions_pri = false;
        executeActionsButton_pri->setText("Executing... Press again to Stop");
        executeActionsButton_pri->setToolTip("Will stop after finishing already executing action/s");
        //Q_EMIT executeNextAction_signal();
        executeNextAction_f();
    }
}

void mainWindow_c::showExecutionDetails_f()
{
    int rowTmp(-1);
    int_fast32_t actionDataId(0);
    if (not actionsTable_pri->selectedItems().isEmpty())
    {
        rowTmp = actionsTable_pri->selectedItems().first()->row();
        actionDataId = dataHub_f().rowToActionDataId_f(rowTmp);
        std::pair<actionData_c&, bool> actionDataTmp(dataHub_f().actionData_f(actionDataId));
        if (actionDataTmp.second)
        {
            if (actionDataTmp.first.actionDataExecutionResultPtr_f() == Q_NULLPTR)
            {
                errorQMessageBox_f("Action has no execution results", "Error", this);
                return;
            }
        }
    }
    else
    {
        errorQMessageBox_f("No action row selected", "Error", this);
        return;
    }

    actionExecutionDetailsWindow_c* actionExecutionDetailsWindowTmp = new actionExecutionDetailsWindow_c(actionDataId, this);
    //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
    actionExecutionDetailsWindowTmp->setWindowFlag(Qt::Window, true);
    actionExecutionDetailsWindowTmp->setWindowModality(Qt::WindowModal);
    actionExecutionDetailsWindowTmp->show();
}

void mainWindow_c::showAboutMessage_f()
{
    plainQMessageBox_f
    (
                tr(
                    "<p>ActonQtg</b> allows run actions in an organized maner i.e. (1) create directory, (2) copy stuff to this new directory... stopping if there are errors on any step or taking alternative paths</p>"
                    "<p>Creaded by: Jouven<br>"
                    R"(Source: <a href="//github.com/jouven/ActonQtg">github.com/jouven/ActonQtg</a><br>)"
                    R"(Homepage: <a href="//avidcalm.com">avidcalm.com</a></p>)"
                    )
                , "About actonQtg"
                , this
    );
}

//mainWindow_c& mainWindowRef_f()
//{
//    static mainWindow_c mainWindow_sta;
//    return mainWindow_sta;
//}
