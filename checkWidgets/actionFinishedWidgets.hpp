#ifndef ACTONQTG_ACTIONFINISHEDWIDGETS_HPP
#define ACTONQTG_ACTIONFINISHEDWIDGETS_HPP

#include <QObject>

class QComboBox;
//class QPushButton;
class QVBoxLayout;
class QCheckBox;

class checkData_c;

class actionFinishedWidgets_c : public QObject
{
    Q_OBJECT

    //to choose actions
    QComboBox* actionStringIdCombo_pri;
    //FUTURE do a grid to select an action with some details
    //right now make do the a combo with autocomplete
    QCheckBox* failCheckOnNotSuccessfulCheckbox_pri;

    checkData_c* const checkData_ptr_pri;

    void loadCheckSpecificData_f();
    void saveCheckDataJSON_f() const;
public:
    actionFinishedWidgets_c() = delete;
    //canviar checkData_c per jsonObject
    explicit actionFinishedWidgets_c(
            checkData_c* const checkData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
            , QObject *parent = nullptr);

Q_SIGNALS:
    void JSONSaved_signal();
public Q_SLOTS:
    void parentClosing_f();
    void save_f();
private Q_SLOTS:
};

#endif // ACTONQTG_ACTIONFINISHEDWIDGETS_HPP
