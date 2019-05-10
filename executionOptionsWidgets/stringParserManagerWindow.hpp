#ifndef ACTONQTG_STRINGPARSERMANAGERWINDOW_HPP
#define ACTONQTG_STRINGPARSERMANAGERWINDOW_HPP

#include <QWidget>

class QTableWidget;
class parserBase_c;
class QInputDialog;

class stringParserManagerWindow_c : public QWidget
{
    Q_OBJECT

    QTableWidget* stringParserConfigsTable_pri;

    QInputDialog* copyStringParserConfigIndexInputDialog_pri = nullptr;

    void closeEvent(QCloseEvent* event) override;
    void loadActonDataHubParserConfigs_f();
public:
    stringParserManagerWindow_c(QWidget* parent_par = nullptr);

private Q_SLOTS:
    //void okButtonPushed_f();
    void addUpdateStringParserConfig_f();
    void removeStringParserConfigs_f();
    void copyStringParseConfig_f();
    void updateActionRow_f(const int row_par_con);
    void updateExistingStringParserConfigRow_f(
            parserBase_c* const parserObjPtr_par
            //this is for copies
            , const int row_par_con
    );
    void insertStringParserConfigRow_f(
            parserBase_c* const parserObjPtr_par
            , const int row_par_con = -1
    );

    void showTips_f();
    void inputDialogCopyStringParserConfigIndexFinished_f(const int result_par);
};

#endif // ACTONQTG_STRINGPARSERMANAGERWINDOW_HPP
