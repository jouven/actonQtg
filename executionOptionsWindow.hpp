#ifndef ACTONQTG_EXECUTIONOPTIONSWINDOW_HPP
#define ACTONQTG_EXECUTIONOPTIONSWINDOW_HPP

#include <QWidget>

class textCompilation_c;

//class QComboBox;
//class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QLineEdit;
class QComboBox;

class executionOptionsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;

    QComboBox* executionLoopTypeCombo_pri;
    //QCheckBox* stopExecutingOnErrorCheckbox_pri;
    QLineEdit* loopXtimesCountLineEdit_pri;
    QLineEdit* extraThreadsLineEdit_pri;
    QLineEdit* killTimeoutMillisecondsLineEdit_pri;

    void closeEvent(QCloseEvent* event) override;

    //returns true if successfull
    bool save_f();
    void load_f();
    bool isFieldsDataValid_f(textCompilation_c* errors_par) const;
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
    void stringParserButtonClicked_f();
};

#endif // ACTONQTG_EXECUTIONOPTIONSWINDOW_HPP
