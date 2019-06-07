#include "createDirectoryWidgets.hpp"

#include "../appConfig.hpp"

#include "actonQtso/actions/createDirectory.hpp"

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

bool createDirectoryWidgets_c::isFieldsDataValid_f() const
{
    bool resultTmp(false);
    while (true)
    {
        QString createDirectoryPathStr(createDirectoryPathPTE_pri->toPlainText());
        if (createDirectoryPathStr.length() > 255)
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Directory path is too long (>255): ") + QString::number(createDirectoryPathStr.length())
                               , appConfig_ptr_ext->translate_f("Error")
                               , static_cast<QWidget*>(this->parent()));
            break;
        }
        if (createDirectoryPathStr.isEmpty())
        {
            errorQMessageBox_f(appConfig_ptr_ext->translate_f("Directory path is empty")
                               , appConfig_ptr_ext->translate_f("Error")
                               , static_cast<QWidget*>(this->parent()));
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

createDirectoryData_c createDirectoryWidgets_c::fieldsToCreateDirectoryDataObject_f() const
{
    return createDirectoryData_c(
                createDirectoryPathPTE_pri->toPlainText()
                , createParentsCheckbox_pri->isChecked()
                , errorIfExistsCheckbox_pri->isChecked()
    );
}

bool createDirectoryWidgets_c::derivedSaveNew_f(const actionData_c& actionDataBlock_par_con)
{
    bool resultTmp(false);
    if (isFieldsDataValid_f())
    {
        createDirectoryAction_ptr_pri = new createDirectoryAction_c(actionDataBlock_par_con, fieldsToCreateDirectoryDataObject_f());

        actionPtr_pro = createDirectoryAction_ptr_pri;
        resultTmp = true;
    }
    return resultTmp;
}

bool createDirectoryWidgets_c::derivedSaveUpdate_f()
{
    bool resultTmp(false);
    if (isFieldsDataValid_f())
    {
        createDirectoryAction_ptr_pri->createDirectoryData_c::operator=(fieldsToCreateDirectoryDataObject_f());
        resultTmp = true;
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
            appConfig_ptr_ext->addDirectoryHistory_f(this->objectName() + browseDirectoryToCreateDialog_pri->objectName(), browseDirectoryToCreateDialog_pri->directory().path());
        }
    }
    browseDirectoryToCreateDialog_pri->deleteLater();
    browseDirectoryToCreateDialog_pri = nullptr;
}

void createDirectoryWidgets_c::loadActionSpecificData_f()
{
    if (createDirectoryAction_ptr_pri not_eq nullptr)
    {
        createDirectoryPathPTE_pri->setPlainText(createDirectoryAction_ptr_pri->directoryPath_f());
        createParentsCheckbox_pri->setChecked(createDirectoryAction_ptr_pri->createParents_f());
        errorIfExistsCheckbox_pri->setChecked(createDirectoryAction_ptr_pri->errorIfExists_f());
    }
}

createDirectoryWidgets_c::createDirectoryWidgets_c(
        action_c*& actionData_ptr_par
        , QVBoxLayout* const variableLayout_par_con
        )
    : baseClassActionWidgets_c(actionData_ptr_par, variableLayout_par_con->parentWidget())
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

