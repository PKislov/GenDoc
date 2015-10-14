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
    bool genForm (const std::string &fName, const std::string &format="pdf") const;


private:
    GenDoc (const GenDoc &g) {}
    GenDoc& operator= (const GenDoc &d) { return *this; }

    // формирует согласно DOM содержимое документа в строке buf, содержащей шаблон
    void makeBuff (const decltype(root) p, std::string &buf) const;
    // заполняет в шаблоне (в строке buf) заголовок документа (title, в строке t)
    void writeTitle (const std::string &t, std::string &buf) const;
    void writeSection1 (const std::string &t, std::string &buf) const;
    void writeSection2 (const std::string &t, std::string &buf) const;
    void writeSection3 (const std::string &t, std::string &buf) const;
    void writeSection4 (const std::string &t, std::string &buf) const;
    // метод возвращает индекс перед выражением "\end{document}" в строке buf, используется для определения места вставки текста
    unsigned whereInsertText (const std::string &buf) const;
    // записывает текст из t в buf по индексу, вычисленному методом whereInsertText
    inline void writeText (const std::string &t, std::string &buf) const;
    void writeImage (const decltype(root) p, std::string &buf) const;
    // делает то же свмое, что headerToLaTex, только еще дублирует переходы на новую строку
    const std::string& strToLaTex (std::string &s) const;
    // ищет в строке s служебные символы языка LaTex и заменяет их командами LaTex, позволяющими печатать эти символы, например все найденные символы # заменит на \# (применяется в методе writeTitle, writeSection и др.)
    const std::string& headerToLaTex (std::string &s) const;
    // записывает текст из t в buf по индексу, вычисленному методом whereInsertText, преобразует текст методом strToLaTex
    inline void writeTextLatex (const std::string &t, std::string &buf) const;

};

#endif // GENDOC_H_INCLUDED
