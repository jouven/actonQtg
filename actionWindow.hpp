#ifndef ACTONQTG_ACTIONWINDOW_HPP
#define ACTONQTG_ACTIONWINDOW_HPP

#include <QWidget>
#include <QJsonObject>

class QComboBox;
class QTableWidget;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QSplitter;

class runProcessWidgets_c;
class createDirectoryWidgets_c;

class actionWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //QLabel* statusBarLabel_pri;
    QVBoxLayout* variableLayout_pri;
    QSplitter* mainSplitter_pri;

    QPlainTextEdit* descriptionPTE_pri;

    //to choose actions, move, copy, delete, create folder, command
    QComboBox* actionsCombo_pri;
    //halt after if this action fails
    QCheckBox* haltOnFail_pri;

    const int row_pri_con = 0;
    const int_fast32_t actionDataId_pri_con = 0;

    //old combo index to know if the initial one (which shouldn't trigger a clear)
    int lastIndex_pri = -1;
    //different action class pointers variables
    runProcessWidgets_c* runProcessWidgets_pri = Q_NULLPTR;
    createDirectoryWidgets_c* createDirectoryWidgets_pri = Q_NULLPTR;

    void clearLayoutItems_f(QLayout* layout_par);

    template <typename T>
    void createActionTypeWidgets_f(T*& actionTypeClassPtr_par);

    void createWidgetsPerAction_f(const int index_par_con);
    void removeWidgetClassPerAction_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

public:
    explicit actionWindow_c(
            const int row_par_con
            , const int_fast32_t actionDataId_par_con
            , QWidget *parent_par = nullptr
    );

Q_SIGNALS:
    void closeWindow_signal();
    void saveChild_signal();
    void updateRow_Signal(const int row_par_con);
public Q_SLOTS:
private Q_SLOTS:
    void actionComboChanged_f(int index_par);
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    void tipsButtonClicked_f();
    void save_f(const QJsonObject& childSaveResult_par_con);
};

#endif // ACTONQTG_ACTIONWINDOW_HPP
