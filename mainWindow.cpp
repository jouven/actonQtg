//1 is QT_NO_* (like QT_NO_CLIPBOARD) neccessary anymore?, the define doesn't lead anywhere
//searching in google/qt site doesn't provide any platform that needs it
//2 add icons, right now in *nix it uses the current loaded theme ones, but in windows
//this will not fly
#include "mainWindow.hpp"

#include "actonDataHubGlobal.hpp"
#include "actionWindow.hpp"
//#include "actionExecutionDetailsWindow.hpp"
#include "executionResultsWindow.hpp"
#include "appConfig.hpp"
#include "optionsWindow.hpp"
#include "executionOptionsWindow.hpp"
#include "commonWidgets.hpp"

#include "stringFormatting.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actionDataExecutionResult.hpp"
#include "actonQtso/actonDataHub.hpp"
#include "actonQtso/actonBaseSerialization.hpp"
#include "actonQtso/actionMappings/actionStrMapping.hpp"
#include "actonQtso/actionMappings/actionExecutionStateStrMapping.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "essentialQtso/macros.hpp"
//just to check the thread count
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
//#include "signalso/signal.hpp"
//#include "sizeConversionso/byte.hpp"
#include "signalProxyQtso/signalProxyQtso.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QStringList>
#include <QtWidgets>

//#include <chrono>
#include <unordered_set>
#include <set>

void mainWindow_c::closeEvent(QCloseEvent* event)
{
    //1 check if anything is executing
    //1A nothing executing, quit normally
    //1B something running --> ask user
    //--> cancel (do nothing and not close-quit)
    //--> stop (basically hit stop button and connect the end to a function that closes after)
    //--> stop and after timeout kill, in general this shouldn't be necessary but all the actions like "runProcess" that depend on external
    //processes will require this, but runProcess already has a kill option (QProcess has).
    //AND even thougth this program doesn't have this case yet, there might be unkillable actions,
    //like a "data move" (if it's not implemented like a copy first + erase at the end)
    //2 an OS signal happens, when this happens it is usually sent by an user or because the system is shutting down
    //in either case the user using the program will have from 5? seconds to maybe infinite (if it's not followed by a SIGKILL)
    //Anyway to make it simple, the program will show a window notifying ther user e.g., "termination signal X received, will try to close this program"
    //and then call closeEvent on the main window
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Exiting..."));
    bool evenAcceptedTmp(false);
    while (true)
    {
        if (actonDataHub_ptr_ext->stoppingActionsExecution_f() and not actonDataHub_ptr_ext->killingActionsExecution_f())
        {
            MACRO_ADDLOG("Close while stopping actions", QString(), messageType_ec::information);
            createMessageBoxAskAboutStoppingExecutionOnClose_f();
            break;
        }

        //the next 2 ifs are mutually exclusive,
        //running checks individually can't happen at the same time of running actions, the reverse can't happen either

        //ask to cancel, stop or kill
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            if (elapsedStopping_pri > 0)
            {
                break;
            }
            MACRO_ADDLOG("Close while executing actions", QString(), messageType_ec::information);
            createMessageBoxAskAboutExecutingActionsOnClose_f();
            break;
        }

        if (triedToSaveOnExit_pri)
        {
            //don't try again to save
        }
        else
        {
            //if a file was loaded previously check if it's different from what could be saved
            //and ask the user to save or not
            if (not lastLoadedFilePath_pri.isEmpty())
            {
                bool loadedDataIsDifferentFromLastSaveTmp(false);
                QFile savedFileToLoadTmp(lastLoadedFilePath_pri);
                if (not savedFileToLoadTmp.exists())
                {
                    break;
                }

                QByteArray jsonByteArrayTmp;
                if (savedFileToLoadTmp.open(QIODevice::ReadOnly))
                {
                    jsonByteArrayTmp = savedFileToLoadTmp.readAll();
                }
                else
                {
                    break;
                }

                auto jsonDocObjTmp(QJsonDocument::fromJson(jsonByteArrayTmp));
                if (jsonDocObjTmp.isNull())
                {
                    break;
                }
                else
                {
                    QJsonObject currentActonDataHubSerializationTmp(serializeActonDataHub_f(actonDataHub_ptr_ext));
                    loadedDataIsDifferentFromLastSaveTmp = currentActonDataHubSerializationTmp not_eq jsonDocObjTmp.object();
                }

                if (loadedDataIsDifferentFromLastSaveTmp)
                {
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri = new QMessageBox(this);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setText(
                                appConfig_ptr_ext->translate_f(
                                R"(Loaded content differs from the last saved, do you want to save-overwrite the last loaded file?<br>)"
                                R"(Choosing "yes" will overwrite the last loaded file<br>)"
                                R"(Choosing "no" will save open a dialog to save a new file<br>)"
                                R"(Choosing "Cancel" will exit without saving)"));
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setTextFormat(Qt::RichText);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setDefaultButton(QMessageBox::Yes);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setWindowModality(Qt::WindowModal);

                    QObject::connect(onExitAskSaveIfLastLoadedFileChangedMessageBox_pri, &QMessageBox::finished, this, &mainWindow_c::messageBoxOverwriteLastActionLoadedFileFinished_f);

                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->show();
                    break;
                }
            }
            else
            {
                //if there are actions to save
                if (actonDataHub_ptr_ext->size_f() > 0)
                {
                    //ask to save before exit
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri = new QMessageBox(this);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setText(
                                appConfig_ptr_ext->translate_f(
                                R"(There are unsaved actions, do you want to save?<br>)"
                                R"(Choosing "yes" will open a save file dialog<br>)"
                                R"(Choosing "no" will exit without saving)"
                                ));
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setTextFormat(Qt::RichText);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setDefaultButton(QMessageBox::Yes);
                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->setWindowModality(Qt::WindowModal);

                    QObject::connect(onExitAskSaveIfLastLoadedFileChangedMessageBox_pri, &QMessageBox::finished, this, &mainWindow_c::messageBoxSaveActionsToFileOnExitFinished_f);

                    onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->show();
                    break;
                }
            }
        }

        //TODO this can probably be removed
        if (threadedFunction_c::threadsInUse_f() > 0)
        {
            text_c logMessageTmp(R"(Nothing running but qThreads exist, count: {0})", threadedFunction_c::threadsInUse_f());
            MACRO_ADDLOG(logMessageTmp, QString(), messageType_ec::information);
        }

        MACRO_ADDLOG("Setting geometry for saving", QString(), messageType_ec::information);
        appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionsTable_pri->objectName() + actionsTable_pri->horizontalHeader()->objectName(), actionsTable_pri->horizontalHeader()->saveState());
        appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + actionsTable_pri->objectName(), actionsTable_pri->saveGeometry());
        appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());

        Q_EMIT closeWindow_signal();
        closingWindow_pri = true;
        evenAcceptedTmp = true;

        break;
    }
    if (evenAcceptedTmp and finalCounterSeconds_pri == 0)
    {
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
    this->setObjectName("mainWindow_");
}

