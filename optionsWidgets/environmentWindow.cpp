#include "environmentWindow.hpp"
#include "../appConfig.hpp"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProcessEnvironment>


void environmentWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

environmentWindow_c::environmentWindow_c(QWidget* parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("environmentWindow_");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QProcessEnvironment processEnvironmentTmp(QProcessEnvironment::systemEnvironment());
    QStringList envStringListTmp(processEnvironmentTmp.toStringList());

    QPlainTextEdit* environmentPTETmp = new QPlainTextEdit;
    auto minHeightTmp(environmentPTETmp->fontMetrics().lineSpacing() + 14);
    environmentPTETmp->setMinimumHeight(minHeightTmp);
    environmentPTETmp->setReadOnly(true);
    for (const QString& envStr_ite_con : envStringListTmp)
    {
        environmentPTETmp->appendPlainText(envStr_ite_con);
    }

    QVBoxLayout* mainLayoutTmp = new QVBoxLayout;
    mainLayoutTmp->addWidget(environmentPTETmp);
    QPushButton* okButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("Ok"));
    mainLayoutTmp->addWidget(okButtonPtrTmp);
    this->setLayout(mainLayoutTmp);

    setWindowTitle(appConfig_ptr_ext->translate_f("Environment"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
    }

    connect(okButtonPtrTmp, &QPushButton::clicked, this, &environmentWindow_c::okButtonPushed_f);
}

void environmentWindow_c::okButtonPushed_f()
{
    this->close();
}

