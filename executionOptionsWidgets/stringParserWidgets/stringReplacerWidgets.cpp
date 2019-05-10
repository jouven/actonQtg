#include "stringReplacerWidgets.hpp"

#include "../appConfig.hpp"

#include "stringParserMapQtso/parsers/stringReplacer.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>


void stringReplacerWidgets_c::derivedParentClosing_f()
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + splitter_pri->objectName(), splitter_pri->saveState());
}

bool stringReplacerWidgets_c::derivedSave_f(const QString& stringTrigger_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        stringReplacer_c::replaceType_ec replaceTypeTmp(stringReplacer_c::strToReplaceTypeMap_sta_con.value(replaceTypeCombo_pri->currentData().toString().toLower()));

        QString valueOrFormatStr(replaceValueOrFormatPTE_pri->toPlainText());
        if (replaceTypeTmp == stringReplacer_c::replaceType_ec::currentDatetimeString and valueOrFormatStr.isEmpty())
        {
            errorQMessageBox_f("Format can't be empty for this type", "Error", qobject_cast<QWidget*>(this->parent()));
            break;
        }
        if (replaceTypeTmp == stringReplacer_c::replaceType_ec::currentDatetimeString)
        {
            if (QDateTime::currentDateTime().toString(valueOrFormatStr).isEmpty())
            {
                errorQMessageBox_f("Parsed format returns empty value", "Error", qobject_cast<QWidget*>(this->parent()));
                break;
            }
        }
#ifdef DEBUGJOUVEN
        //qDebug() << "1 parserBasePtr_pro " << parserBasePtr_pro << endl;
        //qDebug() << "1 specificParserWidgets_pri " << stringReplacer_pri << endl;
#endif
        if (stringReplacer_pri == nullptr)
        {
            stringReplacer_pri = new stringReplacer_c(stringTrigger_par_con, replaceTypeTmp, valueOrFormatStr);
            parserBasePtr_pro = stringReplacer_pri;
        }
        else
        {
            stringReplacer_pri->setReplaceValueOrFormat_f(valueOrFormatStr);
        }
#ifdef DEBUGJOUVEN
        //qDebug() << "2 parserBasePtr_pro " << parserBasePtr_pro << endl;
        //qDebug() << "2 specificParserWidgets_pri " << stringReplacer_pri << endl;
#endif
        resultTmp = true;
        break;
    }
    return resultTmp;
}


void stringReplacerWidgets_c::loadActionSpecificData_f()
{
    if (stringReplacer_pri not_eq nullptr)
    {
        QString replaceTypeStrTmp(stringReplacer_c::replaceTypeToStrUMap_sta_con.at(stringReplacer_pri->replaceType_f()));
        int loadedReplaceTypeIndexTmp(replaceTypeCombo_pri->findData(replaceTypeStrTmp.toLower()));
        replaceTypeCombo_pri->setCurrentIndex(loadedReplaceTypeIndexTmp);
        replaceTypeCombo_pri->setEnabled(false);

        replaceValueOrFormatPTE_pri->setPlainText(stringReplacer_pri->valueFormat_f());
    }
}

stringReplacerWidgets_c::stringReplacerWidgets_c(parserBase_c*& parserBasePtr_par
        , QVBoxLayout* const variableLayout_par_con)
    : baseClassStringParserWidgets_c(parserBasePtr_par, variableLayout_par_con->parentWidget())
    , stringReplacer_pri(parserBasePtr_par == nullptr ? nullptr : static_cast<stringReplacer_c*>(parserBasePtr_par))
{
    this->setObjectName("stringReplacerWidgets_");

    replaceValueOrFormatPTE_pri = new QPlainTextEdit(variableLayout_par_con->parentWidget());
    auto minHeightTmp(replaceValueOrFormatPTE_pri->fontMetrics().lineSpacing() + 14);

    //replace type and replace value/format
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    replaceTypeCombo_pri = new QComboBox();
    replaceTypeCombo_pri->setEditable(true);
    replaceTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    replaceTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    replaceTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Replace Type")));
    firstRowLayoutTmp->addWidget(replaceTypeCombo_pri);

    for (const QString& replaceTypeStr_ite_con : stringReplacer_c::strToReplaceTypeMap_sta_con.keys())
    {
        replaceTypeCombo_pri->insertItem(replaceTypeCombo_pri->count(), appConfig_ptr_ext->translate_f(replaceTypeStr_ite_con), replaceTypeStr_ite_con);
    }

    replaceValueOrFormatPTE_pri->setMinimumHeight(minHeightTmp);
    replaceValueOrFormatPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    firstRowLayoutTmp->addWidget(replaceValueOrFormatPTE_pri);


    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    //useUTC checkbox
    useUTCCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Use UTC"));
    useUTCCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("True = Use UTC, False = use localtime"));
    useUTCCheckbox_pri->setMinimumHeight(minHeightTmp);
    useUTCCheckbox_pri->setChecked(false);
    secondRowLayoutTmp->addWidget(useUTCCheckbox_pri);

    QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row2Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);

    row1Tmp->setLayout(firstRowLayoutTmp);
    row2Tmp->setLayout(secondRowLayoutTmp);

    splitter_pri = new QSplitter(Qt::Vertical);
    splitter_pri->setObjectName("QSplitter_");

    splitter_pri->addWidget(row1Tmp);
    splitter_pri->addWidget(row2Tmp);
    splitter_pri->setCollapsible(0, false);
    //splitter_pri->setCollapsible(1, false);
    splitter_pri->setContentsMargins(0,0,0,0);

    //variableLayout_par_con->setContentsMargins(0,0,0,0);
    //variableLayout_par_con->setSpacing(0);
    //it seems that QVBoxLayout has an innate horizontal margins
    variableLayout_par_con->addWidget(splitter_pri);

    loadActionSpecificData_f();
}
