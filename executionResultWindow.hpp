#ifndef ACTONQTG_EXECUTIONRESULTWINDOW_HPP
#define ACTONQTG_EXECUTIONRESULTWINDOW_HPP

#include <QWidget>

class QPlainTextEdit;
class QTextEdit;
class QVBoxLayout;
//class QSplitter;
class QCheckBox;
class QTableWidget;

class executionResult_c;
class checkExecutionResult_c;
class actionExecutionResult_c;
class executionMessage_c;
class action_c;
class check_c;

class executionResultWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    //QPlainTextEdit* outputPTE_pri;
    //QPlainTextEdit* errorPTE_pri;
    //QPlainTextEdit* externalOutputPTE_pri;
    //QPlainTextEdit* externalErrorPTE_pri;
    QTextEdit* executionStateTE_pri;
    QTextEdit* returnCodeTE_pri = nullptr;
    QCheckBox* anyFinishCheckbox_pri = nullptr;
    QCheckBox* checkResultCheckbox_pri = nullptr;

    QTextEdit* executionStartDatetimeTE_pri;
    QTextEdit* executionFinishDatetimeTE_pri;

    QTableWidget* executionMessagesTable_pri;
    //QSplitter* mainSplitter_pri;

    //actionDataExecutionResult_c* actionDataExecutionResultPtr_pri = nullptr;

    std::vector<executionResult_c*> executionResults_pri;

    executionResult_c* executionResult_pri = nullptr;
    checkExecutionResult_c* checkExecutionResult_pri = nullptr;
    actionExecutionResult_c* actionExecutionResult_pri = nullptr;

    enum class resultMode_ec
    {
        empty
        , allMessages
        , selectedMessages
        , action
        , check
    };

    resultMode_ec resultMode_pri = resultMode_ec::empty;

    void closeEvent(QCloseEvent* event) override;
    void addExecutionMessageEntry_f(const executionMessage_c* const executionMessage_par_con);
    void loadMessages_f();
public:
    explicit executionResultWindow_c(
            const std::vector<executionResult_c*>& executionResults_par_con
            , QWidget *parent_par = nullptr
    );
    void show_f();

public Q_SLOTS:
private Q_SLOTS:
    void cancelButtonClicked_f();
    void tipsButtonClicked_f();

//    void updateOutput_f();
//    void updateError_f();
//    void updateExternalOutput_f();
//    void updateExternalError_f();
    void addExecutionMessage_f(const executionMessage_c* executionMessage_par);
    void addMessageAction_f(executionResult_c* executionResult_par, executionMessage_c* executionMessage_par);
    void addMessageCheck_f(executionResult_c* executionResult_par, executionMessage_c* executionMessage_par);
    void updateState_f();
    void updateReturnCode_f();
    void updateAnyFinish_f();
};

#endif // ACTONQTG_EXECUTIONRESULTWINDOW_HPP
