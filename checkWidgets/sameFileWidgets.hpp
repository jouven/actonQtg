#ifndef ACTONQTG_SAMEFILEWIDGETS_HPP
#define ACTONQTG_SAMEFILEWIDGETS_HPP

#include "baseClassCheckWidgets.hpp"

class QPlainTextEdit;
//class QPushButton;
class QVBoxLayout;
//class QCheckBox;
class QFileDialog;

class sameFileData_c;
class check_c;
class sameFileCheck_c;

class sameFileWidgets_c : public baseClassCheckWidgets_c
{
    Q_OBJECT

    QPlainTextEdit* fileAPTE_pri;
    QPlainTextEdit* fileBPTE_pri;

    check_c* check_ptr_pri = nullptr;

    QFileDialog* browseFileDialog_pri = nullptr;

    bool browsingFileA_pri = false;
    bool browsingFileB_pri = false;

    sameFileCheck_c* sameFileCheck_ptr_pri = nullptr;

    void loadCheckSpecificData_f();
    void browseFile_f();
    bool isFieldsDataValid_f() const;
    sameFileData_c fieldsToSameFileDataObject_f() const;

    bool derivedSaveNew_f(const checkData_c& checkData_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
public:
    explicit sameFileWidgets_c(
            check_c*& checkData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void fileDialogBrowseFinished_f(const int result_par);
    void browseFileA_f();
    void browseFileB_f();
    void showCurrentWorkingDirectoryWindow_f();
};

#endif // ACTONQTG_SAMEFILEWIDGETS_HPP
