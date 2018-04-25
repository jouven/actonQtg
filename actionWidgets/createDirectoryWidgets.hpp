#ifndef ACTONQTG_CREATEDIRECTORYWIDGETS_HPP
#define ACTONQTG_CREATEDIRECTORYWIDGETS_HPP

#include <QObject>
#include <QJsonObject>

#include <utility>

class QPlainTextEdit;
class QPushButton;
class QVBoxLayout;
class QCheckBox;
class QFileDialog;

class createDirectoryWidgets_c : public QObject
{
    Q_OBJECT

    QPlainTextEdit* createDirectoryPathPTE_pri;
    QPushButton* browsePathButton_pri;
    QCheckBox* createParentsCheckbox_pri;
    QFileDialog* browseDirectoryToCreateDialog_pri = Q_NULLPTR;

    const int_fast32_t actionDataId_pri = 0;

    void loadActionSpecificData_f();
    QJsonObject processActionDataJSON_f() const;
public:
    explicit createDirectoryWidgets_c(
            const int_fast32_t actionDataId_par_con
            , QVBoxLayout* const variableLayout_par_con
            , QObject *parent = nullptr);

Q_SIGNALS:
    void saveResult_signal(const QJsonObject& result_par_con);
public Q_SLOTS:
    void parentClosing_f();
    void save_f();
private Q_SLOTS:
    void browseDirectory_f();
    void fileDialogBrowseDirectoryToCreateFinished_f(const int result_par);
};

#endif // ACTONQTG_CREATEDIRECTORYWIDGETS_HPP
