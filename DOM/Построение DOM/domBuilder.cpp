#include <cstdio>
#include <cstring>
#include <cctype>
#include <vector>
//#include <fstream>
//#include <streambuf>
//#include <ios>
//#include <iostream>
//#include <memory>
#include "domBuilder.h"

Dom::Dom()
{
	root = new struct node;
    root->parent = NULL;
	root->id = document;
	temp = root;
}
Dom::~Dom()
{
    delElem(root); // удалить всю часть дерева после узла root
	delete root;
}
void Dom::delElem(struct node *p) // удаление из памяти элементов дерева после узла p
{
	if (!p || !p->children.size()) return;
	for (decltype(p->children.size()) i=0; i < p->children.size(); ++i)
		if (p->children[i]->children.size())
            delElem (p->children[i]);
	for (decltype(p->children.size()) i=0; i < p->children.size(); ++i)
	{
		//puts(p->children[i]->id.c_str());
		delete p->children[i];
		p->children[i] = NULL;
	}
	p->children.clear();
}
// добавить потомка узлу p и возвратить потомка
struct Dom::node* Dom::addChild(struct node *p)
{
    if (!p) return NULL;
    p->children.push_back(new struct node);
    p->children.back()->parent = p;
    return p->children.back();
}

// возвращает true, если узел р - заголовок
bool Dom::isSection (const struct node * p) const
{
    return p->id == section1 || p->id == section2 || p->id == section3 || p->id == section4;
}

bool Dom::isSectionBegin (const struct node * p) const
{
    return p->id == section1Begin || p->id == section2Begin || p->id == section3Begin || p->id == section4Begin;
}

void Dom::addTitleBegin(const char *s) // начало команды @title
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    static bool f = false;
    if (f)
    {
        puts ("Команда \"@title\" указана более одного раза!\n");
        exit(1);
    }
    f = true;
    temp = addChild(root);
    temp->id = titleBegin;
    // под название документа, пока для поисковика ошибок записать текст команды в оригинальном регистре
    temp->value.push_back(s);
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value.back());
    temp = temp->parent;
}
void Dom::addTitle(const char *s) // окончание команды - @end title
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    static bool f = false;
    if (f)
    {
        puts ("Команда \"@end title\" указана более одного раза!\n");
        exit(1);
    }
    f = true;
    // найти в дереве начало команды @title
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
    {
        if (root->children[i]->id == titleBegin) // нашли начало команды
        {
            root->children[i]->id = title;
            root->children[i]->value.back() = "";
            // если пустой заголовок
            if(i+1 == root->children.size())
                return;

            // копирование названия документа в узел root->children[i], удаление текстовых узлов после начала команды title
            for (decltype(root->children.size()) i1=i+1; i1 < root->children.size(); ++i1)
            {

                if(root->children[i1]->id != text)
                {
                    puts ("В теле команды \"@title ... @end title\" можно указывать только текст!\n");
                    exit(1);
                }
                root->children[i]->value[0] += root->children[i1]->value[0];
                delete root->children[i1];
                root->children[i1] = NULL;
                root->children.erase(root->children.begin()+i1);
                i1 = i;
            }
            if (root->children[i]->value.back().find("\n\n", 0) != std::string::npos)
            {
                puts("В команде описания команды \"@title ... @end title\" нельзя использовать двойные переходы на новую строку!");
                exit(1);
            }
            return;
        }
    }
    puts ("Отсутствует начало команды \"@title ... @end title\"!\n");
    exit(1);
}

// начало команды @code {}, type - тип кода (LaTeX или др.)
void Dom::addCodeBegin(const char *s, const char *type)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(temp);
    temp->id = codeBegin;
    // для поисковика ошибок записать текст команды в оригинальном регистре
    temp->value.push_back(s);
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value.back());
    temp->value.push_back(type); // записать тип кода (LaTeX или другой)
    temp = temp->parent;
}

void Dom::addCode(const char *s, const char *type) // окончание команды - @end code {}
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    decltype(temp) pfind = NULL; // для сообщения об ошибке

    // найти в дереве начало команды @code {}
    for (decltype(temp->children.size()) i = temp->children.size()-1; i > 0; --i)
    {
        // нашли начало команды
        if (temp->children[i]->id == codeBegin)
        {
            pfind = temp->children[i];
            // если начало команды подходит по типу кода
            if (temp->children[i]->value[1] == type)
            {
                temp->children[i]->id = code;
                temp->children[i]->value[0].clear(); // текст кода
                // если пустой текст блока кода
                if(i-1 == -1)
                    return;

                // копирование текст кода в узел temp->children[i], удаление текстовых узлов после начала команды code
                for (decltype(temp->children.size()) i1=i+1; i1 < temp->children.size(); ++i1)
                {

                    if(temp->children[i1]->id != text)
                    {
                        printf ("В теле команды \"@code{%s} ... @end code{%s}\" можно указывать только текст!\n", type, type);
                        exit(1);
                    }
                    temp->children[i]->value[0] += temp->children[i1]->value[0];
                    delete temp->children[i1];
                    temp->children[i1] = NULL;
                    temp->children.erase(temp->children.begin()+i1);
                    i1 = i;
                }
                return;
            }
        }
    }
    if (!pfind)
        printf ("Отсутствует начало команды \"@code{%s} ... @end code{%s}\"!\n", type, type);
    else
        printf ("Начало и конец команды \"@code{%s} ... @end code{%s}\" не соответствуют по типу!\n", pfind->value[1].c_str(), type);
    exit(1);
}

