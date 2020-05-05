#ifndef ACTONQTG_FOLDERCHANGEREACTIONWIDGETS_HPP
#define ACTONQTG_FOLDERCHANGEREACTIONWIDGETS_HPP

#include "baseClassActionTypeWidgets.hpp"

class QPlainTextEdit;
class QLineEdit;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;
class QComboBox;
class QLabel;

class folderChangeReactionData_c;
class folderChangeReactionAction_c;
class actionData_c;
class textCompilation_c;

class folderChangeReactionWidgets_c : public baseClassActionTypeWidgets_c
{
    Q_OBJECT

    QPlainTextEdit* folderPathPTE_pri;

    QLineEdit* monitorIntervalMsLineEdit_pri;

    QCheckBox* recurseFoldersCheckBox_pri;
    QCheckBox* useAbsolutePathsCheckbox_pri;

    //changes to monitor
    QCheckBox* fileContentsChangedCheckBox_pri;
    QCheckBox* fileCreatedCheckBox_pri;
    QCheckBox* fileModifiedDateTimeChangedCheckBox_pri;
    QCheckBox* fileRemovedCheckBox_pri;
    QCheckBox* fileSizeChangedCheckBox_pri;

    //fileSizeChangedCheckBox_pri which file size changes to monitor
    QComboBox* fileSizeChangeTypeToMonitorComboBox_pri;

    QLineEdit* fileSizeChangeFixedLineEdit_pri;
    QLineEdit* fileSizeChangeThresholdLineEdit_pri;

    QComboBox* reactionTypeComboBox_pri;
    QLineEdit* argumentPlaceHolderToReplaceWithChangedFilePath_pri;

    QCheckBox* sequentialReactionCheckBox_pri;

    QComboBox* reactionOrderComboBox_pri;
    QCheckBox* reverseReactionOrderCheckBox_pri;

    QComboBox* reactionActionStringIdCombo_pri;

    QFileDialog* browseFolderDialog_pri = nullptr;

    folderChangeReactionAction_c* folderChangeReactionAction_ptr_pri = nullptr;

    void loadActionSpecificData_f();

    bool derivedSaveNew_f(const actionData_c& actionDataBlock_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
    folderChangeReactionData_c fieldsToFolderChangeReactionDataObject_f() const;
    bool isFieldsDataValid_f(textCompilation_c* errors_par) const;
    void loadReactionActionStringIdCombo_f();
public:
    explicit folderChangeReactionWidgets_c(
            action_c*& actionData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void browseDirectory_f();
    void fileDialogBrowseFolderFinished_f(const int result_par);
    void fileSizeChangeTypeToMonitorComboChanged_f(int);
    void fileSizeChangedCheckBoxChanged_f();
    void reactionTypeComboChanged_f(int);
};

#endif // ACTONQTG_FOLDERCHANGEREACTIONWIDGETS_HPP
