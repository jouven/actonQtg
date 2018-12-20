//1 is QT_NO_* (like QT_NO_CLIPBOARD) neccessary anymore?, the define doesn't lead anywhere
//searching in google/qt site doesn't provide any platform that needs it
//2 add icons, right now in *nix it uses the current loaded theme ones, but in windows
//this will not fly
#include "mainWindow.hpp"

#include "actionWindow.hpp"
#include "actionExecutionDetailsWindow.hpp"
#include "appConfig.hpp"
#include "optionsWindow.hpp"
#include "executionOptionsWindow.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actionDataExecutionResult.hpp"
#include "actonQtso/actonDataHub.hpp"
#include "actonQtso/actonBaseSerialization.hpp"
#include "actonQtso/actionMappings/actionStrMapping.hpp"
#include "actonQtso/actionMappings/actionExecutionStateStrMapping.hpp"


#include "essentialQtgso/messageBox.hpp"
//just to check the thread count
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "signalso/signal.hpp"
//#include "sizeConversionso/byte.hpp"

#include <QtWidgets>

//#include <chrono>
#include <unordered_set>
#include <set>

void mainWindow_c::closeEvent(QCloseEvent* event)
{
    //1 check if anything is running
    //1A nothing running, quit normally
    //1B something running --> ask user
    //--> cancel (do nothing and not close-quit)
    //--> stop (basically hit stop button and connect the end to a function that closes after)
    //--> stop and after timeout kill, in general this shouldn't be necessary but all the actions like "runProcess" that depend on external
    //processes will require this, but runProcess already has a kill option (QProcess has).
    //AND even thougth this program doesn't have this case yet, there might be unkillable actions,
    //like a "data move" (if it's not implemented like a copy first + erase at the end)
    bool evenAcceptedTmp(false);
    while (true)
    {
        if (actonDataHub_ptr_ext->stoppingActionsExecution_f() and not actonDataHub_ptr_ext->killingActionsExecution_f())
        {
            MACRO_ADDACTONQTGLOG("Close while stopping actions", logItem_c::type_ec::info);
            createMessageBoxAskAboutStoppingExecutionOnClose_f();
            break;
        }

        //the next 2 ifs are mutually exclusive,
        //running checks individually can't happen at the same time of running actions, the reverse can't happen either

        //ask to cancel, stop or kill
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            if (killCountdown_pri > 0)
            {
                break;
            }
            MACRO_ADDACTONQTGLOG("Close while executing actions", logItem_c::type_ec::info);
            createMessageBoxAskAboutExecutingActionsOnClose_f();
            break;
        }

        if (eines::signal::isRunning_f())
        {
            eines::signal::stopRunning_f();
        }

        if (finalCounterSeconds_pri == 0 and threadedFunction_c::qThreadCount_f() == 0)
        {
            MACRO_ADDACTONQTGLOG("Saving geometry + saving config file/s + clearAllActionData_f, then close", logItem_c::type_ec::info);
            appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionsTable_pri->objectName() + actionsTable_pri->horizontalHeader()->objectName(), actionsTable_pri->horizontalHeader()->saveState());
            appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionsTable_pri->objectName(), actionsTable_pri->saveGeometry());
            appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
            MACRO_ADDACTONQTGLOG("Geometry saved", logItem_c::type_ec::debug);
            appConfig_ptr_ext->saveConfigFile_f();
            MACRO_ADDACTONQTGLOG("Config file/s saved", logItem_c::type_ec::debug);
            //IMPORTANT because otherwise the dtor of the actions in the datahub happens when the qtapp instance has already been dtored
            //because qtapp is a variable in the "main" but actonDataHub_f() has a static variable behind which gets dtored later
            actonDataHub_ptr_ext->clearAllActionData_f();
            MACRO_ADDACTONQTGLOG("clearAllActionData_f", logItem_c::type_ec::debug);
            evenAcceptedTmp = true;
        }
        else
        {
            if (threadedFunction_c::qThreadCount_f() > 0)
            {
                MACRO_ADDACTONQTGLOG("Nothing running but qThreads exist, count: " + QString::number(threadedFunction_c::qThreadCount_f()), logItem_c::type_ec::info);
            }
        }
        break;
    }
    if (evenAcceptedTmp)
    {
        Q_EMIT close_signal();
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

    //new, load and save buttons
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //use QTabWidget? FUTURE?
    //new config file
    QIcon newIcon(QIcon::fromTheme("document-new", QIcon(":/images/new.png")));
    QPushButton* newConfigFileButtonTmp = new QPushButton(newIcon, appConfig_ptr_ext->translate_f("New"));
    newConfigFileButtonTmp->setToolTip("Clears all loaded actions and resets action execution settings");
    firstRowLayoutTmp->addWidget(newConfigFileButtonTmp);
    QObject::connect(newConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::clearAllActions_f);

    //load config file
    QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/browse.png"));
    QPushButton* loadConfigFileButtonTmp = new QPushButton(loadIcon, appConfig_ptr_ext->translate_f("Load"));
    loadConfigFileButtonTmp->setToolTip("Load actions and execution options from a JSON file");
    firstRowLayoutTmp->addWidget(loadConfigFileButtonTmp);
    QObject::connect(loadConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::loadActionsFileButtonPushed_f);

    //save config file
    QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QPushButton* saveConfigFileButtonTmp = new QPushButton(saveIcon, appConfig_ptr_ext->translate_f("Save"));
    saveConfigFileButtonTmp->setToolTip("Saves actions and execution options in a JSON file");
    firstRowLayoutTmp->addWidget(saveConfigFileButtonTmp);
    QObject::connect(saveConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::saveActionFileButtonPushed_f);

    //add/edit action, remove actions and copy actions
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    //add/edit action
    //QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/hash.png"));
    QPushButton* addEditActionButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Add/Edit Action"));
    secondRowLayoutTmp->addWidget(addEditActionButtonTmp);
    QObject::connect(addEditActionButtonTmp, &QPushButton::clicked, this, &mainWindow_c::addUpdateAction_f);

    //remove actions
    QPushButton* removeActionsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Remove Actions"));
    secondRowLayoutTmp->addWidget(removeActionsButtonTmp);
    QObject::connect(removeActionsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::removeActions_f);

    //copy action
    QPushButton* copyActionButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Copy Action"));
    secondRowLayoutTmp->addWidget(copyActionButtonTmp);
    QObject::connect(copyActionButtonTmp, &QPushButton::clicked, this, &mainWindow_c::copyAction_f);

    //execution options, execute actions and execution details
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    //execution options
    QPushButton* executionOptionsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Execution &options"));
    thirdRowLayoutTmp->addWidget(executionOptionsButtonTmp);
    QObject::connect(executionOptionsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showExecutionOptionsButtonClicked_f);

    //execute/stop actions
    executeActionsButton_pri = new QPushButton(appConfig_ptr_ext->translate_f("&Execute Actions"));
    executeActionsButton_pri->setToolTip("No selection executes all actions, selection executes selected actions");
    thirdRowLayoutTmp->addWidget(executeActionsButton_pri);
    QObject::connect(executeActionsButton_pri, &QPushButton::clicked, this, &mainWindow_c::executeActionsButtonClicked_f);

    QPushButton* executionDetailsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Execution &details"));
    thirdRowLayoutTmp->addWidget(executionDetailsButtonTmp);
    QObject::connect(executionDetailsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showExecutionDetails_f);

    //tips, about buttons and program options
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    //options button
    QPushButton* optionsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Options"));
    fourthRowLayoutTmp->addWidget(optionsButtonTmp);
    QObject::connect(optionsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showOptions_f);

    //tips button
    QPushButton* tipsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    fourthRowLayoutTmp->addWidget(tipsButtonTmp);
    QObject::connect(tipsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showTips_f);

    //about button
    QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/images/about.png"));
    QPushButton* aboutButtonTmp = new QPushButton(aboutIcon, appConfig_ptr_ext->translate_f("About"));
    fourthRowLayoutTmp->addWidget(aboutButtonTmp);
    QObject::connect(aboutButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showAboutMessage_f);

    actionsTable_pri = new QTableWidget(0, 5);
    actionsTable_pri->setObjectName("QTableWidget");
    actionsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    //action (type) 0 | description 1 |  Execution state 2 | Last output 3 | Last error 4
    labels << appConfig_ptr_ext->translate_f("Action") << appConfig_ptr_ext->translate_f("Description") << appConfig_ptr_ext->translate_f("Execution state") << appConfig_ptr_ext->translate_f("Last output") << appConfig_ptr_ext->translate_f("Last error");
    actionsTable_pri->setHorizontalHeaderLabels(labels);
    actionsTable_pri->horizontalHeader()->setObjectName("QHeaderView");
    actionsTable_pri->setShowGrid(true);
    //TODO on android there is no right mouse button (touch holding or doing gestures doesn't make it) by default
    //so no context menus
    //commandsTable_pri->setContextMenuPolicy(Qt::CustomContextMenu);
    actionsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    actionsTable_pri->setMinimumHeight(screenGeometry.height() / 3);
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

    setWindowTitle("actonQtg");

    //TODO android has bugs, scrollto has no effect
#ifdef __ANDROID__
    baseWidget_pri->resize(scrollArea_pri->width() * 0.96, scrollArea_pri->height() * 0.95);
#endif
    //resize(screenGeometry.width(), screenGeometry.height() * 0.99);

    auto* mainLoopTimer = new QTimer(this);
    QObject::connect(mainLoopTimer, &QTimer::timeout, this, &mainWindow_c::mainLoop_f);
    mainLoopTimer->start(500);

    if (appConfig_ptr_ext->configLoaded_f())
    {
        //qInfo() << "configFile_f().second" << endl;
        restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
        actionsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionsTable_pri->objectName()));
        actionsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionsTable_pri->objectName() + actionsTable_pri->horizontalHeader()->objectName()));
    }

    //TODO some of these aren't in use
    //QObject::connect(this, &mainWindow_c::setStatusBarText_signal, statusBarLabel_pri, &QLabel::setText);
    QObject::connect(this, &mainWindow_c::scrollToItem_signal, this, &mainWindow_c::scrollToItem_f);
    QObject::connect(this, &mainWindow_c::setRowCellField_signal, this, &mainWindow_c::setRowCellField_f);
    QObject::connect(this, &mainWindow_c::setHashRowCellField_signal, this, &mainWindow_c::setHashRowCellField_f);
    //QObject::connect(this, &Window_c::saveAfterHash_signal_f, this, &Window_c::dialogSaveFileList_f);
    //QObject::connect(this, &mainWindow_c::resizeFileTable_signal, this, &mainWindow_c::resizeFileTable_f);

    QObject::connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionStarted_signal, this, &mainWindow_c::executionStarted_f);
    QObject::connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::stoppingActionsExecution_signal, this, &mainWindow_c::stoppingExecution_f);
    QObject::connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionStopped_signal, this, &mainWindow_c::executionStopped_f);
    QObject::connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionFinished_signal, this, &mainWindow_c::executionFinished_f);

    //FUTURE create a meta library with the log file data reads the actual source file and shows the code around the log add line
    //would require the program to be shipped with the source, but it would make debugging easier
    if (not appConfig_ptr_ext->commandLinePositionalArguments_f().isEmpty())
    {
        MACRO_ADDACTONQTGLOG(R"(Load positional arguments as "acton" files)", logItem_c::type_ec::info);
        loadFileList_f(appConfig_ptr_ext->commandLinePositionalArguments_f());
    }
    MACRO_ADDACTONQTGLOG("Main window ctored", logItem_c::type_ec::debug);
}

