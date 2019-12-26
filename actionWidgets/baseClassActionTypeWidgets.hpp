#ifndef ACTONQTG_BASECLASSACTIONTYPEWIDGETS_HPP
#define ACTONQTG_BASECLASSACTIONTYPEWIDGETS_HPP

#include "actonQtso/actionMappings/actions.hpp"

#include <QObject>

class QString;

class action_c;
class actionData_c;
//class textCompilation_c;

class baseClassActionTypeWidgets_c : public QObject
{
    Q_OBJECT

protected:
    action_c*& actionPtr_pro;

    virtual bool derivedSaveNew_f(const actionData_c& actionData_par_con) = 0;
    virtual bool derivedSaveUpdate_f() = 0;
    virtual void derivedParentClosing_f() = 0;
    virtual QString derivedExtraTips_f() const = 0;

    explicit baseClassActionTypeWidgets_c(action_c*& actionPtrRef_par, QObject* parent_par);

public:
    bool saveNew_f(const actionData_c& actionData_par_con);
    bool saveUpdate_f();
    void parentClosing_f();

    QString extraTips_f() const;
};

#endif // ACTONQTG_BASECLASSACTIONTYPEWIDGETS_HPP
