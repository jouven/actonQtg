#ifndef ACTONQTG_ARGUMENTWINDOW_HPP
#define ACTONQTG_ARGUMENTWINDOW_HPP

#include "actonQtso/actions/runProcess.hpp"

#include <QWidget>

class QPlainTextEdit;
class QLineEdit;
class QCheckBox;
class QVBoxLayout;
//class QSplitter;

class argumentEditWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;

    QPlainTextEdit* argumentField_pri;

    QLineEdit* argumentIndexField_pri;
    QCheckBox* enabledCheckbox_pri;
    //QSplitter* mainSplitter_pri;

    const int rowCount_pri_con = 0;
    const int currentIndex_pri_con = 0;

    void closeEvent(QCloseEvent* event) override;
public:
    argumentEditWindow_c(
            const QString& argument_par_con
            , const int currentIndex_par_con
            , const bool enabled_par_con
            , const int rowCount_par_con
            , QWidget* parent_par = nullptr);

Q_SIGNALS:
    void saveArgumentResult_signal(const argument_c& argument_par_con, const int index_par_con, const bool update_par_con);
    void deleteArgument_signal(const int index_par_con);
    void refreshArgumentIndexColumn_signal(const int index_par_con);
private Q_SLOTS:
    void cancelButtonPushed_f();
    void saveButtonPushed_f();

};

#endif // ACTONQTG_ARGUMENTWINDOW_HPP
