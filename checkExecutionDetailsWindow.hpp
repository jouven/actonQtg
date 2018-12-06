#ifndef ACTONQTG_CHECKEXECUTIONDETAILSWINDOW_HPP
#define ACTONQTG_CHECKEXECUTIONDETAILSWINDOW_HPP

#include <QWidget>

class QPlainTextEdit;
class QTextEdit;
class QVBoxLayout;
class QSplitter;
class QCheckBox;

class checkDataExecutionResult_c;


class checkExecutionDetailsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    QPlainTextEdit* errorPTE_pri;
    QTextEdit* executionStateTE_pri;

    QCheckBox* anyFinishCheckbox_pri;
    QCheckBox* checkResultCheckbox_pri;

    QTextEdit* executionStartDatetimeTE_pri;
    QTextEdit* executionFinishDatetimeTE_pri;

    QSplitter* mainSplitter_pri;

    checkDataExecutionResult_c* checkDataExecutionResultPtr_pri = nullptr;

    void closeEvent(QCloseEvent* event) override;
public:
    explicit checkExecutionDetailsWindow_c(
            checkDataExecutionResult_c* checkDataExecutionResult_ptr_par
            , QWidget *parent_par = nullptr
    );

public Q_SLOTS:
private Q_SLOTS:
    void cancelButtonClicked_f();
    void tipsButtonClicked_f();

    void updateError_f();
    void updateState_f();
    void updateAnyFinish_f();
};

#endif // ACTONQTG_CHECKEXECUTIONDETAILSWINDOW_HPP
