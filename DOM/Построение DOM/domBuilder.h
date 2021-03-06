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
const std::string titleBegin = "titlebegin"; // начало команды заголовка
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
const std::string enumeration = "enum"; // перечисление
const std::string enumerationBegin = "enumbegin"; // начало команды перечисления
const std::string table = "table"; // таблица
const std::string tableBegin = "tableBegin";
const std::string nothing = "nothing"; // пустой узел

const std::string symbCancel = "\\"; // символ отмены команды, например "\\@toc"


struct node
{
    node *parent; // указатель на родителя узла
    std::vector <node*>children; // массив указателей на потомков узла
    std::string id; // тип узла (текст, рисунок, заголовок и т.д.)
    std::vector <std::string>value; // массив значений (аттрибутов) узла (у разных типов разный набор)
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
// удаляет все заданные символы на конце строки
const std::string& delSymbsInEndStr (std::string &s, const char ch = '\n') const;
// записывает в строку ext расширение файла из fname
std::string getExtFname (const std::string &fname, std::string &ext) const;
// возвращает true если файл с таким именем существует
bool existFile (const std::string &fname) const;
// функция обработки отмены команды, например "\\@toc"
// возвращает истину, если перед командой находится символ symbCancel
bool cancelComm (const char *s);
// возвращает ответ на вопрос вида Y/N
bool getAnswer() const;

public:

Dom();
virtual ~Dom();

// добавление элементов в дерево (методы для лексера):
void addText(FILE *f, const char *name); // Текст
void addToc(const char *s); // Содержание
void addTitleBegin(const char *s); // начало команды @title
void addTitle(const char *s); // окончание команды - @end title


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
void addCode(const char *s, const char *type = ""); // окончание команды - @end code {}
// команды вида @code {ref:" ... "; latex}
void addCodeRef(const char *s, const char *type);
// начало команды перечисления, type - тип перечисления (числовое "numeric" или нечисловое ""),
// symbItem - - задан ли символ начала пункта (параметр команды в кавычках)
void addEnumBegin(const char *s, const char *type = "", const bool symbItem = false);
// команда @end enumerate
void addEnum(const char *s);
// команда @include {ref:" ... "; odt} - конвертировать файл odt в tex и вставить его содержимое
// refBeg - указатель на начало имени файла в строке s, если NULL то имя файла ищется с начала строки
void addOdt(const char *s, const char *refBeg = NULL);
// начало команды @table {}, type - тип кода (на языке LaTeX "latex" или "tag" - описана тэгами)
// n1 и n2 - порядок следования параметров в команде @table {text:"...";id:"..."}
void addTableBegin(const char *s, const char *type = "tag", int n1=0, int n2=0);
// команда @end table
void addTableEnd(const char *s);
// команда @table {ref: " ... "}
// refBeg - указатель на начало имени файла в строке s
void addTable1Param(const char *s, const char *refBeg = NULL);
// команда @table {ref: " ... "; text: " ... "}
void addTable2Param(const char *s, int n1=1, int n2=2);
// команда @table {ref: " ... "; text: " ... "; id: " ... "}
void addTable3Param(const char *s, int n1=1, int n2=2, int n3=3);


private:
Dom (const Dom &d) {}
Dom& operator = (const Dom &d) { return *this;}
};
#endif // DOMBUILDER_INCLUDED