void mainWindow_c::start_f()
{
    const QRect screenGeometry = QApplication::desktop()->availableGeometry(this);

    QShortcut* quitShortCut(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this));
    //Qt::WidgetWithChildrenShortcut works when the main window is focused (with other modals windows open)
    //20191103 except Qt::WidgetWithChildrenShortcut
    //all the other Qt::ShortcutContext fail to work with other modal window opened
    //focusing or not focusing the main window
    quitShortCut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(quitShortCut, &QShortcut::activated, this, &mainWindow_c::close);

    statusBarLabel_pri = new QLabel;

    //new, load and save buttons
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //use QTabWidget? FUTURE?
    //new config file
    QIcon newIcon(QIcon::fromTheme("document-new", QIcon(":/images/new.png")));
    QPushButton* newConfigFileButtonTmp = new QPushButton(newIcon, appConfig_ptr_ext->translate_f("New"));
    newConfigFileButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Clears all loaded actions and resets action execution settings"));
    firstRowLayoutTmp->addWidget(newConfigFileButtonTmp);
    QObject::connect(newConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::clearAllActions_f);

    //load config file
    QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/browse.png"));
    QPushButton* loadConfigFileButtonTmp = new QPushButton(loadIcon, appConfig_ptr_ext->translate_f("Load"));
    loadConfigFileButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Load actions and execution options from a JSON file"));
    firstRowLayoutTmp->addWidget(loadConfigFileButtonTmp);
    QObject::connect(loadConfigFileButtonTmp, &QPushButton::clicked, this, &mainWindow_c::loadActionsFileButtonPushed_f);

    //save config file
    QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QPushButton* saveConfigFileButtonTmp = new QPushButton(saveIcon, appConfig_ptr_ext->translate_f("Save"));
    saveConfigFileButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Saves actions and execution options in a JSON file"));
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
    executeActionsButton_pri->setToolTip(appConfig_ptr_ext->translate_f("No selection executes all actions, selection executes selected actions"));
    thirdRowLayoutTmp->addWidget(executeActionsButton_pri);
    QObject::connect(executeActionsButton_pri, &QPushButton::clicked, this, &mainWindow_c::executeActionsButtonClicked_f);

    QPushButton* showExecutionResultsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Execution &results"));
    thirdRowLayoutTmp->addWidget(showExecutionResultsButtonTmp);
    QObject::connect(showExecutionResultsButtonTmp, &QPushButton::clicked, this, &mainWindow_c::showExecutionResults_f);

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

    actionsTable_pri = new QTableWidget(0, 7);
    actionsTable_pri->verticalHeader()->setVisible(false);
    actionsTable_pri->setObjectName("QTableWidget_");
    actionsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    //0 ActionId | 1 action (type) | 2 description | 3 enabled | 4 Execution state | 5 Last output | 6 Last error
    labels
            << appConfig_ptr_ext->translate_f("ActionId")
            << appConfig_ptr_ext->translate_f("Action Type")
            << appConfig_ptr_ext->translate_f("Description")
            << appConfig_ptr_ext->translate_f("Enabled")
            << appConfig_ptr_ext->translate_f("Execution state")
            << appConfig_ptr_ext->translate_f("Last output")
            << appConfig_ptr_ext->translate_f("Last error");
    actionsTable_pri->setHorizontalHeaderLabels(labels);
    actionsTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
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
    moveActionUpButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Move the selected actions up one index"));
    indexChangingLayoutTmp->addWidget(moveActionUpButtonTmp);
    QObject::connect(moveActionUpButtonTmp, &QPushButton::clicked, this, &mainWindow_c::moveSelectedActionsUpByOne_f);

    //change the index of the selected action button
    QPushButton* changeIndexButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Change\naction\nindex"));
    changeIndexButtonTmp->setToolTip(
                appConfig_ptr_ext->translate_f("Changes the first selected action index to a new one moving the destination and all the actions below one index down (except if it's the last index)"));
    indexChangingLayoutTmp->addWidget(changeIndexButtonTmp);
    QObject::connect(changeIndexButtonTmp, &QPushButton::clicked, this, &mainWindow_c::openChangeActionIndexWindow_f);

    //move action down button
    QPushButton* moveActionDownButtonTmp = new QPushButton(QIcon(":/images/downArrow.png"), "");
    moveActionDownButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Move the selected actions down one index"));
    indexChangingLayoutTmp->addWidget(moveActionDownButtonTmp);
    QObject::connect(moveActionDownButtonTmp, &QPushButton::clicked, this, &mainWindow_c::moveSelectedActionsDownByOne_f);

    fifthRowLayoutTmp->addLayout(indexChangingLayoutTmp);

    QVBoxLayout* mainLayoutTmp(new QVBoxLayout);
    mainLayoutTmp->addLayout(firstRowLayoutTmp);
    mainLayoutTmp->addLayout(secondRowLayoutTmp);
    mainLayoutTmp->addLayout(thirdRowLayoutTmp);
    mainLayoutTmp->addLayout(fourthRowLayoutTmp);
    mainLayoutTmp->addLayout(fifthRowLayoutTmp);
    mainLayoutTmp->addWidget(statusBarLabel_pri);
#ifdef __ANDROID__
    baseWidget_pri->setLayout(mainLayoutTmp);

#else
    this->setLayout(mainLayoutTmp);
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

