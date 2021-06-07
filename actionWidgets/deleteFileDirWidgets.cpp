#include "deleteFileDirWidgets.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/actions/deleteFileDir.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
//#include <QSplitter>

#include <QFileDialog>
#include <QDir>

void deleteFileDirWidgets_c::derivedParentClosing_f()
{
    //no widget geometry to save yet
}

QString deleteFileDirWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>create directory widget tips</p>");
}

deleteFileDirData_c deleteFileDirWidgets_c::fieldsToDeleteFileDirDataObject_f() const
{
    return deleteFileDirData_c(
                deleteFileDirPathPTE_pri->toPlainText()
                , errorIfNotExistsCheckbox_pri->isChecked()
                , onlyIfEmptyCheckbox_pri->isChecked()
    );
}

bool deleteFileDirWidgets_c::derivedSaveNew_f(const actionData_c& actionDataBlock_par_con, actonDataHub_c* parentActonDataHub_par)
{
    bool resultTmp(false);
    deleteFileDirData_c objTmp(fieldsToDeleteFileDirDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        deleteFileDirAction_ptr_pri = new deleteFileDirAction_c(parentActonDataHub_par, actionDataBlock_par_con, objTmp);
        actionPtr_pro = deleteFileDirAction_ptr_pri;
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool deleteFileDirWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    deleteFileDirData_c objTmp(fieldsToDeleteFileDirDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        deleteFileDirAction_ptr_pri->updateDeleteFileDirData_f(objTmp);
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

void deleteFileDirWidgets_c::browseDirectory_f()
{
    browseDeleteFileDirDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
    browseDeleteFileDirDialog_pri->setObjectName("browseDeleteFileDirDialog_");
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browseDeleteFileDirDialog_pri->setFileMode(QFileDialog::Directory);
    browseDeleteFileDirDialog_pri->setDirectory(QDir::currentPath());
    browseDeleteFileDirDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select directory..."));
    browseDeleteFileDirDialog_pri->setViewMode(QFileDialog::Detail);
    browseDeleteFileDirDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    browseDeleteFileDirDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    browseDeleteFileDirDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    selectFileDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + browseDeleteFileDirDialog_pri->objectName()));
    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        browseDeleteFileDirDialog_pri->setSidebarUrls(directoriesTmp);
    }

    QObject::connect(browseDeleteFileDirDialog_pri, &QFileDialog::finished, this, &deleteFileDirWidgets_c::fileDialogBrowseDeleteFileDirFinished_f);

    browseDeleteFileDirDialog_pri->show();
}

void deleteFileDirWidgets_c::fileDialogBrowseDeleteFileDirFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browseDeleteFileDirDialog_pri->selectedFiles().isEmpty())
        {
            deleteFileDirPathPTE_pri->setPlainText(browseDeleteFileDirDialog_pri->selectedFiles().first());
            appConfig_ptr_ext->addDirectoryHistory_f(
                        this->objectName() + browseDeleteFileDirDialog_pri->objectName()
                        , browseDeleteFileDirDialog_pri->directory().path()
            );
        }
    }
    browseDeleteFileDirDialog_pri->deleteLater();
    browseDeleteFileDirDialog_pri = nullptr;
}

void deleteFileDirWidgets_c::loadActionSpecificData_f()
{
    deleteFileDirData_c valuesToLoadTmp(deleteFileDirAction_ptr_pri not_eq nullptr ? *deleteFileDirAction_ptr_pri : deleteFileDirData_c());

    deleteFileDirPathPTE_pri->setPlainText(valuesToLoadTmp.path_f());
    errorIfNotExistsCheckbox_pri->setChecked(valuesToLoadTmp.errorIfNotExists_f());
    onlyIfEmptyCheckbox_pri->setChecked(valuesToLoadTmp.onlyIfEmpty_f());
}

deleteFileDirWidgets_c::deleteFileDirWidgets_c(
        action_c*& actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        )
    : baseClassActionTypeWidgets_c(actionData_ptr_par, variableLayout_par_con->parentWidget())
    , deleteFileDirAction_ptr_pri(actionData_ptr_par == nullptr ? nullptr : static_cast<deleteFileDirAction_c*>(actionData_ptr_par))
{
    this->setObjectName("deleteFileDirWidgets_");

    //current working directory
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Current working directory")));
    firstRowLayoutTmp->addSpacing(10);
    firstRowLayoutTmp->addWidget(new QLabel(QDir::currentPath()));
    firstRowLayoutTmp->addStretch(1);

    //error if not exists check
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    errorIfNotExistsCheckbox_pri = new QCheckBox("Error if not exists");
    errorIfNotExistsCheckbox_pri->setToolTip(
R"(Checked: action will error-finish if the path/s don't exist
Unchecked: action will success-finish when trying to delete non-existing paths)"
                );

    //only if empty
    onlyIfEmptyCheckbox_pri = new QCheckBox("Only if empty");
    onlyIfEmptyCheckbox_pri->setToolTip(
R"(Checked: "single file": will only remove if size is 0, "directory": will only remove empty directories (without files or other directories inside)
Unchecked: always remove)");

    secondRowLayoutTmp->addWidget(errorIfNotExistsCheckbox_pri);
    secondRowLayoutTmp->addWidget(onlyIfEmptyCheckbox_pri);

    //path plainTextEdit and browse path button
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    deleteFileDirPathPTE_pri = new QPlainTextEdit;
    deleteFileDirPathPTE_pri->setMinimumHeight(30);
    deleteFileDirPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Remove path")));
    thirdRowLayoutTmp->addWidget(deleteFileDirPathPTE_pri);
    QPushButton* browsePathButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    thirdRowLayoutTmp->addWidget(browsePathButtonTmp);

    connect(browsePathButtonTmp, &QPushButton::clicked, this, &deleteFileDirWidgets_c::browseDirectory_f);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);

    loadActionSpecificData_f();
}

