#ifndef ACTONQTG_ACTIONWINDOW_HPP
#define ACTONQTG_ACTIONWINDOW_HPP

#include "actonQtso/actionData.hpp"

#include <QWidget>

class QComboBox;
class QTableWidget;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QSplitter;
class QMessageBox;

class runProcessWidgets_c;
class createDirectoryWidgets_c;

class actionWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    QVBoxLayout* variableLayout_pri;
    QSplitter* mainSplitter_pri;

    QPlainTextEdit* actionStringIdPTE_pri;
    QPlainTextEdit* descriptionPTE_pri;

    //to choose an specific action type
    QComboBox* actionTypeCombo_pri;

    QCheckBox* checksEnabledCheckbox_pri;
    QCheckBox* enabledCheckbox_pri;

    QMessageBox* askUpdateStringIdDepdenciesWindow_pri = nullptr;
    //halt after if this action fails
    //QCheckBox* haltOnFail_pri;

    //starting reference to get the actionData
    const int row_pri_con = 0;
    bool isNew_pri = true;
    actionData_c actionDataNew_pri;
    actionData_c* actionData_ptr_pri = &actionDataNew_pri;

    //old combo index to know if the initial one (which shouldn't trigger a clear)
    int lastIndex_pri = -1;
    QString oldStringId_pri;
    //different action class pointers variables
    runProcessWidgets_c* runProcessWidgets_pri = nullptr;
    createDirectoryWidgets_c* createDirectoryWidgets_pri = nullptr;

    void clearLayoutItems_f(QLayout* layout_par);

    template <typename T>
    void createActionTypeWidgets_f(T*& actionTypeClassPtr_par);

    void createWidgetsPerAction_f(const int index_par_con);
    void removeWidgetClassPerAction_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

public:
    explicit actionWindow_c(
            const int row_par_con
            , QWidget *parent_par = nullptr
    );

Q_SIGNALS:
    void closeWindow_signal();
    void saveJSON_signal();
    void updateRow_Signal(const int row_par_con);
    void askUpdateStringIdDepdencies_signal();
public Q_SLOTS:
private Q_SLOTS:
    void actionComboChanged_f(int index_par);
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    void manageChecksButtonClicked_f();
    //void tipsButtonClicked_f();
    void save_f();
    void openAskUpdateStringIdDepdenciesWindow_f();
    void askUpdateStringIdDepdenciesWindowFinished_f(const int result_par);
};

#endif // ACTONQTG_ACTIONWINDOW_HPP