// начало команды @table {}, type - тип кода (на языке LaTeX "latex" или "tag" - описана тэгами)
// n1 и n2 - порядок следования параметров в команде @table {text:"...";id:"..."}
void Dom::addTableBegin(const char *s, const char *type, int n1, int n2)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(temp);
    temp->id = tableBegin;
    // для поисковика ошибок записать текст команды в оригинальном регистре
    temp->value.push_back(s); // [0]
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value.back());
    temp->value.push_back("type"); // [1]
    temp->value.push_back(type); // записать тип кода (LaTeX или на тэгах) [2]
    temp->value.push_back(""); // под код таблицы на языке LaTeX или на тэгах [3]

    if (std::string("tag") == type)
    {
        temp->value[2] = type; // "tag"
        temp->value.push_back(""); // ref [4]
        temp->value.push_back(""); // значение ref [5]
        temp->value.push_back(""); // text [6]
        temp->value.push_back(""); // значение text [7]
        temp->value.push_back(""); // id [8]
        temp->value.push_back(""); // значение id [9]

        if (n1) // если команда @table {text: " ... "}
        {
            auto refBeg = s; // должен указывать на первый символ параметра
            temp->value[6] = "text"; // задать подпись
            for (int i=0; i < n1*2-1; ++i)
            {
                while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до начала text
            }
            while(*refBeg != '\"') temp->value[7].push_back(*refBeg), ++refBeg; ++refBeg; // запись подписи
            SeqSymbContrReplace(temp->value[7]);
            if (temp->value[7].find("\n\n", 0) != std::string::npos)
            {
                printf("В команде описания таблицы \"%s\" нельзя использовать двойные переходы на новую строку!", temp->value[0].c_str());
                exit(1);
            }

            if (n2) // если команда @table {text: " ... "; id: " ... "}
            {
                refBeg = s;
                temp->value[8] = "id";
                for (int i=0; i < n2*2-1; ++i)
                {
                    while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до начала id
                }
                while(*refBeg != '\"') temp->value[9].push_back(*refBeg), ++refBeg; ++refBeg; // запись id
                SeqSymbContrReplace(temp->value[9]);
                if (temp->value[9].find("\n\n", 0) != std::string::npos)
                {
                    printf("В команде описания таблицы \"%s\" нельзя использовать двойные переходы на новую строку!", temp->value[0].c_str());
                    exit(1);
                }
                if (!temp->value[9].size())
                {
                    printf("В команде описания таблицы \"%s\" нельзя использовать пустое значение ссылки!\n", temp->value[0].c_str());
                    exit(1);
                }
                // если ссылка уже объявлена
                showDuplicateIdInDom(temp);
            }
        }
    }
    temp = temp->parent;
}
// команда @end table
void Dom::addTableEnd(const char *s)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    decltype(temp) pfind = NULL; // для сообщения об ошибке

    // найти в дереве начало команды @table {}
    for (decltype(temp->children.size()) i = temp->children.size()-1; i > 0; --i)
    {
        // нашли начало команды
        if (temp->children[i]->id == tableBegin)
        {
            pfind = temp->children[i];
            temp->children[i]->id = table;
            // temp->children[i]->value.push_back(""); // под код таблицы на языке LaTeX или на тэгах [3]
                // если пустой текст блока кода
                if(i-1 == -1)
                    return;

                // копирование текст кода в узел temp->children[i], удаление текстовых узлов после начала команды table
                for (decltype(temp->children.size()) i1=i+1; i1 < temp->children.size(); ++i1)
                {

                    if(temp->children[i1]->id != text)
                    {
                        std::string stemp(pfind->value[0]); // строка для отображения ошибки
                        printf ("В теле команды \"%s ... @end table\" можно указывать только текст!\n", delSymbsInEndStr(stemp).c_str());
                        exit(1);
                    }
                    temp->children[i]->value[3] += temp->children[i1]->value[0];
                    delete temp->children[i1];
                    temp->children[i1] = NULL;
                    temp->children.erase(temp->children.begin()+i1);
                    i1 = i;
                }
                return;
        }
    }
    if (!pfind)
        printf ("Отсутствует начало команды \"@table {} ... @end table\"!\n");
    exit(1);
}


// команды вида @code {ref:" ... "; latex}
void Dom::addCodeRef(const char *s, const char *type)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    std::string fname;
    const char * const sbegin = s; // переменная для поиска ошибок
    temp = addChild(temp);
    temp->id = code;
    temp->value.push_back(""); // под текст кода
    temp->value.push_back(type); // под тип кода
    while(*s != '\"') ++s; ++s; // дойти до начала ref
    while(*s != '\"') fname.push_back(*s), ++s; // записать ref
    // замена в строке последовательности символов "\\a" на \"
    SeqSymbContrReplace(fname, false);

    FILE *fp;
// проверка на существование имени файла исходного кода
    if(!(fp = fopen(fname.c_str(), "rt" )))
	{
		printf ("Не удалось найти файл исходного кода \"%s\", указанный в команде \"%s\"\n", fname.c_str(), sbegin);
		exit(1);
	}

	char ch;
    while(!feof(fp)) // копирование содержимого файла в вектор value узла
    {
        ch=getc(fp);
        if (!feof(fp)) temp->value[0].push_back(ch);
    }

 	fclose(fp);
    if (temp->value[0].size() && temp->value[0].back() == 10)
        temp->value[0].pop_back(); // удаляем символ 10
    temp = temp->parent;
}

void Dom::addSection1Begin(const char *s) // заголовок уровня 1
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(root);
    temp->id = section1Begin;
    temp->value.push_back("text"); // параметр
    temp->value.push_back(""); // название заголовка
    temp->value.push_back(""); // без параметра ссылки
    temp->value.push_back(""); // значение ссылки
    // заголовок занесется в методе addText
}

void Dom::addSection2Begin(const char *s) // заголовок уровня 2
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    decltype(temp) plast = NULL; // проверка
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
        if (root->children[i]->id == section1 || root->children[i]->id == section1Begin)
            plast = root->children[i];
    if (!plast)
    {
        printf ("Заголовок \"%s\" опреден как \"section2\", не может быть объявлен раньше, чем \"section1\".\n", s);
        exit(1);
    }

    temp = addChild(plast);
    temp->id = section2Begin;
    temp->value.push_back("text"); // параметр
    temp->value.push_back(""); // название заголовка
    temp->value.push_back(""); // без параметра ссылки
    temp->value.push_back(""); // значение ссылки
}

