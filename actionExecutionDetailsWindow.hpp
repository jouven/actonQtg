#ifndef ACTONQTG_ACTIONEXECUTIONDETAILSWINDOW_HPP
#define ACTONQTG_ACTIONEXECUTIONDETAILSWINDOW_HPP

//OBSOLETE replaced by executionResultsWindow.hpp and executionResultWindows.hpp

#include <QWidget>

class QPlainTextEdit;
class QTextEdit;
class QVBoxLayout;
class QSplitter;
class QCheckBox;

class actionExecutionResult_c;

class actionExecutionDetailsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    QPlainTextEdit* outputPTE_pri;
    QPlainTextEdit* errorPTE_pri;
    QPlainTextEdit* externalOutputPTE_pri;
    QPlainTextEdit* externalErrorPTE_pri;
    QTextEdit* executionStateTE_pri;
    QTextEdit* returnCodeTE_pri;
    QCheckBox* anyFinishCheckbox_pri;

    QTextEdit* executionStartDatetimeTE_pri;
    QTextEdit* executionFinishDatetimeTE_pri;

    QSplitter* mainSplitter_pri;

    actionExecutionResult_c* actionDataExecutionResultPtr_pri = nullptr;

    void closeEvent(QCloseEvent* event) override;
public:
    explicit actionExecutionDetailsWindow_c(
            actionExecutionResult_c* actionDataExecutionResult_ptr_par
            , QWidget *parent_par = nullptr
    );

public Q_SLOTS:
private Q_SLOTS:
    void cancelButtonClicked_f();
    void tipsButtonClicked_f();

    void updateOutput_f();
    void updateError_f();
    void updateExternalOutput_f();
    void updateExternalError_f();
    void updateState_f();
    void updateReturnCode_f();
    void updateAnyFinish_f();
};

#endif // ACTONQTG_ACTIONEXECUTIONDETAILSWINDOW_HPP
