#include "checkWindow.hpp"

#include "appConfig.hpp"
#include "checkWidgets/baseClassCheckTypeWidgets.hpp"
#include "checkWidgets/actionFinishedWidgets.hpp"
#include "checkWidgets/sameFileWidgets.hpp"
#include "checkWidgets/pathExistsWidgets.hpp"
#include "checkWidgets/timerWidgets.hpp"
#include "commonWidgets.hpp"

#include "actonQtso/checkMappings/checkStrMapping.hpp"
#include "actonQtso/checksDataHub.hpp"
#include "actonQtso/checkData.hpp"
//when loading check defaults values, a check obj is necessary and a derived must be ctored
#include "actonQtso/checks/actionFinished.hpp"
#include "actonQtso/actionData.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

void checkWindow_c::closeEvent(QCloseEvent* event)
{
    baseClassCheckWidgets_pri->parentClosing_f();
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

void checkWindow_c::clearLayoutItems_f(QLayout* layout_par)
{
    QLayoutItem* child;
    while (layout_par->count() > 0)
    {
        while (true)
        {
            child = layout_par->takeAt(0);
            layout_par->removeItem(child);
            if (child->layout() not_eq nullptr)
            {
                clearLayoutItems_f(child->layout());
                child->layout()->deleteLater();
                break;
            }
            if (child->widget() not_eq nullptr)
            {
                child->widget()->deleteLater();
                break;
            }
            if (child->spacerItem() not_eq nullptr)
            {
                //has no deleteLater
                child->~QLayoutItem();
                break;
            }
            break;
        }
    }
}


void checkWindow_c::loadCheckData_f()
{
    check_c* valuesToLoadTmp(check_ptr_pri);
    if (valuesToLoadTmp == nullptr)
    {
        //create an action obj to get the default values
        valuesToLoadTmp = new actionFinishedCheck_c();
        valuesToLoadTmp->deleteLater();
    }

    int loadedCheckTypeIndexTmp(checkTypeCombo_pri->findData(valuesToLoadTmp->typeStr_f().toLower()));
    checkTypeCombo_pri->setCurrentIndex(loadedCheckTypeIndexTmp);
    if (check_ptr_pri not_eq nullptr)
    {
        checkTypeCombo_pri->setEnabled(false);
    }

    if (valuesToLoadTmp->resultLogic_f() not_eq checkData_c::resultLogic_ec::empty)
    {
        int loadedResultLogicIndexTmp(resultLogicCombo_pri->findData(checkData_c::resultLogicToStrUMap_sta_con.at(valuesToLoadTmp->resultLogic_f()).toLower()));
        resultLogicCombo_pri->setCurrentIndex(loadedResultLogicIndexTmp);
    }
//    else
//    {
//#ifdef DEBUGJOUVEN
//        qDebug() << "checkWindow_c::loadCheckData_f() result logic empty" << endl;
//#endif
//    }

    descriptionPTE_pri->setPlainText(valuesToLoadTmp->description_f());

    enabledCheckbox_pri->setChecked(valuesToLoadTmp->enabled_f());
    threadedCheckbox_pri->setChecked(valuesToLoadTmp->threaded_f());

    checkTypeComboChanged_f(loadedCheckTypeIndexTmp);
}

checkWindow_c::checkWindow_c(
        const int row_par_con
        , checksDataHub_c* checkDataHub_ptr_par
        , QWidget *parent_par)
    : QWidget(parent_par)
    , row_pri_con(row_par_con)
    , checkDataHub_ptr_pri(checkDataHub_ptr_par)
{
    this->setObjectName("checkWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //statusBarLabel_pri = new QLabel;

    //combo actions + checkbox halt on fail
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    descriptionPTE_pri = new QPlainTextEdit(this);
    auto minHeightTmp(descriptionPTE_pri->fontMetrics().lineSpacing() + 14);

    //enabled
    enabledCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Enabled"));
    enabledCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f("A disabled check will not be executed"));
    //enabledCheckbox_pri->setMinimumHeight(minHeightTmp);
    //enabledCheckbox_pri->setChecked(true);
    firstRowLayoutTmp->addWidget(enabledCheckbox_pri);

    //check type
    checkTypeCombo_pri = new QComboBox();
    checkTypeCombo_pri->setEditable(true);
    checkTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    checkTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    checkTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Type")));
    firstRowLayoutTmp->addWidget(checkTypeCombo_pri);

    for (const QString& checkTypeStr_ite_con : strToCheckTypeMap_ext_con.keys())
    {
        checkTypeCombo_pri->insertItem(checkTypeCombo_pri->count(), appConfig_ptr_ext->translate_f(checkTypeStr_ite_con), checkTypeStr_ite_con);
    }

//    //check string id ?
//    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

//    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("String Id")));
//    checkStringIdPTE_pri = new QPlainTextEdit(this);
//    auto minHeightTmp(actionStringIdPTE_pri->fontMetrics().lineSpacing() + 14);
//    checkStringIdPTE_pri->setMinimumHeight(minHeightTmp);
//    checkStringIdPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
//    secondRowLayoutTmp->addWidget(checkStringIdPTE_pri);

    //description
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Description")));
    descriptionPTE_pri->setMinimumHeight(minHeightTmp);
    descriptionPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    secondRowLayoutTmp->addWidget(descriptionPTE_pri);

    //resultLogic combo, threaded check
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    //result logic
    resultLogicCombo_pri = new QComboBox();
    resultLogicCombo_pri->setEditable(true);
    resultLogicCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    resultLogicCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    resultLogicCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Result logic")));
    thirdRowLayoutTmp->addWidget(resultLogicCombo_pri);

    //when inserting, the "0" enum shouldn't be the first
    //because otherwise when checkTypeCombo_pri->setCurrentIndex(0); won't trigger the
    //combochange and won't render the widgets for the specific type

    for (const QString& resultLogicStr_ite_con : checkData_c::strToResultLogicMap_sta_con.keys())
    {
        resultLogicCombo_pri->insertItem(resultLogicCombo_pri->count(), appConfig_ptr_ext->translate_f(resultLogicStr_ite_con), resultLogicStr_ite_con);
    }

    //threaded
    threadedCheckbox_pri = new QCheckBox(appConfig_ptr_ext->translate_f("Threaded"));
    threadedCheckbox_pri->setToolTip(appConfig_ptr_ext->translate_f(
R"(Affects check/s execution within the action
true: all checks with threaded=true execute in order and execute at the same time
false: all checks with threaded=false execute in order one after the other)"
                                         ));
    //threadedCheckbox_pri->setMinimumHeight(minHeightTmp);
    thirdRowLayoutTmp->addWidget(threadedCheckbox_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Save"));
    lastRowLayoutTmp->addWidget(saveButtonTmp);

    QPushButton* cancelButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Cancel"));
    lastRowLayoutTmp->addWidget(cancelButtonTmp);

    QPushButton* tipsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    lastRowLayoutTmp->addWidget(tipsButtonTmp);

    //QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row4Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);

    //row1Tmp->setLayout(secondRowLayoutTmp);
    row3Tmp->setLayout(secondRowLayoutTmp);
    variableLayout_pri = new QVBoxLayout;
    //variableLayout_pri->setSpacing(0);
    row4Tmp->setLayout(variableLayout_pri);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter_");

    //mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->addWidget(row4Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    //mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);


    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addLayout(thirdRowLayoutTmp);
    mainLayout_pri->addWidget(mainSplitter_pri);
        //mainLayout_pri->addLayout(variableLayout_pri);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    //mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayout_pri);

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }


    connect(saveButtonTmp, &QPushButton::clicked, this, &checkWindow_c::saveButtonClicked_f);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &checkWindow_c::cancelButtonClicked_f);
    connect(tipsButtonTmp, &QPushButton::clicked, this, &checkWindow_c::tipsButtonClicked_f);

    int_fast64_t checkIdTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(row_par_con));
    if (checkIdTmp > 0)
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Update Check"));
        isNew_pri = false;
        check_ptr_pri = checkDataHub_ptr_pri->check_ptr_f(checkIdTmp);