void Dom::addSection3Begin(const char *s) // заголовок уровня 3
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    decltype(temp) plast = NULL; // проверка
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
        if (root->children[i]->id == section1 || root->children[i]->id == section1Begin)
            for (decltype(i) j=0; j < root->children[i]->children.size(); ++j)
            {
                if (root->children[i]->children[j]->id == section2 || root->children[i]->children[j]->id == section2Begin)
                    plast = root->children[i]->children[j];
            }

    if (!plast)
    {
        printf ("Заголовок \"%s\" опреден как \"section3\", не может быть объявлен раньше, чем заголовки более высокого уровня.\n", s);
        exit(1);
    }

    temp = addChild(plast);
    temp->id = section3Begin;
    temp->value.push_back("text"); // параметр
    temp->value.push_back(""); // название заголовка
    temp->value.push_back(""); // без параметра ссылки
    temp->value.push_back(""); // значение ссылки
}

void Dom::addSection4Begin(const char *s) // заголовок уровня 4
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    decltype(temp) plast = NULL; // проверка
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
        if (root->children[i]->id == section1 || root->children[i]->id == section1Begin)
            for (decltype(i) j=0; j < root->children[i]->children.size(); ++j)
                if (root->children[i]->children[j]->id == section2 || root->children[i]->children[j]->id == section2Begin)
                    for (decltype(i) jj=0; jj < root->children[i]->children[j]->children.size(); ++jj)
                        if (root->children[i]->children[j]->children[jj]->id == section3 || root->children[i]->children[j]->children[jj]->id == section3Begin)
                            plast = root->children[i]->children[j]->children[jj];

    if (!plast)
    {
        printf ("Заголовок \"%s\" опреден как \"section4\", не может быть объявлен раньше, чем заголовки более высокого уровня.\n", s);
        exit(1);
    }

    temp = addChild(plast);
    temp->id = section4Begin;
    temp->value.push_back("text"); // параметр
    temp->value.push_back(""); // название заголовка
    temp->value.push_back(""); // без параметра ссылки
    temp->value.push_back(""); // значение ссылки
}

// заголовок уровня sec (команда вида @section1{text:"..."}
void Dom::addSection_1Param(const char *s, const std::string &sec, decltype(root->children.size()) n)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    const char * const sbegin = s; // переменная для поиска ошибок

    // массив указателей на методы addSectionBegin
    void (Dom::*pfaddSecBeg[])(const char*) = {&Dom::addSection1Begin, &Dom::addSection2Begin, &Dom::addSection3Begin, &Dom::addSection4Begin};
    // вызов нужного метода, индекс в массиве указателей берется из строки sec и преобразуется из char в int (ASCII код '1' в 1)
    (this->*(pfaddSecBeg[sec[section1.size()-1]-'0'-1]))(s);
    temp->id = sec; // указать уровень заголовка
    while(*s != '\"') ++s; ++s; // дойти до начала text
    while(*s != '\"') temp->value[n].push_back(*s), ++s; ++s; // записать заголовок
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value[n]);
    // если заголовок содержит двойные переходы на новую строку, то TexLive это
    // примет за ошибку
    if (temp->value[n].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания заголовка \"%s\" нельзя использовать двойные переходы на новую строку!\n", sbegin);
        exit(1);
    }
}

// команда вида @section{text:"..."; id:"..."} (n=3) или @section1{id:"..."; text:"..."} (n=1) - вид определяется параметром n
void Dom::addSection_2Param(const char *s, const std::string &sec, decltype(root->children.size()) n)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    const char * const sbegin = s; // переменная для поиска ошибок
    // text ... id ... // в value[n] записать ссылку, n=3 или =1
    if (n == 3)
        addSection_1Param(s, sec);
    else // n == 1
        addSection_1Param(s, sec, 3);

    while(*s != '\"') ++s; ++s; // дойти до начала text
    while(*s != '\"') ++s; ++s; // дойти до конца text
    while(*s != '\"') ++s; ++s; // дойти до начала id
    while(*s != '\"') temp->value[n].push_back(*s), ++s; ++s; // записать заголовок
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value[n]);
    // если заголовок содержит двойные переходы на новую строку, то TexLive это
    // примет за ошибку
    if (temp->value[n].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания заголовка \"%s\" нельзя использовать двойные переходы на новую строку!\n", sbegin);
        exit(1);
    }
    temp->value[2] = idRef;
    if (!temp->value[3].size())
    {
        printf("В команде описания заголовка \"%s\" нельзя использовать пустое значение ссылки!\n", sbegin);
        exit(1);
    }

    temp->value.push_back("yylex"); // параметр yylex
    temp->value.push_back(sbegin); // значение yylex - строка команды от лексера (нужна для поиска повторов ссылок)
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back(), false);

    // ищет в DOM ссылку, содержащуюся в узле р (метод findIdInDom) и выводит сообщение об ошибке,
    // если ссылка уже объявлена
    showDuplicateIdInDom(temp);
}

void Dom::addToc(const char *s) // Содержание
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(root);
	temp->id = toc;
	temp = temp->parent;
}


