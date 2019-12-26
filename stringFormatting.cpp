#include "stringFormatting.hpp"

QString truncateString_f(
        const QString& string_par_con
        , const uint_fast64_t truncateLength_par_con
        , const QChar fillerChar_par_con
        , const uint_fast64_t fillerLength_par_con)
{
    QString resultTmp(string_par_con);
    if (string_par_con.size() > truncateLength_par_con)
    {
        resultTmp.truncate(truncateLength_par_con);
        if (fillerLength_par_con > 0)
        {
            resultTmp.append(QString(fillerLength_par_con, fillerChar_par_con));
        }
    }
    return resultTmp;
}