void mainWindow_c::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls() and not actonDataHub_ptr_ext->executingActions_f())
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
    if (finalCounterSeconds_pri == 0 and threadedFunction_c::qThreadCount_f() == 0)
    {
        this->close();
    }
    if (not eines::signal::isRunning_f())
    {
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Exiting..."));
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
                appConfig_ptr_ext->translate_f(
                    "<p>1 Action saved file/s can be dropped on the ActionQtg executable file and will be loaded at the start</p>"
                    "<p>2 Action saved file/s can be dropped on the ActionQtg windows and will be loaded</p>"
                    R"(<p>3 Quitting ActonQtg "normally" during an operation won't instantly exit, during execution, it will have to finish executing the current action, during saving, it will have to finish saving</p>)")
            , "Tips"
            , this
    );
}

void mainWindow_c::clearAllActions_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Clear actions disallowed while actions are executing", "Error", this);
            break;
        }
        lastLoadedFilePath_pri.clear();
        actionsTable_pri->setRowCount(0);
        actonDataHub_ptr_ext->clearAllActionData_f();
        break;
    }
}

//FUTURE when I implement the execution dependency order
//allow loading multiple files but check if they use the same exeuction "orders" (sequential or dependency)
//and if dependecy check for nonexisting ones
void mainWindow_c::loadFileList_f(const QStringList& fileList_par_con)
{
    MACRO_ADDACTONQTGLOG("Load file list (for acton files with actions)", logItem_c::type_ec::debug);
    if (not fileList_par_con.isEmpty())
    {
        bool errorTmp(false);
        bool somethingLoaded(false);
        for (const QString& fileStr_ite_con : fileList_par_con)
        {
            QFile savedFile(fileStr_ite_con);
            if (not savedFile.exists())
            {
                statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Save file does not exist"));
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
                statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Could not open config file ") + savedFile.fileName());
                errorTmp = true;
                break;
            }

            auto jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
            bool thisFileLoadedSomething(false);
            if (jsonDocObj.isNull())
            {
                statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Could not parse json from the save file"));
                errorTmp = true;
                break;
            }
            else
            {
                MACRO_ADDACTONQTGLOG("Deserializing file " + fileStr_ite_con + " JSON", logItem_c::type_ec::debug);
                deserializeAndCopyToActonDataHub_f(jsonDocObj.object());
                MACRO_ADDACTONQTGLOG("Found " + QString::number(actonDataHub_ptr_ext->size_f()) + " actions", logItem_c::type_ec::debug);
                for (int rowIndex_ite = 0, l = actonDataHub_ptr_ext->size_f(); rowIndex_ite < l; ++rowIndex_ite)
                {
//#ifdef DEBUGJOUVEN
//                    int_fast64_t actionIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(rowIndex_ite));
//                    qDebug() << "actionIdTmp " << actionIdTmp << endl;
//                    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(actionIdTmp));
//                    qDebug() << "rowTmp " << rowTmp << endl;
//        //qDebug() << "actionDataRefPairTmp.second " << actionDataRefPairTmp.second << endl;
//#endif
                    updateActionRow_f(rowIndex_ite);
                    if (not eines::signal::isRunning_f())
                    {
                        break;
                    }
                }
                thisFileLoadedSomething = actonDataHub_ptr_ext->size_f() > 0;
            }
            if (thisFileLoadedSomething)
            {
                MACRO_ADDACTONQTGLOG("Save file loaded " + fileStr_ite_con, logItem_c::type_ec::info);
                //20181121 was fileStr_ite_con.toUtf8(), why?
                statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Save file loaded " + fileStr_ite_con));
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
            //statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("All save files loaded"));
        }
    }
    else
    {
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("No save files selected"));
    }
}

