#ifndef ACTONQTG_CREATEDIRECTORYWIDGETS_HPP
#define ACTONQTG_CREATEDIRECTORYWIDGETS_HPP

#include <QObject>

#include <utility>

class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;

class actionData_c;

class createDirectoryWidgets_c : public QObject
{
    Q_OBJECT

    QPlainTextEdit* createDirectoryPathPTE_pri;
    QCheckBox* createParentsCheckbox_pri;
    QCheckBox* errorIfExistsCheckbox_pri;
    QFileDialog* browseDirectoryToCreateDialog_pri = nullptr;

    actionData_c* const actionData_ptr_pri;

    void loadActionSpecificData_f();
    void saveActionDataJSON_f() const;
public:
    explicit createDirectoryWidgets_c(
            actionData_c* const actionData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
            , QObject *parent = nullptr);

Q_SIGNALS:
    //this signal needs to always have this name because of template usage in actionWindow.cpp
    void JSONSaved_signal();
public Q_SLOTS:
    void parentClosing_f();
    void save_f();
private Q_SLOTS:
    void browseDirectory_f();
    void fileDialogBrowseDirectoryToCreateFinished_f(const int result_par);
};

#endif // ACTONQTG_CREATEDIRECTORYWIDGETS_HPP
