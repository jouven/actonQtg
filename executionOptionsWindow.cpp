#include "executionOptionsWindow.hpp"

#include "executionOptionsWidgets/stringParserManagerWindow.hpp"

#include "actonQtso/actonDataHub.hpp"

#include "commonWidgets.hpp"
#include "appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

#include <QtWidgets>
#include <QIntValidator>


void executionOptionsWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

bool executionOptionsWindow_c::isFieldsDataValid_f(textCompilation_c* errors_par) const
{
    bool validTmp(false);
    while (true)
    {
        //loop x times
        if (loopXtimesCountLineEdit_pri->isEnabled())
        {
            bool goodNumberConversion(false);
            loopXtimesCountLineEdit_pri->text().toLongLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong loop x times value: {0}", loopXtimesCountLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        //extra threads
        {
            bool goodNumberConversion(false);
            extraThreadsLineEdit_pri->text().toLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong extra threads value: {0}", extraThreadsLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        //kill timeout milliseconds
        {
            bool goodNumberConversion(false);
            killTimeoutMillisecondsLineEdit_pri->text().toLong(&goodNumberConversion);
            if (goodNumberConversion)
            {
                //good
            }
            else
            {
                text_c errorTextTmp("Wrong extra threads value: {0}", killTimeoutMillisecondsLineEdit_pri->text());
                APPENDTEXTPTR(errors_par, errorTextTmp)
                break;
            }
        }

        validTmp = true;
        break;
    }
    return validTmp;
}


bool executionOptionsWindow_c::save_f()
{
    bool saveSuccessTmp(false);
    textCompilation_c errorsTmp;
    if (isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        executionOptions_c objTmp(
                    executionOptions_c::executionLoopType_ec::loopXTimes
                    //loop x times
                    , loopXtimesCountLineEdit_pri->text().toLongLong()
                    , extraThreadsLineEdit_pri->text().toLong()
                    , killTimeoutMillisecondsLineEdit_pri->text().toLong()
        );

        if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
        {
            actonDataHub_ptr_ext->setExecutionOptions_f(objTmp);
            threadedFunction_c::setMaxConcurrentQThreads_f(actonDataHub_ptr_ext->executionOptions_f().extraThreads_f());
            saveSuccessTmp = true;
        }
    }
    if (errorsTmp.size_f() > 0)
    {
         messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this));
    }
    return saveSuccessTmp;
}

void executionOptionsWindow_c::load_f()
{
    if (actonDataHub_ptr_ext->executionOptions_f().executionLoopType_f() not_eq executionOptions_c::executionLoopType_ec::empty)
    {
        auto executionLoopTypeIndexTmp(
                    executionLoopTypeCombo_pri->findData(
                        executionOptions_c::executionLoopTypeToStrUMap_sta_con.at(
                            actonDataHub_ptr_ext->executionOptions_f().executionLoopType_f()
                            ).toLower()
                        )
                    );
        executionLoopTypeCombo_pri->setCurrentIndex(executionLoopTypeIndexTmp);
    }

    loopXtimesCountLineEdit_pri->setText(QString::number(actonDataHub_ptr_ext->executionOptions_f().loopXTimesCount_f()));
    extraThreadsLineEdit_pri->setText(QString::number(actonDataHub_ptr_ext->executionOptions_f().extraThreads_f()));
    killTimeoutMillisecondsLineEdit_pri->setText(QString::number(actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f()));
}

