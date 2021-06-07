#include "createDirectoryWidgets.hpp"

#include "../commonWidgets.hpp"
#include "../appConfig.hpp"

#include "actonQtso/actions/createDirectory.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
//#include <QSplitter>

#include <QFileDialog>
#include <QDir>

void createDirectoryWidgets_c::derivedParentClosing_f()
{
    //no widget geometry to save yet
}

QString createDirectoryWidgets_c::derivedExtraTips_f() const
{
    return appConfig_ptr_ext->translate_f("<p>create directory widget tips</p>");
}

createDirectoryData_c createDirectoryWidgets_c::fieldsToCreateDirectoryDataObject_f() const
{
    return createDirectoryData_c(
                createDirectoryPathPTE_pri->toPlainText()
                , createParentsCheckbox_pri->isChecked()
                , errorIfExistsCheckbox_pri->isChecked()
    );
}

bool createDirectoryWidgets_c::derivedSaveNew_f(const actionData_c& actionDataBlock_par_con, actonDataHub_c* parentActonDataHub_par)
{
    bool resultTmp(false);
    createDirectoryData_c objTmp(fieldsToCreateDirectoryDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        createDirectoryAction_ptr_pri = new createDirectoryAction_c(parentActonDataHub_par, actionDataBlock_par_con, objTmp);
        actionPtr_pro = createDirectoryAction_ptr_pri;
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

bool createDirectoryWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    createDirectoryData_c objTmp(fieldsToCreateDirectoryDataObject_f());
    textCompilation_c errorsTmp;
    if (objTmp.isFieldsDataValid_f(std::addressof(errorsTmp)))
    {
        createDirectoryAction_ptr_pri->updateCreateDirectoryData_f(objTmp);
        resultTmp = true;
    }
    if (errorsTmp.size_f() > 0)
    {
        messageBoxTheErrors_f(errorsTmp, static_cast<QWidget*>(this->parent()));
    }
    return resultTmp;
}

void createDirectoryWidgets_c::browseDirectory_f()
{
    browseDirectoryToCreateDialog_pri = new QFileDialog(static_cast<QWidget*>(this->parent()));
    browseDirectoryToCreateDialog_pri->setObjectName("browseDirectoryToCreateDialog_");
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browseDirectoryToCreateDialog_pri->setFileMode(QFileDialog::Directory);
    browseDirectoryToCreateDialog_pri->setDirectory(QDir::currentPath());
    browseDirectoryToCreateDialog_pri->setWindowTitle(appConfig_ptr_ext->translate_f("Select directory..."));
    browseDirectoryToCreateDialog_pri->setViewMode(QFileDialog::Detail);
    browseDirectoryToCreateDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    browseDirectoryToCreateDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    browseDirectoryToCreateDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    selectFileDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_ptr_ext->directoryHistory_f(this->objectName() + browseDirectoryToCreateDialog_pri->objectName()));
    if (not directoryHistoryTmp.empty())
    {
        QList<QUrl> directoriesTmp;
        directoriesTmp.reserve(directoryHistoryTmp.size());
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
        browseDirectoryToCreateDialog_pri->setSidebarUrls(directoriesTmp);
    }

    QObject::connect(browseDirectoryToCreateDialog_pri, &QFileDialog::finished, this, &createDirectoryWidgets_c::fileDialogBrowseDirectoryToCreateFinished_f);

    browseDirectoryToCreateDialog_pri->show();
}

void createDirectoryWidgets_c::fileDialogBrowseDirectoryToCreateFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browseDirectoryToCreateDialog_pri->selectedFiles().isEmpty())
        {
            createDirectoryPathPTE_pri->setPlainText(browseDirectoryToCreateDialog_pri->selectedFiles().first());
            appConfig_ptr_ext->addDirectoryHistory_f(
                        this->objectName() + browseDirectoryToCreateDialog_pri->objectName()
                        , browseDirectoryToCreateDialog_pri->directory().path()
            );
        }
    }
    browseDirectoryToCreateDialog_pri->deleteLater();
    browseDirectoryToCreateDialog_pri = nullptr;
}

void createDirectoryWidgets_c::loadActionSpecificData_f()
{
    createDirectoryData_c valuesToLoadTmp(createDirectoryAction_ptr_pri not_eq nullptr ? *createDirectoryAction_ptr_pri : createDirectoryData_c());

    createDirectoryPathPTE_pri->setPlainText(valuesToLoadTmp.directoryPath_f());
    createParentsCheckbox_pri->setChecked(valuesToLoadTmp.createParents_f());
    errorIfExistsCheckbox_pri->setChecked(valuesToLoadTmp.errorIfExists_f());

}

createDirectoryWidgets_c::createDirectoryWidgets_c(
        action_c*& actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        )
    : baseClassActionTypeWidgets_c(actionData_ptr_par, variableLayout_par_con->parentWidget())
    , createDirectoryAction_ptr_pri(actionData_ptr_par == nullptr ? nullptr : static_cast<createDirectoryAction_c*>(actionData_ptr_par))
{
    this->setObjectName("createDirectoryWidgets_");

    //current working directory
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    firstRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Current working directory")));
    firstRowLayoutTmp->addSpacing(10);
    firstRowLayoutTmp->addWidget(new QLabel(QDir::currentPath()));
    firstRowLayoutTmp->addStretch(1);

    //create parents checkbox
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    createParentsCheckbox_pri = new QCheckBox("Create parents");
    createParentsCheckbox_pri->setToolTip("If checked parent directories will be created if they don't exist, otherwise if they don't exist it will result in an error");

    errorIfExistsCheckbox_pri = new QCheckBox("Error if exists");
    errorIfExistsCheckbox_pri->setToolTip("Error if exists, otherwise it won't complain if the directory already exists and the action will succeed anyway");

    secondRowLayoutTmp->addWidget(createParentsCheckbox_pri);
    secondRowLayoutTmp->addWidget(errorIfExistsCheckbox_pri);

    //directory plainTextEdit and browse directory button
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    createDirectoryPathPTE_pri = new QPlainTextEdit;
    createDirectoryPathPTE_pri->setMinimumHeight(30);
    createDirectoryPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    thirdRowLayoutTmp->addWidget(new QLabel(appConfig_ptr_ext->translate_f("Create directory path")));
    thirdRowLayoutTmp->addWidget(createDirectoryPathPTE_pri);
    QPushButton* browsePathButtonTmp = new QPushButton(appConfig_ptr_ext->translate_f("Browse"));
    thirdRowLayoutTmp->addWidget(browsePathButtonTmp);

    connect(browsePathButtonTmp, &QPushButton::clicked, this, &createDirectoryWidgets_c::browseDirectory_f);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);

    loadActionSpecificData_f();
}

