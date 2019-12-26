#include "pathExistsWidgets.hpp"

#include "../optionsWidgets/workingDirectoryWindow.hpp"
#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/checkData.hpp"
#include "actonQtso/checks/pathExists.hpp"

#include "essentialQtgso/messageBox.hpp"
#include "textQtso/text.hpp"

#include <QtWidgets>
//#include <QSplitter>

void pathExistsWidgets_c::derivedParentClosing_f()
{
    //nothing to save, geometry wise, when closing
}

void pathExistsWidgets_c::browseFile_f()
{
    browseFileDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
    browseFileDialog_pri->setObjectName("browseFileDialog_");
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browseFileDialog_pri->setFileMode(QFileDialog::AnyFile);
    browseFileDialog_pri->setDirectory(QDir::currentPath());
    browseFileDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select a path..."));
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

    QObject::connect(browseFileDialog_pri, &QFileDialog::finished, this, &pathExistsWidgets_c::fileDialogBrowseFinished_f);

    browseFileDialog_pri->show();
}

void pathExistsWidgets_c::fileDialogBrowseFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browseFileDialog_pri->selectedFiles().isEmpty())
        {
            pathPTE_pri->setPlainText(browseFileDialog_pri->selectedFiles().first());
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + browseFileDialog_pri->objectName(), browseFileDialog_pri->directory().path());
        }
    }
    browseFileDialog_pri->deleteLater();
    browseFileDialog_pri = nullptr;
}

void pathExistsWidgets_c::showCurrentWorkingDirectoryWindow_f()
{
    workingDirectoryWindow_c* workingDirectoryWindowTmp = new workingDirectoryWindow_c(static_cast<QWidget*>(this->parent()));
    workingDirectoryWindowTmp->setWindowFlag(Qt::Window, true);
    workingDirectoryWindowTmp->setWindowModality(Qt::WindowModal);
    workingDirectoryWindowTmp->setAttribute(Qt::WA_DeleteOnClose);
    workingDirectoryWindowTmp->show();
}

pathExistsData_c pathExistsWidgets_c::fieldsToSameFileDataObject_f() const
{
    return pathExistsData_c(pathPTE_pri->toPlainText());
}

bool pathExistsWidgets_c::derivedSaveNew_f(const checkData_c& checkData_par_con)
{
    bool resultTmp(false);
    pathExistsData_c objTmp(fieldsToSameFileDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        pathExistsCheck_ptr_pri = new pathExistsCheck_c(checkData_par_con, objTmp);

        checkPtr_pro = pathExistsCheck_ptr_pri;
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool pathExistsWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    pathExistsData_c objTmp(fieldsToSameFileDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        pathExistsCheck_ptr_pri->updatePathExistsData_f(objTmp);
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

QString pathExistsWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>existing path widget tips</p>");
}

void pathExistsWidgets_c::loadCheckSpecificData_f()
{
    pathExistsData_c valuesToLoadTmp(pathExistsCheck_ptr_pri not_eq nullptr ? *pathExistsCheck_ptr_pri : pathExistsData_c());

    pathPTE_pri->setPlainText(valuesToLoadTmp.path_f());
}

pathExistsWidgets_c::pathExistsWidgets_c(
        check_c*& check_ptr_par
        , QVBoxLayout* const variableLayout_par_con)
    : baseClassCheckTypeWidgets_c(check_ptr_par, variableLayout_par_con->parentWidget())
    , pathExistsCheck_ptr_pri(check_ptr_par == nullptr ? nullptr : static_cast<pathExistsCheck_c*>(check_ptr_par))
{
    this->setObjectName("pathExistsWidgets_");

    //path
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    pathPTE_pri = new QPlainTextEdit;
    auto minHeightTmp(pathPTE_pri->fontMetrics().lineSpacing() + 14);
    pathPTE_pri->setMinimumHeight(minHeightTmp);
    pathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    pathPTE_pri->setToolTip(appConfig_ptr_ext->translate_f("Non-existing path can be used too"));

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Path")));
    firstRowLayoutTmp->addWidget(pathPTE_pri);

    QPushButton* browseFileAButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    firstRowLayoutTmp->addWidget(browseFileAButtonTmp);

    connect(browseFileAButtonTmp, &QPushButton::clicked, this, &pathExistsWidgets_c::browseFile_f);

    //show current working directory
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    QPushButton* showCurrentWorkingDirectoryButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Show current working directory"));
    thirdRowLayoutTmp->addWidget(showCurrentWorkingDirectoryButtonTmp);
    connect(showCurrentWorkingDirectoryButtonTmp, &QPushButton::clicked, this, &pathExistsWidgets_c::showCurrentWorkingDirectoryWindow_f);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);

    loadCheckSpecificData_f();
}
