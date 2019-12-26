#ifndef ACTONQTG_ACTIONFINISHEDWIDGETS_HPP
#define ACTONQTG_ACTIONFINISHEDWIDGETS_HPP

#include "baseClassCheckTypeWidgets.hpp"

class actionFinishedData_c;
class check_c;
class actionFinishedCheck_c;
class action_c;
class textCompilation_c;

class QComboBox;
class QVBoxLayout;
class QCheckBox;
class QSplitter;
class QTableWidget;
class QLineEdit;

class actionFinishedWidgets_c : public baseClassCheckTypeWidgets_c
{
    Q_OBJECT

    //to choose actions
    QComboBox* actionStringIdCombo_pri;
    //FUTURE do a grid to select an action with some details

    QCheckBox* successOnActionSuccessCheckBox_pri;
    QLineEdit* finishedCountLineEdit_pri;

    QTableWidget* actionResultTypeToStringParserTable_pri;

    //QSplitter* mainSplitter_pri;

    actionFinishedCheck_c* actionFinishedCheck_ptr_pri = nullptr;

    void loadCheckSpecificData_f();
    bool isFieldsDataValid_f(textCompilation_c* errors_par) const;
    actionFinishedData_c fieldsToActionFinishedDataObject_f() const;

    bool derivedSaveNew_f(const checkData_c& checkData_par_con) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
public:
    actionFinishedWidgets_c() = delete;
    //canviar checkData_c per jsonObject
    explicit actionFinishedWidgets_c(
            check_c*& check_ptr_par
            , QVBoxLayout* const variableLayout_par
            //special case since the check might not exists but this check action stringid
            //is required so it's not on the actionStringIdCombo_pri
            , const action_c* const parentAction_par_con);
};

#endif // ACTONQTG_ACTIONFINISHEDWIDGETS_HPP