void mainWindow_c::loadActionsFileButtonPushed_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Load disallowed while executing actions", "Error", this);
            break;
        }

        selectActionFilesToLoadDialog_pri = new QFileDialog(this);
        selectActionFilesToLoadDialog_pri->setFileMode(QFileDialog::ExistingFiles);
        selectActionFilesToLoadDialog_pri->setDirectory(QDir::currentPath());
        selectActionFilesToLoadDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select action files..."));
        selectActionFilesToLoadDialog_pri->setViewMode(QFileDialog::Detail);
        selectActionFilesToLoadDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
        selectActionFilesToLoadDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
        selectActionFilesToLoadDialog_pri->setOption(QFileDialog::ReadOnly, true);
        selectActionFilesToLoadDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
        selectActionFilesToLoad_pri->setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

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
        selectActionFilesToLoadDialog_pri->setSidebarUrls(directoriesTmp);

        QObject::connect(selectActionFilesToLoadDialog_pri, &QFileDialog::finished, this, &mainWindow_c::fileDialogActionFilesToLoadFinished_f);

        selectActionFilesToLoadDialog_pri->show();
        break;
    }
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
    selectActionFilesToLoadDialog_pri = nullptr;
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

        QObject::connect(overwriteLastActionLoadedMessageBox_pri, &QMessageBox::finished, this, &mainWindow_c::messageBoxOverwriteLastActionLoadedFileFinished_f);

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
        saveActionFileDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Save action file..."));
        saveActionFileDialog_pri->setViewMode(QFileDialog::Detail);
        saveActionFileDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
        saveActionFileDialog_pri->setDefaultSuffix("json");
        saveActionFileDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
        saveActionFileDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
        saveDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

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
        saveActionFileDialog_pri->setSidebarUrls(directoriesTmp);

        QObject::connect(saveActionFileDialog_pri, &QFileDialog::finished, this, &mainWindow_c::fileDialogSelectSaveActionFileFinished_f);

        saveActionFileDialog_pri->show();
    }
    if (result_par == QMessageBox::Yes)
    {
        saveActionFile_f(lastLoadedFilePath_pri);
    }
    if (overwriteLastActionLoadedMessageBox_pri not_eq nullptr)
    {
        overwriteLastActionLoadedMessageBox_pri->deleteLater();
        overwriteLastActionLoadedMessageBox_pri = nullptr;
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
    saveActionFileDialog_pri = nullptr;
}

