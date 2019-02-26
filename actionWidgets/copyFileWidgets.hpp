#ifndef ACTONQTG_COPYFILEWIDGETS_HPP
#define ACTONQTG_COPYFILEWIDGETS_HPP

#include "actonQtso/actions/copyFile.hpp"

#include <QObject>

class QComboBox;
class QSplitter;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;
class QLineEdit;
class QTableWidget;

class actionData_c;

class copyFileWidgets_c : public QObject
{
    Q_OBJECT

    QSplitter* mainSplitter_pri;

    QPlainTextEdit* sourcePathPTE_pri;
    QPlainTextEdit* destinationPathPTE_pri;

    QComboBox* transferTypeCombobox_pri;
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

    actionData_c* const actionData_ptr_pri;

    bool browseSource_pri = false;
    bool browseDestination_pri = false;

    void loadActionSpecificData_f();
    bool fieldsAreOkToSave_f() const;
    copyFileAction_c fieldsToCopyFileActionObject_f() const;
    void saveActionDataJSON_f() const;

    void browsePath_f();

    //void updateFullExtensionRow_f(const QString& fullExtrension_par_con, const int row_par_con);

//    void updateRegexPatternRow_f(const QString& regexPattern_par_con, const int row_par_con);

public:
    explicit copyFileWidgets_c(
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
