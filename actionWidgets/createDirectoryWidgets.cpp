#include "createDirectoryWidgets.hpp"

#include "actonQtso/actions/createDirectory.hpp"

#include "appConfig.hpp"
#include "actonQtso/actionData.hpp"

#include "essentialQtgso/messageBox.hpp"

#include <QtWidgets>
//#include <QSplitter>

#include <QFileDialog>
#include <QDir>

#include <unordered_set>

void createDirectoryWidgets_c::parentClosing_f()
{
    //appConfig_f().setSplitterState_f(this->objectName(), mainSplitter_pri->saveState());
}

void createDirectoryWidgets_c::save_f()
{
    QString createDirectoryPathStr(createDirectoryPathPTE_pri->toPlainText());
    if (createDirectoryPathStr.isEmpty())
    {
        errorQMessageBox_f("Create directory path is empty", "Error", qobject_cast<QWidget*>(this->parent()));
        return;
    }

    Q_EMIT saveResult_signal(processActionDataJSON_f());
}

void createDirectoryWidgets_c::browseDirectory_f()
{
    browseDirectoryToCreateDialog_pri = new QFileDialog(qobject_cast<QWidget*>(this->parent()));
    //AcceptOpen is the default
    //browseDirectoryToCreateDialog_pri->setAcceptMode(QFileDialog::AcceptOpen);
    browseDirectoryToCreateDialog_pri->setFileMode(QFileDialog::Directory);
    browseDirectoryToCreateDialog_pri->setDirectory(QDir::currentPath());
    browseDirectoryToCreateDialog_pri->setWindowTitle(tr("Select directory..."));
    browseDirectoryToCreateDialog_pri->setViewMode(QFileDialog::Detail);
    browseDirectoryToCreateDialog_pri->setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    browseDirectoryToCreateDialog_pri->setOption(QFileDialog::DontUseNativeDialog, true);
    browseDirectoryToCreateDialog_pri->setWindowModality(Qt::WindowModal);
#ifdef __ANDROID__
    selectFileDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    std::vector<QString> directoryHistoryTmp(appConfig_f().directoryHistory_f());
    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryHistoryTmp.size());
    if (not directoryHistoryTmp.empty())
    {
        for (const QString& directory_par_con : directoryHistoryTmp)
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    browseDirectoryToCreateDialog_pri->setSidebarUrls(directoriesTmp);

    QObject::connect(browseDirectoryToCreateDialog_pri, &QFileDialog::finished, this, &createDirectoryWidgets_c::fileDialogBrowseDirectoryToCreateFinished_f);

    browseDirectoryToCreateDialog_pri->show();

    while (browseDirectoryToCreateDialog_pri->result() == QDialog::Accepted)
    {
        if (not browseDirectoryToCreateDialog_pri->selectedFiles().isEmpty())
        {
            createDirectoryPathPTE_pri->setPlainText(browseDirectoryToCreateDialog_pri->selectedFiles().first());
        }
        break;
    }
}

void createDirectoryWidgets_c::fileDialogBrowseDirectoryToCreateFinished_f(const int result_par)
{
    if (result_par == QDialog::Accepted)
    {
        if (not browseDirectoryToCreateDialog_pri->selectedFiles().isEmpty())
        {
            createDirectoryPathPTE_pri->setPlainText(browseDirectoryToCreateDialog_pri->selectedFiles().first());
        }
    }
    browseDirectoryToCreateDialog_pri->deleteLater();
    browseDirectoryToCreateDialog_pri = Q_NULLPTR;
}

void createDirectoryWidgets_c::loadActionSpecificData_f()
{
    if (actionDataId_pri > 0)
    {
        actionData_c& actionTmp(dataHub_f().actionData_f(actionDataId_pri).first);
        createDirectoryAction_c createDirectoryTmp;
        createDirectoryTmp.read_f(actionTmp.actionDataJSON_f());

        createDirectoryPathPTE_pri->setPlainText(createDirectoryTmp.directoryPath_f());
        createParentsCheckbox_pri->setChecked(createDirectoryTmp.createParents_f());
    }
}

createDirectoryWidgets_c::createDirectoryWidgets_c(
        const int_fast32_t actionDataId_par_con
        , QVBoxLayout* const variableLayout_par_con
        , QObject *parent)
    : QObject(parent), actionDataId_pri(actionDataId_par_con)
{
    this->setObjectName("createDirectoryWidgets");

    //current working directory
    QHBoxLayout* firstRowLayoutTmp = new QHBoxLayout;

    firstRowLayoutTmp->addWidget(new QLabel(tr("Current working directory")));
    firstRowLayoutTmp->addSpacing(10);
    firstRowLayoutTmp->addWidget(new QLabel(QDir::currentPath()));
    firstRowLayoutTmp->addStretch(1);

    //create parents checkbox
    QHBoxLayout* secondRowLayoutTmp = new QHBoxLayout;

    createParentsCheckbox_pri = new QCheckBox("Create parents");
    createParentsCheckbox_pri->setToolTip("If checked parent directories will be created if they don't exist, otherwise if they don't exist it will result in an error");

    secondRowLayoutTmp->addWidget(createParentsCheckbox_pri);

    //directory plainTextEdit and browse directory button
    QHBoxLayout* thirdRowLayoutTmp = new QHBoxLayout;

    createDirectoryPathPTE_pri = new QPlainTextEdit;
    createDirectoryPathPTE_pri->setMinimumHeight(30);
    createDirectoryPathPTE_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    thirdRowLayoutTmp->addWidget(new QLabel(tr("Create directory path")));
    thirdRowLayoutTmp->addWidget(createDirectoryPathPTE_pri);
    browsePathButton_pri = new QPushButton(tr("Browse"));
    thirdRowLayoutTmp->addWidget(browsePathButton_pri);

    variableLayout_par_con->addLayout(firstRowLayoutTmp);
    variableLayout_par_con->addLayout(secondRowLayoutTmp);
    variableLayout_par_con->addLayout(thirdRowLayoutTmp);

    connect(browsePathButton_pri, &QPushButton::clicked, this, &createDirectoryWidgets_c::browseDirectory_f);

    loadActionSpecificData_f();
}

QJsonObject createDirectoryWidgets_c::processActionDataJSON_f() const
{
    QJsonObject resultTmp;

    QString createDirectoryPathTmp(createDirectoryPathPTE_pri->toPlainText());

    createDirectoryAction_c createDirectoryTmp(createDirectoryPathTmp, createParentsCheckbox_pri->isChecked());
    createDirectoryTmp.write_f(resultTmp);
    return resultTmp;
}