executionOptionsWindow_c::executionOptionsWindow_c(
        QWidget *parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("executionOptionsWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //execution loop type combo, loop x times count
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    executionLoopTypeCombo_pri = new QComboBox();
    executionLoopTypeCombo_pri->setEditable(true);
    executionLoopTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    executionLoopTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    executionLoopTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Type")));
    firstRowLayoutTmp->addWidget(executionLoopTypeCombo_pri);

    for (const QString& executionLoopTypeStr_ite_con : executionOptions_c::strToExecutionLoopTypeMap_sta_con.keys())
    {
        executionLoopTypeCombo_pri->insertItem(
                    executionLoopTypeCombo_pri->count()
                    , appConfig_ptr_ext->translate_f(executionLoopTypeStr_ite_con), executionLoopTypeStr_ite_con);
    }

    QLabel* loopXTimesCountLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Loop execution count"));
    loopXTimesCountLabelTmp->setToolTip(appConfig_ptr_ext->translate_f(
                "<p>How many times will the execution be repeated</p>"
                "<p>Minimum 0, maximum INT64MAX</p>"
    ));

    firstRowLayoutTmp->addWidget(loopXTimesCountLabelTmp);

    loopXtimesCountLineEdit_pri = new QLineEdit;
    //that's the "string" length of max 64bit signed number
    QRegExpValidator *a64bitValidatorTmp = new QRegExpValidator(QRegExp("[0-9]{19}"), this);
    //////////////////////////////////////////"9223372036854775808 (length 19)
    //loopXtimesCountLineEdit_pri->setInputMask("9000000000000000000");
    loopXtimesCountLineEdit_pri->setValidator(a64bitValidatorTmp);
    firstRowLayoutTmp->addWidget(loopXtimesCountLineEdit_pri);

//    stopExecutingOnErrorCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Stop execution on error"));
//    firstRowLayoutTmp->addWidget(stopExecutingOnErrorCheckbox_pri);

    //extra threads, kill timeout in milliseconds QLineEdit/s
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    QLabel* extraThreadsLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Extra execution threads"));
    extraThreadsLabelTmp->setToolTip(appConfig_ptr_ext->translate_f(
                "<p>Running an action/check requires 1 extra thread, each additional action running at the same time requires 1 extra each,"
                "if no extra threads are available when an action is ready to run it will wait until one is</p>"
                "<p>Minimum 1, maximum INT32MAX</p>"
    ));

    secondRowLayoutTmp->addWidget(extraThreadsLabelTmp);

    extraThreadsLineEdit_pri = new QLineEdit;
    ///////////////////////////////////////2147483648 (10 length)
    //^-? for negative numbers
    QRegExpValidator *a32bitValidatorTmp = new QRegExpValidator(QRegExp("[1-9][0-9]{10}"), this);
    extraThreadsLineEdit_pri->setValidator(a32bitValidatorTmp);
    secondRowLayoutTmp->addWidget(extraThreadsLineEdit_pri);

    QLabel* killTimeoutMillisecondsLabelTmp = new QLabel(appConfig_ptr_ext->translate_f("Kill timeout"));
    killTimeoutMillisecondsLabelTmp->setToolTip(appConfig_ptr_ext->translate_f(
                "<p>How many milliseconds to wait after stopping before resorting to kill</p>"
                "<p>Minimum 0, maximum INT32MAX</p>"
    ));

    secondRowLayoutTmp->addWidget(killTimeoutMillisecondsLabelTmp);

    killTimeoutMillisecondsLineEdit_pri = new QLineEdit;
    //////////////////////////////////////////////////"2147483648
    //killTimeoutMillisecondsLineEdit_pri->setInputMask("9000000000");
    killTimeoutMillisecondsLineEdit_pri->setValidator(a32bitValidatorTmp);
    secondRowLayoutTmp->addWidget(killTimeoutMillisecondsLineEdit_pri);

    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    QPushButton* openStringParserMainWindowButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("String &Parsers"));
    thirdRowLayoutTmp->addWidget(openStringParserMainWindowButtonTmp);
    connect(openStringParserMainWindowButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::stringParserButtonClicked_f);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Save"));
    lastRowLayoutTmp->addWidget(saveButtonTmp);
    connect(saveButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::saveButtonClicked_f);

    QPushButton* cancelButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Cancel"));
    lastRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::cancelButtonClicked_f);

    QPushButton* tipsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    lastRowLayoutTmp->addWidget(tipsButtonTmp);
    connect(tipsButtonTmp, &QPushButton::clicked, this, &executionOptionsWindow_c::tipsButtonClicked_f);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(secondRowLayoutTmp);
    mainLayout_pri->addLayout(thirdRowLayoutTmp);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Execution options"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
    }

    load_f();
}


void executionOptionsWindow_c::saveButtonClicked_f()
{
   if (save_f())
   {
       close();
   }

}

void executionOptionsWindow_c::cancelButtonClicked_f()
{
    close();
}

void executionOptionsWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>Execution options are included when saving on the main window</p>"
                )
            ,  appConfig_ptr_ext->translate_f("Tips")
            , this
                );
}

void executionOptionsWindow_c::stringParserButtonClicked_f()
{
    stringParserManagerWindow_c* stringParserWindowTmp = new stringParserManagerWindow_c(this);
    stringParserWindowTmp->setWindowFlag(Qt::Window, true);
    stringParserWindowTmp->setWindowModality(Qt::WindowModal);
    stringParserWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    stringParserWindowTmp->show();
}


