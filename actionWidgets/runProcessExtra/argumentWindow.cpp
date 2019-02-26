#include "argumentWindow.hpp"
#include "appConfig.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QPlainTextEdit>
//#include <QSizeGrip>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QSplitter>

void argumentEditWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    //appConfig_ptr_ext->setWidgetGeometry_f(this->objectName() + mainSplitter_pri->objectName(), mainSplitter_pri->saveState());
    event->accept();
}

argumentEditWindow_c::argumentEditWindow_c(
        const QString& argument_par_con
        , const int currentIndex_par_con
        , const bool enabled_par_con
        , const int rowCount_par_con
        , QWidget* parent_par)
    : QWidget(parent_par)
    , rowCount_pri_con(rowCount_par_con)
{
    setObjectName("argumentWindow_");
    setAttribute(Qt::WA_DeleteOnClose);
    //20180209 subwindow doesn't seem to work, popup has no "window", it's only the frame
    setWindowFlag(Qt::Window, true);
    setWindowModality(Qt::WindowModal);

    QHBoxLayout* argumentValueLayoutTmp = new QHBoxLayout;

    argumentField_pri = new QPlainTextEdit(argument_par_con);
    auto minHeightTmp(argumentField_pri->fontMetrics().lineSpacing() + 14);
    argumentField_pri->setMinimumHeight(minHeightTmp);
    argumentField_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    //add a size handler for the processPath
//    QSizeGrip * sizeGripTmp = new QSizeGrip(argumentField_pri);

//    QHBoxLayout* sizeGripLayoutTmp = new QHBoxLayout;
//    argumentField_pri->setLayout(sizeGripLayoutTmp);
//    sizeGripLayoutTmp->addWidget(sizeGripTmp, 0, Qt::AlignBottom | Qt::AlignRight);

    argumentValueLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Value")));
    argumentValueLayoutTmp->addWidget(argumentField_pri);

    QHBoxLayout* argumentIndexLayoutTmp = new QHBoxLayout;

    argumentIndexField_pri = new QLineEdit(QString::number(currentIndex_par_con));
    argumentIndexField_pri->setToolTip("Index change won't replace other arguments, it will reorder them after or before");
    argumentIndexLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Index")));
    argumentIndexLayoutTmp->addWidget(argumentIndexField_pri);

    enabledCheckbox_pri = new QCheckBox("Enabled");
    enabledCheckbox_pri->setChecked(enabled_par_con);
    enabledCheckbox_pri->setToolTip("Ignore this argument without deleting it");
    argumentIndexLayoutTmp->addWidget(enabledCheckbox_pri);

    QHBoxLayout* buttonsLayoutTmp = new QHBoxLayout;

    QPushButton* saveButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Save"));
    buttonsLayoutTmp->addWidget(saveButtonTmp);
    connect(saveButtonTmp, &QPushButton::clicked, this, &argumentEditWindow_c::saveButtonPushed_f);
    QPushButton* cancelButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Cancel"));
    buttonsLayoutTmp->addWidget(cancelButtonTmp);
    connect(cancelButtonTmp, &QPushButton::clicked, this, &argumentEditWindow_c::cancelButtonPushed_f);

//    QWidget* row1Tmp = new QWidget;
//    QWidget* row2Tmp = new QWidget;
//    QWidget* row3Tmp = new QWidget;

//    row1Tmp->setLayout(argumentValueLayoutTmp);
//    row2Tmp->setLayout(argumentIndexLayoutTmp);
//    row3Tmp->setLayout(buttonsLayoutTmp);

//    mainSplitter_pri = new QSplitter(Qt::Vertical, this);
//    mainSplitter_pri->setObjectName("QSplitter_");
//    mainSplitter_pri->addWidget(row1Tmp);
//    mainSplitter_pri->addWidget(row2Tmp);
//    mainSplitter_pri->addWidget(row3Tmp);
//    mainSplitter_pri->setCollapsible(0, false);
//    mainSplitter_pri->setCollapsible(1, false);
//    mainSplitter_pri->setCollapsible(2, false);

    mainLayout_pri = new QVBoxLayout;
    //mainLayout_pri->addWidget(mainSplitter_pri);
    mainLayout_pri->addLayout(argumentValueLayoutTmp);
    mainLayout_pri->addLayout(argumentIndexLayoutTmp);
    mainLayout_pri->addLayout(buttonsLayoutTmp);
    this->setLayout(mainLayout_pri);

    setWindowTitle(appConfig_ptr_ext->translate_f("Add/update argument"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         //mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }
}

void argumentEditWindow_c::cancelButtonPushed_f()
{
    this->close();
}

void argumentEditWindow_c::saveButtonPushed_f()
{
    while (true)
    {
        //if the argument is empty
        //or the index is negative
        //or the index is greater than the last position
        //return
        bool goodNumberConversion(false);
        if (argumentField_pri->toPlainText().isEmpty()
            or argumentIndexField_pri->text().toLong(&goodNumberConversion) < 0
            or argumentIndexField_pri->text().toLong() > rowCount_pri_con
            or not goodNumberConversion)
        {
            errorQMessageBox_f("Empty argument or wrong index value", "Error", this);
            break;
        }

        argument_c argumentTmp(argumentField_pri->toPlainText(), enabledCheckbox_pri->isChecked());

        Q_EMIT saveArgumentResult_signal(
                    argumentTmp
                    , argumentIndexField_pri->text().toLong()
                    );
        close();
        break;
    }
}
