#include "checkExecutionDetailsWindow.hpp"

#include "appConfig.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/checkDataExecutionResult.hpp"
#include "actonQtso/checkMappings/checkExecutionStateStrMapping.hpp"
#include "actonQtso/checksDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QSplitter>

void checkExecutionDetailsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

checkExecutionDetailsWindow_c::checkExecutionDetailsWindow_c(
        checkDataExecutionResult_c* checkDataExecutionResult_ptr_par
        , QWidget *parent_par)
    : QWidget(parent_par)
    , checkDataExecutionResultPtr_pri(checkDataExecutionResult_ptr_par)
{
    this->setObjectName("actionExecutionDetailsWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //state and return code

    //output
    //error

    //actionexternaloutput
    //actionexternalerror

    //execution state and action return code
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    QLabel* executionStateLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Execution state"));
    //I need to figure a better way to calculate a minmum height for the fields
    //AKA a text line height + the upper+lower height margins? of box containing it
    auto minHeightTmp(executionStateLabelTmp->fontMetrics().lineSpacing() + 14);
    firstRowLayoutTmp->addWidget(executionStateLabelTmp);
    executionStateTE_pri = new QTextEdit;
    executionStateTE_pri->setMinimumHeight(minHeightTmp);
    //executionStateTE_pri->setMinimumHeight(executionStateTE_pri->fontMetrics().lineSpacing());
    //executionStateTE_pri->setMinimumHeight(30);
    executionStateTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    executionStateTE_pri->setReadOnly(true);
    firstRowLayoutTmp->addWidget(executionStateTE_pri);

//    anyFinishCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Any finish"));
//    anyFinishCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("True if the check is done executing, no matter what state"));
//    anyFinishCheckbox_pri->setMinimumHeight(minHeightTmp);
//    firstRowLayoutTmp->addWidget(anyFinishCheckbox_pri);

    checkResultCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Result"));
    checkResultCheckbox_pri->setMinimumHeight(minHeightTmp);
    checkResultCheckbox_pri->setTristate(true);
    checkResultCheckbox_pri->setCheckState(Qt::PartiallyChecked);
    firstRowLayoutTmp->addWidget(checkResultCheckbox_pri);

    //start and finish times
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Execution start")));
    executionStartDatetimeTE_pri = new QTextEdit;
    executionStartDatetimeTE_pri->setMinimumHeight(minHeightTmp);
    executionStartDatetimeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionStartDatetimeTE_pri->setReadOnly(true);
    secondRowLayoutTmp->addWidget(executionStartDatetimeTE_pri);

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Execution finish")));
    executionFinishDatetimeTE_pri = new QTextEdit;
    executionFinishDatetimeTE_pri->setMinimumHeight(minHeightTmp);
    executionFinishDatetimeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionFinishDatetimeTE_pri->setReadOnly(true);
    secondRowLayoutTmp->addWidget(executionFinishDatetimeTE_pri);

//    //output
//    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

//    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Output")));

    //error
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    fourthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Error")));
    errorPTE_pri = new QPlainTextEdit;
    errorPTE_pri->setMinimumHeight(minHeightTmp);
    errorPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    errorPTE_pri->setReadOnly(true);
    fourthRowLayoutTmp->addWidget(errorPTE_pri);

//    //action external output
//    QHBoxLayout* fifthRowLayoutTmp = new QHBoxLayout;

//    fifthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("External output")));

//    //action external error
//    QHBoxLayout* sixthRowLayoutTmp = new QHBoxLayout;

//    sixthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("External error")));

    //ok button
    QHBoxLayout* seventhRowTmp = new QHBoxLayout;

    QPushButton* okButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Close"));
    seventhRowTmp->addWidget(okButtonPtrTmp);
    QPushButton* tipsButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    seventhRowTmp->addWidget(tipsButtonPtrTmp);

    QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    //QWidget* row3Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);
//    QWidget* row5Tmp = new QWidget;
//    //row4Tmp->setContentsMargins(0,0,0,0);
//    QWidget* row6Tmp = new QWidget;
//    //row4Tmp->setContentsMargins(0,0,0,0);

    QVBoxLayout* twoFirstRowsTmp = new QVBoxLayout;
    twoFirstRowsTmp->addLayout(firstRowLayoutTmp);
    twoFirstRowsTmp->addLayout(secondRowLayoutTmp);

    row1Tmp->setLayout(twoFirstRowsTmp);
    //row3Tmp->setLayout(thirdRowLayoutTmp);
    row4Tmp->setLayout(fourthRowLayoutTmp);
//    row5Tmp->setLayout(fifthRowLayoutTmp);
//    row6Tmp->setLayout(sixthRowLayoutTmp);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter");

    mainSplitter_pri->addWidget(row1Tmp);
    //mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->addWidget(row4Tmp);
//    mainSplitter_pri->addWidget(row5Tmp);
//    mainSplitter_pri->addWidget(row6Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    //mainSplitter_pri->setCollapsible(2, false);
//    mainSplitter_pri->setCollapsible(3, false);
//    mainSplitter_pri->setCollapsible(4, false);

    mainSplitter_pri->setContentsMargins(0,0,0,0);

    mainLayout_pri = new QVBoxLayout;
    //mainLayout_pri->addLayout(firstRowLayoutTmp);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addWidget(mainSplitter_pri);
    mainLayout_pri->addLayout(seventhRowTmp);

    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Check execution details"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    //"ok" it's the same as a cancel
    connect(okButtonPtrTmp, &QPushButton::clicked, this, &checkExecutionDetailsWindow_c::cancelButtonClicked_f);
    connect(tipsButtonPtrTmp, &QPushButton::clicked, this, &checkExecutionDetailsWindow_c::tipsButtonClicked_f);
    //disabling a checkbox disables any "programming", signals won't work,
    //so to prevent the user changing the state, this lambda reverses user check changes
    //"programming" wise since it's known before hand
    //when the check will change the signals can be blocked to disable this behavior and change the value
    //and since the this uses the GUI thread, the user has no chance to go around this "hack"
//    connect(anyFinishCheckbox_pri, &QCheckBox::stateChanged, this, [this](int state_par)
//    {
//        //to prevent signal recursion
//        anyFinishCheckbox_pri->blockSignals(true);
//        anyFinishCheckbox_pri->setCheckState(state_par == Qt::Checked ? Qt::Unchecked : Qt::Checked);
//        anyFinishCheckbox_pri->blockSignals(false);
//    });
    connect(checkResultCheckbox_pri, &QCheckBox::stateChanged, this, [this](int state_par)
    {
        //to prevent signal recursion
        checkResultCheckbox_pri->blockSignals(true);
//        Qt::CheckState checkStateTmp;
//        if (state_par == Qt::PartiallyChecked)
//        {
//            checkStateTmp = Qt::PartiallyChecked;
//        }
//        if (state_par == Qt::Checked)
//        {
//            checkStateTmp = Qt::Unchecked;
//        }
//        if (state_par == Qt::Unchecked)
//        {
//            checkStateTmp = Qt::Checked;
//        }
        checkResultCheckbox_pri->setCheckState(Qt::PartiallyChecked);
        checkResultCheckbox_pri->blockSignals(false);
    });

    if (checkDataExecutionResultPtr_pri not_eq nullptr)
    {
        updateError_f();
        updateState_f();
        updateAnyFinish_f();

        connect(checkDataExecutionResultPtr_pri, &checkDataExecutionResult_c::error_signal, this, &checkExecutionDetailsWindow_c::updateError_f);
        connect(checkDataExecutionResultPtr_pri, &checkDataExecutionResult_c::executionStateUpdated_signal, this, &checkExecutionDetailsWindow_c::updateState_f);
        connect(checkDataExecutionResultPtr_pri, &checkDataExecutionResult_c::finished_signal, this, &checkExecutionDetailsWindow_c::updateAnyFinish_f);
    }
}

void checkExecutionDetailsWindow_c::cancelButtonClicked_f()
{
    close();
}

void checkExecutionDetailsWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>1 return value and external output/error only apply for runProcess actions</p>"
                    "<p>2 \"Any finish\", checked, means that the action has finished running, successfully or not</p>"
                    )
            , "Tips"
            , this
    );
}


