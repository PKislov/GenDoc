#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED

#include <string>
#include <vector>

class Table
{
protected:
// типы узлов
const std::string table = "table"; // корень дерева, узел генерируется конструктором, нужен формально
const std::string text = "text"; // произвольный текст
const std::string tr = "tr"; // команда {tr}
const std::string td = "td"; // команда {td; rowspan:" ... "; colspan:" ... "}

const std::string symbCancel = "\\"; // символ отмены команды, например "\\{tr}"

struct node
{
    struct node *parent; // указатель на родителя узла
    std::vector <struct node*>children; // массив указателей на потомков узла
    std::string id; // тип узла (текст, рисунок, заголовок и т.д.)
    std::vector <std::string>value; // массив значений (аттрибутов) узла (у разных типов разный набор)
};

struct node *root; // корень дерева (всегда id равно "document")
struct node *temp; // указатель на узел, с которым в последний раз проводилась операция

struct node* addChild(struct node *p); // добавить потомка узлу p и возвратить потомка
void delElem (struct node *p); // удаляет всю часть дерева после узла p. Исп. в деструкторе.

// заменять ли при занесении данных в DOM последовательности символов "\\a"
// на на одну кавычку " (если параметр fPareamInQuotes истина), иначе на "\\""
const std::string& SeqSymbContrReplace (std::string &s, const bool fPareamInQuotes = true) const;
// удаляет все заданные символы на конце строки
const std::string& delSymbsInEndStr (std::string &s, const char ch = '\n') const;

// функция обработки отмены команды, например "\\{tr}"
// возвращает истину, если перед командой находится символ symbCancel
bool cancelComm (const char *s);
// возвращает истину, если строка состоит из пробельных символов
bool isSpace(const std::string &s) const;
// записывает число n в строку s
std::string& intInString (std::string &s, unsigned n) const;

struct Cell // структура ячейки таблицы
{
    std::string st = ""; // текст ячейки
    std::string comm = ""; // текст команды для ячейки "{td;colspan:"...";rowspan:"..."}" - если задана, исп. для отображения ошибок пересечения команд
    bool busy = false; // флаг объединения ячеек командой colspan или rowspan
    bool verticbusy = false; // true если объединения ячеек командой rowspan
    bool backfilled = false; // true если ячейка не указана пользователем и добавлена программой
    bool nothing = false; // флаг используется при генерации кода таблицы на LaTeX, равно true в случае нахождения ячеек в области команды {td;colspan:"...";rowspan:"..."} в области со второго столбца и второй строки
    decltype(st.size()) rowspan = 1; // индексы растяжения ячейки
    decltype(rowspan) colspan = 1;
};
struct Row // структура строки таблицы
{
    std::vector <struct Cell>cells;
};
std::vector <struct Row>rows; // содержит все строки таблицы
std::string sresult; // результат - код описания таблицы на языке LaTeX
decltype(rows.size()) x = 0, y = 0; // размеры таблицы
// поиск синтаксических ошибок в описании таблицы
void findError() const;
// выполнение команды colspan, i - номер строки, i1 - номер ячейки в строке, count - растяжение
// s - текст команды (для отображения ошибки перекрещивания команд)
void runColspan (decltype(x) i, decltype(i) i1, decltype(i) count, const std::string &s);
// выполнение команды rowspan, i - номер строки, i1 - номер ячейки в строке, count - растяжение
void runRowspan (decltype(x) i, decltype(i) i1, decltype(i) count, const std::string &s);


public:

Table();
virtual ~Table();

// добавление элементов в дерево (методы для лексера):
void addText(FILE *f, const char *name); // текст ячейки
void addTR(const char *s); // {tr}
// команда {td; rowspan:" ... "; colspan:" ... "}
void addTD(const char *s, int n1=0, int n2=0);

// возвращает строку sresult, содержащую код таблицы на языке LaTeX
std::string& genTableCode ();

private:
Table (const Table &d) {}
Table& operator = (const Table &d) { return *this; }
};
#endif // TABLE_INCLUDED
