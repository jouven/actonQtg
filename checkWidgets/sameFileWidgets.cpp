#include "sameFileWidgets.hpp"

#include "../optionsWidgets/workingDirectoryWindow.hpp"
#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/checks/sameFile.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "textQtso/text.hpp"

#include <QtWidgets>
//#include <QSplitter>

void sameFileWidgets_c::derivedParentClosing_f()
{
    //nothing to save, geometry wise, when closing
}

void sameFileWidgets_c::browseFile_f()
{
    browseFileDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
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
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(static_cast<QWidget*>(this->parent()));
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

sameFileData_c sameFileWidgets_c::fieldsToSameFileDataObject_f() const
{
    QString fileAStr(fileAPTE_pri->toPlainText());
    QString fileBStr(fileBPTE_pri->toPlainText());

    return sameFileData_c(fileAStr, fileBStr);
}

bool sameFileWidgets_c::derivedSaveNew_f(const checkData_c& checkData_par_con)
{
    bool resultTmp(false);
    sameFileData_c objTmp(fieldsToSameFileDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        sameFileCheck_ptr_pri = new sameFileCheck_c(checkData_par_con, objTmp);

        checkPtr_pro = sameFileCheck_ptr_pri;
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool sameFileWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    sameFileData_c objTmp(fieldsToSameFileDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        sameFileCheck_ptr_pri->updateSameFileData_f(objTmp);
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

QString sameFileWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>same file widget tips</p>");
}

void sameFileWidgets_c::loadCheckSpecificData_f()
{
    sameFileData_c valuesToLoadTmp(sameFileCheck_ptr_pri not_eq nullptr ? *sameFileCheck_ptr_pri : sameFileData_c());

    fileAPTE_pri->setPlainText(valuesToLoadTmp.fileAPath_f());
    fileBPTE_pri->setPlainText(valuesToLoadTmp.fileBPath_f());
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


sameFileWidgets_c::sameFileWidgets_c(
        check_c*& check_ptr_par
        , QVBoxLayout* const variableLayout_par_con)
    : baseClassCheckTypeWidgets_c(check_ptr_par, variableLayout_par_con->parentWidget())
    , sameFileCheck_ptr_pri(check_ptr_par == nullptr ? nullptr : static_cast<sameFileCheck_c*>(check_ptr_par))
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