void mainWindow_c::saveActionFile_f(const QString& savePath_par_con)
{
    //save using lastLoadedFilePath_pri
    QFile saveFile(savePath_par_con);
    if (saveFile.open(QIODevice::WriteOnly))
    {
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Creating JSON save file..."));

        QJsonDocument somethingJsonD(copyFromActonDataHubAndSerialize_f());
        saveFile.write(somethingJsonD.toJson(QJsonDocument::Indented));

        lastLoadedFilePath_pri = savePath_par_con;
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("File saved successfully"));
    }
    else
    {
        errorQMessageBox_f("Failed to save file: " + saveFile.errorString(), "Error", this);
    }
}

void mainWindow_c::addUpdateAction_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Add/edit action disallowed while executing actions", "Error", this);
            break;
        }
        int rowTmp(-1);
        //if the selection is empty use the last row index
        if (actionsTable_pri->selectedItems().isEmpty())
        {
            rowTmp = actionsTable_pri->rowCount();
        }
        else
            //else get the first selected row
        {
            rowTmp = actionsTable_pri->selectedItems().first()->row();
        }

        actionWindow_c* actionWindowTmp(new actionWindow_c(rowTmp, this));
        //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
        actionWindowTmp->setWindowFlag(Qt::Window, true);
        actionWindowTmp->setWindowModality(Qt::WindowModal);

        connect(actionWindowTmp, &actionWindow_c::updateRow_Signal, this, &mainWindow_c::updateActionRow_f);

        actionWindowTmp->show();
        break;
    }
}

void mainWindow_c::copyAction_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Copy Action disallowed while actions are executing", "Error", this);
            break;
        }

        QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No action row selected to copy", "Error", this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f("Action row copy can only be applied to a single row", "Error", this);
            break;
        }

        int selectedRowTmp(selectionTmp.first()->row());

        actionData_c* actionDataSourcePtrTmp(actonDataHub_ptr_ext->actionData_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(selectedRowTmp)));

        copyActionIndexInputDialog_pri = new QInputDialog(this);
        copyActionIndexInputDialog_pri->setWindowTitle("Copy Action");
        copyActionIndexInputDialog_pri->setLabelText(
                    "Input a new index to copy the action to.\nAction source:"
                    "\nType: " + actionTypeToStrUMap_glo_sta_con.at(actionDataSourcePtrTmp->type_f()) +
                    "\nDescription: " + actionDataSourcePtrTmp->description_f().left(80) +
                    "\nRow index: " + QString::number(selectedRowTmp)
                    );
        copyActionIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        copyActionIndexInputDialog_pri->setIntValue(selectedRowTmp);
        copyActionIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(copyActionIndexInputDialog_pri, &QFileDialog::finished, this, &mainWindow_c::inputDialogCopyActionIndexFinished_f);

        copyActionIndexInputDialog_pri->show();
        break;
    }
}

void mainWindow_c::inputDialogCopyActionIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        int newIndexTmp(copyActionIndexInputDialog_pri->intValue());
        if (newIndexTmp >= 0 and newIndexTmp <= actionsTable_pri->rowCount())
        {
            QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
            int selectedRowTmp(selectionTmp.first()->row());
            actionData_c actionDataCopyTmp;
            actionDataCopyTmp = *(actonDataHub_ptr_ext->actionData_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(selectedRowTmp)));

            actionDataCopyTmp.setStringId_f(actionDataCopyTmp.stringId_f() + "_copy");

            actonDataHub_ptr_ext->insertActionData_f(actionDataCopyTmp, newIndexTmp);
            insertActionRow_f(
                        actionDataCopyTmp.type_f()
                        , actionDataCopyTmp.description_f()
                        //, copyTmp.haltOnFail_f()
                        , newIndexTmp
            );
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f("Wrong index to copy the row: " + QString::number(newIndexTmp), "Error", this);
        }
    }
    copyActionIndexInputDialog_pri->deleteLater();
    copyActionIndexInputDialog_pri = nullptr;
}

