#ifndef ACTONQTG_BASECLASSCHECKWIDGETS_HPP
#define ACTONQTG_BASECLASSCHECKWIDGETS_HPP

#include <QObject>

class QString;

class check_c;
class checkData_c;

class baseClassCheckWidgets_c : public QObject
{
    Q_OBJECT

protected:
    check_c*& checkPtr_pro;

    virtual bool derivedSaveNew_f(const checkData_c& checkData_par_con) = 0;
    virtual bool derivedSaveUpdate_f() = 0;
    virtual void derivedParentClosing_f() = 0;
    virtual QString derivedExtraTips_f() const = 0;

    explicit baseClassCheckWidgets_c(check_c*& checkPtrRef_par, QObject* parent_par);
public:

    bool saveNew_f(const checkData_c& checkData_par_con);
    bool saveUpdate_f();
    void parentClosing_f();

    QString extraTips_f() const;
};

#endif // ACTONQTG_BASECLASSCHECKWIDGETS_HPP
