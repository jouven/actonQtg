#include "executionOptionsWindow.hpp"

#include "actonQtso/actonDataHub.hpp"

#include "appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"

#include <QtWidgets>
#include <QIntValidator>


void executionOptionsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

void executionOptionsWindow_c::save_f()
{
    actonDataHub_ptr_ext->setExecutionOptions({

    loopExecutionCheckbox_pri->isChecked()
    , stopExecutingOnErrorCheckbox_pri->isChecked()
    , extraThreadsLineEdit_pri->text().toLong()
    , killTimeoutMillisecondsLineEdit_pri->text().toLong()
    });

    threadedFunction_c::setMaxConcurrentQThreads_f(actonDataHub_ptr_ext->executionOptions_f().extraThreads_f());
}

void executionOptionsWindow_c::load_f()
{
    loopExecutionCheckbox_pri->setChecked(actonDataHub_ptr_ext->executionOptions_f().loopExecution_f());
    stopExecutingOnErrorCheckbox_pri->setChecked(actonDataHub_ptr_ext->executionOptions_f().stopExecutingOnError_f());
    extraThreadsLineEdit_pri->setText(QString::number(actonDataHub_ptr_ext->executionOptions_f().extraThreads_f()));
    killTimeoutMillisecondsLineEdit_pri->setText(QString::number(actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f()));
}

executionOptionsWindow_c::executionOptionsWindow_c(
        QWidget *parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("executionOptionsWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    loopExecutionCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Loop execution"));
    firstRowLayoutTmp->addWidget(loopExecutionCheckbox_pri);

    stopExecutingOnErrorCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Stop execution on error"));
    firstRowLayoutTmp->addWidget(stopExecutingOnErrorCheckbox_pri);


    QLabel* extraThreadsLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Extra execution threads"));
    extraThreadsLabelTmp->setToolTip(
                "<p>Minimum 1, maximum INT32MAX</p>"
                "<p>Running an action/check requires 1 extra thread, each additional action running at the same time requires 1 extra each,"
                "if no extra threads are available when an action is ready to run it will wait until one is</p>"
    );

    firstRowLayoutTmp->addWidget(extraThreadsLabelTmp);

    extraThreadsLineEdit_pri = new QLineEdit;
    //allows 0, integers have trailing 0s?
    //at least it only allows numbers so leave it like this and check if 0 when saving
    QValidator *validatorTmp = new QIntValidator(1, INT32_MAX, this);
    extraThreadsLineEdit_pri->setValidator(validatorTmp);
    firstRowLayoutTmp->addWidget(extraThreadsLineEdit_pri);


    QLabel* killTimeoutMillisecondsLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Kill timeout"));
    killTimeoutMillisecondsLabelTmp->setToolTip(
                "<p>Minimum 0, maximum INT32MAX</p>"
                "<p>How many milliseconds to wait after stopping before resorting to kill</p>"
    );

    firstRowLayoutTmp->addWidget(killTimeoutMillisecondsLabelTmp);

    killTimeoutMillisecondsLineEdit_pri = new QLineEdit;
    killTimeoutMillisecondsLineEdit_pri->setValidator(validatorTmp);
    firstRowLayoutTmp->addWidget(killTimeoutMillisecondsLineEdit_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("&Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);
    connect(saveButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::saveButtonClicked_f);

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::cancelButtonClicked_f);

    QPushButton* tipsButtonTmp = new QPushButton("Tips");
    lastRowLayoutTmp->addWidget(tipsButtonTmp);
    connect(tipsButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::tipsButtonClicked_f);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Execution options"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
    }

    load_f();
}


void executionOptionsWindow_c::saveButtonClicked_f()
{
    while (true)
    {
        //no need to check <int32max since it's a C/C++ native limitation,
        //and if the conversion fails it returns 0 so... win win
        if (extraThreadsLineEdit_pri->text().toLong() < 1)
        {
            errorQMessageBox_f("Wrong extra threads value must be >0 - int32 max value", "Error", this);
            break;
        }
        save_f();
        close();
        break;
    }
}

void executionOptionsWindow_c::cancelButtonClicked_f()
{
    close();
}


void executionOptionsWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>Execution options are included when saving on the main window</p>"
                )
            , "Tips"
            , this
    );
}


