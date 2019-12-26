#ifndef ACTONQTG_ENVIRONMENTPAIRWINDOW_HPP
#define ACTONQTG_ENVIRONMENTPAIRWINDOW_HPP

#include "actonQtso/actions/runProcess.hpp"

#include <QWidget>

class QPlainTextEdit;
class QCheckBox;
class QVBoxLayout;
class QSplitter;

class environmentPairToAddEditWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;

    QPlainTextEdit* keyFieldPTE_pri;
    QPlainTextEdit* valueFieldPTE_pri;

    QCheckBox* enabledCheckbox_pri;
    QSplitter* mainSplitter_pri;

    const int editedRowIndex_pri_con = 0;

    void closeEvent(QCloseEvent* event) override;
public:
    environmentPairToAddEditWindow_c(
            const QString& key_par_con
            , const QString& value_par_con
            , const bool enabled_par_con
            , const int editedRowIndex_par_con
            , QWidget* parent_par = nullptr
    );

Q_SIGNALS:
    void saveEnvironmentPairResult_signal(const QString& key_par_con, const environmentPairConfig_c& environmentPair_par_con, const int editedRowIndex_par_con);
private Q_SLOTS:
    void cancelButtonPushed_f();
    void saveButtonPushed_f();

};

#endif // ACTONQTG_ENVIRONMENTPAIRWINDOW_HPP
