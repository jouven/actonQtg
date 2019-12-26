#ifndef ACTONQTG_OPTIONSWINDOW_HPP
#define ACTONQTG_OPTIONSWINDOW_HPP

#include <QWidget>

//class QComboBox;
//class QPlainTextEdit;
class QVBoxLayout;
//class QCheckBox;

class optionsWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
    //FUTURE add which type of time use by default (local or UTC)

    void closeEvent(QCloseEvent* event) override;

public:
    explicit optionsWindow_c(
            QWidget *parent_par = nullptr
    );

Q_SIGNALS:
public Q_SLOTS:
private Q_SLOTS:
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    //void tipsButtonClicked_f();
    void showEnvironmentWindow_f();
    void showWorkingDirectoryWindow_f();
    void showLogsWindow_f();
};

#endif // ACTONQTG_OPTIONSWINDOW_HPP
