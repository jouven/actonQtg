#include "baseClassStringParserWidgets.hpp"

#include "stringParserMapQtso/parsers/base.hpp"

#include <QString>

baseClassStringParserWidgets_c::baseClassStringParserWidgets_c(
        parserBase_c*& parserBasePtr_par
        , QObject* parent_par)
 : QObject(parent_par)
 , parserBasePtr_pro(parserBasePtr_par)
{}

bool baseClassStringParserWidgets_c::save_f(const QString& stringTrigger_par_con)
{
    return derivedSave_f(stringTrigger_par_con);
}

void baseClassStringParserWidgets_c::parentClosing_f()
{
    derivedParentClosing_f();
}

