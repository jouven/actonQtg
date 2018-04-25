#include "actionExecutionDetailsWindow.hpp"

#include "actonQtso/actionData.hpp"
#include "appConfig.hpp"
#include "actonQtso/mappings/actionExecutionStateStrMapping.hpp"

//#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QSplitter>

void actionExecutionDetailsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_f().setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_f().setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

actionExecutionDetailsWindow_c::actionExecutionDetailsWindow_c(
        const int_fast32_t actionDataId_par_con
        , QWidget *parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("actionExecutionDetailsWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //state and return code

    //output
    //error

    //actionexternaloutput
    //actionexternalerror

    //execution state and action return code
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    QLabel* executionStateLabelTmp = new QLabel(tr("Execution state"));
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

    anyFinish_pri = new QCheckBox(tr("Any finish"));
    anyFinish_pri->setToolTip(tr("True if the action is done executing, no matter what state"));
    anyFinish_pri->setMinimumHeight(minHeightTmp);
    firstRowLayoutTmp->addWidget(anyFinish_pri);

    firstRowLayoutTmp->addWidget(new QLabel(tr("Return code")));
    returnCodeTE_pri = new QTextEdit;
    returnCodeTE_pri->setMinimumHeight(minHeightTmp);
    returnCodeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    returnCodeTE_pri->setReadOnly(true);
    firstRowLayoutTmp->addWidget(returnCodeTE_pri);

    //start and finish times
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(tr("Execution start")));
    executionStartDatetimeTE_pri = new QTextEdit;
    executionStartDatetimeTE_pri->setMinimumHeight(minHeightTmp);
    executionStartDatetimeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionStartDatetimeTE_pri->setReadOnly(true);
    secondRowLayoutTmp->addWidget(executionStartDatetimeTE_pri);

    secondRowLayoutTmp->addWidget(new QLabel(tr("Execution finish")));
    executionFinishDatetimeTE_pri = new QTextEdit;
    executionFinishDatetimeTE_pri->setMinimumHeight(minHeightTmp);
    executionFinishDatetimeTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    executionFinishDatetimeTE_pri->setReadOnly(true);
    secondRowLayoutTmp->addWidget(executionFinishDatetimeTE_pri);

    //output
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(new QLabel(tr("Output")));
    outputPTE_pri = new QPlainTextEdit;
    outputPTE_pri->setMinimumHeight(minHeightTmp);
    outputPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    outputPTE_pri->setReadOnly(true);
    thirdRowLayoutTmp->addWidget(outputPTE_pri);

    //error
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    fourthRowLayoutTmp->addWidget(new QLabel(tr("Error")));
    errorPTE_pri = new QPlainTextEdit;
    errorPTE_pri->setMinimumHeight(minHeightTmp);
    errorPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    errorPTE_pri->setReadOnly(true);
    fourthRowLayoutTmp->addWidget(errorPTE_pri);

    //action external output
    QHBoxLayout* fifthRowLayoutTmp = new QHBoxLayout;

    fifthRowLayoutTmp->addWidget(new QLabel(tr("External output")));
    externalOutputPTE_pri = new QPlainTextEdit;
    externalOutputPTE_pri->setMinimumHeight(minHeightTmp);
    externalOutputPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    externalOutputPTE_pri->setReadOnly(true);
    fifthRowLayoutTmp->addWidget(externalOutputPTE_pri);

    //action external error
    QHBoxLayout* sixthRowLayoutTmp = new QHBoxLayout;

    sixthRowLayoutTmp->addWidget(new QLabel(tr("External error")));
    externalErrorPTE_pri = new QPlainTextEdit;
    externalErrorPTE_pri->setMinimumHeight(minHeightTmp);
    externalErrorPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    externalErrorPTE_pri->setReadOnly(true);
    sixthRowLayoutTmp->addWidget(externalErrorPTE_pri);


    //ok button
    QHBoxLayout* seventhRowTmp = new QHBoxLayout;

    QPushButton* okButtonPtrTmp = new QPushButton(tr("Ok"));
    seventhRowTmp->addWidget(okButtonPtrTmp);
    QPushButton* tipsButtonPtrTmp = new QPushButton(tr("Tips"));
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
    mainSplitter_pri->setObjectName("QSplitter");

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

    setWindowTitle(tr("Action execution details"));

    if (appConfig_f().configLoaded_f())
    {
         restoreGeometry(appConfig_f().widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_f().widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    //"ok" it's the same as a cancel
    connect(okButtonPtrTmp, &QPushButton::clicked, this, &actionExecutionDetailsWindow_c::cancelButtonClicked_f);
    connect(tipsButtonPtrTmp, &QPushButton::clicked, this, &actionExecutionDetailsWindow_c::tipsButtonClicked_f);
    connect(anyFinish_pri, &QCheckBox::stateChanged, this, [this](int state_par)
    {
        anyFinish_pri->blockSignals(true);
        anyFinish_pri->setCheckState(state_par == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        anyFinish_pri->blockSignals(false);
    });

    std::pair<actionData_c&, bool> actionPairTmp(dataHub_f().actionData_f(actionDataId_par_con));
    if (actionPairTmp.second)
    {
        actionData_c& actionDataTmp(actionPairTmp.first);
        actionDataExecutionResultPtr_pri = actionDataTmp.actionDataExecutionResultPtr_f();

        updateOutput_f();
        updateError_f();
        updateExternalOutput_f();
        updateExternalError_f();
        updateState_f();
        updateReturnCode_f();
        updateAnyFinish_f();

        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::outputUpdated_signal, this, &actionExecutionDetailsWindow_c::updateOutput_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::errorUpdated_signal, this, &actionExecutionDetailsWindow_c::updateError_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::externalOutputUpdated_signal, this, &actionExecutionDetailsWindow_c::updateExternalOutput_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::externalErrorUpdated_signal, this, &actionExecutionDetailsWindow_c::updateExternalError_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::executionStateUpdated_signal, this, &actionExecutionDetailsWindow_c::updateState_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::returnCodeUpdated_signal, this, &actionExecutionDetailsWindow_c::updateReturnCode_f);
        connect(actionDataExecutionResultPtr_pri, &actionDataExecutionResult_c::anyFinish_signal, this, &actionExecutionDetailsWindow_c::updateAnyFinish_f);
    }
}

void actionExecutionDetailsWindow_c::cancelButtonClicked_f()
{
    close();
}

void actionExecutionDetailsWindow_c::tipsButtonClicked_f()
{
    //TODO
}

void actionExecutionDetailsWindow_c::updateOutput_f()
{
    outputPTE_pri->setPlainText(actionDataExecutionResultPtr_pri->output_f());
}

void actionExecutionDetailsWindow_c::updateError_f()
{
    errorPTE_pri->setPlainText(actionDataExecutionResultPtr_pri->error_f());
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

    if (executionStartDatetimeTE_pri->toPlainText().isEmpty() and actionDataExecutionResultPtr_pri->startTimeSet_f())
    {
        executionStartDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(actionDataExecutionResultPtr_pri->startTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    executionStateTE_pri->setText(actionExecutionStateToStrUMap_glo_sta_con.at(actionDataExecutionResultPtr_pri->actionState_f()));
}

void actionExecutionDetailsWindow_c::updateReturnCode_f()
{
    returnCodeTE_pri->setText(QString::number(actionDataExecutionResultPtr_pri->returnCode_f()));
}

void actionExecutionDetailsWindow_c::updateAnyFinish_f()
{
    if (actionDataExecutionResultPtr_pri->finishedTimeSet_f())
    {
        executionFinishDatetimeTE_pri->setText(
                    QDateTime::fromMSecsSinceEpoch(actionDataExecutionResultPtr_pri->finishedTime_f()).toLocalTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
        );
    }
    anyFinish_pri->blockSignals(true);
    anyFinish_pri->setCheckState(actionDataExecutionResultPtr_pri->anyFinish_f() ? Qt::Checked : Qt::Unchecked);
    anyFinish_pri->blockSignals(false);
}

