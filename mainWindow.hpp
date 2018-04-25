#ifndef ACTONQTG_MAINWINDOW_HPP
#define ACTONQTG_MAINWINDOW_HPP

#include "actonQtso/mappings/actions.hpp"

#include <QWidget>
#include <QMutexLocker>

#include <vector>

class QTableWidget;
//class QPlainTextEdit;
//class QComboBox;
class QPushButton;
class QLabel;
class QVBoxLayout;
//class QRadioButton;
//class QCheckBox;
class QFileDialog;
class QMessageBox;
class QInputDialog;

class actionWindow_c;

#ifdef __ANDROID__
class QScrollArea;
#endif

//TODO commit other projects first, other libraries and make a repo for essentialQtgso

//TODO 1 add shortcut keys to the buttons using the "&" notation
//2 actions that use paths check for the existence
//"2" requires some narrowing on when, it might not be worth it, maybe an action to check other actions existing paths

class mainWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif
    QLabel* statusBarLabel_pri;
    QPushButton* executeActionsButton_pri;

    QTableWidget* actionsTable_pri;
    actionWindow_c* actionWindow_pri;

#ifdef Q_OS_WIN32
    QCheckBox* useSlashSeparatorCheckbox_pri;
#endif

    //depends on the mainloop interval (it might not be a second)
    int finalCounterSeconds_pri = 2;

    QString lastLoadedFilePath_pri;

    std::vector<int_fast32_t> runningActionIds_pri;
    bool runningActions_pri = false;
    bool haltRunningActions_pri = false;

    QFileDialog* selectActionFilesToLoadDialog_pri = Q_NULLPTR;
    QFileDialog* saveActionFileDialog_pri = Q_NULLPTR;
    QMessageBox* overwriteLastActionLoadedMessageBox_pri = Q_NULLPTR;
    QInputDialog* copyActionIndexInputDialog_pri = Q_NULLPTR;
    QInputDialog* changeActionIndexInputDialog_pri = Q_NULLPTR;
    QMessageBox* runFromHaltedActionMessageBox_pri = Q_NULLPTR;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

    void clearActions_f();

    void loadFileList_f(const QStringList& fileList_par_con);
    void updateExistingActionRow_f(const action_ec& actionType_par_con, const QString& description_par_con, const bool haltOnFail_par_con, const int row_par_con);
    void insertActionRow_f(const action_ec& actionType_par_con, const QString& description_par_con, const bool haltOnFail_par_con, const int row_par_con = -1);
    void moveSelectedActions_f(const int moveOffSet_par_con);

    void saveActionFile_f(const QString& savePath_par_con);
    void executeActions_f();
    void clearAllRowsResultColumns_f();
public:
    mainWindow_c();
    void processPositionalArguments_f(const QStringList& positionalArguments_par_con);
    bool actionsRunning_f() const;
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

    //void executeNextAction_signal();
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
    void mainLoop_f();
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

    void showExecutionDetails_f();
    void updateActionRow_f(const int row_par_con);
    void moveSelectedActionsDownByOne_f();
    void moveSelectedActionsUpByOne_f();
    void openChangeActionIndexWindow_f();
    void inputDialogChangeActionIndexFinished_f(const int result_par);
    void copyAction_f();
    void inputDialogCopyActionIndexFinished_f(const int result_par);

    //functions dealing with action signal results
    void updateActionOutput_f(const int_fast32_t actionId_par_con);
    void updateActionError_f(const int_fast32_t actionId_par_con);
    void updateActionExecutionState_f(const int_fast32_t actionId_par_con);

    void executeNextAction_f();
//public Q_SLOTS:
    //void actionChangedStatus_f(const int_fast32_t actionId_par_con);
    void haltExecution_f();
    void runFromHaltedActionsMessageBoxResult_f(const int result_par);
    void executeActionsButtonPushed_f();
};

//extern mainWindow_c& mainWindowRef_f();

#endif //ACTONQTG_MAINWINDOW_HPP
