#ifndef DOMBUILDER_INCLUDED
#define DOMBUILDER_INCLUDED

#include <string>
#include <vector>

class Dom
{
protected:
// типы узлов
const std::string document="document"; // корень дерева, узел генерируется конструктором, нужен формально
const std::string title="title"; // заголовок документа
const std::string toc="toc"; // содержание (оглавление)
const std::string section1="section1";
const std::string section2="section2";
const std::string section3="section3";
const std::string section4="section4";
const std::string text="text"; // произвольный текст
const std::string image="image";
const std::string nothing="nothing"; // пустой узел

bool  ftoc=false; // флаг, сигнализирующий, что в дерево занесена команда toc

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
//struct node* addBro(struct node *p); // добавить в дерево узел одинакового уpовня с p и возвратить его
void delElem(struct node *p); // удаляет всю часть дерева после узла p. Исп. в деструкторе.
inline bool isSection (const struct node *p) const; // возвращает true, если узел р - заголовок
void delNewLineEndStr (std::string &s) const; // удаляет символы новой строки на конце строки (исп. в методах addSection1)

public:

Dom();
virtual ~Dom();

// добавление элементов в дерево (методы для лексера):
void addText(FILE *f, const char *name); // Текст
void addToc(); // Содержание
void addTitle(const char *s); // Название документа
//void addImageId(const char *s); // рисунок с id
void addImageRef(const char *s); // рисунок с ссылкой
void addSection1(const char *s); // заголовок уровня 1
void addSection2(const char *s); // заголовок уровня 2
void addSection3(const char *s); // заголовок уровня 3
void addSection4(const char *s); // заголовок уровня 4
bool getToc () { return ftoc; } // получить значение флага ftoc

private:
Dom (const Dom &d) {}
Dom& operator= (const Dom &d) { return *this;}
};
#endif // DOMBUILDER_INCLUDED
