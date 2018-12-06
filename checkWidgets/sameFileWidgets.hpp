#ifndef ACTONQTG_SAMEFILEWIDGETS_HPP
#define ACTONQTG_SAMEFILEWIDGETS_HPP

#include <QObject>

class QPlainTextEdit;
//class QPushButton;
class QVBoxLayout;
//class QCheckBox;
class QFileDialog;

class checkData_c;

class sameFileWidgets_c : public QObject
{
    Q_OBJECT

    QPlainTextEdit* fileAPTE_pri;
    QPlainTextEdit* fileBPTE_pri;

    checkData_c* const checkData_ptr_pri;

    QFileDialog* browseFileDialog_pri;

    bool browsingFileA_pri = false;
    bool browsingFileB_pri = false;

    void loadCheckSpecificData_f();
    void saveCheckDataJSON_f() const;
    void browseFile_f();
public:
    explicit sameFileWidgets_c(
            checkData_c* const checkData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
            , QObject *parent = nullptr);

Q_SIGNALS:
    void JSONSaved_signal();
public Q_SLOTS:
    void parentClosing_f();
    void save_f();
private Q_SLOTS:
    void fileDialogBrowseFinished_f(const int result_par);
    void browseFileA_f();
    void browseFileB_f();
    void showCurrentWorkingDirectoryWindow_f();
};

#endif // ACTONQTG_SAMEFILEWIDGETS_HPP
