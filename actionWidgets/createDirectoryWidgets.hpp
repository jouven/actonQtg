#ifndef ACTONQTG_CREATEDIRECTORYWIDGETS_HPP
#define ACTONQTG_CREATEDIRECTORYWIDGETS_HPP

#include "baseClassActionTypeWidgets.hpp"

class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;

class createDirectoryData_c;
class createDirectoryAction_c;
class actionData_c;

class createDirectoryWidgets_c : public baseClassActionTypeWidgets_c
{
    Q_OBJECT

    QPlainTextEdit* createDirectoryPathPTE_pri;
    QCheckBox* createParentsCheckbox_pri;
    QCheckBox* errorIfExistsCheckbox_pri;
    QFileDialog* browseDirectoryToCreateDialog_pri = nullptr;

    createDirectoryAction_c* createDirectoryAction_ptr_pri = nullptr;

    void loadActionSpecificData_f();

    bool derivedSaveNew_f(const actionData_c& actionDataBlock_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
    createDirectoryData_c fieldsToCreateDirectoryDataObject_f() const;
public:
    explicit createDirectoryWidgets_c(
            action_c*& actionData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void browseDirectory_f();
    void fileDialogBrowseDirectoryToCreateFinished_f(const int result_par);
};

#endif // ACTONQTG_CREATEDIRECTORYWIDGETS_HPP
