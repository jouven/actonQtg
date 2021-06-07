#ifndef ACTONQTG_FILELISTWINDOW_HPP
#define ACTONQTG_FILELISTWINDOW_HPP

#include <QWidget>

class QVBoxLayout;
class QTableWidget;
#ifdef __ANDROID__
class QScrollArea;
#endif

class fileListWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif
    //this might need an optimization (QTableView + model)
    QTableWidget* fileListTable_pri;

    void closeEvent(QCloseEvent* event) override;

    void addFileEntry_f(
            const QString& filename_par_con
            , const QString& datetime_par_con
            , const uint_fast64_t size_par_con
            );
    void loadFileVectorToTableGrid_f(const std::vector<QString>& fileVector_par_con);
public:
    explicit fileListWindow_c(
            const std::vector<QString>& fileVector_par_con
            , QWidget* parent_par = nullptr
    );

Q_SIGNALS:
public Q_SLOTS:
private Q_SLOTS:
    void cancelButtonClicked_f();

    //void tipsButtonClicked_f();
};

#endif // ACTONQTG_FILELISTWINDOW_HPP
