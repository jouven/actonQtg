#include "optionsWindow.hpp"

#include "optionsWidgets/environmentWindow.hpp"
#include "optionsWidgets/workingDirectoryWindow.hpp"
#include "logsWindow.hpp"

#include "appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>


void optionsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

optionsWindow_c::optionsWindow_c(
        QWidget *parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("optionsWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //enviroment print
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;
    QPushButton* showEnvironmentButtonTmp = new QPushButton("Display environment");
    firstRowLayoutTmp->addWidget(showEnvironmentButtonTmp);
    connect(showEnvironmentButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::showEnvironmentWindow_f);

    QPushButton* showWorkingDirectoryButtonTmp = new QPushButton("Display current working directory");
    firstRowLayoutTmp->addWidget(showWorkingDirectoryButtonTmp);
    connect(showWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::showWorkingDirectoryWindow_f);

    //show logs
    QPushButton* showLogsWindowButtonTmp = new QPushButton("Show Logs");
    firstRowLayoutTmp->addWidget(showLogsWindowButtonTmp);
    connect(showLogsWindowButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::showLogsWindow_f);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("&Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);
    connect(saveButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::saveButtonClicked_f);

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::cancelButtonClicked_f);

//    QPushButton* tipsButtonTmp = new QPushButton("Tips");
//    lastRowLayoutTmp->addWidget(tipsButtonTmp);
//    connect(tipsButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::tipsButtonClicked_f);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Options"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
    }
}


void optionsWindow_c::saveButtonClicked_f()
{
    //when I add something to save...
    close();
}

void optionsWindow_c::cancelButtonClicked_f()
{
    close();
}

//no need for now
//void optionsWindow_c::tipsButtonClicked_f()
//{
//
//}

void optionsWindow_c::showEnvironmentWindow_f()
{
    environmentWindow_c* environmentWindowTmp = new environmentWindow_c(this);
    environmentWindowTmp->setWindowFlag(Qt::Window, true);
    environmentWindowTmp->setWindowModality(Qt::WindowModal);
    environmentWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    environmentWindowTmp->show();
}

void optionsWindow_c::showWorkingDirectoryWindow_f()
{
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(this);
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

void optionsWindow_c::showLogsWindow_f()
{
    bool logWindowAlreadyOpenTmp(false);
    //check if it's already open
    for (QWidget* topLevelWidget_ite : QApplication::topLevelWidgets())
    {
        if (topLevelWidget_ite->objectName().startsWith("logsWindow_"))
        {
            logWindowAlreadyOpenTmp = true;
            break;
        }
    }

    if (not logWindowAlreadyOpenTmp)
    {
        logsWindow_c* logsWindowPtrTmp = new logsWindow_c;
        logsWindowPtrTmp->setWindowFlag(Qt::Window, true);
        //logsWindowPtrTmp->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        logsWindowPtrTmp->setWindowModality(Qt::NonModal);
        logsWindowPtrTmp->show();
    }
}
