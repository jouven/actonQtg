#ifndef ACTONQTG_STRINGFORMATTING_HPP
#define ACTONQTG_STRINGFORMATTING_HPP

#include <QString>

QString truncateString_f(const QString& string_par_con
                         , const uint_fast64_t truncateLength_par_con
                         //filler to append after, IF the string is truncated
                         //and fillerLength_par_con > 0
                         , const QChar fillerChar_par_con = '.'
                         , const uint_fast64_t fillerLength_par_con = 3);


#endif // ACTONQTG_STRINGFORMATTING_HPP