// команда вида @image{text:" ... "; ref:" ... "; id:" ... "}
void Dom::addImageRef(const char *s, decltype(root->children.size()) n1, decltype(n1) n2, decltype(n1) n3)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    const char * const sbegin = s; // переменная для поиска ошибок
    addImageRef(s, n1, n2);
    temp = temp->children.back(); // вернуться к текущему узлу
    temp->value[4] = idRef;
    temp->value.push_back("yylex"); // параметр yylex
    temp->value.push_back(sbegin); // значение yylex - строка команды от лексера (нужна для поиска повторов ссылок)
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back(), false);

    while(*s != '\"') ++s; ++s; // пропустить первый параметр
    while(*s != '\"') ++s; ++s;
    while(*s != '\"') ++s; ++s;
    while(*s != '\"') ++s; ++s;
    while(*s != '\"') ++s; ++s; // дойти до начала третьего параметра
    while(*s != '\"') temp->value[n3].push_back(*s), ++s; // записать параметр
    SeqSymbContrReplace(temp->value[n3]);
    if (temp->value[n3].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания рисунка \"%s\" нельзя использовать двойные переходы на новую строку!", sbegin);
        exit(1);
    }

        if (!temp->value[5].size())
        {
            printf("В команде описания рисунка \"%s\" нельзя использовать пустое значение ссылки!\n", sbegin);
            exit(1);
        }

    // ищет в DOM ссылку, содержащуюся в узле р (метод findIdInDom) и выводит сообщение об ошибке,
    // если ссылка уже объявлена
    showDuplicateIdInDom(temp);
    temp = temp->parent;
}

// команда вида @image{text:" ... "; ref:" ... "}
// n - индекс элемента массива value
void Dom::addImageRef(const char *s,  decltype(root->children.size()) n1, decltype(n1) n2)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    const char * const sbegin = s; // переменная для поиска ошибок
    addImageRef(s, n1);
    temp = temp->children.back(); // вернуться к текущему узлу
    temp->value[0] = text;
    while(*s != '\"') ++s; ++s; // пропустить первый параметр
    while(*s != '\"') ++s; ++s;
    while(*s != '\"') ++s; ++s; // дойти до начала второго параметра
    while(*s != '\"') temp->value[n2].push_back(*s), ++s; // записать параметр
    SeqSymbContrReplace(temp->value[n2]);
    if (temp->value[n2].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания рисунка \"%s\" нельзя использовать двойные переходы на новую строку!", sbegin);
        exit(1);
    }
    temp = temp->parent;
}

// команда вида @image{ref:" ... "}
void Dom::addImageRef(const char *s,  decltype(root->children.size()) n1) // рисунок с ref
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    const char * const sbegin = s; // переменная для поиска ошибок
    temp = addChild(temp);
    temp->id = image;
    temp->value.push_back("");
    temp->value.push_back(""); // подпись рисунка
    temp->value.push_back("ref");
    temp->value.push_back(""); // значение ref
    temp->value.push_back(""); // параметр id
    temp->value.push_back(""); // значение id
    while(*s != '\"') ++s; ++s; // дойти до начала ref
    while(*s != '\"') temp->value[n1].push_back(*s), ++s; ++s; // записать ref
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value[n1]);
    if (temp->value[n1].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания рисунка \"%s\" нельзя использовать двойные переходы на новую строку!", sbegin);
        exit(1);
    }
	temp = temp->parent;
}

void Dom::addText(FILE *f, const char *name) // Текст
{
    // Считать из файла текст и добавить в дерево/
    // В файл f перенаправлен поток символов между командами.
    // Перед обработкой любой команды текст нужно записать
    // в дерево и обнулить файл.

    fclose(f);

    if (!(f = fopen(name, "rt")) || feof (f))
    {
        printf ("Не удалось открыть временный файл \"%s\"\n", name);
		exit (1);
    }
    if (temp && temp->children.size() &&\
        temp->children.back()->id == text) // узлы "text" идущие друг за другом объединять
        {
            temp = temp->children.back();
        }
        else
        {
            temp = addChild(temp);
            temp->id  = text;
            temp->value.push_back("");
        }

    char ch;
    while(!(feof(f)))
    {
        ch=fgetc(f);
        if (!feof(f)) temp->value.back().push_back(ch);
    }

    fclose(f);

/*std::ifstream is(name);
if (!is.is_open())
    {
        printf ("Не удалось открыть временный файл \"%s\"\n", name);
		exit (1);
    }
    //здесь получим размер файла
  is.seekg (0, std::ios::end);
  unsigned long long len = is.tellg();
  is.seekg (0, std::ios::beg);
char * buffer = new char [len+1];
  is.read (buffer,len);
  buffer[len-1] = '\0';

is.close();
temp->value.back() = buffer;
delete[] buffer;*/


    remove(name);
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back(), false);
    if (!(f = fopen(name, "wt")))
    {
        printf ("Не удалось открыть временный файл \"%s\"\n", name);
		exit (1);
    }
    // если родитель узла - начало команды section, в значение родителя
    // записать в значение родителя  текст из текущего узла до символа новой строки,
    // удалить это текст из текста тек. узла и поменять идентификатор родителя sectionBegin
    // на section
    if(isSectionBegin(temp->parent))
    {
        decltype(temp->value.back().size()) i=0;
        // поиск индекса окончания заголовка - до символа новой строки
        while ((i < temp->value.back().size()) && (temp->value.back()[i] != '\n')) ++i;
        // запись текста заголовка
        temp->parent->value[1].insert(0, temp->value.back(), 0 , i);
        temp->value.back().erase(0, i); // удаление текста заголовка из текста тек. узла
        // меняем id заголовка с sectionBegin на section
        sectionBegToSection(temp->parent->id);
    }
    temp = temp->parent;
}

// меняет строку id с sectionBegin на section (используется в построении узлов заголовков)
const std::string& Dom::sectionBegToSection (std::string &id) const
{
    id.erase(section1.size(), section1Begin.size() - section1.size());
    return id;
}

// ищет в дереве узел содержащий ссылку id и возвращает его, или NULL если узел не найден (поиск повторов ссылок)
const struct Dom::node* Dom::findIdInDom (const struct node *p, const std::string &id) const
{
    if (!p || !p->children.size()) return NULL; // если пустрое дерево

    for (decltype(p->children.size()) i=0; i < p->children.size(); ++i) // цикл прохода дерева
    {
        // если узел - заголовок или рисунок и содержит ссылку id
        auto indexId = getIndexId(p->children[i]);
        if (indexId && (p->children[i]->value[indexId] == id))
            return p->children[i];

        if (p->children[i]->children.size())
        {
            const struct node* result = findIdInDom (p->children[i], id);
            if (result) return result; // ссылка найдена
        }
    }
    return NULL;
}