void checkExecutionDetailsWindow_c::updateError_f()
{
    errorPTE_pri->setPlainText(checkDataExecutionResultPtr_pri->error_f());
}

void checkExecutionDetailsWindow_c::updateState_f()
{

    if (executionStartDatetimeTE_pri->toPlainText().isEmpty() and checkDataExecutionResultPtr_pri->started_f())
    {
        executionStartDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(checkDataExecutionResultPtr_pri->startTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    executionStateTE_pri->setText(checkExecutionStateToStrUMap_ext_con.at(checkDataExecutionResultPtr_pri->lastState_f()));
}

void checkExecutionDetailsWindow_c::updateAnyFinish_f()
{
    if (checkDataExecutionResultPtr_pri->finished_f())
    {
        executionFinishDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(checkDataExecutionResultPtr_pri->finishedTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );

        checkResultCheckbox_pri->blockSignals(true);
        checkResultCheckbox_pri->setCheckState(checkDataExecutionResultPtr_pri->result_f() ? Qt::Checked : Qt::Unchecked);
        checkResultCheckbox_pri->blockSignals(false);
    }
//    anyFinishCheckbox_pri->blockSignals(true);
//    anyFinishCheckbox_pri->setCheckState(checkDataExecutionResultPtr_pri->finished_f() ? Qt::Checked : Qt::Unchecked);
//    anyFinishCheckbox_pri->blockSignals(false);
}