void mainWindow_c::insertActionRow_f(
        const actionType_ec& actionType_par_con
        , const QString& description_par_con
        //, const bool haltOnFail_par_con
        , const int row_par_con)
{
    QString actionTypeStr(actionTypeToStrUMap_glo_sta_con.at(actionType_par_con));

    QTableWidgetItem *actionValueCellTmp(new QTableWidgetItem(actionTypeStr));
    actionValueCellTmp->setFlags(actionValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *descriptionCellTmp(new QTableWidgetItem(description_par_con));
    descriptionCellTmp->setFlags(descriptionCellTmp->flags() bitand compl Qt::ItemIsEditable);

    int newIndexTmp(row_par_con);
    if (newIndexTmp == -1)
    {
        newIndexTmp = actionsTable_pri->rowCount();
    }

    //action (type) 0 | description 1 | Execution state 2 | Last output 3 | Last error 4
    actionsTable_pri->insertRow(newIndexTmp);
    actionsTable_pri->setItem(newIndexTmp, 0, actionValueCellTmp);
    actionsTable_pri->setItem(newIndexTmp, 1, descriptionCellTmp);
    //actionsTable_pri->setItem(newIndexTmp, 2, haltOnFailCellTmp);
    //2 execution state, 3 last output, 4 last error
    actionsTable_pri->setItem(newIndexTmp, 2, new QTableWidgetItem);
    actionsTable_pri->setItem(newIndexTmp, 3, new QTableWidgetItem);
    actionsTable_pri->setItem(newIndexTmp, 4, new QTableWidgetItem);
}

void mainWindow_c::updateExistingActionRow_f(
        const actionType_ec& actionType_par_con
        , const QString& description_par_con
        //, const bool haltOnFail_par_con
        , const int row_par_con)
{
    QString actionTypeStr(actionTypeToStrUMap_glo_sta_con.at(actionType_par_con));

    actionsTable_pri->item(row_par_con, 0)->setText(actionTypeStr);
    actionsTable_pri->item(row_par_con, 1)->setText(description_par_con);
    //Qt::CheckState checkValue(haltOnFail_par_con ? Qt::Checked : Qt::Unchecked);
    //actionsTable_pri->item(row_par_con, 2)->setCheckState(checkValue);
}

void mainWindow_c::clearAllRowsResultColumns_f()
{
    for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        //2 execution state, 3 last output, 4 last error
        actionsTable_pri->item(rowIndex_ite, 2)->setText("");
        actionsTable_pri->item(rowIndex_ite, 3)->setText("");
        actionsTable_pri->item(rowIndex_ite, 4)->setText("");
    }
}

void mainWindow_c::createMessageBoxAskAboutExecutingActionsOnClose_f()
{
    askAboutExecutingActionsOnCloseMessageBox_pri = new QMessageBox(this);
    askAboutExecutingActionsOnCloseMessageBox_pri->setText(R"(Action/s are executing. What to do?)");
    askAboutExecutingActionsOnCloseMessageBox_pri->setTextFormat(Qt::RichText);
    askAboutExecutingActionsOnCloseMessageBox_pri->setWindowModality(Qt::WindowModal);
    askAboutExecutingActionsOnCloseMessageBox_pri->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton* cancelButton = new QPushButton(appConfig_ptr_ext->translate_f("Nothing"));
    cancelButton->setToolTip(appConfig_ptr_ext->translate_f("Close this window"));
    connect(cancelButton, &QPushButton::clicked, askAboutExecutingActionsOnCloseMessageBox_pri, &QMessageBox::close);
    askAboutExecutingActionsOnCloseMessageBox_pri->addButton(cancelButton, QMessageBox::ButtonRole::AcceptRole);

    QPushButton* stopExecutionButton = new QPushButton(appConfig_ptr_ext->translate_f("Stop execution"));
    stopExecutionButton->setToolTip(appConfig_ptr_ext->translate_f("Stop the execution and, after it has stopped, exit"));
    connect(stopExecutionButton, &QPushButton::clicked, this, &mainWindow_c::stopExecutingActionsAndClose_f);
    askAboutExecutingActionsOnCloseMessageBox_pri->addButton(stopExecutionButton, QMessageBox::ButtonRole::AcceptRole);

    QPushButton* stopAndKillExecutionButton = new QPushButton(appConfig_ptr_ext->translate_f("Stop execution else kill"));
    stopAndKillExecutionButton->setToolTip(appConfig_ptr_ext->translate_f("Stop the execution and, after a timeout if the stop didn't work kill the execution, then exit"));
    connect(stopAndKillExecutionButton, &QPushButton::clicked, this, &mainWindow_c::stopExecutingActionsElseKillAndClose_f);
    askAboutExecutingActionsOnCloseMessageBox_pri->addButton(stopAndKillExecutionButton, QMessageBox::ButtonRole::AcceptRole);

    askAboutExecutingActionsOnCloseMessageBox_pri->show();
}

void mainWindow_c::createMessageBoxAskAboutStoppingExecutionOnClose_f()
{
    askAboutStoppingExecutionOnCloseMessageBox_pri = new QMessageBox(this);
    askAboutStoppingExecutionOnCloseMessageBox_pri->setText(R"(Execution is stopping. What to do?)");
    askAboutStoppingExecutionOnCloseMessageBox_pri->setTextFormat(Qt::RichText);
    askAboutStoppingExecutionOnCloseMessageBox_pri->setWindowModality(Qt::WindowModal);
    askAboutStoppingExecutionOnCloseMessageBox_pri->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton* cancelButton = new QPushButton(appConfig_ptr_ext->translate_f("Nothing"));
    cancelButton->setToolTip(appConfig_ptr_ext->translate_f("Close this window"));
    connect(cancelButton, &QPushButton::clicked, askAboutStoppingExecutionOnCloseMessageBox_pri, &QMessageBox::close);
    askAboutStoppingExecutionOnCloseMessageBox_pri->addButton(cancelButton, QMessageBox::ButtonRole::AcceptRole);

    QPushButton* stopExecutionButton = new QPushButton(appConfig_ptr_ext->translate_f("Kill execution now"));
    stopExecutionButton->setToolTip(appConfig_ptr_ext->translate_f("Kill execution now"));
    connect(stopExecutionButton, &QPushButton::clicked, this, &mainWindow_c::killExecutingActionsAndClose_f);
    askAboutStoppingExecutionOnCloseMessageBox_pri->addButton(stopExecutionButton, QMessageBox::ButtonRole::AcceptRole);

    askAboutStoppingExecutionOnCloseMessageBox_pri->show();
}


