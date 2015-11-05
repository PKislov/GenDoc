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
    // замена в строке последовательности символов "\\a" на "\""
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
                    puts ("В теле команды \"@title ... @end title\" необходимо указывать только текст!\n");
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
    puts ("Отсутствует начало команды \"@end title\"!\n");
    exit(1);
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
    // замена в строке последовательности символов "\\a" на "\""
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
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value[n]);
    // если заголовок содержит двойные переходы на новую строку, то TexLive это
    // примет за ошибку
    if (temp->value[n].find("\n\n", 0) != std::string::npos)
    {
        printf("В команде описания заголовка \"%s\" нельзя использовать двойные переходы на новую строку!\n", sbegin);
        exit(1);
    }
    temp->value[2] = "id";
    if (!temp->value[3].size())
    {
        printf("В команде описания заголовка \"%s\" нельзя использовать пустое значение ссылки!\n", sbegin);
        exit(1);
    }

    temp->value.push_back("yylex"); // параметр yylex
    temp->value.push_back(sbegin); // значение yylex - строка команды от лексера (нужна для поиска повторов ссылок)
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back());

    showDuplicateIdInDom(temp);

}

void Dom::addToc() // Содержание
{
    temp = addChild(root);
	temp->id = toc;
	temp = temp->parent;
}

void Dom::addImageRef(const char *s,  decltype(root->children.size()) n1, decltype(n1) n2) // рисунок с ref
{
    temp = addChild(temp);
    temp->id = image;
    temp->value.push_back("text");
    temp->value.push_back(""); // подпись рисунка
    temp->value.push_back("ref");
    temp->value.push_back(""); // значение ref
    temp->value.push_back(""); // параметр id
    temp->value.push_back(""); // значение id
    while(*s != '\"') ++s; ++s; // дойти до начала подписи
    while(*s != '\"') temp->value[n1].push_back(*s), ++s; ++s; // записать подпись
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value[n1]);
	while(*s != '\"') ++s; ++s;
	while (isspace(*s)) ++s;
	while(*s != '\"') temp->value[n2].push_back(*s), ++s;
	// замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value[n2]);
	// поиск в параметрах двойных переходов на новую строку
	if ((temp->value[1].find("\n\n", 0) != std::string::npos) || (temp->value[3].find("\n\n", 0) != std::string::npos))
    {
        puts("В команде описания рисунка нельзя использовать двойные переходы на новую строку!");
        exit(1);
    }
	temp = temp->parent;
}
void Dom::addImageRef(const char *s) // рисунок с ref
{
    temp = addChild(temp);
    temp->id = image;
    temp->value.push_back("");
    temp->value.push_back(""); // подпись рисунка
    temp->value.push_back("ref");
    temp->value.push_back(""); // значение ref
    temp->value.push_back(""); // параметр id
    temp->value.push_back(""); // значение id
    while(*s != '\"') ++s; ++s; // дойти до начала ref
    while(*s != '\"') temp->value[3].push_back(*s), ++s; ++s; // записать ref
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value[3]);
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
    SeqSymbContrReplace(temp->value.back());
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
        /*if (!indexYYtextRes)
            return false;*/

        // индекс выражения yytext в узле p (зависит от типа узла)
        auto indexYYtextP = getIndexYytext(p);
       /* if (!indexYYtextP)
            return false;*/

        printf ("Ссылка \"%s\", указанная в выражении \"%s\", уже была указана в выражении \"%s\"!\n", p->value[indexIdp].c_str(), p->value[indexYYtextP].c_str(), pfind->value[indexYYtextRes].c_str());
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
    SeqSymbContrReplace(temp->value.back());
    temp->value.push_back("res"); // на какой ресурс указывает (рисунок, таблица, ...)
    temp->value.push_back(res);
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back());
    while(*s != '\"') ++s; ++s; // дойти до начала ref
    while(*s != '\"') temp->value[1].push_back(*s), ++s; ++s; // записать id
    // замена в строке последовательности символов "\\a" на "\""
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

// замена в строке последовательности символов "\\a" на "\""
const std::string& Dom::SeqSymbContrReplace (std::string &s) const
{
    if (replaceSeqSymbContr && s.size() >= 2)
    {
        for(decltype(s.size()) i=0; i < s.size()-1; ++i)
            if (s[i] == '\\' && s[i+1] == '\a')
                s[i+1] = '\"', ++i;
    }
    return s;

}
