#ifndef GENDOC_H_INCLUDED
#define GENDOC_H_INCLUDED

#include <string>
#include <vector>
#include "domBuilder.h"

class GenDoc : public Dom
{
public:
    GenDoc() {}
    ~GenDoc() {}

    // метод генерации документа формуляр, в параметрах - имя вых. файла без расширения, флаг findErrSyntax - запускать ли метод поиска синтаксических ошибок в дереве, формат вых. данных (pdf, odt, tex и т.д.).
    // Возвращает истину, если генерация прошла успешно
    bool genForm (const std::string &fName, const bool findErrSyntax = true, const std::string &format = "pdf");


private:
    GenDoc (const GenDoc &g) {}
    GenDoc& operator = (const GenDoc &d) { return *this; }

    // формирует согласно DOM содержимое документа в строке buf, куда предварительно записали содержимое шаблона
    void makeBuff (const decltype(root) p, std::string &buf);
    // заполняет в шаблоне (в строке buf) заголовок документа (команда title, содержится в строке t)
    void writeTitle (const std::string &t, std::string &buf) const;
    // заполняет в шаблоне (в строке buf) ссылку - команда &... {id:"..."}
    void writeId (const decltype(root) p, std::string &buf) const;
    // заполняет заголовки, t - значение заголовка, id - ссылка, флаг f - нужно ли вставить код описания ссылки (\label {...})
    void writeSection1 (const std::string &t, std::string &buf, const std::string &id = "", const bool f = false) const;
    void writeSection2 (const std::string &t, std::string &buf, const std::string &id = "", const bool f = false) const;
    void writeSection3 (const std::string &t, std::string &buf, const std::string &id = "", const bool f = false) const;
    void writeSection4 (const std::string &t, std::string &buf, const std::string &id = "", const bool f = false) const;
    // метод возвращает индекс перед выражением "\end{document}" в строке buf, используется для определения места вставки текста
    std::string::size_type whereInsertText (const std::string &buf) const;
    // записывает текст из t в buf по индексу, вычисленному методом whereInsertText
    inline void writeText (const std::string &t, std::string &buf) const;
    // вставка рисунка
    void writeImage (const decltype(root) p, std::string &buf) const;
    // создать ссылку на страницу - команда @pageid	{id:" ... "}
    void writePageId(const decltype(root) p, std::string &buf);
    // вставить код (LaTeX или другой) - команда @code {}
    void writeCode(const decltype(root) p, std::string &buf);
    // вставить перечисление (команда @enumerate)
    void writeEnum (const decltype(root) p, std::string &buf);
     // вставить таблицу (команда @table)
    void writeTable (const decltype(root) p, std::string &buf);
    // записывает в строку s начало описания перечисления (или таблицы) в виде @enumerate {} ...
    // метод используется в writeEnum для вывода на экран ошибок в перечислении
    std::string& infoEnumInStr (const decltype(root) p, std::string &s) const;
    // делает то же самое, что headerToLaTex, только еще дублирует переходы на новую строку
    const std::string& strToLaTex (std::string &s) const;
    // ищет в строке s служебные символы языка LaTex и заменяет их командами LaTex, позволяющими печатать эти символы, например все найденные символы # заменит на \# (применяется в методе writeTitle, writeSection и др.)
    const std::string& headerToLaTex (std::string &s) const;
    // записывает текст из t в buf по индексу, вычисленному методом whereInsertText, преобразует текст методом strToLaTex
    inline void writeTextLatex (const std::string &t, std::string &buf) const;


    // Поиск в дереве предполагаемых синтаксических ошибок, возвращает true если ошибки найдены.
    // Способ поиска ошибок: метод рекурсивно проходит DOM, в элементах DOM с типом text ищет вхождения
    // команд из vcommands, ищет индекс окончания команды, открывает файл "list.tmp", заполняет список
    // файлов в vInclFiles, затем в vInclFiles заносит содержимое файлов, проходит список файлов и находит
    // в них вхождения вычисленной команды, подсчитывает номер строки в файле с командой, заносит
    // инф-цию о команде в vErrMess, проверив, нет ли в vErrMess уже такой инф-ции.
    // Затем метод showErrMessage() выводит список ошибок и векторы vInclFiles и vErrMess усекаются.
    void findSyntaxErr(const decltype(root) p);
    // имя файла, куда записаны имена всех подключаемых файлов (файл формируется модулем include)
    const char * const listFileName = "list.tmp";
    FILE *flist;
    // флаг существования файла "list.tmp"
    bool bflist = false;
    // список команд, по которым будут искаться синтакс. ошибки
    std::vector<std::string> vcommands = {"@title", "@section", "@image", "@figure", "@include", "&section", "&image", "&figure", "@pageid", "&pageid", "@code", "@enumerate", "@table", "&table"};
    // структура подключаемого файла
    struct InclFiles
    {
        std::string name; // имя файла
        std::string text; // его содержимое
    };
    // список включаемых файлов - заполняется из файла "list.tmp"
    std::vector<struct InclFiles> vInclFiles;
    // структура сообщения об ошибке
    struct errMessage
    {
        std::string commande; // текст некорректной команды
        std::string fname; // имя подключаемого файла, где находится
        std::string title; // под каким заголовком находится
        decltype (vcommands.size()) line; // номер строки в файле
    };
    // список синтакс. ошибок, вектор нужен для поиска и удаления повторов ошибок
    std::vector<struct errMessage> vErrMess;
    // отображение на экране списка синтакс. ошибок из вектора vErrMess
    void showErrMessage();
};

#endif // GENDOC_H_INCLUDED