//    auto* mainLoopTimer = new QTimer(this);
//    QObject::connect(mainLoopTimer, &QTimer::timeout, this, &mainWindow_c::mainLoop_f);
//    mainLoopTimer->start(500);

    if (appConfig_ptr_ext->configLoaded_f())
    {
        //qInfo() << "configFile_f().second" << endl;
        restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
        actionsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionsTable_pri->objectName()));
        actionsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + actionsTable_pri->objectName() + actionsTable_pri->horizontalHeader()->objectName()));
    }

    //TODO some of these aren't in use, see .hpp
    //QObject::connect(this, &mainWindow_c::setStatusBarText_signal, statusBarLabel_pri, &QLabel::setText);
    QObject::connect(this, &mainWindow_c::scrollToItem_signal, this, &mainWindow_c::scrollToItem_f);
    QObject::connect(this, &mainWindow_c::setRowCellField_signal, this, &mainWindow_c::setRowCellField_f);
    QObject::connect(this, &mainWindow_c::setHashRowCellField_signal, this, &mainWindow_c::setHashRowCellField_f);
    //QObject::connect(this, &Window_c::saveAfterHash_signal_f, this, &Window_c::dialogSaveFileList_f);
    //QObject::connect(this, &mainWindow_c::resizeFileTable_signal, this, &mainWindow_c::resizeFileTable_f);

    QObject::connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionStarted_signal, this, &mainWindow_c::executionStarted_f);
    QObject::connect(actonDataHub_ptr_ext, &actonDataHub_c::stoppingActionsExecution_signal, this, &mainWindow_c::stoppingExecution_f);
    QObject::connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionStopped_signal, this, &mainWindow_c::executionStopped_f);
    QObject::connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionFinished_signal, this, &mainWindow_c::executionFinished_f);

    //FUTURE create a meta library with the log file data reads the actual source file and shows the code around the log add line
    //would require the program to be shipped with the source, but it would make debugging easier
    if (not appConfig_ptr_ext->commandLinePositionalArguments_f().isEmpty())
    {
        MACRO_ADDLOG(R"(Load positional arguments as "acton" files)", QString(), messageType_ec::information);
        loadFileList_f(appConfig_ptr_ext->commandLinePositionalArguments_f());
    }
    else
    {
        updateMaxThreads_f();
    }
    MACRO_ADDLOG("Main window ctored", QString(), messageType_ec::debug);
    show();
}

void mainWindow_c::updateMaxThreads_f()
{
    threadedFunction_c::setMaxUsableThreads_f(actonDataHub_ptr_ext->executionOptions_f().extraThreads_f());
    MACRO_ADDLOG({R"(Using {0} extra threads)", actonDataHub_ptr_ext->executionOptions_f().extraThreads_f()}, QString(), messageType_ec::information);
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

//void mainWindow_c::mainLoop_f()
//{
//    if (finalCounterSeconds_pri == 0)
//    {
//        this->close();
//    }
//    if (closingWindow_pri)
//    {
//        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Exiting..."));
//        finalCounterSeconds_pri = finalCounterSeconds_pri - 1;
//    }
////    QTimer::singleShot(0,[=]
////    {
////        //directoryComboBox_pri->adjustSize();
////        adjustSize();
//    //    });
//}

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
                    R"(<p>3 Quitting ActonQtg "normally" during an operation won't instantly exit, during execution, it will have to finish executing the current action, during saving, it will have to finish saving</p>)"
                    R"(<p>4 There are tooltips, hover on the fields/labels/buttons/widgets to view them</p>)"
                    R"(<p>5 Control+q with the main window focused, even if others are opened, to quit</p>)"
                    )
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
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Clear actions disallowed while actions are executing")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        lastLoadedFilePath_pri.clear();
        actionsTable_pri->setRowCount(0);
        actonDataHub_ptr_ext->clearAllActionData_f();
        break;
    }
}

void mainWindow_c::loadFileList_f(const QStringList& fileList_par_con)
{
    MACRO_ADDLOG("Load file list (for acton files with actions)", QString(), messageType_ec::debug);
    textCompilation_c errorsTmp;
    if (not fileList_par_con.isEmpty())
    {
        int_fast32_t loadedFileCountTmp(0);
        const int_fast32_t actionCountBeforeLoad(actonDataHub_ptr_ext->size_f());
        for (const QString& fileStr_ite_con : fileList_par_con)
        {
            QFile savedFileToLoadTmp(fileStr_ite_con);
            if (not savedFileToLoadTmp.exists())
            {
                errorsTmp.append_f({"Save file {0} does not exist", fileStr_ite_con});
                //statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Save file does not exist"));
                continue;
            }

            QByteArray jsonByteArrayTmp;
            if (savedFileToLoadTmp.open(QIODevice::ReadOnly))
            {
                jsonByteArrayTmp = savedFileToLoadTmp.readAll();
            }
            else
            {
                errorsTmp.append_f({"Could not open config file {0}", fileStr_ite_con});
                //statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Could not open config file ") + savedFile.fileName());
                continue;
            }

            auto jsonDocObjTmp(QJsonDocument::fromJson(jsonByteArrayTmp));
            bool thisFileLoadedSomethingTmp(false);
            if (jsonDocObjTmp.isNull())
            {
                errorsTmp.append_f({"Could not parse json from the save file {0}", fileStr_ite_con});
                //statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Could not parse json from the save file"));
                continue;
            }
            else
            {
                const int_fast32_t sizeBeforeLoad(actonDataHub_ptr_ext->size_f());
                {
                    text_c logMessageTmp(R"(Deserializing file {0} JSON)", fileStr_ite_con);
                    MACRO_ADDLOG(logMessageTmp, QString(), messageType_ec::debug);
                }
                deserializeActonDataHub_f(jsonDocObjTmp.object(), actonDataHub_ptr_ext, true, std::addressof(errorsTmp));
                {
                    text_c logMessageTmp(R"(Found {0} actions)", actonDataHub_ptr_ext->size_f());
                    MACRO_ADDLOG(logMessageTmp, QString(), messageType_ec::debug);
                }
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
                    if (closingWindow_pri)
                    {
                        break;
                    }
                }
                thisFileLoadedSomethingTmp = actonDataHub_ptr_ext->size_f() > sizeBeforeLoad;
            }
            if (thisFileLoadedSomethingTmp)
            {
                text_c logMessageTmp(R"(Save file loaded {0})", fileStr_ite_con);
                MACRO_ADDLOG(logMessageTmp, QString(), messageType_ec::information);
                loadedFileCountTmp += 1;
                if (fileList_par_con.size() == 1)
                {
                    lastLoadedFilePath_pri = fileList_par_con.at(0);
                }
            }
            else
            {
                //statusBar message is done at the end
                //statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("JSON file " + fileStr_ite_con + " has no actions"));
            }
            if (closingWindow_pri)
            {
                break;
            }
        }
        if (loadedFileCountTmp > 0)
        {
            updateMaxThreads_f();
            //resizeFileTable_f();
            int_fast32_t actionLoadCountTmp(actonDataHub_ptr_ext->size_f() - actionCountBeforeLoad);
            text_c loadedMessageTmp("Loaded {0} actions from {1} file/s", actionLoadCountTmp, loadedFileCountTmp);
#ifdef DEBUGJOUVEN
//            qDebug() << "loadedMessageTmp.rawText_f() " << loadedMessageTmp.rawText_f() << endl;
//            qDebug() << "loadedMessageTmp.rawReplace_f() " << loadedMessageTmp.rawReplace_f() << endl;
//            qDebug() << "appConfig_ptr_ext->translateAndReplace_f(loadedMessageTmp) " << appConfig_ptr_ext->translateAndReplace_f(loadedMessageTmp) << endl;
#endif
            statusBarLabel_pri->setText(appConfig_ptr_ext->translateAndReplace_f(loadedMessageTmp));
        }
    }
    else
    {
        errorsTmp.append_f("No save file/s selected");
        //statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("No save files selected"));
    }
    if (not errorsTmp.empty_f())
    {
        messageBoxTheErrors_f(errorsTmp, this);
    }
}

