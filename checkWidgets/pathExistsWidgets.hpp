#ifndef ACTONQTG_PATHEXISTSWIDGETS_HPP
#define ACTONQTG_PATHEXISTSWIDGETS_HPP

#include "baseClassCheckTypeWidgets.hpp"

class QPlainTextEdit;
//class QPushButton;
class QVBoxLayout;
//class QCheckBox;
class QFileDialog;

class pathExistsData_c;
class check_c;
class pathExistsCheck_c;

class pathExistsWidgets_c : public baseClassCheckTypeWidgets_c
{
    Q_OBJECT

    QPlainTextEdit* pathPTE_pri;

    QFileDialog* browseFileDialog_pri = nullptr;

    pathExistsCheck_c* pathExistsCheck_ptr_pri = nullptr;

    void loadCheckSpecificData_f();

    pathExistsData_c fieldsToSameFileDataObject_f() const;

    bool derivedSaveNew_f(const checkData_c& checkData_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
public:
    explicit pathExistsWidgets_c(
            check_c*& check_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void fileDialogBrowseFinished_f(const int result_par);
    void browseFile_f();
    void showCurrentWorkingDirectoryWindow_f();
};

#endif // ACTONQTG_PATHEXISTSWIDGETS_HPP
