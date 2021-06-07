#ifndef ACTONQTG_METAENDEXECUTIONCYCLEWIDGETS_HPP
#define ACTONQTG_METAENDEXECUTIONCYCLEWIDGETS_HPP

#include "baseClassActionTypeWidgets.hpp"

class QComboBox;
class QVBoxLayout;
class QCheckBox;

class metaEndExecutionCycleData_c;
class metaEndExecutionCycleAction_c;

class metaEndExecutionCycleWidgets_c : public baseClassActionTypeWidgets_c
{
    Q_OBJECT

    QComboBox* endTypeCombo_pri;
    QCheckBox* killAfterStopTimeoutCheckbox_pri;

    metaEndExecutionCycleAction_c* metaEndExecutionCycleAction_ptr_pri = nullptr;

    void loadActionSpecificData_f();

    bool derivedSaveNew_f(const actionData_c& actionDataBlock_par_con, actonDataHub_c* parentActonDataHub_par) override;
    bool derivedSaveUpdate_f() override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;
    metaEndExecutionCycleData_c fieldsToMetaEndExecutionCycleDataObject_f() const;
public:
    explicit metaEndExecutionCycleWidgets_c(
            action_c*& actionData_ptr_par
            , QVBoxLayout* const variableLayout_par_con
    );

};

#endif // ACTONQTG_METAENDEXECUTIONCYCLEWIDGETS_HPP