void mainWindow_c::updateActionRow_f(const int row_par_con)
{
    int_fast32_t actionDataId(actonDataHub_ptr_ext->rowToActionDataId_f(row_par_con));
    actionData_c* actionDataPtrTmp(actonDataHub_ptr_ext->actionData_ptr_f(actionDataId));
    //qInfo() << "3 actionDataTmp.description_f() " << actionDataTmp.description_f() << endl;
    if (row_par_con == actionsTable_pri->rowCount())
    {
        //qInfo() << "insert action row " << endl;
        insertActionRow_f(
                    actionDataPtrTmp->type_f()
                    , actionDataPtrTmp->description_f()
                    //, actionDataTmp.haltOnFail_f()
        );
    }
    else
    {
        //qInfo() << "update row " << row_par_con << endl;
        updateExistingActionRow_f(
                    actionDataPtrTmp->type_f()
                    , actionDataPtrTmp->description_f()
                    //, actionDataTmp.haltOnFail_f()
                    , row_par_con
        );
    }
}

void mainWindow_c::moveSelectedActions_f(const int moveOffSet_par_con)
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Action moving disallowed while executing actions", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
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
            if (destinationRow >= 0 and destinationRow < actionsTable_pri->rowCount())
            {
                actonDataHub_ptr_ext->moveRowActionData_f(index_ite_con, destinationRow);
                //change the grid (visual)
                actionsTable_pri->removeRow(index_ite_con);
                actionData_c* actionDataTmp(actonDataHub_ptr_ext->actionData_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(destinationRow)));
                insertActionRow_f(
                            actionDataTmp->type_f()
                            , actionDataTmp->description_f()
                            //, actionDataTmp.haltOnFail_f()
                            , destinationRow
                );
                //deselects previous stuff, doesn't maintain "old" selection
                //actionsTable_pri->selectRow(destinationRow);
            }
        }
        break;
    }
}

//bool mainWindow_c::actonDataHub_ptr_ext->executingActions_f() const
//{
//    return runningActions_pri;
//}

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
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Action index change disallowed while executing actions", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No action row selected to change row index", "Error", this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f("Action row index change can only be applied to a single row", "Error", this);
            break;
        }

        int currentIndexTmp(selectionTmp.first()->row());

        changeActionIndexInputDialog_pri = new QInputDialog(this);
        changeActionIndexInputDialog_pri->setWindowTitle("Change Action index");
        changeActionIndexInputDialog_pri->setLabelText("Input a new index to move the action to (current index: " + QString::number(currentIndexTmp) + ")");
        changeActionIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        changeActionIndexInputDialog_pri->setIntValue(currentIndexTmp);
        changeActionIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(changeActionIndexInputDialog_pri, &QFileDialog::finished, this, &mainWindow_c::inputDialogChangeActionIndexFinished_f);

        changeActionIndexInputDialog_pri->show();
    }
}

void mainWindow_c::inputDialogChangeActionIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
        int currentIndexTmp(selectionTmp.first()->row());
        int newIndexTmp(changeActionIndexInputDialog_pri->intValue());
        if (newIndexTmp not_eq currentIndexTmp and (newIndexTmp >= 0 and newIndexTmp < actionsTable_pri->rowCount()))
        {
            //change the data
            actonDataHub_ptr_ext->moveRowActionData_f(currentIndexTmp, newIndexTmp);
            //change the grid (visual)
            actionData_c* actionDataTmp(actonDataHub_ptr_ext->actionData_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(newIndexTmp)));
            actionsTable_pri->removeRow(currentIndexTmp);
            insertActionRow_f(
                        actionDataTmp->type_f()
                        , actionDataTmp->description_f()
                        //, actionDataTmp->haltOnFail_f()
                        , newIndexTmp
            );
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f("Same or wrong index to move the row: " + QString::number(newIndexTmp), "Error", this);
        }
    }
    changeActionIndexInputDialog_pri->deleteLater();
    changeActionIndexInputDialog_pri = nullptr;
}

void mainWindow_c::updateActionOutput_f(actionData_c* actionData_par_ptr_con)
{
    //get the row, get the acionData Obj for the id (to get the executionResult Obj)
    //update the cell of the row with the last part of the str
    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(actionData_par_ptr_con->id_f()));
    //action (type) 0 | description 1 | Execution state 2 | Last output 3 | Last error 4
    actionsTable_pri->item(rowTmp, 3)->setText(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->output_f().left(32));
}

void mainWindow_c::updateActionError_f(actionData_c* actionData_par_ptr_con)
{
    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(actionData_par_ptr_con->id_f()));
    //action (type) 0 | description 1 |  Execution state 2 | Last output 3 | Last error 4
    actionsTable_pri->item(rowTmp, 4)->setText(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->error_f().left(32));
}

void mainWindow_c::updateActionExecutionState_f(actionData_c* actionData_par_ptr_con)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "actionData_par_con.id_f() " << QString::number(actionData_par_ptr_con->id_f()) << endl;
#endif
    QString actionExecutionStateStrTmp(actionExecutionStateToStrUMap_glo_sta_con.at(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->lastState_f()));
    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(actionData_par_ptr_con->id_f()));

#ifdef DEBUGJOUVEN
    //qDebug() << "rowTmp " << QString::number(rowTmp) << endl;
    //qDebug() << "actionExecutionStateStrTmp " << actionExecutionStateStrTmp << endl;
#endif

    //action (type) 0 | description 1 | Execution state 2 | Last output 3 | Last error 4
    actionsTable_pri->item(rowTmp, 2)->setText(actionExecutionStateStrTmp.left(32));
}

void mainWindow_c::removeActions_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Action removal disallowed while executing actions", "Error", this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f("No action rows selected for removal", "Error", this);
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
            actionsTable_pri->removeRow(item_ite_con);
            actonDataHub_ptr_ext->removeActionDataUsingRow_f(item_ite_con);
        }
    }
}

