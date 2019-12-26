#include "timerWidgets.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/checks/timer.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

#include <QtWidgets>
//#include <QSplitter>

void timerWidgets_c::derivedParentClosing_f()
{
    //nothing to save, geometry wise, when closing
}

bool timerWidgets_c::isFieldsDataValid_f(textCompilation_c* errors_par) const
{
    bool validTmp(false);
    while (true)
    {
        //value
        bool goodNumberConversion(false);
        timerValueLineEdit_pri->text().toLongLong(&goodNumberConversion);
        if (goodNumberConversion)
        {
            //good
        }
        else
        {
            text_c errorTextTmp("Wrong timer value: {0}", timerValueLineEdit_pri->text());
            APPENDTEXTPTR(errors_par, errorTextTmp)
            break;
        }

        validTmp = true;
        break;
    }
    return validTmp;
}

timerData_c timerWidgets_c::fieldsTotimerDataObject_f() const
{
    timerData_c::type_ec typeTmp(timerData_c::strTotimerTypeMap_sta_con.value(alarmTypeCombobox_pri->currentData().toString()));
    return timerData_c(
                typeTmp
                , timerValueLineEdit_pri->text().toLongLong()
                , errorOnPastDateTimeValueCheckBox_pri->isChecked()
    );
}

bool timerWidgets_c::derivedSaveNew_f(const checkData_c& checkData_par_con)
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        timerData_c objTmp(fieldsTotimerDataObject_f());

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            timerCheck_ptr_pri = new timerCheck_c(checkData_par_con, objTmp);
            checkPtr_pro = timerCheck_ptr_pri;
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool timerWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        timerData_c objTmp(fieldsTotimerDataObject_f());

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            timerCheck_ptr_pri->updateTimerData_f(objTmp);
            resultTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

QString timerWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>same file widget tips</p>");
}

void timerWidgets_c::loadCheckSpecificData_f()
{
    timerData_c valuesToLoadTmp(timerCheck_ptr_pri not_eq nullptr ? *timerCheck_ptr_pri : timerData_c());

    timerValueLineEdit_pri->setText(QString::number(valuesToLoadTmp.value_f()));
    if (valuesToLoadTmp.alarmType_f() not_eq timerData_c::type_ec::empty)
    {
        QString alarmTypeStrTmp(timerData_c::timerTypeToStrUMap_sta_con.at(valuesToLoadTmp.alarmType_f()));
        int loadedIndexTmp(alarmTypeCombobox_pri->findData(alarmTypeStrTmp.toLower()));
        alarmTypeCombobox_pri->setCurrentIndex(loadedIndexTmp);
        alarmTypeComboChanged_f(loadedIndexTmp);
    }

    if (valuesToLoadTmp.alarmType_f() == timerData_c::type_ec::dateTime)
    {
        QDateTime datetimeTmp;
        datetimeTmp.setMSecsSinceEpoch(valuesToLoadTmp.value_f());
        datetimeEdit_pri->setDateTime(datetimeTmp);
    }
    if (valuesToLoadTmp.alarmType_f() == timerData_c::type_ec::timer)
    {
        QDateTime datetimeTmp(QDateTime::currentDateTime());
        datetimeTmp.setMSecsSinceEpoch(datetimeTmp.toMSecsSinceEpoch() + valuesToLoadTmp.value_f());
        datetimeEdit_pri->setDateTime(datetimeTmp);
    }

    errorOnPastDateTimeValueCheckBox_pri->setChecked(valuesToLoadTmp.errorOnPastDateTimeValue_f());
}

