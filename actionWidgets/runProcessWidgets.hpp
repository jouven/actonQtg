#ifndef ACTONQTG_RUNPROCESSAWIDGETS_HPP
#define ACTONQTG_RUNPROCESSAWIDGETS_HPP

#include "actonQtso/actions/runProcess.hpp"

#include <QObject>
#include <QJsonObject>

#include <utility>

class QPlainTextEdit;
class QTableWidget;
class QVBoxLayout;
class QSplitter;
class QCheckBox;
class QFileDialog;

class argumentEditWindow_c;
class environmentPairToAddEditWindow_c;

class runProcessWidgets_c : public QObject
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

    const int_fast32_t actionDataId_pri = 0;

    QFileDialog* selectWorkingDirectoryDialog_pri = Q_NULLPTR;
    QFileDialog* selectProcessFileDialog_pri = Q_NULLPTR;

    void loadActionSpecificData_f();

    void insertArgumentRow_f(const argument_c& argument_par_con);
    void updateArgumentRow_f(const argument_c& argument_par_con, const int row_par_con);

    void insertEnvironmentPairRow_f(const QString& key_par_con, const environmentPair_c& environmentPair_par_con);
    void updateEnvironmentPairRow_f(const QString& key_par_con, const environmentPair_c& environmentPair_par_con, const int row_par_con);

    QJsonObject processActionDataJSON_f() const;
public:
    explicit runProcessWidgets_c(
            const int_fast32_t actionDataId_par_con
            , QVBoxLayout* const variableLayout_par_con
            , QObject *parent = nullptr);

Q_SIGNALS:
    void saveResult_signal(const QJsonObject& result_par_con);
public Q_SLOTS:
    void parentClosing_f();
    void save_f();
private Q_SLOTS:
    void browseProcessFile_f();
    void fileDialogProcessFileFinished_f(const int result_par);
    void openArgumentEditWindow_f();
    void removeArgument_f();
    void addUpdateArgumentRow_f(const argument_c& saveResult_par_con, const int index_par_con);
    void browseWorkingDirectory_f();
    void fileDialogWorkingDirectoryFinished_f(const int result_par);
    void showCurrentWorkingDirectoryWindow_f();
    void openEnvironmentPairEditWindow_f();
    void removeEnvironmentPair_f();
    void addUpdateEnvironmentPairRow_f(const QString& savedKey_par_con, const environmentPair_c& savedEnvironmentPair_par_con, const int index_par_con);
    void showCurrentEnvironmentWindow_f();
};

#endif // ACTONQTG_RUNPROCESSAWIDGETS_HPP