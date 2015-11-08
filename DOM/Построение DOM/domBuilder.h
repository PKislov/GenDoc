#ifndef DOMBUILDER_INCLUDED
#define DOMBUILDER_INCLUDED

#include <string>
#include <vector>

class Dom
{
protected:
// типы узлов
const std::string document = "document"; // корень дерева, узел генерируется конструктором, нужен формально
const std::string title = "title"; // заголовок документа
const std::string titleBegin = "titlebegin";
const std::string toc = "toc"; // содержание (оглавление)
const std::string section1Begin = "section1begin"; // начало команды заголовка (без имени заголовка)
const std::string section2Begin = "section2begin";
const std::string section3Begin = "section3begin";
const std::string section4Begin = "section4begin";
const std::string section1 = "section1"; // заголовок с именем
const std::string section2 = "section2";
const std::string section3 = "section3";
const std::string section4 = "section4";
const std::string text = "text"; // произвольный текст
const std::string image = "image";
const std::string idRef = "id"; // ссылка (вставить номер ресурса)
const std::string pageid = "pageid"; // ссылка на страницу
const std::string codeBegin = "codebegin"; // начало блока кода @code {}
const std::string code = "code"; // код (LaTeX или другой)
const std::string nothing = "nothing"; // пустой узел


struct node
{
    node *parent; // указатель на родителя узла
    std::vector <node*>children; // массив указателей на потомков узла
    std::string id; // тип узла (текст, рисунок, заголовок и т.д.)
    std::vector <std::string>value; // массив значений (аттрибутов) узла (у разных типов разный набор)
    //std::string yytext; // доп. параметр, строка команды приходит от лексера
};

node *root; // корень дерева (всегда id равно "document")
node *temp; // указатель на узел, с которым в последний раз проводилась операция

struct node* addChild(struct node *p); // добавить потомка узлу p и возвратить потомка
void delElem (struct node *p); // удаляет всю часть дерева после узла p. Исп. в деструкторе.
bool isSection (const struct node *p) const; // возвращает true, если узел р - заголовок
bool isSectionBegin (const struct node *p) const; // начало команды section
// меняет строку id с sectionBegin на section (используется в построении узлов заголовков)
const std::string& sectionBegToSection (std::string &id) const;
// ищет в дереве узел содержащий ссылку id и возвращает его, или NULL если узел не найден (поиск повторов ссылок)
const struct node* findIdInDom (const struct node *p, const std::string &id) const;
// ищет в DOM ссылку, содержащуюся в узле р (метод findIdInDom)
// и выводит сообщение о результате. Возвращает true если ссылка найдена.
bool showDuplicateIdInDom (const struct node *p) const;
// возвращает индекс значения ссылки (id) в векторе value узла р,
// возвращает 0 если узел не содержит ссылку
std::string::size_type getIndexId(const struct node *p) const;
// возвращает индекс значения yytext в векторе value узла р,
// возвращает 0 если узел не содержит yytext
std::string::size_type getIndexYytext(const struct node *p) const;

// заменять ли при занесении данных в DOM последовательности символов "\\a"
// на на одну кавычку " (если параметр fPareamInQuotes истина), иначе на "\\""
const std::string& SeqSymbContrReplace (std::string &s, const bool fPareamInQuotes = true) const;


public:

Dom();
virtual ~Dom();

// добавление элементов в дерево (методы для лексера):
void addText(FILE *f, const char *name); // Текст
void addToc(); // Содержание
void addTitleBegin(const char *s); // начало команды @title
void addTitle(); // окончание команды - @end title


// команда вида @image{text:" ... "; ref:" ... "; id:" ... "}
void addImageRef(const char *s, decltype(root->children.size()) n1, decltype(n1) n2, decltype(n1) n3);
// рисунок с подписью и путем к файлу, в параметрах индексы - индексы для вектора value,
// куда записывать параметры (в команде могут следовать в другом порядке).
// команда вида @image{text:" ... "; ref:" ... "}
void addImageRef(const char *s, decltype(root->children.size()) n1, decltype(n1) n2);
// рисунок без подписи и без нумерации в документе
// команда вида @image{ref:" ... "}
void addImageRef(const char *s, decltype(root->children.size()) n1=3);
void addSection1Begin(const char *s); // начало команды заголовка уровня 1 (команда вида @section ...), название заголовка заполняется в методе addText
void addSection2Begin(const char *s); // заголовок уровня 2
void addSection3Begin(const char *s); // заголовок уровня 3
void addSection4Begin(const char *s); // заголовок уровня 4
// заголовок уровня sec (команда вида @section1{text:"..."}, n - индекс массива value
void addSection_1Param(const char *s, const std::string &sec, decltype(root->children.size()) n=1);
// команда вида @section1{text:"..."; id:"..."} или @section1{id:"..."; text:"..."} - вид определяется параметром n
void addSection_2Param(const char *s, const std::string &sec, decltype(root->children.size()) n);
// ссылка - команда &... {id:"..."}, параметр res означает, на что указывает ссылка (заголовок, рисунок, ...)
void addId(const char *s, const std::string &res);
// создать ссылку на страницу - команда @pageid	{id:" ... "}
void addPageId(const char *s);
// начало команды @code {}, type - тип кода (LaTeX или др.)
void addCodeBegin(const char *s, const char *type = "");
void addCode(const char *type = ""); // окончание команды - @end code {}


private:
Dom (const Dom &d) {}
Dom& operator = (const Dom &d) { return *this;}
};
#endif // DOMBUILDER_INCLUDED
