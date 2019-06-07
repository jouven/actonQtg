#ifndef ACTONQTG_BASECLASSACTIONWIDGETS_HPP
#define ACTONQTG_BASECLASSACTIONWIDGETS_HPP

#include <QObject>

class QString;

class action_c;
class actionData_c;

class baseClassActionWidgets_c : public QObject
{
    Q_OBJECT

protected:
    action_c*& actionPtr_pro;

    virtual bool derivedSaveNew_f(const actionData_c& actionData_par_con) = 0;
    virtual bool derivedSaveUpdate_f() = 0;
    virtual void derivedParentClosing_f() = 0;
    virtual QString derivedExtraTips_f() const = 0;

    explicit baseClassActionWidgets_c(action_c*& actionPtrRef_par, QObject* parent_par);
public:

    bool saveNew_f(const actionData_c& actionData_par_con);
    bool saveUpdate_f();
    void parentClosing_f();

    QString extraTips_f() const;
};

#endif // ACTONQTG_BASECLASSACTIONWIDGETS_HPP
