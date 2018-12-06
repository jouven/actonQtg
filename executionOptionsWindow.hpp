#ifndef ACTONQTG_EXECUTIONOPTIONSWINDOW_HPP
#define ACTONQTG_EXECUTIONOPTIONSWINDOW_HPP

#include <QWidget>

//class QComboBox;
//class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QLineEdit;

class executionOptionsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;

    //TODO loop count, i.e. loop 100 times
    QCheckBox* loopExecutionCheckbox_pri;
    QCheckBox* stopExecutingOnErrorCheckbox_pri;
    QLineEdit* extraThreadsLineEdit_pri;
    QLineEdit* killTimeoutMillisecondsLineEdit_pri;

    void closeEvent(QCloseEvent* event) override;

    void save_f();
    void load_f();
public:
    explicit executionOptionsWindow_c(
            QWidget *parent_par = nullptr
    );

//Q_SIGNALS:
//public Q_SLOTS:
private Q_SLOTS:
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    void tipsButtonClicked_f();
};

#endif // ACTONQTG_EXECUTIONOPTIONSWINDOW_HPP