void mainWindow_c::loadActionsFileButtonPushed_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Load disallowed while executing actions")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        selectActionFilesToLoadDialog_pri = new QFileDialog(this);
        selectActionFilesToLoadDialog_pri->setObjectName("selectActionFilesToLoadDialog_");
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

        std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + selectActionFilesToLoadDialog_pri->objectName()));
        if (not directoryHistoryTmp.empty())
        {
            QList<QUrl> directoriesTmp;
            directoriesTmp.reserve(directoryHistoryTmp.size());
            for (const QString& directory_par_con : directoryHistoryTmp)
            {
                directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
            }
            selectActionFilesToLoadDialog_pri->setSidebarUrls(directoriesTmp);
        }

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
#ifdef DEBUGJOUVEN
            qDebug() << "selectActionFilesToLoadDialog_pri->directory().path() " << selectActionFilesToLoadDialog_pri->directory().path() << Qt::endl;
#endif
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + selectActionFilesToLoadDialog_pri->objectName(), selectActionFilesToLoadDialog_pri->directory().path());
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
        overwriteLastActionLoadedMessageBox_pri->setText(appConfig_ptr_ext->translate_f(R"(Overwrite loaded file?<br>Choosing "no" will save open a dialog to save a new file)"));
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

void mainWindow_c::messageBoxSaveActionsToFileOnExitFinished_f(const int result_par)
{
    //browse a file to save
    if (result_par == QMessageBox::Yes)
    {
        saveActionFileDialog_pri = new QFileDialog(this);
        saveActionFileDialog_pri->setObjectName("saveActionFileDialog_");
        saveActionFileDialog_pri->setAcceptMode(QFileDialog::AcceptSave);
        saveActionFileDialog_pri->setFileMode(QFileDialog::AnyFile);
        saveActionFileDialog_pri->setDirectory(QDir::currentPath());
        QString timestampStr(QDateTime::currentDateTime().toString("_yyyyMMdd_hhmmss"));
        saveActionFileDialog_pri->selectFile("actions" + timestampStr + ".json");
        saveActionFileDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Save action file..."));
        saveActionFileDialog_pri->setViewMode(QFileDialog::Detail);
        saveActionFileDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
        saveActionFileDialog_pri->setDefaultSuffix("json");
        saveActionFileDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
        saveActionFileDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
        saveDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

        std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + saveActionFileDialog_pri->objectName()));
        if (not directoryHistoryTmp.empty())
        {
            QList<QUrl> directoriesTmp;
            directoriesTmp.reserve(directoryHistoryTmp.size());
            for (const QString& directory_par_con : directoryHistoryTmp)
            {
                directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
            }
            saveActionFileDialog_pri->setSidebarUrls(directoriesTmp);
        }

        QObject::connect(saveActionFileDialog_pri, &QFileDialog::finished, this, &mainWindow_c::fileDialogSelectSaveActionFileFinished_f);

        saveActionFileDialog_pri->show();
    }
    if (result_par == QMessageBox::No)
    {
        if (onExitAskSaveIfLastLoadedFileChangedMessageBox_pri not_eq nullptr)
        {
            //main dtor will do dtor this
            //onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->deleteLater();
            //onExitAskSaveIfLastLoadedFileChangedMessageBox_pri = nullptr;
            triedToSaveOnExit_pri = true;
            close();
        }
    }
}

void mainWindow_c::messageBoxOverwriteLastActionLoadedFileFinished_f(const int result_par)
{
    //browse a file to save
    if (result_par == QMessageBox::No)
    {
        saveActionFileDialog_pri = new QFileDialog(this);
        saveActionFileDialog_pri->setObjectName("saveActionFileDialog_");
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

        std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + saveActionFileDialog_pri->objectName()));
        if (not directoryHistoryTmp.empty())
        {
            QList<QUrl> directoriesTmp;
            directoriesTmp.reserve(directoryHistoryTmp.size());
            for (const QString& directory_par_con : directoryHistoryTmp)
            {
                directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
            }
            saveActionFileDialog_pri->setSidebarUrls(directoriesTmp);
        }

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
    if (onExitAskSaveIfLastLoadedFileChangedMessageBox_pri not_eq nullptr)
    {
        //no need for this, main window getting dtored will dtor this anyway
//        if (equalOnce_ft(result_par, QMessageBox::Yes, QMessageBox::Cancel))
//        {
//            onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->deleteLater();
//            onExitAskSaveIfLastLoadedFileChangedMessageBox_pri = nullptr;
//        }

        if (result_par == QMessageBox::Cancel)
        {
            triedToSaveOnExit_pri = true;
            close();
        }
    }
}

