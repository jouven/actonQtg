#include "stringParserEditorWindow.hpp"

#include "stringParserWidgets/stringReplacerWidgets.hpp"

#include "../appConfig.hpp"

#include "actonQtso/actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

void stringParserEditorWindow_c::closeEvent(QCloseEvent* event)
{
    Q_EMIT closeWindow_signal();
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

void stringParserEditorWindow_c::clearLayoutItems_f(QLayout* layout_par)
{
    QLayoutItem* child;
    while (layout_par->count() > 0)
    {
        while (true)
        {
            child = layout_par->takeAt(0);
            layout_par->removeItem(child);
            if (child->layout() not_eq 0)
            {
                clearLayoutItems_f(child->layout());
                child->layout()->deleteLater();
                break;
            }
            if (child->widget() not_eq 0)
            {
                child->widget()->deleteLater();
                break;
            }
            if (child->spacerItem() not_eq 0)
            {
                //has no deleteLater
                child->~QLayoutItem();
                break;
            }
            break;
        }
    }
}

//void stringParserEditorWindow_c::askUpdateStringTriggerDepdenciesWindowFinished_f(const int result_par)
//{
//    if (result_par == QMessageBox::Yes)
//    {
//        actonDataHub_ptr_ext->updateStringTriggerDependencies_f(parserObj_pri->stringTrigger_f(), oldStringTrigger_pri);
//    }
//    askUpdateStringTriggerDepdenciesWindow_pri->deleteLater();
//    askUpdateStringTriggerDepdenciesWindow_pri = nullptr;
//    close();
//}

//void stringParserEditorWindow_c::openAskUpdateStringTriggerDepdenciesWindow_f()
//{
//    askUpdateStringTriggerDepdenciesWindow_pri = new QMessageBox(this);
//    askUpdateStringTriggerDepdenciesWindow_pri->setText(R"(The updated string trigger is used in other checks, update them?)");
//    askUpdateStringTriggerDepdenciesWindow_pri->setTextFormat(Qt::RichText);
//    askUpdateStringTriggerDepdenciesWindow_pri->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//    askUpdateStringTriggerDepdenciesWindow_pri->setDefaultButton(QMessageBox::Yes);
//    askUpdateStringTriggerDepdenciesWindow_pri->setWindowModality(Qt::WindowModal);

//    QObject::connect(askUpdateStringTriggerDepdenciesWindow_pri, &QMessageBox::finished, this, &stringParserEditorWindow_c::askUpdateStringTriggerDepdenciesWindowFinished_f);

//    askUpdateStringTriggerDepdenciesWindow_pri->show();
//}

void stringParserEditorWindow_c::testParserButtonClicked_f()
{
    if (parserObj_pri not_eq nullptr)
    {
        QString stringCopyTmp(stringTriggerPTE_pri->toPlainText());
        parserObj_pri->execute_f(std::addressof(stringCopyTmp));
        testParserResultPTE_pri->setPlainText(stringCopyTmp);
    }
}

stringParserEditorWindow_c::stringParserEditorWindow_c(
        const int row_par_con
        , QWidget *parent_par)
    : QWidget(parent_par)
    , row_pri_con(row_par_con)
{
    this->setObjectName("stringParserConfig_");
    this->setAttribute(Qt::WA_DeleteOnClose);


    stringTriggerPTE_pri = new QPlainTextEdit(this);
    auto minHeightTmp(stringTriggerPTE_pri->fontMetrics().lineSpacing() + 14);

    //statusBarLabel_pri = new QLabel;

    //enabled + parser type
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //enabled
    enabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Enabled"));
    enabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("Disabled string parser configs will not be executed"));
    enabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    enabledCheckbox_pri->setChecked(true);
    firstRowLayoutTmp->addWidget(enabledCheckbox_pri);

    parserTypeCombo_pri = new QComboBox();
    parserTypeCombo_pri->setEditable(true);
    parserTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    parserTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    parserTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Type")));
    firstRowLayoutTmp->addWidget(parserTypeCombo_pri);

    for (const QString& typeStr_ite_con : parserBase_c::strToTypeMap_sta_con.keys())
    {
        parserTypeCombo_pri->insertItem(parserTypeCombo_pri->count(), appConfig_ptr_ext->translate_f(typeStr_ite_con), typeStr_ite_con);
    }

    //string trigger
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("String trigger")));

    stringTriggerPTE_pri->setMinimumHeight(minHeightTmp);
    stringTriggerPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    secondRowLayoutTmp->addWidget(stringTriggerPTE_pri);

    //test button and test result field
    QHBoxLayout* fourthRowLayout = new QHBoxLayout;

    QPushButton* testParserButton_pri = new QPushButton("&Dry run");
    testParserButton_pri->setToolTip("Requires saving first\nShow-test current format/value replace result\nWARNING this will change/execute stuff that might be irreversible depending on the parser");
    fourthRowLayout->addWidget(testParserButton_pri);
    connect(testParserButton_pri, &QPushButton::clicked, this, &stringParserEditorWindow_c::testParserButtonClicked_f);

    testParserResultPTE_pri = new QPlainTextEdit(this);
    testParserResultPTE_pri->setMinimumHeight(minHeightTmp);
    testParserResultPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    fourthRowLayout->addWidget(testParserResultPTE_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("&Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);

//    QPushButton* tipsButtonTmp = new QPushButton("Tips");
//    lastRowLayoutTmp->addWidget(tipsButtonTmp);


    QWidget* row2Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;

    row2Tmp->setLayout(secondRowLayoutTmp);
    variableLayout_pri = new QVBoxLayout;
    //variableLayout_pri->setSpacing(0);
    row3Tmp->setLayout(variableLayout_pri);
    row4Tmp->setLayout(fourthRowLayout);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter_");

    mainSplitter_pri->addWidget(row2Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->addWidget(row4Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);

    QVBoxLayout* mainLayoutTmp(new QVBoxLayout);
    mainLayoutTmp->addLayout(firstRowLayoutTmp);
    mainLayoutTmp->addWidget(mainSplitter_pri);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    //mainLayout_pri->addLayout(variableLayout_pri);
    mainLayoutTmp->addLayout(lastRowLayoutTmp);
    //mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayoutTmp);

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }

    connect(saveButtonTmp, &QPushButton::clicked, this, &stringParserEditorWindow_c::saveButtonClicked_f);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &stringParserEditorWindow_c::cancelButtonClicked_f);
    //connect(tipsButtonTmp, &QPushButton::clicked, this, &actionWindow_c::tipsButtonClicked_f);

    int loadedActionTypeIndexTmp(0);
    parserObj_pri = stringParserMap_ptr_ext->orderToParserBaseMap_f().value(row_pri_con);
    if (parserObj_pri not_eq nullptr)
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Update String Parser"));
        isNew_pri = false;

        QString actionTypeStrTmp(parserBase_c::typeToStrUMap_sta_con.at(parserObj_pri->type_f()));
        loadedActionTypeIndexTmp = parserTypeCombo_pri->findData(actionTypeStrTmp.toLower());
        parserTypeCombo_pri->setCurrentIndex(loadedActionTypeIndexTmp);
        parserTypeCombo_pri->setEnabled(false);

        stringTriggerPTE_pri->setPlainText(parserObj_pri->stringTrigger_f());
        stringTriggerPTE_pri->setReadOnly(true);

        enabledCheckbox_pri->setChecked(parserObj_pri->enabled_f());
    }
    else
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Add String Parser"));
    }

    actionComboChanged_f(loadedActionTypeIndexTmp);

    connect(parserTypeCombo_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &stringParserEditorWindow_c::actionComboChanged_f);
}

