#ifndef ACTONQTG_CHECKWINDOW_HPP
#define ACTONQTG_CHECKWINDOW_HPP

#include "actonQtso/checksDataHub.hpp"

#include <QWidget>
#include <QJsonObject>

class QComboBox;
class QTableWidget;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QSplitter;

class sameFileWidgets_c;
class actionFinishedWidgets_c;

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

    const int row_pri_con = 0;
    checksDataHub_c* checkDataHub_ptr_pri;
    bool isNew_pri = true;
    checkData_c checkDataNew_pri;
    checkData_c* checkData_ptr_pri = &checkDataNew_pri;

    //old combo index to know if the initial one (which shouldn't trigger a clear)
    int lastIndex_pri = -1;
    //different check class pointers variables
    sameFileWidgets_c* sameFileWidgets_pri = nullptr;
    actionFinishedWidgets_c* actionFinishedWidgets_pri = nullptr;

    void clearLayoutItems_f(QLayout* layout_par);

    template <typename T>
    void createCheckTypeWidgets_f(T*& checkTypeClassPtr_par);

    void createWidgetsPerCheck_f(const int index_par_con);
    void removeWidgetClassPerCheck_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

public:
    //continue
    //passar sa referencia en es hub de checks
    explicit checkWindow_c(
            const int row_par_con
            , checksDataHub_c* checkDataHub_ptr_par
            , QWidget *parent_par = nullptr
    );

Q_SIGNALS:
    void closeWindow_signal();
    void saveJSON_signal();
    void updateRow_Signal(const int row_par_con);
public Q_SLOTS:
private Q_SLOTS:
    void checkComboChanged_f(int index_par);
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    void manageChecksButtonClicked_f();
    void tipsButtonClicked_f();
    void save_f();
};

#endif // ACTONQTG_CHECKWINDOW_HPP