void mainWindow_c::fileDialogSelectSaveActionFileFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        QString savePathTmp;
        if (not saveActionFileDialog_pri->selectedFiles().isEmpty())
        {
            savePathTmp = saveActionFileDialog_pri->selectedFiles().constFirst();
            saveActionFile_f(savePathTmp);
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + saveActionFileDialog_pri->objectName(), saveActionFileDialog_pri->directory().path());
        }
        else
        {
            //I don't think it will ever enter here...
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Empty save filename")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
        }
    }
    else
    {
        //nothing to do if the user cancels the save file dialog
    }

    saveActionFileDialog_pri->deleteLater();
    saveActionFileDialog_pri = nullptr;

    if (onExitAskSaveIfLastLoadedFileChangedMessageBox_pri not_eq nullptr)
    {
        //if it enters here it means functions call hierarchy comes from
        //the close event
        //onExitAskSaveIfLastLoadedFileChangedMessageBox_pri->deleteLater();
        //onExitAskSaveIfLastLoadedFileChangedMessageBox_pri = nullptr;
        triedToSaveOnExit_pri = true;
        close();
    }
}

void mainWindow_c::saveActionFile_f(const QString& savePath_par_con)
{
    QFile saveFile(savePath_par_con);
    if (saveFile.open(QIODevice::WriteOnly))
    {
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Creating JSON save file..."));

        QJsonDocument somethingJsonD(serializeActonDataHub_f(actonDataHub_ptr_ext));
        saveFile.write(somethingJsonD.toJson(QJsonDocument::Indented));
        //replace lastLoadedFilePath_pri when saving
        lastLoadedFilePath_pri = savePath_par_con;
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("File saved successfully"));

        if (onExitAskSaveIfLastLoadedFileChangedMessageBox_pri not_eq nullptr)
        {
            triedToSaveOnExit_pri = true;
            close();
        }
    }
    else
    {
        errorQMessageBox_f(
                    appConfig_ptr_ext->translateAndReplace_f({"Failed to save file: {0}", saveFile.errorString()})
                    , appConfig_ptr_ext->translate_f("Error")
                    , this);
    }
}

void mainWindow_c::addUpdateAction_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Add/edit action disallowed while executing actions")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
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
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Copy Action disallowed while actions are executing")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No action row selected to copy"),
                        appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Action row copy can only be applied to a single row")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        int selectedRowTmp(selectionTmp.first()->row());

        action_c* actionDataSourcePtrTmp(actonDataHub_ptr_ext->action_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(selectedRowTmp)));

        copyActionIndexInputDialog_pri = new QInputDialog(this);
        copyActionIndexInputDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Copy Action"));
        copyActionIndexInputDialog_pri->setLabelText(
                    appConfig_ptr_ext->translateAndReplace_f({
                    "Input a new index to copy the action to.\nAction source:"
                    "\nType: {0}"
                    "\nDescription: {1}"
                    "\nRow index: {2}"
                    , actionTypeToStrUMap_ext_con.at(actionDataSourcePtrTmp->type_f())
                    , truncateString_f(actionDataSourcePtrTmp->description_f(), 255)
                    , selectedRowTmp
                    })
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
            action_c* actionCopyTmp(actonDataHub_ptr_ext->action_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(selectedRowTmp))->clone_f());

            actionCopyTmp->setStringId_f(actionCopyTmp->stringId_f() + "_copy");

            actonDataHub_ptr_ext->insertActionData_f(actionCopyTmp, newIndexTmp);
            insertActionRow_f(
                        actionCopyTmp
                        , newIndexTmp
            );
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translateAndReplace_f({"Wrong index to copy the row: {0}", newIndexTmp})
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
        }
    }
    copyActionIndexInputDialog_pri->deleteLater();
    copyActionIndexInputDialog_pri = nullptr;
}