timerWidgets_c::timerWidgets_c(
        check_c*& check_ptr_par
        , QVBoxLayout* const variableLayout_par_con)
    : baseClassCheckTypeWidgets_c(check_ptr_par, variableLayout_par_con->parentWidget())
    , timerCheck_ptr_pri(check_ptr_par == nullptr ? nullptr : static_cast<timerCheck_c*>(check_ptr_par))
{
    this->setObjectName("timerWidgets_");

    //alarm type combo
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    alarmTypeCombobox_pri = new QComboBox;
    alarmTypeCombobox_pri->setEditable(true);
    alarmTypeCombobox_pri->setInsertPolicy(QComboBox::NoInsert);
    alarmTypeCombobox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    alarmTypeCombobox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    alarmTypeCombobox_pri->setToolTip(appConfig_ptr_ext->translate_f(
R"(datetime: the alarm is triggered when a datetime is reached, value field is milliseconds since epoch (UTC)

timer: the alarm is triggered when a timer expires, value field is the timer timeout interval in milliseconds,
calendar is enabled but it's only to see what the timer value would be adding it to the calendar current datetime

Changes in-between fields, datetimeEdit/Calendar and Value, happen when the fields are left/out of focus)"
    ));
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Alarm type")));
    firstRowLayoutTmp->addWidget(alarmTypeCombobox_pri);

    for (const QString& alarmTypeStr_ite_con : timerData_c::strTotimerTypeMap_sta_con.keys())
    {
        alarmTypeCombobox_pri->insertItem(alarmTypeCombobox_pri->count(), appConfig_ptr_ext->translate_f(alarmTypeStr_ite_con), alarmTypeStr_ite_con);
    }

    //time type combo, datetime datetimeEdit and value LineEdit
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    timeTypeCombobox_pri = new QComboBox;
    timeTypeCombobox_pri->setEditable(true);
    timeTypeCombobox_pri->setInsertPolicy(QComboBox::NoInsert);
    timeTypeCombobox_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    timeTypeCombobox_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    timeTypeCombobox_pri->setToolTip(appConfig_ptr_ext->translate_f(
R"(What type of time spec is used in the calendar/"datetime editor"

localtime: use this computer localtime datetime
UTC: use Coordinated Universal Time)"
    ));
    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Time type")));
    secondRowLayoutTmp->addWidget(timeTypeCombobox_pri);

    //localtime is the default (the first inserted)
    timeTypeCombobox_pri->insertItem(timeTypeCombobox_pri->count(), appConfig_ptr_ext->translate_f(localtimeStr_pri_con), localtimeStr_pri_con);
    timeTypeCombobox_pri->insertItem(timeTypeCombobox_pri->count(), appConfig_ptr_ext->translate_f(utcStr_pri_con), utcStr_pri_con);

    datetimeEdit_pri = new QDateTimeEdit;
    datetimeEdit_pri->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    datetimeEdit_pri->setCalendarPopup(true);
    datetimeEdit_pri->setToolTip(
R"(See alarm type combo tooltip)");

    QCalendarWidget* calendarObj(new QCalendarWidget(datetimeEdit_pri));
    datetimeEdit_pri->setCalendarWidget(calendarObj);
    secondRowLayoutTmp->addWidget(datetimeEdit_pri);

    timerValueLineEdit_pri = new QLineEdit;
    timerValueLineEdit_pri->setToolTip(
R"(See alarm type combo tooltip)");
    //that's the "string" length of max 64bit signed number
    /////////////////////////////////////"9223372036854775808 (length 19)
    QRegExpValidator *a64bitValidatorTmp = new QRegExpValidator(QRegExp("[0-9]{19}"), this);
    timerValueLineEdit_pri->setValidator(a64bitValidatorTmp);
    //timerValueLineEdit_pri->setInputMask("0000000000000000009");

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Value")));
    secondRowLayoutTmp->addWidget(timerValueLineEdit_pri);

    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    //error on past dateTime value
    errorOnPastDateTimeValueCheckBox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Error on past DateTime"));
    errorOnPastDateTimeValueCheckBox_pri->setToolTip(appConfig_ptr_ext->translate_f(
R"(On execution, if the timer datetime value is past:
Checked: error and fail the check
UnChecked: treat as a 0 timeout (execute immediately))"));
    //enabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    //enabledCheckbox_pri->setChecked(true);
    thirdRowLayoutTmp->addWidget(errorOnPastDateTimeValueCheckBox_pri);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);

    QObject::connect(timerValueLineEdit_pri, &QLineEdit::editingFinished, this, &timerWidgets_c::valueLineEditChanged_f);

    loadCheckSpecificData_f();

    QObject::connect(datetimeEdit_pri, &QDateTimeEdit::dateTimeChanged, this, &timerWidgets_c::datetimeEditChanged_f);
    QObject::connect(alarmTypeCombobox_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &timerWidgets_c::alarmTypeComboChanged_f);
    QObject::connect(timeTypeCombobox_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &timerWidgets_c::timeTypeComboChanged_f);
}

void timerWidgets_c::valueLineEditChanged_f()
{
    //gimmick to see what date would be when adding the timer to the calendar datetime value
    QDateTime dateTimeTmp(datetimeEdit_pri->dateTime().addMSecs(timerValueLineEdit_pri->text().toLongLong()));
    datetimeEdit_pri->blockSignals(true);
    datetimeEdit_pri->setDateTime(dateTimeTmp);
    datetimeEdit_pri->blockSignals(false);
}

void timerWidgets_c::datetimeEditChanged_f(const QDateTime& dateTime_par_con)
{
    timerData_c::type_ec alarmTypeTmp(timerData_c::strTotimerTypeMap_sta_con.value(alarmTypeCombobox_pri->itemData(alarmTypeCombobox_pri->currentIndex()).toString()));
    if (alarmTypeTmp == timerData_c::type_ec::dateTime)
    {
        timerValueLineEdit_pri->blockSignals(true);
        timerValueLineEdit_pri->setText(QString::number(dateTime_par_con.toMSecsSinceEpoch()));
        timerValueLineEdit_pri->blockSignals(false);
    }
}

void timerWidgets_c::alarmTypeComboChanged_f(int index_par)
{
    timerData_c::type_ec alarmTypeTmp(timerData_c::strTotimerTypeMap_sta_con.value(alarmTypeCombobox_pri->itemData(index_par).toString()));
    while (true)
    {
        if (alarmTypeTmp == timerData_c::type_ec::timer)
        {
            timeTypeCombobox_pri->setEnabled(false);
            break;
        }
        if (alarmTypeTmp == timerData_c::type_ec::dateTime)
        {
            timeTypeCombobox_pri->setEnabled(true);
            break;
        }
        break;
    }
}

void timerWidgets_c::timeTypeComboChanged_f(int index_par)
{
    QString timeTypeTmp(timeTypeCombobox_pri->itemData(index_par).toString());
    while (true)
    {
        if (timeTypeTmp == utcStr_pri_con)
        {
            datetimeEdit_pri->setTimeSpec(Qt::UTC);
            break;
        }
        if (timeTypeTmp == localtimeStr_pri_con)
        {
            datetimeEdit_pri->setTimeSpec(Qt::LocalTime);
            break;
        }
        break;
    }
}

