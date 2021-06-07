#ifndef ACTONQTG_EXECUTIONRESULTSWINDOW_HPP
#define ACTONQTG_EXECUTIONRESULTSWINDOW_HPP

#include <QWidget>

class QVBoxLayout;
class QTableWidget;
#ifdef __ANDROID__
class QScrollArea;
#endif

class action_c;
class check_c;
class executionResult_c;

//this object windows don't block other windows and are parented to the main window
class executionResultsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri = nullptr;
#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif
    //TODO scrolltobottom checkbox, review what happens on multiple executions

    QTableWidget* executionResultsTable_pri = nullptr;

    std::vector<action_c*> actionsToFilter_pri;
    std::vector<check_c*> checksToFilter_pri;

    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event_par) override;

    void addExecutionResultEntry_f(const executionResult_c* logItem_par_con);

    void loadExecutionResults_f();

public:
    //to show all executionResults
    explicit executionResultsWindow_c();
    //param can be empty in both ctors, will work like the above ctor
    //when selecting one or more actions only show the execution messages of the selected (plus the executions message of check/s children of the actions)
    explicit executionResultsWindow_c(const std::vector<action_c*>& actionsToFilter_par);
    //when selecting one or more checks only show the execution messages of the selected
    explicit executionResultsWindow_c(const std::vector<check_c*>& checksToFilter_par);

    void show_f();

Q_SIGNALS:
public Q_SLOTS:
private Q_SLOTS:
    void closeButtonClicked_f();

    //void tipsButtonClicked_f();
    void addSingleExecutionResultEntry_f(const executionResult_c* executionResult_par_con);
    void updateTableData_f(const executionResult_c* executionResultItem_par_con);
    void viewExecutionResultButtonClicked_f();
};

#endif // ACTONQTG_EXECUTIONRESULTSWINDOW_HPP
