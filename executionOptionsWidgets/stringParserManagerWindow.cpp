#include "stringParserManagerWindow.hpp"

#include "stringParserEditorWindow.hpp"

#include "../appConfig.hpp"
#include "../stringFormatting.hpp"

#include "actonQtso/actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>

#include <set>

void stringParserManagerWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + stringParserConfigsTable_pri->objectName(), stringParserConfigsTable_pri->saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + stringParserConfigsTable_pri->objectName() + stringParserConfigsTable_pri->horizontalHeader()->objectName(), stringParserConfigsTable_pri->horizontalHeader()->saveState());
    event->accept();
}

void stringParserManagerWindow_c::loadActonDataHubParserConfigs_f()
{
    if (actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f() not_eq nullptr)
    {
         QMap<int_fast64_t, parserBase_c*>::const_iterator iteTmp(actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->orderToParserBaseMap_f().constBegin());
         while (iteTmp not_eq actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->orderToParserBaseMap_f().constEnd())
         {
             updateActionRow_f(iteTmp.key());
             ++iteTmp;
         }
    }
}

stringParserManagerWindow_c::stringParserManagerWindow_c(QWidget* parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("stringParserMainWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    //add/edit, remove and copy buttons
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    //add/edit string parser config
    QPushButton* addEditStringParserConfigButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Add/Edit"));
    addEditStringParserConfigButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Add/Edit a string parser configuration"));
    firstRowLayoutTmp->addWidget(addEditStringParserConfigButtonTmp);
    QObject::connect(addEditStringParserConfigButtonTmp, &QPushButton::clicked, this, &stringParserManagerWindow_c::addUpdateStringParserConfig_f);

    //remove string parser configs
    QPushButton* removeStringParserConfigButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Remove"));
    removeStringParserConfigButtonTmp->setToolTip(appConfig_ptr_ext->translate_f("Remove selected string parser configurations"));
    firstRowLayoutTmp->addWidget(removeStringParserConfigButtonTmp);
    QObject::connect(removeStringParserConfigButtonTmp, &QPushButton::clicked, this, &stringParserManagerWindow_c::removeStringParserConfigs_f);

    //copy string parser config
    QPushButton* copyStringParserConfigButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("&Copy"));
    firstRowLayoutTmp->addWidget(copyStringParserConfigButtonTmp);
    QObject::connect(copyStringParserConfigButtonTmp, &QPushButton::clicked, this, &stringParserManagerWindow_c::copyStringParseConfig_f);

    //tips button
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    //tips button
    QPushButton* tipsButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Tips"));
    secondRowLayoutTmp->addWidget(tipsButtonTmp);
    QObject::connect(tipsButtonTmp, &QPushButton::clicked, this, &stringParserManagerWindow_c::showTips_f);

    //parser config table
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    stringParserConfigsTable_pri = new QTableWidget(0, 4);
    stringParserConfigsTable_pri->setObjectName("QTableWidget_");
    stringParserConfigsTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    //WARNING if this ever changes, in this file there are several places where this kind of comment that should be changed
    // parser (type) 0 | key 1 | value/format/"description" 2 | enabled 3
    labels << appConfig_ptr_ext->translate_f("Type") << appConfig_ptr_ext->translate_f("Key") << appConfig_ptr_ext->translate_f("Value/Format") << appConfig_ptr_ext->translate_f("Enabled");
    //stringParserConfigsTable_pri->horizontalHeaderItem(0)->setToolTip(appConfig_ptr_ext->translate_f("Type"));
    stringParserConfigsTable_pri->setHorizontalHeaderLabels(labels);
    stringParserConfigsTable_pri->horizontalHeaderItem(1)->setToolTip(appConfig_ptr_ext->translate_f("String key that triggers the parser"));
    stringParserConfigsTable_pri->horizontalHeaderItem(2)->setToolTip(appConfig_ptr_ext->translate_f("Format/value which will replace the key"));
    stringParserConfigsTable_pri->horizontalHeaderItem(3)->setToolTip(appConfig_ptr_ext->translate_f("Disabled parser configurations will be ignored on execution"));
    stringParserConfigsTable_pri->horizontalHeader()->setObjectName("QHeaderView_");
    stringParserConfigsTable_pri->setShowGrid(true);
    //TODO on android there is no right mouse button (touch holding or doing gestures doesn't make it) by default
    //so no context menus
    //commandsTable_pri->setContextMenuPolicy(Qt::CustomContextMenu);
    stringParserConfigsTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    stringParserConfigsTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif

    thirdRowLayoutTmp->addWidget(stringParserConfigsTable_pri);

    QVBoxLayout* mainLayoutTmp = new QVBoxLayout;
    mainLayoutTmp->addLayout(firstRowLayoutTmp);
    mainLayoutTmp->addLayout(secondRowLayoutTmp);
    mainLayoutTmp->addLayout(thirdRowLayoutTmp);
    this->setLayout(mainLayoutTmp);

    setWindowTitle(appConfig_ptr_ext->translate_f("String parser configurations"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         stringParserConfigsTable_pri->restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + stringParserConfigsTable_pri->objectName()));
         stringParserConfigsTable_pri->horizontalHeader()->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + stringParserConfigsTable_pri->objectName() + stringParserConfigsTable_pri->horizontalHeader()->objectName()));
    }

    //connect(okButtonPtrTmp, &QPushButton::clicked, this, &stringParserWindow_c::okButtonPushed_f);
    loadActonDataHubParserConfigs_f();
}

