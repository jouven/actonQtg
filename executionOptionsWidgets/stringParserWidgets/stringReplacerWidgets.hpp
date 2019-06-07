#ifndef ACTONQTG_STRINGREPLACERWIDGETS_HPP
#define ACTONQTG_STRINGREPLACERWIDGETS_HPP

#include "baseClassStringParserWidgets.hpp"

class QComboBox;
class QPlainTextEdit;
class QVBoxLayout;
class QSplitter;
class QCheckBox;

class stringReplacer_c;

class stringReplacerWidgets_c : public baseClassStringParserWidgets_c
{
    Q_OBJECT

    QComboBox* replaceTypeCombo_pri;
    QPlainTextEdit* replaceValueOrFormatPTE_pri;
    QCheckBox* useUTCCheckbox_pri;

    QSplitter* splitter_pri;

    stringReplacer_c* stringReplacer_pri = nullptr;

    bool derivedSave_f(const QString& stringTrigger_par_con) override;
    void derivedParentClosing_f() override;
    QString derivedExtraTips_f() const override;

    void loadActionSpecificData_f();
public:
    explicit stringReplacerWidgets_c(
            parserBase_c*& parserBasePtr_par
            , QVBoxLayout* const variableLayout_par_con);

//private Q_SLOTS:
    //void dryRunButtonClicked_f();
};

#endif // ACTONQTG_STRINGREPLACERWIDGETS_HPP
