#include "commonWidgets.hpp"

#include "appConfig.hpp"

#include "textQtso/text.hpp"
#include "essentialQtgso/messageBox.hpp"

#include <QWidget>

void messageBoxTheErrors_f(const textCompilation_c& errors_par_con, QWidget* parentPtr_par)
{
    for (int i = 0, l = errors_par_con.size_f(); i < l; ++i)
    {
        QString translatedAndReplacedTextTmp(appConfig_ptr_ext->translateAndReplace_f(errors_par_con.text_f(i)));
        errorQMessageBox_f(
                    translatedAndReplacedTextTmp
                    , appConfig_ptr_ext->translate_f("Error")
                    , parentPtr_par
                    );
    }
}