void stringParserManagerWindow_c::insertStringParserConfigRow_f(
        parserBase_c* const parserObjPtr_par
        , const int row_par_con)
{
    QString parserTypeStr(parserBase_c::typeToStrUMap_sta_con.at(parserObjPtr_par->type_f()));
#ifdef DEBUGJOUVEN
    //qDebug() << "9";
#endif
    QTableWidgetItem *parserTypeCellTmp(new QTableWidgetItem(parserTypeStr));
    parserTypeCellTmp->setFlags(parserTypeCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *stringKeyCellTmp(new QTableWidgetItem(parserObjPtr_par->stringTrigger_f()));
    stringKeyCellTmp->setToolTip(parserObjPtr_par->stringTrigger_f());
    stringKeyCellTmp->setFlags(stringKeyCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *valueFormatCellTmp(new QTableWidgetItem(parserObjPtr_par->valueFormat_f()));
    valueFormatCellTmp->setToolTip(parserObjPtr_par->valueFormat_f());
    valueFormatCellTmp->setFlags(stringKeyCellTmp->flags() bitand compl Qt::ItemIsEditable);

    QTableWidgetItem *enabledCellTmp(new QTableWidgetItem);
    Qt::CheckState checkstateTmp(parserObjPtr_par->enabled_f() ? Qt::Checked : Qt::Unchecked);
    enabledCellTmp->setCheckState(checkstateTmp);
    enabledCellTmp->setFlags(stringKeyCellTmp->flags() bitand compl Qt::ItemIsEditable);

    int newIndexTmp(row_par_con);
    if (newIndexTmp == -1)
    {
        newIndexTmp = stringParserConfigsTable_pri->rowCount();
    }

    // parser (type) 0 | key 1 | value/format/"description" 2 | enabled 3
    stringParserConfigsTable_pri->insertRow(newIndexTmp);
    stringParserConfigsTable_pri->setItem(newIndexTmp, 0, parserTypeCellTmp);
    stringParserConfigsTable_pri->setItem(newIndexTmp, 1, stringKeyCellTmp);
    stringParserConfigsTable_pri->setItem(newIndexTmp, 2, valueFormatCellTmp);
    stringParserConfigsTable_pri->setItem(newIndexTmp, 3, enabledCellTmp);
}

void stringParserManagerWindow_c::updateExistingStringParserConfigRow_f(
        parserBase_c* const parserObjPtr_par
        //this is for copies
        , const int row_par_con)
{
    QString parserTypeStr(parserBase_c::typeToStrUMap_sta_con.at(parserObjPtr_par->type_f()));

    // parser (type) 0 | key 1 | value/format/"description" 2 | enabled 3
    stringParserConfigsTable_pri->item(row_par_con, 0)->setText(parserTypeStr);
    stringParserConfigsTable_pri->item(row_par_con, 0)->setToolTip(parserTypeStr);

    stringParserConfigsTable_pri->item(row_par_con, 1)->setText(parserObjPtr_par->stringTrigger_f());
    stringParserConfigsTable_pri->item(row_par_con, 1)->setToolTip(parserObjPtr_par->stringTrigger_f());

    stringParserConfigsTable_pri->item(row_par_con, 2)->setText(parserObjPtr_par->valueFormat_f());
    stringParserConfigsTable_pri->item(row_par_con, 2)->setText(parserObjPtr_par->valueFormat_f());

    Qt::CheckState checkStateTmp(parserObjPtr_par->enabled_f() ? Qt::Checked : Qt::Unchecked);
    stringParserConfigsTable_pri->item(row_par_con, 3)->setCheckState(checkStateTmp);
}

void stringParserManagerWindow_c::updateActionRow_f(const int row_par_con)
{
    parserBase_c* parserPtrTmp(stringParserMap_ptr_ext->orderToParserBaseMap_f().value(row_par_con));

    if (row_par_con == stringParserConfigsTable_pri->rowCount())
    {
        //qDebug() << "insert action row, parserPtrTmp " << parserPtrTmp << endl;
        insertStringParserConfigRow_f(
                    parserPtrTmp
        );
    }
    else
    {
        //qDebug() << "update row " << row_par_con << endl;
        updateExistingStringParserConfigRow_f(
                    parserPtrTmp
                    , row_par_con
        );
    }
}

void stringParserManagerWindow_c::addUpdateStringParserConfig_f()
{
//    while (true)
//    {
    int rowTmp(-1);
    //if the selection is empty use the last row index
    if (stringParserConfigsTable_pri->selectedItems().isEmpty())
    {
        rowTmp = stringParserConfigsTable_pri->rowCount();
    }
    else
        //else get the first selected row
    {
        rowTmp = stringParserConfigsTable_pri->selectedItems().first()->row();
    }

    stringParserEditorWindow_c* stringParserConfigWindowTmp(new stringParserEditorWindow_c(rowTmp, this));
    //20180209 subwindow doesn't seem to work, popup has no "window" it's only the frame
    stringParserConfigWindowTmp->setWindowFlag(Qt::Window, true);
    stringParserConfigWindowTmp->setWindowModality(Qt::WindowModal);

    connect(stringParserConfigWindowTmp, &stringParserEditorWindow_c::updateRow_Signal, this, &stringParserManagerWindow_c::updateActionRow_f);

    stringParserConfigWindowTmp->show();
//        break;
//    }
}

void stringParserManagerWindow_c::removeStringParserConfigs_f()
{
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp = stringParserConfigsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No string parser config rows selected for removal")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::set<int> rowIndexSetTmp;
        //get the selected row (indexes)
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexSetTmp.emplace(item_ite_con->row());
        }

        std::vector<int> rowsToRemoveTmp(rowIndexSetTmp.begin(), rowIndexSetTmp.end());
        std::reverse(rowsToRemoveTmp.begin(), rowsToRemoveTmp.end());
        for (const int item_ite_con : rowsToRemoveTmp)
        {
            stringParserConfigsTable_pri->removeRow(item_ite_con);
            stringParserMap_ptr_ext->removeParser_f(item_ite_con);
        }
        break;
    }
}