// ищет в DOM ссылку, содержащуюся в узле р (метод findIdInDom)
// и выводит сообщение о результате. Возвращает true если ссылка найдена.
bool Dom::showDuplicateIdInDom (const struct node *p) const
{
    auto indexIdp = getIndexId(p); // индекс ссылки в векторе value узла р
    if (!indexIdp)
        return false;
    const struct node *pfind = findIdInDom(root, p->value[indexIdp]);
    // если такая ссылка уже есть в DOM (кроме узла p)
    if (pfind && (pfind != p))
    {
        // индекс выражения yytext в найденном узле (зависит от типа узла)
        auto indexYYtextRes = getIndexYytext(pfind);

        // индекс выражения yytext в узле p (зависит от типа узла)
        auto indexYYtextP = getIndexYytext(p);

        printf ("Ссылка \"%s\", указанная в выражении \"%s\", уже была объявлена в выражении \"%s\"!\n", p->value[indexIdp].c_str(), p->value[indexYYtextP].c_str(), pfind->value[indexYYtextRes].c_str());
        exit(1);
        return true;
    }
    return false;
}

// возвращает индекс значения ссылки (id) в векторе value узла р,
// возвращает 0 если узел не содержит ссылку
std::string::size_type Dom::getIndexId(const struct node *p) const
{
    if (isSection(p))
        return 3;
    else
        if (p->id == image)
            return 5;
        else
            if (p->id == pageid)
                return 1;
            else
                if ((p->id == table || p->id == tableBegin) && p->value.size() == 10 && p->value[8].size())
                    return 9;
                else
                    return 0;
}

// возвращает индекс значения yytext в векторе value узла р,
// возвращает 0 если узел не содержит yytext
std::string::size_type Dom::getIndexYytext(const struct node *p) const
{
    if (isSection(p))
        return 5;
    else
        if ((p->id == image) && p->value.size() >= 8) // у рисунка полей под yytext может не быть
            return 7;
        else
            if (p->id == pageid)
                return 3;
            else
                if ((p->id == table || p->id == tableBegin) && p->value.size() == 10 && p->value[8].size())
                    return 0;
                else
                    return 0;
}

// ссылка - команда &... {id:"..."}, параметр res означает, на что указывает ссылка (заголовок, рисунок, ...)
void Dom::addId(const char *s, const std::string &res)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(temp);
    temp->id = idRef;
    temp->value.push_back("id");
    temp->value.push_back(""); // значение ссылки
    temp->value.push_back("yylex"); // параметр yylex
    temp->value.push_back(s);
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back(), false);
    temp->value.push_back("res"); // на какой ресурс указывает (рисунок, таблица, ...)
    temp->value.push_back(res);
    while(*s != '\"') ++s; ++s; // дойти до начала ref
    while(*s != '\"') temp->value[1].push_back(*s), ++s; ++s; // записать id
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value[1]);
    if (temp->value[1].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания ссылки \"%s\" нельзя использовать двойные переходы на новую строку!\n", s);
        exit(1);
    }
    if (!temp->value[1].size())
    {
        printf("В команде описания ссылки \"%s\" нельзя использовать пустое значение ссылки!\n", s);
        exit(1);
    }
	temp = temp->parent;
}

// заменять ли при занесении данных в DOM последовательности символов "\\a"
// на на одну кавычку " (если параметр fPareamInQuotes истина), иначе на "\\""
const std::string& Dom::SeqSymbContrReplace (std::string &s, const bool fPareamInQuotes) const
{
    if (s.size() >= 2)
    {
        for(decltype(s.size()) i=0; i < s.size()-1; ++i)
            if (s[i] == '\\' && s[i+1] == '\a')
            {
                s[i+1] = '\"';
                if (fPareamInQuotes)
                    s.erase(i,1);
                else
                    ++i;
            }
    }
    return s;
}

// создать ссылку на страницу - команда @pageid	{id:" ... "}
void Dom::addPageId(const char *s)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(temp);
    temp->id = pageid;
    temp->value.push_back("id");
    temp->value.push_back(""); // значение ссылки
    temp->value.push_back("yylex"); // параметр yylex
    temp->value.push_back(s);
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back(), false);
    while(*s != '\"') ++s; ++s; // дойти до начала id
    while(*s != '\"') temp->value[1].push_back(*s), ++s; ++s; // записать id
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value[1]);
    if (temp->value[1].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания ссылки \"%s\" нельзя использовать двойные переходы на новую строку!\n", s);
        exit(1);
    }
    if (!temp->value[1].size())
    {
        printf("В команде описания ссылки \"%s\" нельзя использовать пустое значение ссылки!\n", s);
        exit(1);
    }
    // ищет в DOM ссылку, содержащуюся в узле р (метод findIdInDom) и выводит сообщение об ошибке,
    // если ссылка уже объявлена
    showDuplicateIdInDom(temp);
	temp = temp->parent;
}

