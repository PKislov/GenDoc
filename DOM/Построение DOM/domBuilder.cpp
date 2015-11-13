#include <cstdio>
#include <cstring>
#include <cctype>
#include <vector>
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
void Dom::addTitle() // окончание команды - @end title
{
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
    temp = addChild(temp);
    temp->id = codeBegin;
    // для поисковика ошибок записать текст команды в оригинальном регистре
    temp->value.push_back(s);
    // замена в строке последовательности символов "\\a" на одну кавычку "
    SeqSymbContrReplace(temp->value.back());
    temp->value.push_back(type); // записать тип кода (LaTeX или другой)
    temp = temp->parent;
}

void Dom::addCode(const char *type) // окончание команды - @end code {}
{
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


// команды вида @code {ref:" ... "; latex}
void Dom::addCodeRef(const char *s, const char *type)
{
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

void Dom::addToc() // Содержание
{
    temp = addChild(root);
	temp->id = toc;
	temp = temp->parent;
}


// команда вида @image{text:" ... "; ref:" ... "; id:" ... "}
void Dom::addImageRef(const char *s, decltype(root->children.size()) n1, decltype(n1) n2, decltype(n1) n3)
{
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
    // ищет в DOM ссылку, содержащуюся в узле р (метод findIdInDom) и выводит сообщение об ошибке,
    // если ссылка уже объявлена
    showDuplicateIdInDom(temp);
    temp = temp->parent;
}

// команда вида @image{text:" ... "; ref:" ... "}
// n - индекс элемента массива value
void Dom::addImageRef(const char *s,  decltype(root->children.size()) n1, decltype(n1) n2)
{
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
    temp = addChild(temp);
    temp->id  = text;
    temp->value.push_back("");

    char ch;
    while(!(feof(f)))
    {
        ch=fgetc(f);
        if (!feof(f)) temp->value.back().push_back(ch);
    }
    fclose(f);
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
        int i=0;
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
                return 0;
}

// ссылка - команда &... {id:"..."}, параметр res означает, на что указывает ссылка (заголовок, рисунок, ...)
void Dom::addId(const char *s, const std::string &res)
{
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
        // замена в строке последовательности символов "\\a" на одну кавычку "
        SeqSymbContrReplace(temp->value[4]);
    }
    temp = temp->parent;
}
// команда @end enumerate
void Dom::addEnum(const char *s)
{
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
void Dom::addOdt(const char *s)
{
    addCodeBegin(s, "latex");
    temp = temp->children.back(); // вернуться к текущему узлу
    temp->id = code;
    temp->value[0].clear(); // очистить от строки s под код на LaTeX

    std::string stemp, fnameExt;
    auto argBegin = std::strchr(s, '\"');
    auto argEnd = std::strrchr(s, '\"');
    // записать в stemp имя файла
    for(auto ptr = argBegin + 1; ptr < argEnd; ++ptr)
    {
        stemp.push_back(*ptr);
    }
    SeqSymbContrReplace(stemp, false);

    if (getExtFname (stemp, fnameExt).size())
    {
        if (fnameExt != "odt")
        {
            printf("Формат файла \"%s\" указанного в команде \"%s\" не поддерживается программой!\n", stemp.c_str(), s);
            exit(1);
        }
    }
    else // не содержит расширения
    {
        printf("Некорректное имя файла \"%s\" указанного в команде \"%s\"!\n", stemp.c_str(), s);
        exit(1);
    }

    if (!existFile(stemp))
    {
        met1: printf ("Не удалось найти файл \"%s\" указанного в команде \"%s\"!\n", stemp.c_str(), s);
		exit(1);
    }

    // перевести файл из odt в tex
    if (std::system(("w2l " + stemp).c_str()))
    {
        met2: printf("Ошибка работы приложения LibreOffice, необходимого для выполнения команды \"%s\"!", s);
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
