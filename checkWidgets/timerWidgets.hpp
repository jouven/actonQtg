#ifndef ACTONQTG_TIMERWIDGETS_HPP
#define ACTONQTG_TIMERWIDGETS_HPP

#include "baseClassCheckTypeWidgets.hpp"

class timerData_c;
class check_c;
class timerCheck_c;
class textCompilation_c;


//class QPushButton;
class QVBoxLayout;
class QComboBox;
class QDateTimeEdit;
class QLineEdit;
class QCheckBox;

class timerWidgets_c : public baseClassCheckTypeWidgets_c
{
    Q_OBJECT

    QComboBox* alarmTypeCombobox_pri;
    QDateTimeEdit* datetimeEdit_pri;
    QComboBox* timeTypeCombobox_pri;

    QCheckBox* errorOnPastDateTimeValueCheckBox_pri;

    //type "timer": timer time in milliseconds
    //type "datetime": epoch time in milliseconds (UTC)
    QLineEdit* timerValueLineEdit_pri;

    timerCheck_c* timerCheck_ptr_pri = nullptr;

    void loadCheckSpecificData_f();
    timerData_c fieldsTotimerDataObject_f() const;

    bool derivedSaveNew_f(const checkData_c& checkData_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;


    bool isFieldsDataValid_f(textCompilation_c* errors_par) const;
public:
    explicit timerWidgets_c(
            check_c*& check_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

    const QString utcStr_pri_con = "UTC";
    const QString localtimeStr_pri_con = "localtime";
private Q_SLOTS:
    void valueLineEditChanged_f();
    void datetimeEditChanged_f(const QDateTime& dateTime_par_con);
    void alarmTypeComboChanged_f(int index_par);
    void timeTypeComboChanged_f(int index_par);
};

#endif // ACTONQTG_TIMERWIDGETS_HPP
