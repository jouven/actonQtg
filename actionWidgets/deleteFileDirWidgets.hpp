#ifndef ACTONQTG_DELETEFILEDIRWIDGETS_HPP
#define ACTONQTG_DELETEFILEDIRWIDGETS_HPP

#include "baseClassActionTypeWidgets.hpp"

class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;

class deleteFileDirData_c;
class deleteFileDirAction_c;
class actionData_c;

class deleteFileDirWidgets_c : public baseClassActionTypeWidgets_c
{
    Q_OBJECT

    QPlainTextEdit* deleteFileDirPathPTE_pri;
    QCheckBox* errorIfNotExistsCheckbox_pri;
    QCheckBox* onlyIfEmptyCheckbox_pri;
    QFileDialog* browseDeleteFileDirDialog_pri = nullptr;

    deleteFileDirAction_c* deleteFileDirAction_ptr_pri = nullptr;

    void loadActionSpecificData_f();

    bool derivedSaveNew_f(const actionData_c& actionDataBlock_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
    deleteFileDirData_c fieldsToDeleteFileDirDataObject_f() const;
public:
    explicit deleteFileDirWidgets_c(
            action_c*& actionData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void browseDirectory_f();
    void fileDialogBrowseDeleteFileDirFinished_f(const int result_par);
};

#endif // ACTONQTG_DELETEFILEDIRWIDGETS_HPP
