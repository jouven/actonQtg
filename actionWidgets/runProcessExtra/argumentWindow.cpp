#include "argumentWindow.hpp"

#include "../../appConfig.hpp"

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
    , currentIndex_pri_con(currentIndex_par_con)
{
    setObjectName("argumentWindow_");
    setAttribute(Qt::WA_DeleteOnClose);
    //20180209 subwindow doesn't seem to work, popup has no "window", it's only the frame
    setWindowFlag(Qt::Window, true);
    setWindowModality(Qt::WindowModal);

    QHBoxLayout* argumentValueLayoutTmp = new QHBoxLayout;

    argumentFieldPTE_pri = new QPlainTextEdit(argument_par_con);
    auto minHeightTmp(argumentFieldPTE_pri->fontMetrics().lineSpacing() + 14);
    argumentFieldPTE_pri->setMinimumHeight(minHeightTmp);
    argumentFieldPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    //add a size handler for the processPath
//    QSizeGrip * sizeGripTmp = new QSizeGrip(argumentField_pri);

//    QHBoxLayout* sizeGripLayoutTmp = new QHBoxLayout;
//    argumentField_pri->setLayout(sizeGripLayoutTmp);
//    sizeGripLayoutTmp->addWidget(sizeGripTmp, 0, Qt::AlignBottom | Qt::AlignRight);

    argumentValueLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Value")));
    argumentValueLayoutTmp->addWidget(argumentFieldPTE_pri);

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

    if (currentIndex_par_con == rowCount_pri_con)
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Add argument"));
    }
    else
    {
        setWindowTitle(appConfig_ptr_ext->translate_f("Update argument"));
    }

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
         //mainSplitter_pri->restoreState(appConfig_ptr_ext->widgetGeometry_f(this->objectName() + mainSplitter_pri->objectName()));
    }
}

void argumentEditWindow_c::cancelButtonPushed_f()
{
    close();
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
        if (argumentFieldPTE_pri->toPlainText().isEmpty()
            or argumentIndexField_pri->text().toLong(&goodNumberConversion) < 0
            or argumentIndexField_pri->text().toLong() > rowCount_pri_con
            or not goodNumberConversion)
        {
            errorQMessageBox_f("Empty argument or wrong index value", "Error", this);
            break;
        }

        argument_c argumentTmp(argumentFieldPTE_pri->toPlainText(), enabledCheckbox_pri->isChecked());
        int newIndexTmp(argumentIndexField_pri->text().toInt());
        const bool rowUpdateTmp_con(newIndexTmp == currentIndex_pri_con);

        //case index changed, it's a "move"
        //trivia this "move" is usless for index+1 changes, since it "pushes" the other indexes to +1
        //i.e. moving the first row (0) to row (1) will leave it like before,
        //since the new row (and all that follow) get +1 index wise
        if (currentIndex_pri_con < rowCount_pri_con and not rowUpdateTmp_con)
        {
            //delete current index
            Q_EMIT deleteArgument_signal(currentIndex_pri_con);
            //if the new index is greater than the current
            //substract one to the new index (because the current row just got removed)
            if (newIndexTmp > currentIndex_pri_con)
            {
                newIndexTmp = newIndexTmp - 1;
            }
        }

        Q_EMIT saveArgumentResult_signal(
                    argumentTmp
                    , newIndexTmp
                    //it's an update?
                    , newIndexTmp < rowCount_pri_con and rowUpdateTmp_con
        );

        if (rowUpdateTmp_con)
        {
            //if an argument row is updated there is no index changes
        }
        else
        {
            int lowestIndexTmp(std::min(currentIndex_pri_con, newIndexTmp));
            Q_EMIT refreshArgumentIndexColumn_signal(lowestIndexTmp);
        }

        close();
        break;
    }
}