#ifdef DEBUGJOUVEN
        //qDebug() << "checkData_ptr_pri->parentAction_f() " << checkData_ptr_pri->parentAction_f() << endl;
#endif
    }
    else
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Add Check"));
    }

    loadCheckData_f();

    connect(checkTypeCombo_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &checkWindow_c::checkTypeComboChanged_f);
}

void checkWindow_c::createWidgetsPerCheck_f(
        const int index_par_con
)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "createWidgetsPerAction_f index_par_con " << index_par_con << endl;
#endif
    QString checkTypeStrTmp(checkTypeCombo_pri->itemData(index_par_con).toString());
#ifdef DEBUGJOUVEN
    //qDebug() << "createWidgetsPerCheck_f checknTypeStrTmp " << checkTypeStrTmp << endl;
#endif
    const std::unordered_map<checkType_ec, std::function<baseClassCheckTypeWidgets_c*()>> checkTypeToCheckCreationFunctionMap_con(
    {
        {	checkType_ec::timer, [this]() -> baseClassCheckTypeWidgets_c* { return new timerWidgets_c(check_ptr_pri, variableLayout_pri);}}
        , {	checkType_ec::sameFile, [this]() -> baseClassCheckTypeWidgets_c* { return new sameFileWidgets_c(check_ptr_pri, variableLayout_pri);}}
        , {	checkType_ec::pathExists, [this]() -> baseClassCheckTypeWidgets_c* { return new pathExistsWidgets_c(check_ptr_pri, variableLayout_pri);}}
        , {	checkType_ec::actionFinished, [this]() -> baseClassCheckTypeWidgets_c* { return new actionFinishedWidgets_c(check_ptr_pri, variableLayout_pri, static_cast<action_c*>(checkDataHub_ptr_pri->parent()));}}
    });

    auto findResultTmp(checkTypeToCheckCreationFunctionMap_con.find(strToCheckTypeMap_ext_con.value(checkTypeStrTmp)));
    if (findResultTmp not_eq checkTypeToCheckCreationFunctionMap_con.cend())
    {
        baseClassCheckWidgets_pri = findResultTmp->second();
    }
    else
    {
        text_c errorTextTmp("Check type {0} has no editor widgets", checkTypeStrTmp);
        errorQMessageBox_f(appConfig_ptr_ext->translateAndReplace_f(errorTextTmp));
    }

}

