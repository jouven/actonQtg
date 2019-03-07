#include "sameFileWidgets.hpp"

#include "appConfig.hpp"
#include "optionsWidgets/workingDirectoryWindow.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/checks/sameFile.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
#include <QJsonObject>
//#include <QSplitter>

void sameFileWidgets_c::parentClosing_f()
{

}

void sameFileWidgets_c::save_f()
{
    while (true)
    {
        if (fileAPTE_pri->toPlainText().isEmpty())
        {
            errorQMessageBox_f("File A path is empty", "Error", qobject_cast<QWidget*>(this->parent()));
            break;
        }
        if (fileBPTE_pri->toPlainText().isEmpty())
        {
            errorQMessageBox_f("File B path is empty", "Error", qobject_cast<QWidget*>(this->parent()));
            break;
        }

        saveCheckDataJSON_f();
        Q_EMIT JSONSaved_signal();
        break;
    }
}

void sameFileWidgets_c::browseFile_f()
{
    browseFileDialog_pri = new QFileDialog(qobject_cast<QWidget*>(this->parent()));
    browseFileDialog_pri->setObjectName("browseFileDialog_");
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browseFileDialog_pri->setFileMode(QFileDialog::AnyFile);
    browseFileDialog_pri->setDirectory(QDir::currentPath());
    QString whichFileTmp;
    if (browsingFileA_pri)
    {
        whichFileTmp = appConfig_ptr_ext->translate_f("Select file A...");
    }
    if (browsingFileB_pri)
    {
        whichFileTmp = appConfig_ptr_ext->translate_f("Select file B...");
    }
    browseFileDialog_pri->setWindowTitle(whichFileTmp);
    browseFileDialog_pri->setViewMode(QFileDialog::Detail);
    browseFileDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    browseFileDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    browseFileDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    browseFileDialog_pri.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + browseFileDialog_pri->objectName()));
    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        browseFileDialog_pri->setSidebarUrls(directoriesTmp);
    }

    QObject::connect(browseFileDialog_pri, &QFileDialog::finished, this, &sameFileWidgets_c::fileDialogBrowseFinished_f);

    browseFileDialog_pri->show();
}

void sameFileWidgets_c::fileDialogBrowseFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browseFileDialog_pri->selectedFiles().isEmpty())
        {
            if (browsingFileA_pri)
            {
                fileAPTE_pri->setPlainText(browseFileDialog_pri->selectedFiles().first());
            }
            if (browsingFileB_pri)
            {
                fileBPTE_pri->setPlainText(browseFileDialog_pri->selectedFiles().first());
            }
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + browseFileDialog_pri->objectName(), browseFileDialog_pri->directory().path());
        }
    }
    browsingFileA_pri = false;
    browsingFileB_pri = false;
    browseFileDialog_pri->deleteLater();
    browseFileDialog_pri = nullptr;
}

void sameFileWidgets_c::showCurrentWorkingDirectoryWindow_f()
{
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(qobject_cast<QWidget*>(this->parent()));
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

void sameFileWidgets_c::loadCheckSpecificData_f()
{
    if (not checkData_ptr_pri->checkDataJSON_f().isEmpty())
    {
        sameFileCheck_c sameFileTmp;
        sameFileTmp.read_f(checkData_ptr_pri->checkDataJSON_f());

        fileAPTE_pri->setPlainText(sameFileTmp.fileAPath_f());
        fileBPTE_pri->setPlainText(sameFileTmp.fileBPath_f());
    }
}

void sameFileWidgets_c::browseFileA_f()
{
    browsingFileA_pri = true;
    browseFile_f();
}

void sameFileWidgets_c::browseFileB_f()
{
    browsingFileB_pri = true;
    browseFile_f();
}

void sameFileWidgets_c::saveCheckDataJSON_f() const
{
    QString fileAStr(fileAPTE_pri->toPlainText());
    QString fileBStr(fileBPTE_pri->toPlainText());

    sameFileCheck_c sameFileCheckTmp(fileAStr, fileBStr);

    QJsonObject saveValuesJSONObjectTmp;
    sameFileCheckTmp.write_f(saveValuesJSONObjectTmp);
    checkData_ptr_pri->setCheckDataJSON_f(saveValuesJSONObjectTmp);
}


sameFileWidgets_c::sameFileWidgets_c(
        checkData_c* const checkData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        , QObject *parent)
    : QObject(parent)
    , checkData_ptr_pri(checkData_ptr_par)
{
    this->setObjectName("sameFileWidgets_");

    //file A
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    fileAPTE_pri = new QPlainTextEdit;
    auto minHeightTmp(fileAPTE_pri->fontMetrics().lineSpacing() + 14);
    fileAPTE_pri->setMinimumHeight(minHeightTmp);
    fileAPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("File A")));
    firstRowLayoutTmp->addSpacing(10);
    firstRowLayoutTmp->addWidget(fileAPTE_pri);
    //firstRowLayoutTmp->addStretch(1);

    QPushButton* browseFileAButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    firstRowLayoutTmp->addWidget(browseFileAButtonTmp);

    connect(browseFileAButtonTmp, &QPushButton::clicked, this, &sameFileWidgets_c::browseFileA_f);
    //continue testing
    //file B
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    fileBPTE_pri = new QPlainTextEdit;
    fileBPTE_pri->setMinimumHeight(minHeightTmp);
    fileBPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    secondRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("File B")));
    secondRowLayoutTmp->addSpacing(10);
    secondRowLayoutTmp->addWidget(fileBPTE_pri);
    //secondRowLayoutTmp->addStretch(1);

    QPushButton* browseFileBButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    secondRowLayoutTmp->addWidget(browseFileBButtonTmp);

    connect(browseFileBButtonTmp, &QPushButton::clicked, this, &sameFileWidgets_c::browseFileB_f);

    //show current working directory
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    QPushButton* showCurrentWorkingDirectoryButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Show current working directory"));
    thirdRowLayoutTmp->addWidget(showCurrentWorkingDirectoryButtonTmp);
    connect(showCurrentWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &sameFileWidgets_c::showCurrentWorkingDirectoryWindow_f);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);

    loadCheckSpecificData_f();
}
