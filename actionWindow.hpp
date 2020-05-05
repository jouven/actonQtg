#ifndef ACTONQTG_ACTIONWINDOW_HPP
#define ACTONQTG_ACTIONWINDOW_HPP

#include <QWidget>

class QComboBox;
class QTableWidget;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QSplitter;
class QMessageBox;
class QPushButton;
class QLineEdit;

class actionData_c;
class action_c;
class baseClassActionTypeWidgets_c;

class actionWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    QVBoxLayout* variableLayout_pri;
    QSplitter* mainSplitter_pri;

    QLineEdit* actionIdLineEdit_pri;

    QPlainTextEdit* actionStringIdPTE_pri;
    QPlainTextEdit* descriptionPTE_pri;

    //to choose an specific action type
    QComboBox* actionTypeCombo_pri;

    QCheckBox* checksEnabledCheckbox_pri;
    QCheckBox* enabledCheckbox_pri;
    QCheckBox* checksResultLogicAndCheckbox_pri;
    QCheckBox* stopAllExecutionOnErrorCheckbox_pri;
    QCheckBox* repeatExecutionCheckbox_pri;

    QPushButton* manageChecks_pri;

    QMessageBox* askUpdateStringIdDepdenciesWindow_pri = nullptr;
    //halt after if this action fails
    //QCheckBox* haltOnFail_pri;

    //starting reference to get the actionData
    const int row_pri_con = 0;
    bool isNew_pri = true;

    action_c* action_ptr_pri = nullptr;

    //old combo index to know if the initial one (which shouldn't trigger a clear)
    int lastIndex_pri = -1;
    //after save if the stringId changed it's asked to update the dependencies with the old value
    //since the save is already done this field is used before saving to save the old value
    //so it can be used, after, to replace dependencies if that option is selected
    QString oldStringId_pri;

    baseClassActionTypeWidgets_c* baseClassActionTypeWidgets_pri = nullptr;

    void clearLayoutItems_f(QLayout* layout_par);

    void createWidgetsPerAction_f(const int index_par_con);
    void removeWidgetClassPerAction_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

    void loadActionData_f();
    actionData_c fieldsToActionDataObject_f() const;
public:
    explicit actionWindow_c(
            const int row_par_con
            , QWidget *parent_par = nullptr
    );

Q_SIGNALS:
    void closeWindow_signal();
    void updateRow_Signal(const int row_par_con);
    //void askUpdateStringIdDepdencies_signal();
public Q_SLOTS:
private Q_SLOTS:
    void actionTypeComboChanged_f(int index_par);
    void saveButtonClicked_f();
    void closeButtonClicked_f();
    void manageChecksButtonClicked_f();
    //void tipsButtonClicked_f();
    bool save_f();
    void openAskUpdateStringIdDepdenciesWindow_f();
    void askUpdateStringIdDepdenciesWindowFinished_f(const int result_par);
};

#endif // ACTONQTG_ACTIONWINDOW_HPP
