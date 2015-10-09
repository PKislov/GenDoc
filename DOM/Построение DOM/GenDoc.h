#ifndef GENDOC_H_INCLUDED
#define GENDOC_H_INCLUDED

#include <string>
#include "domBuilder.h"

class GenDoc : public Dom
{
public:
    GenDoc() {}
    ~GenDoc() {}

    // метод генерации документа формуляр, в параметрах - имя вых. файла, формат (pdf, odt, tex и т.д.)
    // Возвращает истину, если генерация прошла успешно
    bool genForm (const std::string &fName, const std::string &format="pdf");


private:
    GenDoc (const GenDoc &g) {}
    GenDoc& operator= (const GenDoc &d) { return *this; }
};

#endif // GENDOC_H_INCLUDED
