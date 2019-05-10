#ifndef ACTONQTG_STRINGPARSEREDITORWINDOW_HPP
#define ACTONQTG_STRINGPARSEREDITORWINDOW_HPP

#include <QWidget>

class QComboBox;
class QTableWidget;
class QPlainTextEdit;
class QVBoxLayout;
class QCheckBox;
class QSplitter;
class QMessageBox;

class baseClassStringParserWidgets_c;
class parserBase_c;

class stringParserEditorWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* variableLayout_pri;

    QSplitter* mainSplitter_pri;

    QPlainTextEdit* stringTriggerPTE_pri;

    QComboBox* parserTypeCombo_pri;

    QCheckBox* enabledCheckbox_pri;

    QPlainTextEdit* testParserResultPTE_pri;

    QMessageBox* askUpdateStringTriggerDepdenciesWindow_pri = nullptr;

    //reference to get the string parser config
    const int row_pri_con = 0;
    bool isNew_pri = true;

    //old combo index to know if it's the initial one (which shouldn't trigger a clear)
    int lastIndex_pri = -1;
    //when saving the old one must be saved somwhere because
    //just after dependencies might be updated
    QString oldStringTrigger_pri;
    parserBase_c* parserObj_pri = nullptr;

    baseClassStringParserWidgets_c* specificParserWidgets_pri = nullptr;

    void clearLayoutItems_f(QLayout* layout_par);

    void createWidgetsPerParser_f(const int index_par_con);

    void closeEvent(QCloseEvent* event) override;

    //void openAskUpdateStringTriggerDepdenciesWindow_f();
public:
    explicit stringParserEditorWindow_c(
            const int row_par_con
            , QWidget *parent_par = nullptr
    );

Q_SIGNALS:
    void closeWindow_signal();
    void updateRow_Signal(const int row_par_con);
public Q_SLOTS:
private Q_SLOTS:
    void actionComboChanged_f(int index_par);
    void saveButtonClicked_f();
    void cancelButtonClicked_f();
    //void tipsButtonClicked_f();
    void save_f();

    //void askUpdateStringTriggerDepdenciesWindowFinished_f(const int result_par);
    void testParserButtonClicked_f();
};

#endif // ACTONQTG_STRINGPARSEREDITORWINDOW_HPP
