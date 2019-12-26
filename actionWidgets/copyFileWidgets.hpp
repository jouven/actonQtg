#ifndef ACTONQTG_COPYFILEWIDGETS_HPP
#define ACTONQTG_COPYFILEWIDGETS_HPP

#include "baseClassActionTypeWidgets.hpp"

class copyFileData_c;
class copyFileAction_c;
class actionData_c;
class action_c;
class textCompilation_c;

class QComboBox;
class QSplitter;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;
class QLineEdit;
class QTableWidget;

class copyFileWidgets_c : public baseClassActionTypeWidgets_c
{
    Q_OBJECT

    QSplitter* mainSplitter_pri;

    QPlainTextEdit* sourcePathPTE_pri;
    QPlainTextEdit* destinationPathPTE_pri;

    QComboBox* transferTypeCombobox_pri;
    QComboBox* resumeTypeCombobox_pri;
    QComboBox* destinationTreatmentCombobox_pri;

    QTableWidget* filenameFullExtensionsTable_pri;
    QTableWidget* filenameRegexTable_pri;

    QCheckBox* copyHiddenCheckbox_pri;
    QCheckBox* navigateSubdirectoriesCheckbox_pri;
    QCheckBox* navigateHiddenCheckbox_pri;
    QCheckBox* copyEmptyDirectoriesCheckbox_pri;
    QCheckBox* createDestinationAndParentsCheckbox_pri;
    QCheckBox* noFilesCopiedIsErrorCheckbox_pri;
    QCheckBox* stopAllCopyOnFileCopyErrorCheckbox_pri;

    QLineEdit* bufferSizeLineEdit_pri;

    QFileDialog* browsePathFileDialog_pri = nullptr;
    QFileDialog* browseDestinationPathFileDialog_pri = nullptr;

    bool browseSource_pri = false;
    bool browseDestination_pri = false;

    copyFileAction_c* copyFileAction_ptr_pri = nullptr;

    void loadActionSpecificData_f();
    bool isFieldsDataValid_f(textCompilation_c* errors_par) const;
    copyFileData_c fieldsToCopyFileDataObject_f() const;

    void browsePath_f();

    bool derivedSaveNew_f(const actionData_c& actionDataBlock_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
    //void updateFullExtensionRow_f(const QString& fullExtrension_par_con, const int row_par_con);

//    void updateRegexPatternRow_f(const QString& regexPattern_par_con, const int row_par_con);

public:
    explicit copyFileWidgets_c(
            action_c*& action_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void browseSourcePath_f();
    void browseDestinationPath_f();
    void fileDialogBrowseSourceFinished_f(const int result_par);
    void tryGenerateFileList_f() const;
    void showCurrentWorkingDirectoryWindow_f();
    void addFilenameFullExtensionRow_f();
    void insertFullExtensionRow_f(const QString& fullExtrension_par_con = QString());
    void removeSelectedFilenameFullExtensionRow_f();

    void addFilenameRegexPatternRow_f();
    void insertRegexPatternRow_f(const QString& regexPattern_par_con = QString());
    void removeSelectedRegexPatternRow_f();
};

#endif // ACTONQTG_COPYFILEWIDGETS_HPP