void mainWindow_c::insertActionRow_f(
        const action_c* actionPtr_par_con
        , const int row_par_con)
{
    QTableWidgetItem *actionIdCellTmp(new QTableWidgetItem(QString::number(actionPtr_par_con->id_f())));
    actionIdCellTmp->setFlags(actionIdCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *actionValueCellTmp(new QTableWidgetItem(actionPtr_par_con->typeStr_f()));
    actionValueCellTmp->setFlags(actionValueCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *descriptionCellTmp(new QTableWidgetItem(actionPtr_par_con->description_f()));
    descriptionCellTmp->setToolTip(actionPtr_par_con->description_f());
    descriptionCellTmp->setFlags(descriptionCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *enabledCellTmp(new QTableWidgetItem);
    Qt::CheckState checkValueTmp(actionPtr_par_con->enabled_f() ? Qt::Checked : Qt::Unchecked);
    enabledCellTmp->setCheckState(checkValueTmp);
    enabledCellTmp->setFlags(enabledCellTmp->flags() bitand compl Qt::ItemIsEditable);

    int newIndexTmp(row_par_con);
    if (newIndexTmp == -1)
    {
        newIndexTmp = actionsTable_pri->rowCount();
    }

    //0 ActionId | 1 action (type) | 2 description | 3 enabled | 4 Execution state | 5 Last output | 6 Last error
    actionsTable_pri->insertRow(newIndexTmp);
    actionsTable_pri->setItem(newIndexTmp, 0, actionIdCellTmp);
    actionsTable_pri->setItem(newIndexTmp, 1, actionValueCellTmp);
    actionsTable_pri->setItem(newIndexTmp, 2, descriptionCellTmp);
    actionsTable_pri->setItem(newIndexTmp, 3, enabledCellTmp);

    actionsTable_pri->setItem(newIndexTmp, 4, new QTableWidgetItem);
    actionsTable_pri->setItem(newIndexTmp, 5, new QTableWidgetItem);
    actionsTable_pri->setItem(newIndexTmp, 6, new QTableWidgetItem);
}

void mainWindow_c::updateExistingActionRow_f(
        const action_c* actionPtr_par_con
        , const int row_par_con)
{
    actionsTable_pri->item(row_par_con, 0)->setText(QString::number(actionPtr_par_con->id_f()));
    actionsTable_pri->item(row_par_con, 1)->setText(actionPtr_par_con->typeStr_f());
    actionsTable_pri->item(row_par_con, 2)->setText(actionPtr_par_con->description_f());
    actionsTable_pri->item(row_par_con, 2)->setToolTip(actionPtr_par_con->description_f());
    Qt::CheckState checkValueTmp(actionPtr_par_con->enabled_f() ? Qt::Checked : Qt::Unchecked);
    actionsTable_pri->item(row_par_con, 3)->setCheckState(checkValueTmp);
}

void mainWindow_c::clearAllRowsResultColumns_f()
{
    for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        //4 execution state, 5 last output, 6 last error
        actionsTable_pri->item(rowIndex_ite, 4)->setText("");
        actionsTable_pri->item(rowIndex_ite, 5)->setText("");
        actionsTable_pri->item(rowIndex_ite, 6)->setText("");
    }
}

void mainWindow_c::createMessageBoxAskAboutExecutingActionsOnClose_f()
{
    askAboutExecutingActionsOnCloseMessageBox_pri = new QMessageBox(this);
    askAboutExecutingActionsOnCloseMessageBox_pri->setText(appConfig_ptr_ext->translate_f(R"(Action/s are executing. What to do?)"));
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
    askAboutStoppingExecutionOnCloseMessageBox_pri->setText(appConfig_ptr_ext->translate_f(R"(Execution is stopping. What to do?)"));
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
    action_c* actionDataPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actionDataId));
#ifdef DEBUGJOUVEN
    //qDebug() << "actionDataPtrTmp is nullptr " << QSTRINGBOOL(actionDataPtrTmp == nullptr) << Qt::endl;
    //qDebug() << "actionDataPtrTmp->actonDataHubParent_f() is nullptr " << QSTRINGBOOL(actionDataPtrTmp->actonDataHubParent_f() == nullptr) << Qt::endl;
#endif
    //qInfo() << "3 actionDataTmp.description_f() " << actionDataTmp.description_f() << endl;
    if (row_par_con == actionsTable_pri->rowCount())
    {
        //qInfo() << "insert action row " << endl;
        insertActionRow_f(
                    actionDataPtrTmp
        );
    }
    else
    {
        //qInfo() << "update row " << row_par_con << endl;
        updateExistingActionRow_f(
                    actionDataPtrTmp
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
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Action moving disallowed while executing actions")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Empty selection to move")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
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
                action_c* actionPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(destinationRow)));
                insertActionRow_f(
                            actionPtrTmp
                            , destinationRow
                );
                if (selectionTmp.size() == 1)
                {
                    actionsTable_pri->selectRow(destinationRow);
                }
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
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Action index change disallowed while executing actions")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No action row selected to change row index")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Action row index change can only be applied to a single row")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        int currentIndexTmp(selectionTmp.first()->row());

        changeActionIndexInputDialog_pri = new QInputDialog(this);
        changeActionIndexInputDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Change Action index"));
        changeActionIndexInputDialog_pri->setLabelText(
                    appConfig_ptr_ext->translateAndReplace_f({"Input a new index to move the action to (current index: {0})", currentIndexTmp}));
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
            action_c* actionPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actonDataHub_ptr_ext->rowToActionDataId_f(newIndexTmp)));
            actionsTable_pri->removeRow(currentIndexTmp);
            insertActionRow_f(
                        actionPtrTmp
                        , newIndexTmp
            );
            //actionsTable_pri->selectRow(newIndexTmp);
        }
        else
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translateAndReplace_f({"Same or wrong index to move the row: {0}", newIndexTmp})
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
        }
    }
    changeActionIndexInputDialog_pri->deleteLater();
    changeActionIndexInputDialog_pri = nullptr;
}

void mainWindow_c::updateActionOutput_f(const executionResult_c* executionResult_ptr_par, const executionMessage_c* message_par)
{
    //get the row, get the acionData Obj for the id (to get the executionResult Obj)
    //update the cell of the row with the last part of the str
    const actionExecutionResult_c* actionExecutionResultPtrTmp(static_cast<const actionExecutionResult_c*>(executionResult_ptr_par));
    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(actionExecutionResultPtrTmp->action_ptr_f()->id_f()));
    //0 ActionId | 1 action (type) | 2 description | 3 enabled | 4 Execution state | 5 Last output | 6 Last error
    QString translationTmp(appConfig_ptr_ext->translateAndReplace_f(message_par->text_f()));
    actionsTable_pri->item(rowTmp, 5)->setText(truncateString_f(translationTmp, 32));
    actionsTable_pri->item(rowTmp, 5)->setToolTip(translationTmp);
}

void mainWindow_c::updateActionError_f(const executionResult_c* executionResult_ptr_par, const executionMessage_c* message_par)
{
    const actionExecutionResult_c* actionExecutionResultPtrTmp(static_cast<const actionExecutionResult_c*>(executionResult_ptr_par));
    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(actionExecutionResultPtrTmp->action_ptr_f()->id_f()));
    //0 ActionId | 1 action (type) | 2 description | 3 enabled | 4 Execution state | 5 Last output | 6 Last error
    QString translationTmp(appConfig_ptr_ext->translateAndReplace_f(message_par->text_f()));
    actionsTable_pri->item(rowTmp, 6)->setText(truncateString_f(translationTmp, 32));
    actionsTable_pri->item(rowTmp, 6)->setToolTip(translationTmp);
}

void mainWindow_c::updateActionExecutionState_f(const action_c* action_par_ptr_con)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "actionData_par_con.id_f() " << QString::number(actionData_par_ptr_con->id_f()) << endl;
#endif
    QString actionExecutionStateStrTmp(actionExecutionStateToString_f(action_par_ptr_con->actionDataExecutionResult_ptr_f()->lastState_f()));
    int rowTmp(actonDataHub_ptr_ext->actionDataIdToRow_f(action_par_ptr_con->id_f()));