// удаляет все заданные символы на конце строки
const std::string& Dom::delSymbsInEndStr (std::string &s, const char ch) const
{
    while (s.size())
    {
        if (s.back() == ch)
            s.pop_back();
        else
            break;
    }
    return s;
}
// начало команды перечисления, type - тип перечисления (числовое "numeric" или нечисловое ""),
// symbItem - задан ли символ начала пункта (параметр команды в кавычках)
void Dom::addEnumBegin(const char *s, const char *type, const bool symbItem)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(temp);
    temp->id = enumerationBegin;
    // под название документа, пока для поисковика ошибок записать текст команды в оригинальном регистре
    temp->value.push_back(s);
    // замена в строке последовательности символов "\\a" на последовательность \"
    SeqSymbContrReplace(temp->value.back(), false);
    temp->value.push_back("type");
    temp->value.push_back(type); // вид перечисления (numeric нумерованное или "" простое через *)
    temp->value.push_back("symb");
    temp->value.push_back(""); // под символ обозначающего начало пункта
    temp->value.push_back("text");
    temp->value.push_back(""); // текст перечисления
    if (!symbItem)
    {
        temp->value[4] = "*"; // символ по умолчанию
    }
    else // записать символ заданный пользователем
    {
        // найти начало и конец аргумента команды " ... "
        auto argBegin = std::strchr(s, '\"'); // найти первое вхождение символа " в строке
        auto argEnd = std::strrchr(s, '\"'); // найти последнее вхождение символа " в строке
        // записать символ начала пункта
        for(auto ptr = argBegin + 1; ptr < argEnd; ++ptr)
            temp->value[4].push_back(*ptr);

        if (!temp->value[4].size())
        {
            std::string stemp(temp->value[0]);
            printf("Пустой символ начала пунктов в команде \"%s\"!\n", delSymbsInEndStr(stemp).c_str());
            exit(1);
        }
        else
            if (std::isspace(temp->value[4][0]))
            {
                std::string stemp(temp->value[0]);
                printf("Пробельный символ начала пунктов в команде \"%s\"!\n", delSymbsInEndStr(stemp).c_str());
                exit(1);
            }
        // замена в строке последовательности символов "\\a" на одну кавычку "
        SeqSymbContrReplace(temp->value[4]);
    }
    temp = temp->parent;
}
// команда @end enumerate
void Dom::addEnum(const char *s)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    decltype(temp) pfind = NULL; // для сообщения об ошибке

    // найти в дереве начало команды @enumerate {}
    for (decltype(temp->children.size()) i = temp->children.size()-1; i > 0; --i)
    {
        // нашли начало команды
        if (temp->children[i]->id == enumerationBegin)
        {
            pfind = temp->children[i];
                temp->children[i]->id = enumeration;
                // если пустой текст блока перечисления
                if(i-1 == -1)
                    return;

                // копирование текст перечисления в узел temp->children[i], удаление текстовых узлов после начала команды
                for (decltype(temp->children.size()) i1=i+1; i1 < temp->children.size(); ++i1)
                {
                    if(temp->children[i1]->id != text)
                    {
                        std::string stemp(pfind->value[0]); // строка для отображения ошибки
                        printf ("В теле команды \"%s ... @end enumeration\" можно указывать только текст!\n", delSymbsInEndStr(stemp).c_str());
                        exit(1);
                    }
                    temp->children[i]->value[6] += temp->children[i1]->value[0]; // текст перечисления
                    delete temp->children[i1];
                    temp->children[i1] = NULL;
                    temp->children.erase(temp->children.begin()+i1);
                    i1 = i;
                }
                return;
        }
    }
    if (!pfind)
        printf ("Отсутствует начало команды \"@enumerate {} ... @end enumerate\"!\n");
   exit(1);
}

// команда @include {ref:" ... "; odt} - конвертировать файл odt в tex и вставить его содержимое
// refBeg - указатель на начало имени файла в строке s, указывает на первый символ имени
void Dom::addOdt(const char *s, const char *refBeg)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }

    addCodeBegin(s, "latex");
    temp = temp->children.back(); // вернуться к текущему узлу
    temp->id = code;
    temp->value[0].clear(); // очистить от строки s под код на LaTeX

    std::string stemp, fnameExt, stemp2(s);
    if (!refBeg) // если команда @include {odt;ref:" ... "}
    {
        refBeg = s;
        while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до начала ref
    }
    while(*refBeg != '\"') stemp.push_back(*refBeg), ++refBeg; ++refBeg; // записать ref

    SeqSymbContrReplace(stemp, false);
    SeqSymbContrReplace(stemp2, false);

    if (getExtFname (stemp, fnameExt).size())
    {
        if (fnameExt != "odt")
        {
            printf("Формат файла \"%s\" указанного в команде \"%s\" не поддерживается программой!\n", stemp.c_str(), stemp2.c_str());
            exit(1);
        }
    }
    else // не содержит расширения
    {
        printf("Некорректное имя файла \"%s\" указанного в команде \"%s\"!\n", stemp.c_str(), stemp2.c_str());
        exit(1);
    }

    if (!existFile(stemp))
    {
        met1: printf ("Не удалось найти файл \"%s\" указанный в команде \"%s\"!\n", stemp.c_str(), stemp2.c_str());
		exit(1);
    }

    // перевести файл из odt в tex
    if (std::system(("w2l " + stemp).c_str()))
    {
        met2: printf("Ошибка работы приложения LibreOffice, необходимого для выполнения команды \"%s\"!", stemp2.c_str());
        exit(1);
    }

    // заменить  расширение файла с odt на tex
    stemp.erase (stemp.size()-fnameExt.size()-1, fnameExt.size()+1) += ".tex";

    FILE *fp;
    // открыть файл tex
    if(!(fp = fopen(stemp.c_str(), "rt" )))
	{
		goto met1;
	}

    std::string &stext = temp->value[0];
	char ch;
    while(!feof(fp)) // копирование содержимого файла в строку fnameExt
    {
        ch=getc(fp);
        if (!feof(fp)) stext.push_back(ch);
    }

 	fclose(fp);
 	remove(stemp.c_str());
 	// команды в tex файле, содержимое между ними вставить в код, остальное удалить
 	const std::string stextBeg = "\\begin{document}", stextEnd = "\\end{document}";
 	// если длина содержимого tex файла меньше длины команд
 	if (stext.size() < stextBeg.size() || stext.size() < stextEnd.size())
    {
        goto met2;
    }

    for (decltype(stext.size()) ibeg=0; ibeg < stext.size() - stextBeg.size() + 1; ibeg++)
    {
        if(!stext.compare(ibeg, stextBeg.size(), stextBeg)) // нашли начало команды stextBeg
        {
            stext.erase(0, ibeg + stextBeg.size()); // удалить всё до команды stextBeg вместе с командой
            goto met3;
        }
    }
    goto met2; // не нашли начало команды stextBeg
    met3:;

    for (decltype(stext.size()) iend = stext.size() - stextEnd.size(); iend >= 0; iend--)
    {
        if(!stext.compare(iend, stextEnd.size(), stextEnd)) // нашли начало команды stextEnd
        {
            stext.erase(iend, stext.size()-iend); // удалить всё после команды stextEnd вместе с командой
            goto met4;
        }
    }
    goto met2; // не нашли начало команды stextEnd
    met4: temp = temp->parent;
}