void stringParserManagerWindow_c::copyStringParseConfig_f()
{
    while (true)
    {
        QList<QTableWidgetItem *> selectionTmp = stringParserConfigsTable_pri->selectedItems();
        if (selectionTmp.isEmpty())
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("No string parser config row selected to copy")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        std::unordered_set<int> rowIndexesTmp;
        for (const QTableWidgetItem* item_ite_con : selectionTmp)
        {
            rowIndexesTmp.emplace(item_ite_con->row());
        }

        if (rowIndexesTmp.size() > 1)
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translate_f("String parser config row copy can only be applied to a single row")
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
            break;
        }

        int selectedRowTmp(selectionTmp.first()->row());

        parserBase_c* parserPtrTmp(stringParserMap_ptr_ext->orderToParserBaseMap_f().value(selectedRowTmp));

        copyStringParserConfigIndexInputDialog_pri = new QInputDialog(this);
        copyStringParserConfigIndexInputDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Copy string parser configuration"));
        copyStringParserConfigIndexInputDialog_pri->setLabelText(
                    appConfig_ptr_ext->translateAndReplace_f({
                    "Input a new index to copy the string parser configuration to.\nString parser configuration source:"
                    "\nString trigger: {0}"
                    "\nType: {1}"
                    "\nValue/Format: {2}"
                    "\nRow index: {3}",
                    parserPtrTmp->stringTrigger_f()
                    , parserBase_c::typeToStrUMap_sta_con.at(parserPtrTmp->type_f())
                    , truncateString_f(parserPtrTmp->valueFormat_f(), 255)
                    , selectedRowTmp
                    })
        );
        copyStringParserConfigIndexInputDialog_pri->setInputMode(QInputDialog::IntInput);
        copyStringParserConfigIndexInputDialog_pri->setIntValue(selectedRowTmp);
        copyStringParserConfigIndexInputDialog_pri->setWindowModality(Qt::WindowModal);

        QObject::connect(copyStringParserConfigIndexInputDialog_pri, &QFileDialog::finished, this, &stringParserManagerWindow_c::inputDialogCopyStringParserConfigIndexFinished_f);

        copyStringParserConfigIndexInputDialog_pri->show();
        break;
    }
}