#ifdef DEBUGJOUVEN
    //qDebug() << "rowTmp " << QString::number(rowTmp) << endl;
    //qDebug() << "actionExecutionStateStrTmp " << actionExecutionStateStrTmp << endl;
#endif
    QString translationTmp(appConfig_ptr_ext->translateAndReplace_f(actionExecutionStateStrTmp));
    //0 actionId | 1 action (type) | 2 description | 3 Enabled | 4 Execution state | 5 Last output | 6 Last error
    actionsTable_pri->item(rowTmp, 4)->setText(truncateString_f(translationTmp, 32));
    actionsTable_pri->item(rowTmp, 4)->setText(translationTmp);
}

void mainWindow_c::removeActions_f()
{
    while (true)
    {
        if (actonDataHub_ptr_ext->executingActions_f())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Action removal disallowed while executing actions")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }
        QList<QTableWidgetItem *> selectionTmp = actionsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No action rows selected for removal")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
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
            actionsTable_pri->removeRow(item_ite_con);
            actonDataHub_ptr_ext->removeActionDataUsingRow_f(item_ite_con);
        }
        break;
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
            runFromStoppedActionMessageBox_pri->setText(appConfig_ptr_ext->translate_f(R"(Run from last unsuccessful action?)"));
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
            executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));
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
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("Execution options window disallowed while actions are executing")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        executionOptionsWindow_c* executeOptionsWindowTmp(new executionOptionsWindow_c(this));
        QObject::connect(executeOptionsWindowTmp, &executionOptionsWindow_c::updateMaxThreads_signal, this, &mainWindow_c::updateMaxThreads_f);
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
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Action/s execution finished"));
    //restore execute button text values
    executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("&Execute Actions"));
    executeActionsButton_pri->setToolTip(appConfig_ptr_ext->translate_f("No selection executes all actions, selection executes selected actions"));
}

void mainWindow_c::executionStarted_f()
{
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Action/s execution started"));

    executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("Executing... Press again to Stop"));
    executeActionsButton_pri->setToolTip(appConfig_ptr_ext->translate_f("Will stop after finishing already executing action/s"));
}

void mainWindow_c::stoppingExecution_f()
{
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Trying to stop execution..."));
}

void mainWindow_c::executionStopped_f()
{
    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Execution stopped"));
    //restore execute button text values
    executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("&Execute Actions"));
    executeActionsButton_pri->setToolTip(appConfig_ptr_ext->translate_f("No selection executes all actions, selection executes selected actions"));
}

//void mainWindow_c::actionResultsCleared_f(action_c* const action_par_ptr_con)
//{
//    updateActionOutput_f(action_par_ptr_con);
//    updateActionError_f(action_par_ptr_con);
//    updateActionExecutionState_f(action_par_ptr_con);
//}

void mainWindow_c::stopExecutingActionsAndClose_f()
{
    if (actonDataHub_ptr_ext->executingActions_f())
    {
        connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionFinished_signal, this, &mainWindow_c::close);
        actonDataHub_ptr_ext->tryStopExecutingActions_f();
        executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... (exiting after)"));
    }
    if (askAboutExecutingActionsOnCloseMessageBox_pri not_eq nullptr)
    {
        askAboutExecutingActionsOnCloseMessageBox_pri->close();
        askAboutExecutingActionsOnCloseMessageBox_pri = nullptr;
    }
}

void mainWindow_c::stopExecutingActionsElseKillAndClose_f()
{
    if (actonDataHub_ptr_ext->executingActions_f())
    {
        connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionFinished_signal, this, &mainWindow_c::close);
        actonDataHub_ptr_ext->tryStopExecutingActions_f(true);
        executeActionsButton_pri->setText(appConfig_ptr_ext->translate_f("Stopping..."));

        //TODO add and elapsed timmer so the user knows how much has he waited

        QTimer* elapsedCountdownTimerPtr(new QTimer(this));
        connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionFinished_signal, elapsedCountdownTimerPtr, &QTimer::deleteLater);
        connect(elapsedCountdownTimerPtr, &QTimer::timeout, [mainWindowPtr=this]()
        {
            mainWindowPtr->elapsedStopping_pri = mainWindowPtr->elapsedStopping_pri - 1;
            text_c elapsedTextTmp("Stopping execution... {0} seconds elapsed", mainWindowPtr->elapsedStopping_pri);
            mainWindowPtr->statusBarLabel_pri->setText(appConfig_ptr_ext->translateAndReplace_f(elapsedTextTmp));
        });
        elapsedCountdownTimerPtr->start(1000);

//        if (actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f() > 1000)
//        {
//            killCountdown_pri = actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f() / 1000;
//            statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... will kill in ") + QString::number(killCountdown_pri));
//            QTimer* killCountdownTimerPtr(new QTimer(this));
//            connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionFinished_signal, killCountdownTimerPtr, &QTimer::deleteLater);
//            connect(killCountdownTimerPtr, &QTimer::timeout, [this, killCountdownTimerPtr]()
//            {
//                killCountdown_pri = killCountdown_pri - 1;
//                statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping execution... will kill in ") + QString::number(killCountdown_pri));
//                if (killCountdown_pri <= 0)
//                {
//                    killCountdownTimerPtr->deleteLater();
//                    statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Killing execution...  (exiting after)"));
//                }
//            });
//            killCountdownTimerPtr->start(1000);
//        }
//        else
//        {
//            statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Stopping-Killing execution... (exiting after)"));
//        }
    }
    askAboutExecutingActionsOnCloseMessageBox_pri->close();
    askAboutExecutingActionsOnCloseMessageBox_pri = nullptr;
}

void mainWindow_c::killExecutingActionsAndClose_f()
{
    if (actonDataHub_ptr_ext->executingActions_f() and actonDataHub_ptr_ext->stoppingActionsExecution_f())
    {
        statusBarLabel_pri->setText(appConfig_ptr_ext->translate_f("Killing execution... (exiting after)"));
        connect(actonDataHub_ptr_ext, &actonDataHub_c::actionsExecutionFinished_signal, this, &mainWindow_c::close, Qt::UniqueConnection);
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
            resumeActionsExecution_f();
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

            action_c* actionPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actionIdToRunTmp));
