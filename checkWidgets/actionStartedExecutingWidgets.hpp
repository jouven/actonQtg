#ifndef ACTONQTG_ACTIONSTARTEDEXECUTINGWIDGETS_HPP
#define ACTONQTG_ACTIONSTARTEDEXECUTINGWIDGETS_HPP

#include "baseClassCheckTypeWidgets.hpp"

class actionStartedExecutingData_c;
class check_c;
class actionStartedExecutingCheck_c;
class action_c;
class textCompilation_c;

class QComboBox;
class QVBoxLayout;
class QCheckBox;
class QSplitter;
class QLineEdit;

class actionStartedExecutingWidgets_c : public baseClassCheckTypeWidgets_c
{
    Q_OBJECT

    //to choose actions
    QComboBox* actionStringIdCombo_pri;
    //FUTURE do a grid to select an action with some details

    QLineEdit* waitFinishMsLineEdit_pri;

    //QSplitter* mainSplitter_pri;

    actionStartedExecutingCheck_c* actionStartedExecutionCheck_ptr_pri = nullptr;

    void loadCheckSpecificData_f();
    bool isFieldsDataValid_f(textCompilation_c* errors_par) const;
    actionStartedExecutingData_c fieldsToActionFinishedDataObject_f() const;

    bool derivedSaveNew_f(const checkData_c& checkData_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
public:
    actionStartedExecutingWidgets_c() = delete;
    //canviar checkData_c per jsonObject
    explicit actionStartedExecutingWidgets_c(
            check_c*& check_ptr_par
            , QVBoxLayout* const variableLayout_par
            //special case since the check might not exists but this check action stringid
            //is required so it's not on the actionStringIdCombo_pri
            , const action_c* const parentAction_par_con);
};

#endif // ACTONQTG_ACTIONSTARTEDEXECUTINGWIDGETS_HPP