void checkWindow_c::checkTypeComboChanged_f(int index_par)
{
    if (lastIndex_pri not_eq -1)
    {
        baseClassCheckWidgets_pri->deleteLater();
        baseClassCheckWidgets_pri = nullptr;
    }

    clearLayoutItems_f(variableLayout_pri);
    createWidgetsPerCheck_f(index_par);
    lastIndex_pri = index_par;
}

void checkWindow_c::saveButtonClicked_f()
{
    save_f();
}

void checkWindow_c::cancelButtonClicked_f()
{
    close();
}

void checkWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
             appConfig_ptr_ext->translate_f(
                    "<p>TODO</p>"
                    "<p>TODO</p>"
                    )
            , appConfig_ptr_ext->translate_f("Tips")
            , this
    );
}

checkData_c checkWindow_c::fieldsToCheckDataObject_f() const
{
    QString descriptionTmp(descriptionPTE_pri->toPlainText());
    checkData_c::resultLogic_ec resultLogicTmp(checkData_c::strToResultLogicMap_sta_con.value(resultLogicCombo_pri->currentData().toString()));

    return checkData_c(
                //unique id string (not in use)
                //QString()
                descriptionTmp
                , threadedCheckbox_pri->isChecked()
                , enabledCheckbox_pri->isChecked()
                , resultLogicTmp
    );
}

bool checkWindow_c::save_f()
{
    bool resultTmp(false);
    textCompilation_c errorsTmp;
    checkData_c objTmp(fieldsToCheckDataObject_f());
    if (objTmp.isFieldsCheckDataValid_f(std::addressof(errorsTmp)))
    {
        if (isNew_pri)
        {
            baseClassCheckWidgets_pri->saveNew_f(objTmp);
            checkDataHub_ptr_pri->insertCheck_f(check_ptr_pri, row_pri_con);
        }
        else
        {
            bool actionTypeSaveResultTmp(baseClassCheckWidgets_pri->saveUpdate_f());
            if (actionTypeSaveResultTmp)
            {
                check_ptr_pri->updateCheckData_f(objTmp);
            }
        }

        resultTmp = true;
        Q_EMIT updateRow_Signal(row_pri_con);
        close();
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}
