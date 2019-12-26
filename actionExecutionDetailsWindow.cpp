#include "actionExecutionDetailsWindow.hpp"

#include "appConfig.hpp"

#include "actonQtso/actionData.hpp"
#include "actonQtso/actionDataExecutionResult.hpp"
#include "actonQtso/actionMappings/actionExecutionStateStrMapping.hpp"
#include "actonQtso/actonDataHub.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QSplitter>

void actionExecutionDetailsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

actionExecutionDetailsWindow_c::actionExecutionDetailsWindow_c(
        actionDataExecutionResult_c* actionDataExecutionResult_ptr_par
        , QWidget *parent_par)
    : QWidget(parent_par)
    , actionDataExecutionResultPtr_pri(actionDataExecutionResult_ptr_par)
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

    anyFinishCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Any finish"));
    anyFinishCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("True if the action is done executing, no matter what state"));
    //anyFinishCheckbox_pri->setMinimumHeight(minHeightTmp);
    firstRowLayoutTmp->addWidget(anyFinishCheckbox_pri);

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Return code")));
    returnCodeTE_pri = new QTextEdit;
    returnCodeTE_pri->setMinimumHeight(minHeightTmp);
    returnCodeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    returnCodeTE_pri->setReadOnly(true);
    firstRowLayoutTmp->addWidget(returnCodeTE_pri);

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

    //output
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Output")));
    outputPTE_pri = new QPlainTextEdit;
    outputPTE_pri->setMinimumHeight(minHeightTmp);
    outputPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    outputPTE_pri->setReadOnly(true);
    thirdRowLayoutTmp->addWidget(outputPTE_pri);

    //error
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    fourthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Error")));
    errorPTE_pri = new QPlainTextEdit;
    errorPTE_pri->setMinimumHeight(minHeightTmp);
    errorPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    errorPTE_pri->setReadOnly(true);
    fourthRowLayoutTmp->addWidget(errorPTE_pri);

    //action external output
    QHBoxLayout* fifthRowLayoutTmp = new QHBoxLayout;

    fifthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("External output")));
    externalOutputPTE_pri = new QPlainTextEdit;
    externalOutputPTE_pri->setMinimumHeight(minHeightTmp);
    externalOutputPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    externalOutputPTE_pri->setReadOnly(true);
    fifthRowLayoutTmp->addWidget(externalOutputPTE_pri);

    //action external error
    QHBoxLayout* sixthRowLayoutTmp = new QHBoxLayout;

    sixthRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("External error")));
    externalErrorPTE_pri = new QPlainTextEdit;
    externalErrorPTE_pri->setMinimumHeight(minHeightTmp);
    externalErrorPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    externalErrorPTE_pri->setReadOnly(true);
    sixthRowLayoutTmp->addWidget(externalErrorPTE_pri);


    //ok button
    QHBoxLayout* seventhRowTmp = new QHBoxLayout;

    QPushButton* okButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Close"));
    seventhRowTmp->addWidget(okButtonPtrTmp);
    QPushButton* tipsButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    seventhRowTmp->addWidget(tipsButtonPtrTmp);

    QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);
    QWidget* row5Tmp = new QWidget;
    //row4Tmp->setContentsMargins(0,0,0,0);
    QWidget* row6Tmp = new QWidget;
    //row4Tmp->setContentsMargins(0,0,0,0);

    QVBoxLayout* twoFirstRowsTmp = new QVBoxLayout;
    twoFirstRowsTmp->addLayout(firstRowLayoutTmp);
    twoFirstRowsTmp->addLayout(secondRowLayoutTmp);

    row1Tmp->setLayout(twoFirstRowsTmp);
    row3Tmp->setLayout(thirdRowLayoutTmp);
    row4Tmp->setLayout(fourthRowLayoutTmp);
    row5Tmp->setLayout(fifthRowLayoutTmp);
    row6Tmp->setLayout(sixthRowLayoutTmp);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter_");

    mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->addWidget(row4Tmp);
    mainSplitter_pri->addWidget(row5Tmp);
    mainSplitter_pri->addWidget(row6Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setCollapsible(3, false);
    mainSplitter_pri->setCollapsible(4, false);

    mainSplitter_pri->setContentsMargins(0,0,0,0);

    mainLayout_pri = new QVBoxLayout;
    //mainLayout_pri->addLayout(firstRowLayoutTmp);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addWidget(mainSplitter_pri);
    mainLayout_pri->addLayout(seventhRowTmp);

    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Action execution details"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    //"ok" it's the same as a cancel
    connect(okButtonPtrTmp, &QPushButton::clicked, this, &actionExecutionDetailsWindow_c::cancelButtonClicked_f);
    connect(tipsButtonPtrTmp, &QPushButton::clicked, this, &actionExecutionDetailsWindow_c::tipsButtonClicked_f);
    //disabling a checkbox disables any "programming", signals won't work,
    //so to prevent the user changing the state, this lambda reverses user check changes
    //"programming" wise since it's known before hand
    //when the check will change the signals can be blocked to disable this behavior and change the value
    //and since the this uses the GUI thread, the user has no chance to go around this "hack"
    connect(anyFinishCheckbox_pri, &QCheckBox::stateChanged, this, [this](int state_par)
    {
        //to prevent signal recursion
        anyFinishCheckbox_pri->blockSignals(true);
        anyFinishCheckbox_pri->setCheckState(state_par == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        anyFinishCheckbox_pri->blockSignals(false);
    });

    if (actionDataExecutionResultPtr_pri not_eq nullptr)
    {
        updateOutput_f();
        updateError_f();
        updateExternalOutput_f();
        updateExternalError_f();
        updateState_f();
        updateReturnCode_f();
        updateAnyFinish_f();

        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::outputUpdated_signal, this, &actionExecutionDetailsWindow_c::updateOutput_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::error_signal, this, &actionExecutionDetailsWindow_c::updateError_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::externalOutputUpdated_signal, this, &actionExecutionDetailsWindow_c::updateExternalOutput_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::externalErrorUpdated_signal, this, &actionExecutionDetailsWindow_c::updateExternalError_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::executionStateUpdated_signal, this, &actionExecutionDetailsWindow_c::updateState_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::returnCodeSet_signal, this, &actionExecutionDetailsWindow_c::updateReturnCode_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::finished_signal, this, &actionExecutionDetailsWindow_c::updateAnyFinish_f);
    }
}

