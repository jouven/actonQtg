#ifndef ACTONQTG_MAINWINDOW_HPP
#define ACTONQTG_MAINWINDOW_HPP

#include "actonQtso/actionMappings/actions.hpp"

#include <QWidget>

class QTableWidget;
//class QPlainTextEdit;
//class QComboBox;
class QPushButton;
class QLabel;
//class QRadioButton;
//class QCheckBox;
class QFileDialog;
class QMessageBox;
class QInputDialog;
class QStringList;

#ifdef __ANDROID__
class QScrollArea;
#endif

class action_c;
class executionResult_c;
class executionMessage_c;

class mainWindow_c : public QWidget
{
    Q_OBJECT

#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif
    QLabel* statusBarLabel_pri = nullptr;
    QPushButton* executeActionsButton_pri = nullptr;

    QTableWidget* actionsTable_pri = nullptr;

    //depends on the mainloop interval (it might not be a second)
    int finalCounterSeconds_pri = 2;
    //uint_fast64_t killCountdown_pri = 0;
    uint_fast64_t elapsedStopping_pri = 0;

    QString lastLoadedFilePath_pri;
    bool closingWindow_pri = false;
    bool triedToSaveOnExit_pri = false;

    QFileDialog* selectActionFilesToLoadDialog_pri = nullptr;
    QFileDialog* saveActionFileDialog_pri = nullptr;
    QMessageBox* overwriteLastActionLoadedMessageBox_pri = nullptr;
    QMessageBox* onExitAskSaveIfLastLoadedFileChangedMessageBox_pri = nullptr;
    QInputDialog* copyActionIndexInputDialog_pri = nullptr;
    QInputDialog* changeActionIndexInputDialog_pri = nullptr;
    QMessageBox* runFromStoppedActionMessageBox_pri = nullptr;

    QMessageBox* askAboutExecutingActionsOnCloseMessageBox_pri = nullptr;
    QMessageBox* askAboutStoppingExecutionOnCloseMessageBox_pri = nullptr;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

    void clearActions_f();

    void loadFileList_f(const QStringList& fileList_par_con);
    void updateExistingActionRow_f(const action_c* actionPtr_par_con, const int row_par_con);
    void insertActionRow_f(const action_c* actionPtr_par_con, const int row_par_con = -1);
    void moveSelectedActions_f(const int moveOffSet_par_con);

    void saveActionFile_f(const QString& savePath_par_con);
    void executeActions_f();
    void resumeActionsExecution_f();
    void clearAllRowsResultColumns_f();

    void createMessageBoxAskAboutExecutingActionsOnClose_f();
    void createMessageBoxAskAboutExecutingChecksOnClose_f();
    void createMessageBoxAskAboutStoppingExecutionOnClose_f();
public:
    explicit mainWindow_c();
public Q_SLOTS:
    void start_f();
    void updateMaxThreads_f();
Q_SIGNALS:
    //NOT IN USE, QString is the text to set
    void setStatusBarText_signal(const QString&);
    //NOT IN USE, args are row, column and text to set
    void setRowCellField_signal(const int, const int, const QString&);
    //NOT IN USE
    void setHashRowCellField_signal(const int, const int, const QString&);
    //NOT IN USE, row index
    void scrollToItem_signal(const int);
    //void resizeFileTable_signal();

    void closeWindow_signal();
private Q_SLOTS:
    //void contextMenu(const QPoint &pos);

    //NOT IN USE
    void setRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con);
    //NOT IN USE
    void setHashRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con);
    //NOT IN USE
    void scrollToItem_f(const int row_par_con);

    //reenable for android? (resizeFileTable_signal too)
    //void resizeFileTable_f();

    void showAboutMessage_f();
    //void mainLoop_f();
    void showOptions_f();
    void showTips_f();

    void loadActionsFileButtonPushed_f();
    void fileDialogActionFilesToLoadFinished_f(const int result_par);
    void saveActionFileButtonPushed_f();
    void messageBoxOverwriteLastActionLoadedFileFinished_f(const int result_par);
    void fileDialogSelectSaveActionFileFinished_f(const int result_par);

    void clearAllActions_f();
    void addUpdateAction_f();
    void removeActions_f();

    void showExecutionResults_f();
    void updateActionRow_f(const int row_par_con);
    void moveSelectedActionsDownByOne_f();
    void moveSelectedActionsUpByOne_f();
    void openChangeActionIndexWindow_f();
    void inputDialogChangeActionIndexFinished_f(const int result_par);
    void copyAction_f();
    void inputDialogCopyActionIndexFinished_f(const int result_par);

    //functions dealing with action signal results
    void updateActionOutput_f(const executionResult_c* executionResult_ptr_par, const executionMessage_c* message_par);
    void updateActionError_f(const executionResult_c* executionResult_ptr_par, const executionMessage_c* message_par);
    void updateActionExecutionState_f(const action_c* action_par_ptr_con);

    void runFromStoppedActionsMessageBoxResult_f(const int result_par);
    void executeActionsButtonClicked_f();
    void showExecutionOptionsButtonClicked_f();

    void executionFinished_f();
    void executionStarted_f();
    void stoppingExecution_f();
    void executionStopped_f();
    //void actionResultsCleared_f(action_c* const action_par_ptr_con);
    void stopExecutingActionsAndClose_f();
    void stopExecutingActionsElseKillAndClose_f();
    void killExecutingActionsAndClose_f();
    //public Q_SLOTS:
    void messageBoxSaveActionsToFileOnExitFinished_f(const int result_par);
};

extern mainWindow_c* mainWindow_ptr_ext;

#endif //ACTONQTG_MAINWINDOW_HPP
