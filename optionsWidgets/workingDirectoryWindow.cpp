#include "workingDirectoryWindow.hpp"
#include "appConfig.hpp"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDir>


void workingDirectoryWindow_c::closeEvent(QCloseEvent* event)
{
    appConfig_ptr_ext->setWidgetGeometry_f(this->objectName(), saveGeometry());
    event->accept();
}

workingDirectoryWindow_c::workingDirectoryWindow_c(QWidget* parent_par)
    : QWidget(parent_par)
{
    this->setObjectName("workingDirectoryWindow");
    this->setAttribute(Qt::WA_DeleteOnClose);

    QPlainTextEdit* workingDirectoryPTETmp = new QPlainTextEdit;
    auto minHeightTmp(workingDirectoryPTETmp->fontMetrics().lineSpacing() + 14);
    workingDirectoryPTETmp->setMinimumHeight(minHeightTmp);
    workingDirectoryPTETmp->appendPlainText(QDir::currentPath());
    workingDirectoryPTETmp->setReadOnly(true);

    QVBoxLayout* mainLayoutTmp = new QVBoxLayout;
    mainLayoutTmp->addWidget(workingDirectoryPTETmp);
    QPushButton* okButtonPtrTmp = new QPushButton(appConfig_ptr_ext->translate_f("Ok"));
    mainLayoutTmp->addWidget(okButtonPtrTmp);
    this->setLayout(mainLayoutTmp);

    setWindowTitle(appConfig_ptr_ext->translate_f("Environment"));

    if (appConfig_ptr_ext->configLoaded_f())
    {
         restoreGeometry(appConfig_ptr_ext->widgetGeometry_f(this->objectName()));
    }

    connect(okButtonPtrTmp, &QPushButton::clicked, this, &workingDirectoryWindow_c::okButtonPushed_f);
}

void workingDirectoryWindow_c::okButtonPushed_f()
{
    this->close();
}