void stringParserEditorWindow_c::createWidgetsPerParser_f(
        const int index_par_con
)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "createWidgetsPerAction_f index_par_con " << index_par_con << endl;
#endif
    while (true)
    {
        QString parserTypeStrTmp(parserTypeCombo_pri->itemData(index_par_con).toString());
#ifdef DEBUGJOUVEN
        //qDebug() << "createWidgetsPerParser_f parserTypeStrTmp " << parserTypeStrTmp << endl;
#endif
        if (parserTypeStrTmp == parserBase_c::typeToStrUMap_sta_con.at(parserBase_c::type_ec::stringReplace).toLower())
        {
            //createParserTypeWidgets_f(specificParserWidgets_pri);
            specificParserWidgets_pri = new stringReplacerWidgets_c(parserObj_pri, variableLayout_pri);

            break;
        }
        break;
    }
    //make the saving of the window position/size, and its items, generic
    connect(this, &stringParserEditorWindow_c::closeWindow_signal, specificParserWidgets_pri, &baseClassStringParserWidgets_c::parentClosing_f);
}

void stringParserEditorWindow_c::actionComboChanged_f(int index_par)
{
    if (lastIndex_pri not_eq -1)
    {
        specificParserWidgets_pri->deleteLater();
        specificParserWidgets_pri = nullptr;
    }

    clearLayoutItems_f(variableLayout_pri);
    createWidgetsPerParser_f(index_par);
    lastIndex_pri = index_par;
}

