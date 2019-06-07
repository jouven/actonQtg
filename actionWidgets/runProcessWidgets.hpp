#ifndef ACTONQTG_RUNPROCESSAWIDGETS_HPP
#define ACTONQTG_RUNPROCESSAWIDGETS_HPP

#include "baseClassActionWidgets.hpp"

class QPlainTextEdit;
class QTableWidget;
class QVBoxLayout;
class QSplitter;
class QCheckBox;
class QFileDialog;

class argumentEditWindow_c;
class environmentPairToAddEditWindow_c;
class runProcessData_c;
class runProcessAction_c;
class actionData_c;
class action_c;
class argument_c;
class environmentPairConfig_c;

class runProcessWidgets_c : public baseClassActionWidgets_c
{
    Q_OBJECT

    QPlainTextEdit* processPathPTE_pri;
    QPlainTextEdit* workingDirectoryPTE_pri;
    QTableWidget* argumentsTable_pri;
    QTableWidget* environmentToAddTable_pri;

    QCheckBox* useActonEnviroment_pri;

    QSplitter* mainSplitter_pri;

    argumentEditWindow_c* argumentWindow_pri;
    environmentPairToAddEditWindow_c* environmentPairWindow_pri;

    QFileDialog* selectWorkingDirectoryDialog_pri = nullptr;
    QFileDialog* selectProcessFileDialog_pri = nullptr;

    runProcessAction_c* runProcessAction_ptr_pri = nullptr;

    void loadActionSpecificData_f();

    void insertArgumentRow_f(const argument_c& argument_par_con, const int row_par_con = -1);
    void updateArgumentRow_f(const argument_c& argument_par_con, const int row_par_con);

    void insertEnvironmentPairRow_f(const QString& key_par_con, const environmentPairConfig_c& environmentPair_par_con);
    void updateEnvironmentPairRow_f(const QString& key_par_con, const environmentPairConfig_c& environmentPair_par_con, const int row_par_con);

    bool derivedSaveNew_f(const actionData_c& actionDataBlock_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
    runProcessData_c fieldsToRunProcessDataObject_f() const;
    bool isFieldsDataValid_f() const;
public:
    explicit runProcessWidgets_c(
            action_c*& actionData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

private Q_SLOTS:
    void browseProcessFile_f();
    void fileDialogProcessFileFinished_f(const int result_par);
    void openArgumentEditWindow_f();
    void refreshIndexColumn_f(const int index_par_con);
    void removeArgument_f();
    void removeArgumentRow_f(const int index_par_con);
    void addUpdateArgumentRow_f(const argument_c& saveResult_par_con, const int index_par_con, const bool update_par_con);
    void browseWorkingDirectory_f();
    void fileDialogWorkingDirectoryFinished_f(const int result_par);
    void showCurrentWorkingDirectoryWindow_f();
    void openEnvironmentPairEditWindow_f();
    void removeEnvironmentPair_f();
    void addUpdateEnvironmentPairRow_f(const QString& savedKey_par_con, const environmentPairConfig_c& savedEnvironmentPair_par_con, const int index_par_con);
    void showCurrentEnvironmentWindow_f();
};

#endif // ACTONQTG_RUNPROCESSAWIDGETS_HPP
