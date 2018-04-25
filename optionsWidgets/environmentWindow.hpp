#ifndef ACTONQTG_ENVIRONMENTWINDOW_HPP
#define ACTONQTG_ENVIRONMENTWINDOW_HPP

#include <QWidget>

class environmentWindow_c : public QWidget
{
    Q_OBJECT

    void closeEvent(QCloseEvent* event) override;
public:
    environmentWindow_c(QWidget* parent_par = nullptr);

private Q_SLOTS:
    void okButtonPushed_f();

};

#endif // ACTONQTG_ENVIRONMENTWINDOW_HPP