void stringParserEditorWindow_c::saveButtonClicked_f()
{
    save_f();
}

void stringParserEditorWindow_c::cancelButtonClicked_f()
{
    close();
}


//no need for now
//void actionWindow_c::tipsButtonClicked_f()
//{
//    plainQMessageBox_f
//    (
//                appConfig_ptr_ext->translate_f(
//                    "<p>1 If a previous action is edited and the type is changed the old values last until the action is saved again, it is always possible to switch back to the original type (no need to cancel and edit again)</p>"
//                    "<p>2 \"Any finish\", checked, means that the action has finished running, successfully or not</p>"
//                    )
//            , "Tips"
//            , this
//    );
//}

void stringParserEditorWindow_c::save_f()
{
    while (true)
    {
        QString stringTriggerTmp(stringTriggerPTE_pri->toPlainText());
        if (stringTriggerTmp.isEmpty())
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Empty string trigger")
                               , "Error"
                               , this);
            break;
        }

        //checking existing parsers or checks/actions that will create parsers only applies
        //when saving new parser since the string trigger can't be modified after
        if (isNew_pri)
        {
            if (actonDataHub_ptr_ext->hasStringTriggerAnyDependency_f(stringTriggerTmp, nullptr) > 0)
            {
                errorQMessageBox_f(appConfig_ptr_ext->translate_f("String trigger already in use in an action-check")
                                   , appConfig_ptr_ext->translate_f("Error")
                                   , this);
                break;
            }

            int_fast64_t existingStringParserIndexTmp(stringParserMap_ptr_ext->stringTriggerIndex_f(stringTriggerTmp));
            if (existingStringParserIndexTmp not_eq -1)
            {
                errorQMessageBox_f(appConfig_ptr_ext->translate_f("String trigger already in use in another string parser, index " + QString::number(existingStringParserIndexTmp))
                                   , appConfig_ptr_ext->translate_f("Error"), this);
                break;
            }
        }

#ifdef DEBUGJOUVEN
        //qDebug() << "parserObj_pri " << parserObj_pri << endl;
        //qDebug() << "specificParserWidgets_pri " << specificParserWidgets_pri << endl;
#endif
        if (specificParserWidgets_pri->save_f(stringTriggerTmp))
        {

        }
        else
        {
            break;
        }

        parserObj_pri->setEnabled_f(enabledCheckbox_pri->isChecked());

        if (isNew_pri)
        {
            actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->addParser_f(parserObj_pri);
            if (actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->anyError_f())
            {
                errorQMessageBox_f(actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->getError_f(), "Error", this);
                break;
            }
        }

        Q_EMIT updateRow_Signal(row_pri_con);
        close();
        break;
    }
}