// команда @table {ref: " ... "}
void Dom::addTable1Param(const char *s, const char *refBeg)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    addOdt(s, refBeg);
    temp = temp->children.back(); // вернуться к текущему узлу
    // создать параметры таблицы
    temp->id = table;
    temp->value.push_back("");      // значение типа таблицы (LaTeX)    [2]
    temp->value.push_back("");      // описание (rjl) таблицы на LaTeX  [3]
    temp->value.push_back("ref");   //                                  [4]
    temp->value.push_back("");      // путь к файлу                     [5]
    temp->value.push_back("");      // text                             [6]
    temp->value.push_back("");      // подпись к таблице                [7]
    temp->value.push_back("");      //  id                              [8]
    temp->value.push_back("");      // ссылка на таблицу                [9]
    // откорр. узел табдицы после выполнения addOdt
    temp->value[3] = temp->value[0]; // переместить код таблицы
    temp->value[2] = temp->value[1]; // тип таблицы (LaTeX)
    temp->value[0] = s; // код команды
    SeqSymbContrReplace(temp->value[0], false);
    temp->value[1] = "type";
    if (!refBeg) // если команда @table {ref:" ... "}
    {
        refBeg = s;
        while(*s != '\"') ++s; ++s; // дойти до начала ref
    }
    while(*s != '\"') temp->value[5].push_back(*s), ++s; ++s; // записать ref
    SeqSymbContrReplace(temp->value[5], false);
    temp = temp->parent;
}

// команда @table {ref: " ... "; text: " ... "} n1=1, n2=2
// @table {text: " ... "; ref: " ... "} n1=2, n2=1
// n1 - ref, n2 - text
void Dom::addTable2Param(const char *s, int n1, int n2)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    auto refBeg = s; // должен указывать на первый символ имени
    for (int i=0; i < n1*2-1; ++i)
    {
        while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до начала ref
    }
    addTable1Param(s, refBeg);
    temp = temp->children.back(); // вернуться к текущему узлу
    temp->value[6] = "text";
    refBeg = s; // записать параметр text - подпись таблицы
    for (int i=0; i < n2*2-1; ++i)
    {
        while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до подписи
    }
    while(*refBeg != '\"') temp->value[7].push_back(*refBeg), ++refBeg; ++refBeg; // запись подписи
    SeqSymbContrReplace(temp->value[7]);
    if (temp->value[7].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания таблицы \"%s\" нельзя использовать двойные переходы на новую строку!", temp->value[0].c_str());
        exit(1);
    }

    // вставка подписи к таблице - для разных видов таблиц по-разному

    // вектор возможных команд начала описания таблицы
    std::vector<std::string> vcommBegTab = {"\\begin{tabular}", "\\begin{supertabular}", "\\begin{longtable}"};
    // вектор возможных команд конца описания таблицы
    std::vector<std::string> vcommEndTab = {"\\end{tabular}", "\\end{supertabular}", "\\end{longtable}"};

    std::string &stemp = temp->value[3];
    bool writeCap = false; // для проверки, что блок таблицы был найден

        // для таблицы вида "\\begin{tabular}" вставить подпись после "\\end{tabular}" и заверщить блок "\\end{table}"
        // проход кода LaTeX
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommEndTab[0].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommEndTab[0].size(), vcommEndTab[0], 0, vcommEndTab[0].size()))
            {
                std::string scap = "\n\\caption{" + temp->value[7] + "}\n\\end{table}";
                stemp.insert(i1 + vcommEndTab[0].size(), scap);
                i1 += scap.size() - 1;
                writeCap = true;
            }
        }
        // для таблицы вида "\\begin{tabular}" установить начало блока "\\begin{table}" - вставить перед "\\begin{tabular}"
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommBegTab[0].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommBegTab[0].size(), vcommBegTab[0], 0, vcommBegTab[0].size()))
            {
                std::string scap = "\\begin{table}[ht]\n";
                stemp.insert(i1, scap);
                i1 += scap.size() + vcommBegTab[0].size() - 1;
            }
        }

        // для таблицы вида "\\begin{supertabular}" вставить подпись перед "\\begin{supertabular}"
        // проход кода LaTeX
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommBegTab[1].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommBegTab[1].size(), vcommBegTab[1], 0, vcommBegTab[1].size()))
            {
                std::string scap = "\\topcaption{" + temp->value[7] + "}\n";
                stemp.insert(i1, scap);
                i1 += scap.size() + vcommBegTab[1].size() - 1;
                writeCap = true;
            }
        }

        // для таблицы longtable вставить подпись перед "\\end{longtable}"
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommEndTab[2].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommEndTab[2].size(), vcommEndTab[2], 0, vcommEndTab[2].size()))
            {
                std::string scap = "\\caption{" + temp->value[7] + "}";
                stemp.insert(i1, scap);
                i1 += scap.size() + vcommEndTab[2].size() - 1;
                writeCap = true;
            }
        }

        if (!writeCap)
        {
            printf("Не удалось установить подпись для таблицы, указанной в команде \"%s\" !\n", temp->value[0].c_str());
            printf("Игнорировать [Y/N] ? ");
            if (!getAnswer()) // ответ N
            {
                puts("\nГенерация документа прервана.");
                exit(1);
            }
        }

    temp = temp->parent;
}

