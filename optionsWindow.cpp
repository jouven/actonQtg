#include "optionsWindow.hpp"

#include "optionsWidgets/environmentWindow.hpp"
#include "optionsWidgets/workingDirectoryWindow.hpp"

#include "appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>


void optionsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_f().setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

optionsWindow_c::optionsWindow_c(
        QWidget *parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("optionsWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //enviroment print
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;
    QPushButton* showEnvironmentButtonTmp = new QPushButton("Display environment");
    firstRowLayoutTmp->addWidget(showEnvironmentButtonTmp);
    connect(showEnvironmentButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::showEnvironmentWindow_f);

    QPushButton* showWorkingDirectoryButtonTmp = new QPushButton("Display current working directory");
    firstRowLayoutTmp->addWidget(showWorkingDirectoryButtonTmp);
    connect(showWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::showWorkingDirectoryWindow_f);


    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);
    connect(saveButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::saveButtonClicked_f);

    QPushButton* cancelButtonTmp = new QPushButton("Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::cancelButtonClicked_f);

    QPushButton* tipsButtonTmp = new QPushButton("Tips");
    lastRowLayoutTmp->addWidget(tipsButtonTmp);
    connect(tipsButtonTmp, &QPushButton::clicked, this, &optionsWindow_c::tipsButtonClicked_f);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    this->setLayout(mainLayout_pri);

    setWindowTitle(tr("Options"));

    if (appConfig_f().configLoaded_f())
    {
         restoreGeometry(appConfig_f().widgetGeometry_f(this->objectName()));
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

void optionsWindow_c::tipsButtonClicked_f()
{
    //TODO
}

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
