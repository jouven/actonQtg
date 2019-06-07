#include "checkWindow.hpp"

#include "appConfig.hpp"
#include "checkWidgets/baseClassCheckWidgets.hpp"
#include "checkWidgets/actionFinishedWidgets.hpp"
#include "checkWidgets/sameFileWidgets.hpp"

#include "actonQtso/checkMappings/checkStrMapping.hpp"
#include "actonQtso/checksDataHub.hpp"
#include "actonQtso/checkData.hpp"

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
    mainSplitter_pri->setObjectName("QSplitter_");

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
        setWindowTitle(appConfig_ptr_ext->translate_f("Update Check"));
        isNew_pri = false;
        check_ptr_pri = checkDataHub_ptr_pri->check_ptr_f(checkIdTmp);
#ifdef DEBUGJOUVEN
        //qDebug() << "checkData_ptr_pri->parentAction_f() " << checkData_ptr_pri->parentAction_f() << endl;
#endif

        loadedCheckTypeIndexTmp = checkTypeCombo_pri->findData(check_ptr_pri->typeStr_f().toLower());
        checkTypeCombo_pri->setCurrentIndex(loadedCheckTypeIndexTmp);
        checkTypeCombo_pri->setEnabled(false);

        //actionStringIdPTE_pri->setPlainText(itemTmp.stringId_f());
        descriptionPTE_pri->setPlainText(check_ptr_pri->description_f());
    }
    else
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Add Check"));
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
        qDebug() << "createWidgetsPerCheck_f checknTypeStrTmp " << checkTypeStrTmp << endl;
#endif
        if (checkTypeStrTmp == checkTypeToStrUMap_glo_sta_con.at(checkType_ec::actionFinished).toLower())
        {
            baseClassCheckWidgets_pri = new actionFinishedWidgets_c(check_ptr_pri, variableLayout_pri, checkDataHub_ptr_pri->parentAction_f());
            break;
        }
        if (checkTypeStrTmp == checkTypeToStrUMap_glo_sta_con.at(checkType_ec::sameFile).toLower())
        {
            baseClassCheckWidgets_pri = new sameFileWidgets_c(check_ptr_pri, variableLayout_pri);
            break;
        }
        break;
    }
}

void checkWindow_c::checkComboChanged_f(int index_par)
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

void checkWindow_c::save_f()
{
    while (true)
    {
        //QString actionStringIdTmp(actionStringIdPTE_pri->toPlainText());
        QString descriptionTmp(descriptionPTE_pri->toPlainText());
        if (descriptionTmp.isEmpty())
        {
            errorQMessageBox_f("Empty description", "Error", this);
            break;
            //FUTURE if description is empty, create a descrition Gen for each chck, i.e. for *action finished* "action finished " + actionStringId (limit the length of this)
        }

        if (isNew_pri)
        {
            //FUTURE implement the literal value arguments
            checkData_c checkDataTmp(QString(), descriptionTmp, false, enabledCheckbox_pri->isChecked());
            baseClassCheckWidgets_pri->saveNew_f(checkDataTmp);
            checkDataHub_ptr_pri->insertCheck_f(check_ptr_pri, row_pri_con);
        }
        else
        {
            check_ptr_pri->setDescription_f(descriptionTmp);
            check_ptr_pri->setEnabled_f(enabledCheckbox_pri->isChecked());
            baseClassCheckWidgets_pri->saveUpdate_f();
        }

        Q_EMIT updateRow_Signal(row_pri_con);
        close();
        break;
    }
}
