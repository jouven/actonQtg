#ifndef ACTONQTG_LOGSWINDOW_HPP
#define ACTONQTG_LOGSWINDOW_HPP

#include <QWidget>

class QVBoxLayout;
class QTableWidget;
#ifdef __ANDROID__
class QScrollArea;
#endif

class logItem_c;

class logsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif

    QTableWidget* logsTable_pri;

    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event_par) override;

    void addLogEntry_f(const int index_par_con, const logItem_c* const logItem_par_con, const QDateTime* const logDateTime_par_con);

    void loadLogs_f();
public:
    explicit logsWindow_c();

Q_SIGNALS:
public Q_SLOTS:
private Q_SLOTS:
    void cancelButtonClicked_f();

    //void tipsButtonClicked_f();
    void addSingleLogEntry_f(const int index_par_con, const logItem_c* const logItem_par_con, const QDateTime* const logDateTime_par_con);
};

#endif // ACTONQTG_LOGSWINDOW_HPP