// команда @table {ref: " ... "; text: " ... "; id: " ... "}
void Dom::addTable3Param(const char *s, int n1, int n2, int n3)
{
        // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    addTable2Param(s, n1, n2);
    temp = temp->children.back(); // вернуться к текущему узлу
    temp->value[8] = "id";
    auto idBeg = s; // должен указывать на первый символ имени
    for (int i=0; i < n3*2-1; ++i)
    {
        while(*idBeg != '\"') ++idBeg; ++idBeg; // дойти до id
    }
    while(*idBeg != '\"') temp->value[9].push_back(*idBeg), ++idBeg; ++idBeg; // запись id
    SeqSymbContrReplace(temp->value[9]);
    if (temp->value[9].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания таблицы \"%s\" нельзя использовать двойные переходы на новую строку!", temp->value[0].c_str());
        exit(1);
    }
    if (!temp->value[9].size())
    {
        printf("В команде описания таблицы \"%s\" нельзя использовать пустое значение ссылки!\n", temp->value[0].c_str());
        exit(1);
    }
    // если ссылка уже объявлена
    showDuplicateIdInDom(temp);

    // вставка ссылки к таблице - для разных видов таблиц по-разному. Ссылку вставить только в одну таблицу!

    // вектор возможных команд начала описания таблицы
    std::vector<std::string> vcommBegTab = {"\\begin{tabular}", "\\begin{supertabular}", "\\begin{longtable}"};
    // вектор возможных команд конца описания таблицы
    std::vector<std::string> vcommEndTab = {"\\end{tabular}", "\\end{supertabular}", "\\end{longtable}", "\\end{table}"};

    std::string &stemp = temp->value[3];
    bool writeCap = false; // для проверки, что блок таблицы был найден

    // для таблицы вида "\\begin{tabular}" вставить ссылку перед "\\end{table}"
    // проход кода LaTeX
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommEndTab[3].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommEndTab[3].size(), vcommEndTab[3], 0, vcommEndTab[3].size()))
            {
                if (writeCap)
                {
                    goto met1;
                }
                std::string scap = "\\label{" + temp->value[9] + "}";
                stemp.insert(i1, scap);
                i1 += scap.size() + vcommEndTab[3].size() - 1;
                writeCap = true;
            }
        }

        // для таблицы вида "\\begin{supertabular}" вставить ссылку перед "\\begin{supertabular}"
        // проход кода LaTeX
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommBegTab[1].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommBegTab[1].size(), vcommBegTab[1], 0, vcommBegTab[1].size()))
            {
                if (writeCap)
                {
                    goto met1;
                }
                std::string scap = "\\label{" + temp->value[9] + "}\n";
                stemp.insert(i1, scap);
                i1 += scap.size() + vcommBegTab[1].size() - 1;
                writeCap = true;
            }
        }

        // для таблицы longtable вставить ссылку перед "\\end{longtable}"
        for (decltype(stemp.size()) i1 = 0; i1 < stemp.size() - vcommEndTab[2].size() + 1; ++i1)
        {
            if (!stemp.compare(i1, vcommEndTab[2].size(), vcommEndTab[2], 0, vcommEndTab[2].size()))
            {
                if (writeCap)
                {
                    goto met1;
                }
                std::string scap = "\\label{" + temp->value[9] + "}";
                stemp.insert(i1, scap);
                i1 += scap.size() + vcommEndTab[2].size() - 1;
                writeCap = true;
            }
        }

        if (!writeCap)
        {
            printf("Не удалось установить ссылку для таблицы, указанной в команде \"%s\" !\n", temp->value[0].c_str());
            printf("Игнорировать [Y/N] ? ");
            if (!getAnswer()) // ответ N
            {
                puts("\nГенерация документа прервана.");
                exit(1);
            }
        }

    temp = temp->parent;
    return;

    met1: printf("В файле, указанном в команде описания таблицы \"%s\" содержится более одной таблицы!\n", temp->value[0].c_str());
    exit(1);
}

// записывает в строку ext расширение файла из fname
std::string Dom::getExtFname (const std::string &fname, std::string &ext) const
{
    ext.clear();
    decltype(fname.size()) i = 0, j = 0;
    while(i < fname.size()) // поиск расширения в имени файла с рисунком
    {
        if (fname[i] == '.') j = i;
        ++i;
    }
    if (j) // если имя содержит точку
    {
        ++j;
        ext.insert(0, fname, j, fname.size()-j); // вставить расширение имени файла в строку
    }
    return ext;
}

// возвращает true если файл с таким именем существует
bool Dom::existFile (const std::string &fname) const
{
    FILE *fp;
    if(!(fp = fopen(fname.c_str(), "rt" )))
	{
        return false;
	}
	fclose(fp);
	return true;
}

// функция обработки отмены команды, например "\\@toc"
// возвращает истину, если перед командой находится символ symbCancel
bool Dom::cancelComm (const char *s)
{
    // если предыдущий узел - текст и на конце текста знак '\\'
    if (temp && temp->children.size() &&\
        temp->children.back()->id == text &&\
        temp->children.back()->value[0].size() >= symbCancel.size())
    {
        // индекс начала символа отмены команды на конце текста
         auto i =  temp->children.back()->value[0].size() - symbCancel.size();
        // если на конце текста пред. пункта содержится символ отмены команды
        if(!temp->children.back()->value[0].compare(i, symbCancel.size(), symbCancel))
        {
            // удалить символ отмены команды
            temp->children.back()->value[0].erase(i, i+symbCancel.size());
            std::string stemp(s);
            // к концу текста добавить текст команды, перед которой стоял знак
            temp->children.back()->value[0] += SeqSymbContrReplace(stemp, false);
            return true;
        }
    }
    return false;
}

// возвращает ответ на вопрос вида Y/N
bool Dom::getAnswer() const
{
    met1: fflush(stdin);
    switch(std::tolower(getchar()))
    {
    case 'y':
        return true;
        break;
    case 'n':
        return false;
        break;
    default:
        goto met1;
        break;
    }
}
