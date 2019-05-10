#ifndef ACTONQTG_BASECLASSSTRINGPARSERWIDGETS_HPP
#define ACTONQTG_BASECLASSSTRINGPARSERWIDGETS_HPP

//this is just to make the object deletion easier
//and to have setobjectname/objectname
#include <QObject>

class QString;

class parserBase_c;

class baseClassStringParserWidgets_c : public QObject
{
    Q_OBJECT

protected:
    parserBase_c*& parserBasePtr_pro;

    virtual bool derivedSave_f(const QString& stringTrigger_par_con) = 0;
    virtual void derivedParentClosing_f() = 0;
public:
    explicit baseClassStringParserWidgets_c(parserBase_c*& parserBasePtr_par, QObject* parent_par);

    bool save_f(const QString& stringTrigger_par_con);
    void parentClosing_f();
};

#endif // ACTONQTG_BASECLASSSTRINGPARSERWIDGETS_HPP
