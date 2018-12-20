#include "checkWindow.hpp"

#include "appConfig.hpp"
#include "checkWidgets/actionFinishedWidgets.hpp"
#include "checkWidgets/sameFileWidgets.hpp"

#include "actonQtso/checkMappings/checkStrMapping.hpp"
#include "actonQtso/actionData.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

void checkWindow_c::closeEvent(QCloseEvent* event)
{
    Q_EMIT closeWindow_signal();
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

//T*& because there is an assign operation, by value would make it useless
template <typename T>
void checkWindow_c::createCheckTypeWidgets_f(T*& checkTypeClassPtr_par)
{
#ifdef DEBUGJOUVEN
//    if (checkData_ptr_pri->parentAction_f() not_eq nullptr)
//    {
//        qDebug() << "checkData_ptr_pri->parentAction_f()->id_f() " << QString::number(checkData_ptr_pri->parentAction_f()->id_f()) << endl;
//    }
#endif
    checkTypeClassPtr_par = new T(checkData_ptr_pri, variableLayout_pri, this);

    //make the saving of the window position/size, and its items, generic
    connect(this, &checkWindow_c::closeWindow_signal, checkTypeClassPtr_par, &T::parentClosing_f);
    //save the unique fields of the check type
    connect(this, &checkWindow_c::saveJSON_signal, checkTypeClassPtr_par, &T::save_f);
    //once the above is done, get the save result from the "check type"/parent and save *here*
    connect(checkTypeClassPtr_par, &T::JSONSaved_signal, this, &checkWindow_c::save_f);
}

checkWindow_c::checkWindow_c(
        const int row_par_con
        , checksDataHub_c* checkDataHub_ptr_par
        , QWidget *parent_par)
    : QWidget(parent_par)
    , row_pri_con(row_par_con)
    , checkDataHub_ptr_pri(checkDataHub_ptr_par)
{
    this->setObjectName("checkWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //statusBarLabel_pri = new QLabel;

    //combo actions + checkbox halt on fail
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    checkTypeCombo_pri = new QComboBox();
    checkTypeCombo_pri->setEditable(true);
    checkTypeCombo_pri->setInsertPolicy(QComboBox::NoInsert);
    checkTypeCombo_pri->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    checkTypeCombo_pri->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Checks")));
    firstRowLayoutTmp->addWidget(checkTypeCombo_pri);

    //when inserting, the "0" enum shouldn't be the first
    //because otherwise when checkTypeCombo_pri->setCurrentIndex(0); won't trigger the
    //combochange and won't render the widgets for the specific type

    for (const QString& checkTypeStr_ite_con : strToCheckTypeMap_glo_sta_con.keys())
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
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Description")));
    descriptionPTE_pri = new QPlainTextEdit(this);
    auto minHeightTmp(descriptionPTE_pri->fontMetrics().lineSpacing() + 14);
    descriptionPTE_pri->setMinimumHeight(minHeightTmp);
    descriptionPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    thirdRowLayoutTmp->addWidget(descriptionPTE_pri);

    //for the bottom buttons
    QHBoxLayout* lastRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton("&Save");
    lastRowLayoutTmp->addWidget(saveButtonTmp);

    QPushButton* cancelButtonTmp = new QPushButton("&Cancel");
    lastRowLayoutTmp->addWidget(cancelButtonTmp);

    QPushButton* tipsButtonTmp = new QPushButton("Tips");
    lastRowLayoutTmp->addWidget(tipsButtonTmp);

    //QWidget* row1Tmp = new QWidget;
    //row1Tmp->setContentsMargins(0,0,0,0);
    QWidget* row2Tmp = new QWidget;
    //row2Tmp->setContentsMargins(0,0,0,0);
    QWidget* row3Tmp = new QWidget;
    //row3Tmp->setContentsMargins(0,0,0,0);

    //row1Tmp->setLayout(secondRowLayoutTmp);
    row2Tmp->setLayout(thirdRowLayoutTmp);
    variableLayout_pri = new QVBoxLayout;
    //variableLayout_pri->setSpacing(0);
    row3Tmp->setLayout(variableLayout_pri);

    //seems that qsplitter has an innate margin/border and, as 20180222, I don't see how to remove/hide/reduce
    mainSplitter_pri = new QSplitter(Qt::Vertical);
    mainSplitter_pri->setObjectName("QSplitter");

    //mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row2Tmp);
    mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    //mainSplitter_pri->setCollapsible(2, false);
    mainSplitter_pri->setContentsMargins(0,0,0,0);


    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(firstRowLayoutTmp);
    mainLayout_pri->addWidget(mainSplitter_pri);
    //mainLayout_pri->addLayout(secondRowLayoutTmp);
    //mainLayout_pri->addLayout(variableLayout_pri);
    mainLayout_pri->addLayout(lastRowLayoutTmp);
    //mainLayout_pri->addWidget(statusBarLabel_pri);
    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Add/update Check"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }


    connect(saveButtonTmp, &QPushButton::clicked, this, &checkWindow_c::saveButtonClicked_f);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &checkWindow_c::cancelButtonClicked_f);
    connect(tipsButtonTmp, &QPushButton::clicked, this, &checkWindow_c::tipsButtonClicked_f);

    int loadedCheckTypeIndexTmp(0);
    int_fast64_t checkIdTmp(checkDataHub_ptr_pri->rowToCheckDataId_f(row_par_con));
    if (checkIdTmp > 0)
    {
        isNew_pri = false;
        checkData_ptr_pri = checkDataHub_ptr_pri->checkData_ptr_f(checkIdTmp);
#ifdef DEBUGJOUVEN
        //qDebug() << "checkData_ptr_pri->parentAction_f() " << checkData_ptr_pri->parentAction_f() << endl;
#endif
        QString checkTypeStrTmp(checkTypeToStrUMap_glo_sta_con.at(checkData_ptr_pri->type_f()));
        loadedCheckTypeIndexTmp = checkTypeCombo_pri->findData(checkTypeStrTmp.toLower());
        checkTypeCombo_pri->setCurrentIndex(loadedCheckTypeIndexTmp);

        //checkTypeCombo_pri->setEditable(false);

        //actionStringIdPTE_pri->setPlainText(itemTmp.stringId_f());
        descriptionPTE_pri->setPlainText(checkData_ptr_pri->description_f());
    }
    else
    {
        //the empty "new" check needs the parent to be set
        checkDataNew_pri.setParentAction_f(checkDataHub_ptr_par->parentAction_f());
    }

    checkComboChanged_f(loadedCheckTypeIndexTmp);

    connect(checkTypeCombo_pri, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &checkWindow_c::checkComboChanged_f);
}

void checkWindow_c::createWidgetsPerCheck_f(
        const int index_par_con
)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "createWidgetsPerAction_f index_par_con " << index_par_con << endl;
#endif
    //switch? nope
    while (true)
    {
        QString checkTypeStrTmp(checkTypeCombo_pri->itemData(index_par_con).toString());
#ifdef DEBUGJOUVEN
        //qDebug() << "createWidgetsPerCheck_f checknTypeStrTmp " << checkTypeStrTmp << endl;
#endif
        if (checkTypeStrTmp == checkTypeToStrUMap_glo_sta_con.at(checkType_ec::actionFinished).toLower())
        {
            createCheckTypeWidgets_f(actionFinishedWidgets_pri);
            break;
        }
        if (checkTypeStrTmp == checkTypeToStrUMap_glo_sta_con.at(checkType_ec::sameFile).toLower())
        {
            createCheckTypeWidgets_f(sameFileWidgets_pri);
            break;
        }
        break;
    }
}

