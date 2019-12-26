#ifndef ACTONQTG_CHECKWINDOW_HPP
#define ACTONQTG_CHECKWINDOW_HPP

#include <QWidget>

class QComboBox;
class QTableWidget;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QSplitter;

class checksDataHub_c;
class checkData_c;
class check_c;
class baseClassCheckTypeWidgets_c;

class checkWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    QVBoxLayout* variableLayout_pri;
    QSplitter* mainSplitter_pri;

    //QPlainTextEdit* checkStringIdPTE_pri;
    QPlainTextEdit* descriptionPTE_pri;

    //to choose the check type ("check same file", "check if action finished"...)
    QComboBox* checkTypeCombo_pri;
    QComboBox* resultLogicCombo_pri;

    QCheckBox* enabledCheckbox_pri;
    QCheckBox* threadedCheckbox_pri;

    const int row_pri_con = 0;
    checksDataHub_c* checkDataHub_ptr_pri;
    bool isNew_pri = true;

    check_c* check_ptr_pri = nullptr;

    //old combo index to know if the initial one (which shouldn't trigger a clear)
    int lastIndex_pri = -1;

    baseClassCheckTypeWidgets_c* baseClassCheckWidgets_pri = nullptr;

    void clearLayoutItems_f(QLayout* layout_par);

    void createWidgetsPerCheck_f(const int index_par_con);
    void removeWidgetClassPerCheck_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

    checkData_c fieldsToCheckDataObject_f() const;
    void loadCheckData_f();
public:
    explicit checkWindow_c(
            const int row_par_con
            , checksDataHub_c* checkDataHub_ptr_par
            , QWidget *parent_par = nullptr
    );

Q_SIGNALS:
    void updateRow_Signal(const int row_par_con);
public Q_SLOTS:
private Q_SLOTS:
    void checkTypeComboChanged_f(int index_par);
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    void tipsButtonClicked_f();
    bool save_f();
};

#endif // ACTONQTG_CHECKWINDOW_HPP