#ifdef DEBUGJOUVEN
//            qDebug() << "actionIdToRunTmp " << actionIdToRunTmp << endl;
//            qDebug() << "actionDataPtrTmp is? " << (actionDataPtrTmp == nullptr ? "null" : "not null") << endl;
//            qDebug() << "actionDataPtrTmp->id_f() " << actionDataPtrTmp->id_f() << endl;
            //qDebug() << "actionDataPtrTmp->enabled_f() " << actionDataPtrTmp->enabled_f() << endl;
            //qDebug() << "actionDataPtrTmp->checksEnabled_f() " << actionDataPtrTmp->checksEnabled_f() << endl;
#endif
            if (actionPtrTmp not_eq nullptr)
            {
                actionExecutionResult_c* actionExecutionResultPtr(actionPtrTmp->createGetActionDataExecutionResult_ptr_f());
                //will be nullptr if the action is disabled
                if (actionExecutionResultPtr not_eq nullptr)
                {
                    QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::executionStateUpdated_signal, this, &mainWindow_c::updateActionExecutionState_f, Qt::UniqueConnection);
                    QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::informationMessageAdded_signal, this, &mainWindow_c::updateActionOutput_f, Qt::UniqueConnection);
                    QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::errorMessageAdded_signal, this, &mainWindow_c::updateActionError_f, Qt::UniqueConnection);
                    //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::errors_signal, this, &mainWindow_c::updateActionError_f, Qt::UniqueConnection);
                    //QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::resultsCleared_signal, this, &mainWindow_c::actionResultsCleared_f, Qt::UniqueConnection);

                    //on the grid only the above are used
                    //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);
                    //QObject::connect(actionExecutionResultPtr, &actionDataExecutionResult_c::actionExternalOutputUpdated_signal, this, &mainWindow_c::updateActionExternalOutput_f);
                }
            }
        }

#ifdef DEBUGJOUVEN
        //qDebug() << "actonDataHub_ptr_ext->executeActionDataRows_f(rowsToExecuteTmp);" << Qt::endl;
#endif

        textCompilation_c errorsTmp;
        bool resultTmp(actonDataHub_ptr_ext->executeActionDataRows_f(rowsToExecuteTmp, std::addressof(errorsTmp)));
        if (resultTmp)
        {

        }
        else
        {
            errorQMessageBox_f(errorsTmp.toRawReplace_f());
        }
    }
}

void mainWindow_c::resumeActionsExecution_f()
{
    for (auto rowIndex_ite = 0, l = actionsTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        int_fast64_t actionIdToRunTmp(actonDataHub_ptr_ext->rowToActionDataId_f(rowIndex_ite));

        action_c* actionPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actionIdToRunTmp));
#ifdef DEBUGJOUVEN
//            qDebug() << "actionIdToRunTmp " << actionIdToRunTmp << endl;
#endif
        if (actionPtrTmp not_eq nullptr)
        {
            if (actionPtrTmp->actionDataExecutionResult_ptr_f() not_eq nullptr and actionPtrTmp->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::stopped)
            {
                //action execution result of stopped objs need to be regenerated prior because otherwise they will be regenerated
                //when each action execution detects they aren't on a initial state, then it's too late to "connect" anything
                actionExecutionResult_c* actionExecutionResultPtr(actionPtrTmp->regenerateGetActionDataExecutionResult_ptr_f());

                QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::executionStateUpdated_signal, this, &mainWindow_c::updateActionExecutionState_f, Qt::UniqueConnection);
                QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::informationMessageAdded_signal, this, &mainWindow_c::updateActionOutput_f, Qt::UniqueConnection);
                QObject::connect(actionExecutionResultPtr, &actionExecutionResult_c::errorMessageAdded_signal, this, &mainWindow_c::updateActionError_f, Qt::UniqueConnection);
            }
        }
    }
    actonDataHub_ptr_ext->tryResumeActionsExecution_f();
}

void mainWindow_c::showExecutionResults_f()
{
    executionResultsWindow_c* executionResultsWindowTmp;
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp(actionsTable_pri->selectedItems());
        std::set<int> rowIndexSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexSetTmp.emplace(item_ite_con->row());
        }

        if (rowIndexSetTmp.empty())
        {
            executionResultsWindowTmp = new executionResultsWindow_c();
            break;
        }

        std::vector<action_c*> selectedActionsTmp;
        for (const int row_ite_con : rowIndexSetTmp)
        {
            int_fast64_t actionDataIdTmp(actonDataHub_ptr_ext->rowToActionDataId_f(row_ite_con));
            action_c* actionPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actionDataIdTmp));
            //no need to check if the actions have results because executionResultsWindow_c can stay empty
            selectedActionsTmp.emplace_back(actionPtrTmp);
        }

        executionResultsWindowTmp = new executionResultsWindow_c(selectedActionsTmp);
        break;
    }

    executionResultsWindowTmp->setWindowFlag(Qt::Window, true);
    executionResultsWindowTmp->setWindowModality(Qt::WindowModal);
    //it is set in the class ctor of executionResultsWindow_c
    //executionResultsWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    executionResultsWindowTmp->show_f();
}

void mainWindow_c::showAboutMessage_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>Execute actions in an organized maner i.e. (1) create directory, (2) copy stuff to this new directory..."
                    " stopping if there are errors on any step or taking alternative paths</p>"
                    "<p>Creaded by: Jouven<br>"
                    R"(Source: <a href="https://github.com/jouven/ActonQtg">github.com/jouven/ActonQtg</a><br>)"
                    R"(Homepage: <a href="https://avidcalm.com">avidcalm.com</a></p>)"
                    )
                , appConfig_ptr_ext->translate_f("About actonQtg")
                , this
    );
}

mainWindow_c* mainWindow_ptr_ext = nullptr;

//mainWindow_c& mainWindowRef_f()
//{
//    static mainWindow_c mainWindow_sta;
//    return mainWindow_sta;
//}