void checkWindow_c::removeWidgetClassPerCheck_f(
        const int index_par_con
)
{
    while (true)
    {
        QString checkTypeStrTmp(checkTypeCombo_pri->itemData(index_par_con).toString());
        if (checkTypeStrTmp == checkTypeToStrUMap_glo_sta_con.at(checkType_ec::sameFile).toLower())
        {
            sameFileWidgets_pri->deleteLater();
            sameFileWidgets_pri = nullptr;
            break;
        }
        if (checkTypeStrTmp == checkTypeToStrUMap_glo_sta_con.at(checkType_ec::actionFinished).toLower())
        {
            actionFinishedWidgets_pri->deleteLater();
            actionFinishedWidgets_pri = nullptr;
            break;
        }
        break;
    }
}

void checkWindow_c::checkComboChanged_f(int index_par)
{
    if (lastIndex_pri not_eq -1)
    {
        removeWidgetClassPerCheck_f(lastIndex_pri);
    }

    clearLayoutItems_f(variableLayout_pri);
    createWidgetsPerCheck_f(index_par);
    lastIndex_pri = index_par;
}

void checkWindow_c::saveButtonClicked_f()
{
    Q_EMIT saveJSON_signal();
}

void checkWindow_c::cancelButtonClicked_f()
{
    close();
}

void checkWindow_c::manageChecksButtonClicked_f()
{
    //continuar
}

void checkWindow_c::tipsButtonClicked_f()
{
    plainQMessageBox_f
    (
             appConfig_ptr_ext->translate_f(
                    "<p>TODO</p>"
                    "<p>TODO</p>"
                    )
            , "Tips"
            , this
    );
}

void checkWindow_c::save_f()
{
    while (true)
    {
        checkType_ec checkTypeTmp(strToCheckTypeMap_glo_sta_con.value(checkTypeCombo_pri->currentData().toString()));
        //QString actionStringIdTmp(actionStringIdPTE_pri->toPlainText());
        QString descriptionTmp(descriptionPTE_pri->toPlainText());
        if (descriptionTmp.isEmpty())
        {
            errorQMessageBox_f("Empty description", "Error", this);
            break;
            //FUTURE if description is empty, create a descrition Gen for each chck, i.e. for *action finished* "action finished " + actionStringId (limit the length of this)
        }

        checkData_ptr_pri->setType_f(checkTypeTmp);
        checkData_ptr_pri->setDescription_f(descriptionTmp);
        //it's set in each check type widget
        //checkDataHub_ref_pri->setActionDataJSON_f(childSaveResult_par_con);
        if (isNew_pri)
        {
            checkDataHub_ptr_pri->insertCheckData_f(*checkData_ptr_pri, row_pri_con);
        }

        Q_EMIT updateRow_Signal(row_pri_con);
        close();
        break;
    }
}