void mainWindow_c::executeActionsButtonClicked_f()
{
    while (true)
    {
        //in the process of stopping actions
        if (actonDataHub_ptr_ext->stoppingActionsExecution_f())
        {
            break;
        }
        //if the are stopped actions ask to resume them
        if (actonDataHub_ptr_ext->actionsExecutionStopped_f())
        {
            runFromStoppedActionMessageBox_pri = new QMessageBox(this);
            runFromStoppedActionMessageBox_pri->setText(R"(Run from last unsuccessful action?)");
            //runFromHaltedActionMessageBox_pri->setTextFormat(Qt::RichText);
            runFromStoppedActionMessageBox_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            runFromStoppedActionMessageBox_pri->setDefaultButton(QMessageBox::Yes);
            runFromStoppedActionMessageBox_pri->setWindowModality(Qt::WindowModal);

            QObject::connect(runFromStoppedActionMessageBox_pri, &QMessageBox::finished, this, &mainWindow_c::runFromStoppedActionsMessageBoxResult_f);

            runFromStoppedActionMessageBox_pri->show();
            break;
        }
        //if there are executing actions ask to stop execution
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            //stops everything. There isn't, yet, a "selection stop" option
            //NOTDO
            //it could be done but the user would need to have action dependencies in mind
            //because if the user stops one action that has a dependent
            //the dependent one would stay indefinitely "excuting checks"
            //still the user could stop everything later anyway if
            //he gets "stuck" trying to manage the execution
            executeActionsButton_pri->setText("Stopping...");
            actonDataHub_ptr_ext->tryStopExecutingActions_f();
            break;
        }

        executeActions_f();

        break;
    }
}

void mainWindow_c::showExecutionOptionsButtonClicked_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f("Execution options window disallowed while actions are executing", "Error", this);
            break;
        }

        executionOptionsWindow_c* executeOptionsWindowTmp(new executionOptionsWindow_c(this));
        //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
        executeOptionsWindowTmp->setWindowFlag(Qt::Window, true);
        executeOptionsWindowTmp->setWindowModality(Qt::WindowModal);
        executeOptionsWindowTmp->show();
        break;
    }
}

void mainWindow_c::executionFinished_f()
{
    //FUTURE this might need more details like duration, if errors or not, x/y successfull
    statusBarLabel_pri->setText("Action/s execution finished");
    //restore execute button text values
    executeActionsButton_pri->setText("&Execute Actions");
    executeActionsButton_pri->setToolTip("No selection executes all actions, selection executes selected actions");
}

void mainWindow_c::executionStarted_f()
{
    statusBarLabel_pri->setText("Action/s execution started");

    executeActionsButton_pri->setText("Executing... Press again to Stop");
    executeActionsButton_pri->setToolTip("Will stop after finishing already executing action/s");
}

void mainWindow_c::stoppingExecution_f()
{
    statusBarLabel_pri->setText("Trying to stop execution...");
}

void mainWindow_c::executionStopped_f()
{
    statusBarLabel_pri->setText("Execution stopped");
    //restore execute button text values
    executeActionsButton_pri->setText("&Execute Actions");
    executeActionsButton_pri->setToolTip("No selection executes all actions, selection executes selected actions");
}

void mainWindow_c::actionResultsCleared_f(actionData_c* const actionData_par_ptr_con)
{
    updateActionOutput_f(actionData_par_ptr_con);
    updateActionError_f(actionData_par_ptr_con);
    updateActionExecutionState_f(actionData_par_ptr_con);
}

void mainWindow_c::stopExecutingActionsAndClose_f()
{
    if (actonDataHub_ptr_ext->executingActions_f())
    {
        connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionFinished_signal, this, &mainWindow_c::close);
        actonDataHub_ptr_ext->tryStopExecutingActions_f();
        executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... (exiting after)"));
    }
    askAboutExecutingActionsOnCloseMessageBox_pri->close();
    askAboutExecutingActionsOnCloseMessageBox_pri = nullptr;
}

void mainWindow_c::stopExecutingActionsElseKillAndClose_f()
{
    if (actonDataHub_ptr_ext->executingActions_f())
    {
        connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionFinished_signal, this, &mainWindow_c::close);
        actonDataHub_ptr_ext->tryStopExecutingActions_f(true);
        executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));

        if (actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f() > 1000)
        {
            killCountdown_pri = actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f() / 1000;
            statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... will kill in ") + QString::number(killCountdown_pri));
            QTimer* killCountdownTimerPtr(new QTimer(this));
            connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionFinished_signal, killCountdownTimerPtr, &QTimer::deleteLater);
            connect(killCountdownTimerPtr, &QTimer::timeout, [this, killCountdownTimerPtr]()
            {
                killCountdown_pri = killCountdown_pri - 1;
                statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... will kill in ") + QString::number(killCountdown_pri));
                if (killCountdown_pri <= 0)
                {
                    killCountdownTimerPtr->deleteLater();
                    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Killing execution...  (exiting after)"));
                }
            });
            killCountdownTimerPtr->start(1000);
        }
        else
        {
            statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping-Killing execution... (exiting after)"));
        }
    }
    askAboutExecutingActionsOnCloseMessageBox_pri->close();
    askAboutExecutingActionsOnCloseMessageBox_pri = nullptr;
}

void mainWindow_c::killExecutingActionsAndClose_f()
{
    if (actonDataHub_ptr_ext->executingActions_f() and actonDataHub_ptr_ext->stoppingActionsExecution_f())
    {
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Killing execution... (exiting after)"));
        connect(actonDataHub_ptr_ext->proxyQObj_f(), &actonDataHubProxyQObj_c::actionsExecutionFinished_signal, this, &mainWindow_c::close, Qt::UniqueConnection);
        actonDataHub_ptr_ext->tryStopExecutingActions_f(true);
    }
    askAboutStoppingExecutionOnCloseMessageBox_pri->close();
    askAboutStoppingExecutionOnCloseMessageBox_pri = nullptr;
    close();
}

