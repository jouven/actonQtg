#include "environmentPairWindow.hpp"

#include "../../appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

void environmentPairToAddEditWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

environmentPairToAddEditWindow_c::environmentPairToAddEditWindow_c(const QString& key_par_con
        , const QString& value_par_con
        , const bool enabled_par_con
        , const int editedRowIndex_par_con
        , QWidget* parent_par)
    : QWidget(parent_par)
    , editedRowIndex_pri_con(editedRowIndex_par_con)
{
    setObjectName("environmentPairWindow_");
    setAttribute(Qt::WA_DeleteOnClose);
    //20180209 subwindow doesn't seem to work, popup has no "window", it's only the frame
    setWindowFlag(Qt::Window, true);
    setWindowModality(Qt::WindowModal);

    //first row, environment key field
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    keyFieldPTE_pri = new QPlainTextEdit(key_par_con);
    auto minHeightTmp(keyFieldPTE_pri->fontMetrics().lineSpacing() + 14);
    keyFieldPTE_pri->setMinimumHeight(minHeightTmp);
    keyFieldPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Key")));
    firstRowLayoutTmp->addWidget(keyFieldPTE_pri);

    //second row, environment key field
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    valueFieldPTE_pri = new QPlainTextEdit(value_par_con);
    valueFieldPTE_pri->setMinimumHeight(minHeightTmp);
    valueFieldPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Value")));
    secondRowLayoutTmp->addWidget(valueFieldPTE_pri);

    //third row, enabled checbkox
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    enabledCheckbox_pri = new QCheckBox("Enabled");
    enabledCheckbox_pri->setChecked(enabled_par_con);
    enabledCheckbox_pri->setToolTip("Ignore this environment key-value pair without deleting it");
    thirdRowLayoutTmp->addWidget(enabledCheckbox_pri);

    //fourth row, save and cancel buttons
    QHBoxLayout* fourthRowLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Save"));
    fourthRowLayoutTmp->addWidget(saveButtonTmp);
    connect(saveButtonTmp, &QPushButton::clicked, this, &environmentPairToAddEditWindow_c::saveButtonPushed_f);
    QPushButton* cancelButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Cancel"));
    fourthRowLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &environmentPairToAddEditWindow_c::cancelButtonPushed_f);

    QWidget* row1Tmp = new QWidget;
    QWidget* row2Tmp = new QWidget;
    //QWidget* row3Tmp = new QWidget;

    row1Tmp->setLayout(firstRowLayoutTmp);
    row2Tmp->setLayout(secondRowLayoutTmp);
    //row3Tmp->setLayout(thirdRowLayoutTmp);

    mainSplitter_pri = new QSplitter(Qt::Vertical, this);
    mainSplitter_pri->setObjectName("QSplitter_");
    mainSplitter_pri->addWidget(row1Tmp);
    mainSplitter_pri->addWidget(row2Tmp);
    //mainSplitter_pri->addWidget(row3Tmp);
    mainSplitter_pri->setCollapsible(0, false);
    mainSplitter_pri->setCollapsible(1, false);
    //mainSplitter_pri->setCollapsible(2, false);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addWidget(mainSplitter_pri);
    mainLayout_pri->addLayout(thirdRowLayoutTmp);
    mainLayout_pri->addLayout(fourthRowLayoutTmp);

    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Add/update environment pair"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }
}

void environmentPairToAddEditWindow_c::cancelButtonPushed_f()
{
    this->close();
}

void environmentPairToAddEditWindow_c::saveButtonPushed_f()
{
    while (true)
    {
        //if the key is empty
        if (keyFieldPTE_pri->toPlainText().isEmpty())
        {
            errorQMessageBox_f("Empty key", "Error", this);
            break;
        }

        environmentPairConfig_c environmentPairTmp(valueFieldPTE_pri->toPlainText(), enabledCheckbox_pri->isChecked());

        Q_EMIT saveEnvironmentPairResult_signal
        (
                    keyFieldPTE_pri->toPlainText()
                    , environmentPairTmp
                    , editedRowIndex_pri_con
        );
        close();
        break;
    }
}
