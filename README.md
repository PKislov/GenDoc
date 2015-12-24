####Запуск
./gendoc <имя головного файла> <тип документа> <формат документа>

Например: ./gendoc data.txt form pdf

Типы документов: text - произвольный документ, form - формуляр, ... .

####Поддерживаемые форматы выходных документов
	pdf (выходной формат по умолчанию, параметр можно не указывать),
	odt 
	tex (исходный код на LaTeX), 
	html
	xml (в формате html)
	tei (формат TEI XML) 
	db (DocBook, в формате xml)
	doc
	docx
	markdown

Для генерации документов в форматах pdf, odt, html, xml (в формате html), tei (формат TEI XML), 
db (DocBook, в xml) - требуется установленный пакет TexLive 2015, для doc - TexLive и pandoc, 
для docx и markdown - pandoc. 



####@include {ref:" ... "}
Вставить код из другого файла (txt).

####@include {odt; ref:" ... "} или @include {ref:" ... "; odt}
Вставить содержимое odt-файла (ТОЛЬКО odt!). Для работы команды требуется установленный LibreOffice

####@title 
####...	
####@end title
Указать название документа на титульном листе, после @title переход на новую строку обязателен.

####@toc
Оглавление.

####@section
* @section1 ...

* @section2 ...

* @section3 ...

* @section4 ...

* @section1 {text:" ... "}

* @section2 {text:" ... "}

* @section3 {text:" ... "}

* @section4 {text:" ... "}

* @section1 {text:" ... "; id:" ... "}

* @section2 {text:" ... "; id:" ... "}

* @section3 {text:" ... "; id:" ... "}

* @section4 {text:" ... "; id:" ... "}

* @section1 {id:" ... "; text:" ... "}

4 уровня заголовков. Порядок параметров в команде можно менять.

id - ссылка на заголовок.


####&sectionN {id:" ... "}
Вставить номер заголовка.

####@figure, @image
Команды figure и image идентичны.

* @figure{text:" ... "; ref:" ... "} 	- вставить рисунок (подпись, путь к файлу рисунка). Допустимые форматы файлов: pdf, png, jpg, jpeg. Параметры text и ref - в любом порядке.
* @image{text:" ... "; ref:" ... "}
* @figure{ref:" ... "}	- рисунок без подписи
* @image{ref:" ... "}
* @figure{text:" ... "; ref:" ... "; id:" ... "} - параметры text, ref и id можно указывать в любом порядке.
* @image{text:" ... "; ref:" ... "; id:" ... "}

####&image, &figure
Вставить номер рисунка.
* &image {id:" ... "} 
* &figure {id:" ... "}

####@pageid {id:" ... "}
Задать ссылку на страницу.
####&pageid {id:" ... "}
Вставить номер страницы.

####@code {latex} 
####... 
####@end code {latex}
Выполнить код на LaTeX, , после @code переход на новую строку обязателен.

####@code {latex; ref:" ... "}, @code {ref:" ... "; latex}
Выполнить код на LaTeX из файла.

####@code {} 
####... 
####@end code {}
Вставить листинг программы на языке программирования без нумерации строк.

####@code {ref:" ... "}
Вставить из файла листинг программы на языке программирования без нумерации строк.

####@code {listing} 
####... 
####@end code {listing}
Вставить листинг программы на языке программирования c нумерацией строк.

####@code {listing; ref:" ... "}, @code {ref:" ... "; listing}
Вставить из файла листинг программы на языке программирования c нумерацией строк.

Например:

@code {listing}

@include {ref:"main.cpp"}

@end code {listing}

или: 

@code {listing; ref:"main.cpp"}	

Результат работы команд одинаков.

####@enumerate
Ненумерованый список (до 4 уровней вложенности), звездочка означает начало нового пункта.

@enumerate {}

\* ...

** ...

*** ...

**** ...

\* ...

@end enumerate


@enumerate {"@"} - ненумерованый список с заданным символом начала нового пункта

@ ...

@ ...

@end enumerate


@enumerate {numeric} - нумерованый список

\* ...

\* ...

@end enumerate


@enumerate {numeric; "@"} - нумерованый список с заданным символом начала нового пункта
\* ...

\* ...

@end enumerate

####@table
@table {latex} - описание таблицы на языке LaTeX

...

@end table

@table {ref: " ... "} - вставить таблицу из odt-файла (ТОЛЬКО odt!)

@table {ref: " ... "; text: " ... "} - вставить таблицу из odt-файла и установить подпись.

@table {ref: " ... "; text: " ... "; id: " ... "} - вставить таблицу из odt-файла и установить подпись и ссылку.

@table {} - описание таблицы командами, без указания подписи и ссылки.

{tr} {td} ... {td} ...

{tr} {td} ... {td} ...

...

@end table

Команда {tr} - создать новую строку, {td} - новая ячейка. 
{td;colspan:"N"} - растянуть ячейку на N вправо, N должно быть положительным от 2 и более, {td;rowspan:"N"} - растянуть ячейку на N вниз, {td;colspan:"N";rowspan:"M"} - растянуть ячейку на N вправо и на М вниз. Допускается менять порядок параметров в команде: {td;rowspan:"M";colspan:"N"}. Если текст ячейки пустой, ячейку допускается командой {td} не указывать:

@table {} - создаст таблицу 2х2

{tr} {td} ... {td} ...

{tr} {td} ...

@end table

@table {text: " ... "} - описание таблицы командами, с указанием подписи.

{tr} {td} ... {td} ...

{tr} {td} ... {td} ...

...

@end table

@table {text: " ... "; id: " ... "} - описание таблицы командами, с указанием подписи и ссылки.

{tr} {td} ... {td} ...

{tr} {td} ... {td} ...

...

@end table


####&table {id:" ... "}
Вставить номер таблицы.

####P.S.
Регистр набора команд произвольный. Лексемы команд можно разделять любым количеством пробелов и табуляций.

В параметрах команд заключенных в  кавычки " ... " для вставки кавычек " использовать последовательность \"

Параметры команд можно менять местами в любом порядке.

Лексемы команд можно разделять любым количеством пробелов и табуляций.

Перехода на новую строку означает конец абзаца.

Символ "\" перед любой командой или символом начала пункта отменяет действие команды, и команда записывается в
выходной файл как обычный текст.
