#ifndef ACTONQTG_ACTIONCHECKSWINDOW_HPP
#define ACTONQTG_ACTIONCHECKSWINDOW_HPP

#include "actonQtso/checkMappings/checks.hpp"

#include <QWidget>

#include <deque>
#include <vector>

class QTableWidget;
class QVBoxLayout;
class QSplitter;
class QInputDialog;
class QPushButton;
class QLabel;
class QMessageBox;

#ifdef __ANDROID__
class QScrollArea;
#endif

class executionResult_c;
class executionMessage_c;
class checkExecutionResult_c;
class check_c;
class checksDataHub_c;

class actionChecksWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif
    QLabel* statusBarLabel_pri;
    //QSplitter* mainSplitter_pri;
    QPushButton* executeChecksButton_pri;

    QTableWidget* actionChecksTable_pri;

    checksDataHub_c* checkDataHub_ptr_pri;

    //checks to run sequentially
    std::deque<check_c*> checksToExecuteSeq_pri;
    //checks that are running or finished running (only the last part is considered right now)
    std::vector<check_c*> checksStarted_pri;

    bool stoppingCheckExecution_pri = false;
    bool finishedExecutingChecks_pri = false;

    QInputDialog* copyCheckIndexInputDialog_pri = nullptr;
    QInputDialog* changeCheckIndexInputDialog_pri = nullptr;

    QMessageBox* askAboutExecutingChecksOnCloseMessageBox_pri = nullptr;

    //different action class pointers variables
    //runProcessWidgets_c* runProcessWidgets_pri = nullptr;
    //createDirectoryWidgets_c* createDirectoryWidgets_pri = nullptr;

    //void clearLayoutItems_f(QLayout* layout_par);

    //template <typename T>
    //void createActionTypeWidgets_f(T*& actionTypeClassPtr_par);

    //void createWidgetsPerAction_f(const int index_par_con);
    //void removeWidgetClassPerAction_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

    void clearAllRowsResultColumns_f();
    void updateExistingCheckRow_f(const check_c* checkPtr_par_con, const int row_par_con);
    void insertCheckRow_f(const check_c* checkPtr_par_con, const int row_par_con = -1);
    void moveSelectedChecks_f(const int moveOffSet_par_con);
    void executeChecks_f();

    void createMessageBoxAskAboutExecutingChecksOnClose_f();
public:
    explicit actionChecksWindow_c(
            checksDataHub_c* checkDataHub_ptr_par
            , QWidget *parent_par = nullptr
    );

    void show_f();
Q_SIGNALS:
    //void closeWindow_signal();
    void saveChild_signal();
    void updateRow_Signal(const int row_par_con);
public Q_SLOTS:
private Q_SLOTS:
    void tipsButtonClicked_f();
    void okButtonClicked_f();
    void addEditActionCheckButtonClicked_f();
    void updateCheckRow_f(const int row_par_con);
    void removeChecksButtonClicked_f();
    void copyCheckButtonClicked_f();
    void executeChecksButtonClicked_f();
    void showExecutionDetailsButtonClicked_f();

    void moveSelectedChecksUpByOneButtonClicked_f();
    void openChangeCheckIndexWindowButtonClicked_f();
    void moveSelectedChecksDownByOneButtonClicked_f();
    void inputDialogCopyCheckIndexFinished_f(const int result_par);
    void inputDialogChangeCheckIndexFinished_f(const int result_par);
    void updateCheckError_f(const executionResult_c* executionResult_par, const executionMessage_c* errorMessage_par);
    void updateCheckExecutionState_f(check_c* const check_par_ptr_con);
    void updateCheckResult_f(const executionResult_c* checkExecutionResult_par_con);
    void stoppingExecution_f();
    void executionStarted_f();
    void executionFinished_f();
    //void checkResultsCleared_f(check_c* const check_par_ptr_con);

    void stopExecutingChecksAndClose_f();
};

#endif // ACTONQTG_ACTIONCHECKSWINDOW_HPP