void stringParserManagerWindow_c::inputDialogCopyStringParserConfigIndexFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        int newIndexTmp(copyStringParserConfigIndexInputDialog_pri->intValue());
        if (newIndexTmp >= 0 and newIndexTmp <= stringParserConfigsTable_pri->rowCount())
        {
            QList<QTableWidgetItem *> selectionTmp(stringParserConfigsTable_pri->selectedItems());
            int selectedRowTmp(selectionTmp.first()->row());

            parserBase_c* parserPtr(stringParserMap_ptr_ext->orderToParserBaseMap_f().value(selectedRowTmp)->clone_f());

            stringParserMap_ptr_ext->addParser_f(parserPtr, newIndexTmp);
            insertStringParserConfigRow_f(parserPtr, newIndexTmp);
        }
        else
        {
            errorQMessageBox_f(
                        appConfig_ptr_ext->translateAndReplace_f({"Wrong index to copy the row: {0}", newIndexTmp})
                        , appConfig_ptr_ext->translate_f("Error")
                        , this);
        }
    }
    copyStringParserConfigIndexInputDialog_pri->deleteLater();
    copyStringParserConfigIndexInputDialog_pri = nullptr;
}

//void stringParserWindow_c::okButtonPushed_f()
//{
//    this->close();
//}

void stringParserManagerWindow_c::showTips_f()
{
    plainQMessageBox_f
    (
                appConfig_ptr_ext->translate_f(
                    "<p>TODO</p>"
                    //"<p>2 Action saved file/s can be dropped on the ActionQtg windows and will be loaded</p>"
                    //R"(<p>3 Quitting ActonQtg "normally" during an operation won't instantly exit, during execution, it will have to finish executing the current action, during saving, it will have to finish saving</p>)"
                    )
            , "Tips"
            , this
    );
}