void mainWindow_c::runFromStoppedActionsMessageBoxResult_f(const int result_par)
{
    while (true)
    {
        if (result_par == QMessageBox::Cancel)
        {
            break;
        }
        if (result_par == QMessageBox::No)
        {
            executeActions_f();
            break;
        }
        if (result_par == QMessageBox::Yes)
        {
            actonDataHub_ptr_ext->tryResumeActionsExecution_f();
            break;
        }
        break;
    }
    runFromStoppedActionMessageBox_pri->deleteLater();
    runFromStoppedActionMessageBox_pri = nullptr;
}


void mainWindow_c::executeActions_f()
{
    std::vector<int> rowsToExecuteTmp;
    clearAllRowsResultColumns_f();

    //FUTURE timer or confirmation (and make it optional on an option)
    //to prevent users from executing unwanted stuff just by missclick

    //this is NOT the amount of rows, this gets CELLS, the same row will appear repeatedly, that's why a std::set is used later
    QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
    //all
    if (selectionTmp.isEmpty())
    {
        rowsToExecuteTmp.reserve(actionsTable_pri->rowCount());
        for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
        {
            rowsToExecuteTmp.emplace_back(rowIndex_ite);
        }
    }
    else
        //selection
    {
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
        for (const int actionRow_ite_con : rowsToExecuteTmp)
        {
            int_fast64_t actionIdToRunTmp(actonDataHub_ptr_ext->rowToActionDataId_f(actionRow_ite_con));

            actionData_c* actionDataPtrTmp(actonDataHub_ptr_ext->actionData_ptr_f(actionIdToRunTmp));
#ifdef DEBUGJOUVEN
//            qDebug() << "actionIdToRunTmp " << actionIdToRunTmp << endl;
//            qDebug() << "actionDataPtrTmp is? " << (actionDataPtrTmp == nullptr ? "null" : "not null") << endl;
//            qDebug() << "actionDataPtrTmp->id_f() " << actionDataPtrTmp->id_f() << endl;
            //qDebug() << "actionDataPtrTmp->enabled_f() " << actionDataPtrTmp->enabled_f() << endl;
            //qDebug() << "actionDataPtrTmp->checksEnabled_f() " << actionDataPtrTmp->checksEnabled_f() << endl;
#endif
            if (actionDataPtrTmp not_eq nullptr)
            {
                actionDataExecutionResult_c* actionExecutionResultPtr(actionDataPtrTmp->createGetActionDataExecutionResult_ptr_f());

                QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::executionStateUpdated_signal, this, &mainWindow_c::updateActionExecutionState_f, Qt::UniqueConnection);
                QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::outputUpdated_signal, this, &mainWindow_c::updateActionOutput_f, Qt::UniqueConnection);
                QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::error_signal, this, &mainWindow_c::updateActionError_f, Qt::UniqueConnection);
                QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::resultsCleared_signal, this, &mainWindow_c::actionResultsCleared_f, Qt::UniqueConnection);

                //on the grid only the above are used
                //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);
                //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);

            }
        }

#ifdef DEBUGJOUVEN
        //qDebug() << "actonDataHub_ptr_ext->executeActionDataRows_f(rowsToExecuteTmp);" << endl;
#endif
        actonDataHub_ptr_ext->executeActionDataRows_f(rowsToExecuteTmp);
    }
}

void mainWindow_c::showExecutionDetails_f()
{
    actionDataExecutionResult_c* actionResultTmp_ptr(nullptr);
    while (true)
    {
        if (actionsTable_pri->selectedItems().isEmpty())
        {
            errorQMessageBox_f("No action row selected", "Error", this);
            break;
        }

        int rowTmp(actionsTable_pri->selectedItems().first()->row());
        int_fast64_t actionDataIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(rowTmp));
        actionData_c* actionDataPtrTmp(actonDataHub_ptr_ext->actionData_ptr_f(actionDataIdTmp));
        if (actionDataPtrTmp not_eq nullptr)
        {
            if (actionDataPtrTmp->actionDataExecutionResult_ptr_f() == nullptr)
            {
                errorQMessageBox_f("Action has no execution results", "Error", this);
                break;
            }
            else
            {
                actionResultTmp_ptr = actionDataPtrTmp->actionDataExecutionResult_ptr_f();
            }
        }
        else
        {
            //this shouldn't be able to happen, it means the action doesn't exist but somehow a row was selected to show it's execution details
            //            errorQMessageBox_f("Action for the actionId (" + QString::number(actionDataIdTmp) + ") not found", "Error", this);
            //            break;
        }


        actionExecutionDetailsWindow_c* actionExecutionDetailsWindowTmp(new actionExecutionDetailsWindow_c(actionResultTmp_ptr, this));
        //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
        actionExecutionDetailsWindowTmp->setWindowFlag(Qt::Window, true);
        actionExecutionDetailsWindowTmp->setWindowModality(Qt::WindowModal);
        //it is set in the class ctor of actionExecutionDetailsWindow_c
        //actionExecutionDetailsWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
        actionExecutionDetailsWindowTmp->show();
        break;
    }
}

void mainWindow_c::showAboutMessage_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>ActonQtg</b> allows run actions in an organized maner i.e. (1) create directory, (2) copy stuff to this new directory... stopping if there are errors on any step or taking alternative paths</p>"
                    "<p>Creaded by: Jouven<br>"
                    R"(Source: <a href="//github.com/jouven/ActonQtg">github.com/jouven/ActonQtg</a><br>)"
                    R"(Homepage: <a href="//avidcalm.com">avidcalm.com</a></p>)"
                    )
                , "About actonQtg"
                , this
    );
}

mainWindow_c* mainWindow_ptr_ext = nullptr;

//mainWindow_c& mainWindowRef_f()
//{
//    static mainWindow_c mainWindow_sta;
//    return mainWindow_sta;
//}
