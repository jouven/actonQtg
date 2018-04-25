#ifndef ACTONQTG_WORKINGDIRECTORYWINDOW_HPP
#define ACTONQTG_WORKINGDIRECTORYWINDOW_HPP

#include <QWidget>

class workingDirectoryWindow_c : public QWidget
{
    Q_OBJECT

    void closeEvent(QCloseEvent* event) override;
public:
    workingDirectoryWindow_c(QWidget* parent_par = nullptr);

private Q_SLOTS:
    void okButtonPushed_f();

};

#endif // ACTONQTG_WORKINGDIRECTORYWINDOW_HPP