void actionExecutionDetailsWindow_c::cancelButtonClicked_f()
{
    close();
}

void actionExecutionDetailsWindow_c::tipsButtonClicked_f()
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

void actionExecutionDetailsWindow_c::updateOutput_f()
{
    outputPTE_pri->setPlainText(appConfig_ptr_ext->translateAndReplace_f(actionDataExecutionResultPtr_pri->output_f()));
}

void actionExecutionDetailsWindow_c::updateError_f()
{
    errorPTE_pri->setPlainText(appConfig_ptr_ext->translateAndReplace_f(actionDataExecutionResultPtr_pri->errors_f()));
}

void actionExecutionDetailsWindow_c::updateExternalOutput_f()
{
    externalOutputPTE_pri->setPlainText(actionDataExecutionResultPtr_pri->externalOutput_f());
}

void actionExecutionDetailsWindow_c::updateExternalError_f()
{
    externalErrorPTE_pri->setPlainText(actionDataExecutionResultPtr_pri->externalErrorOutput_f());
}

void actionExecutionDetailsWindow_c::updateState_f()
{
    if (executionStartDatetimeTE_pri->toPlainText().isEmpty() and actionDataExecutionResultPtr_pri->started_f())
    {
        executionStartDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(actionDataExecutionResultPtr_pri->startTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    executionStateTE_pri->setText(actionExecutionStateToStrUMap_ext_con.at(actionDataExecutionResultPtr_pri->lastState_f()));
}

void actionExecutionDetailsWindow_c::updateReturnCode_f()
{
    returnCodeTE_pri->setText(QString::number(actionDataExecutionResultPtr_pri->returnCode_f()));
}

void actionExecutionDetailsWindow_c::updateAnyFinish_f()
{
    if (actionDataExecutionResultPtr_pri->finished_f() and actionDataExecutionResultPtr_pri->finishedTime_f() not_eq 0)
    {
        executionFinishDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(actionDataExecutionResultPtr_pri->finishedTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    anyFinishCheckbox_pri->blockSignals(true);
    anyFinishCheckbox_pri->setCheckState(actionDataExecutionResultPtr_pri->finished_f() ? Qt::Checked : Qt::Unchecked);
    anyFinishCheckbox_pri->blockSignals(false);
}

