%{
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cctype>
#include "GenDoc.h"
//компилировать совместно с domBuilder.cpp и GenDoc.cpp

// основной объект генератора
static GenDoc *gen;

static const char *fileIn;
static const char * const fileOut = "temp.tmp"; // временный файл, нельзя получать библиотечной функцией, т.к. постоянно происходит открытие/закрытие файла

#define GETTITLEBEGIN {\
	gen->addText(yyout, fileOut);/*записать в дерево фрагмент текста перед командой*/\
	/*После выполнения каждой команды содержимое вых. файла затирается*/\
	gen->addTitleBegin(yytext);\
		 }
// конструкция @title ... @end title разбита на три части и заносится в DOM отдельно (начало команды, текст названия и окончание команды)
#define GETTITLEEND {\
	gen->addText(yyout, fileOut);\
	gen->addTitle(yytext);\
		 }

#define GETTOC	{\
	gen->addText(yyout, fileOut);\
	gen->addToc(yytext);\
		}

#define	GETSECTION1BEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addSection1Begin(yytext);\
		 }

#define	GETSECTION2BEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addSection2Begin(yytext);\
		 }

#define	GETSECTION3BEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addSection3Begin(yytext);\
		 }

#define	GETSECTION4BEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addSection4Begin(yytext);\
		 }

#define	GETSECTION1_1PARAM {\
	gen->addText(yyout, fileOut);\
	gen->addSection_1Param(yytext, "section1");\
		 }
#define	GETSECTION2_1PARAM {\
	gen->addText(yyout, fileOut);\
	gen->addSection_1Param(yytext, "section2");\
		 }
#define	GETSECTION3_1PARAM {\
	gen->addText(yyout, fileOut);\
	gen->addSection_1Param(yytext, "section3");\
		 }
#define	GETSECTION4_1PARAM {\
	gen->addText(yyout, fileOut);\
	gen->addSection_1Param(yytext, "section4");\
		 }
#define	GETSECTION1_2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section1", 3);\
		 }
#define	GETSECTION2_2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section2", 3);\
		 }
#define	GETSECTION3_2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section3", 3);\
		 }
#define	GETSECTION4_2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section4", 3);\
		 }


#define	GETSECTION1_2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section1", 1);\
		 }
#define	GETSECTION2_2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section2", 1);\
		 }
#define	GETSECTION3_2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section3", 1);\
		 }
#define	GETSECTION4_2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addSection_2Param(yytext, "section4", 1);\
		 }



#define	GETIMAGE {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 1, 3);\
		 }

#define	GETIMAGE2 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 3, 1);\
		 }

#define	GETIMAGE3 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext);\
		 }

#define	GETIDSECTION1 {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "section1");\
		 }
#define	GETIDSECTION2 {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "section2");\
		 }
#define	GETIDSECTION3 {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "section3");\
		 }
#define	GETIDSECTION4 {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "section4");\
		 }

#define	GETIMAGE2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 1, 3, 5);\
		}
#define	GETIMAGE2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 1, 5, 3);\
		}
#define	GETIMAGE2PARAM_3 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 5, 1, 3);\
		}
#define	GETIMAGE2PARAM_4 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 5, 3, 1);\
		}
#define	GETIMAGE2PARAM_5 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 3, 5, 1);\
		}
#define	GETIMAGE2PARAM_6 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 3, 1, 5);\
		}

#define	GETIDIMAGE {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "image");\
		 }

#define	CREATPAGEID {\
	gen->addText(yyout, fileOut);\
	gen->addPageId(yytext);\
		 }
#define	GETPAGEID {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "pageid");\
		}

#define	GETCODELATEXBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addCodeBegin(yytext, "latex");\
		 }
#define	GETCODEOTHERBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addCodeBegin(yytext);\
		 }

#define	GETCODELATEX {\
	gen->addText(yyout, fileOut);\
	gen->addCode(yytext, "latex");\
		 }
#define	GETCODEOTHER {\
	gen->addText(yyout, fileOut);\
	gen->addCode(yytext);\
		 }

#define	GETCODELISTINGBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addCodeBegin(yytext, "listing");\
		 }
#define	GETCODELISTING {\
	gen->addText(yyout, fileOut);\
	gen->addCode(yytext, "listing");\
		 }

#define	GETCODELATEXREF {\
	gen->addText(yyout, fileOut);\
	gen->addCodeRef(yytext, "latex");\
		 }
#define	GETCODEOTHERREF {\
	gen->addText(yyout, fileOut);\
	gen->addCodeRef(yytext, "");\
		 }
#define	GETCODELISTINGREF {\
	gen->addText(yyout, fileOut);\
	gen->addCodeRef(yytext, "listing");\
		 }

// @enumerate {}
#define	GETENUMERATENOTNUMBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addEnumBegin(yytext);\
		 }
// @enumerate {"@"}
#define	GETENUMERATENOTNUMSYMBBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addEnumBegin(yytext, "", true);\
		 }
// @enumerate {numeric}
#define	GETENUMERATEBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addEnumBegin(yytext, "numeric");\
		 }
// @enumerate {numeric;"@"} или @enumerate {"@";numeric}
#define	GETENUMERATESYMBBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addEnumBegin(yytext, "numeric", true);\
		 }
// @end enumerate
#define	GETENUMEND {\
	gen->addText(yyout, fileOut);\
	gen->addEnum(yytext);\
		 }

// @include {ref:" ... "; odt}
#define	GETINCLUDEODT {\
	gen->addText(yyout, fileOut);\
	gen->addOdt(yytext);\
		 }

// @table {latex}
#define	GETTABLELATEXBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addTableBegin(yytext, "latex");\
		 }
// @end table	- на языке LaTeX или тэгами
#define	GETENDTABLE {\
	gen->addText(yyout, fileOut);\
	gen->addTableEnd(yytext);\
		 }

// @table {}
#define	GETTABLETAGBEGIN {\
	gen->addText(yyout, fileOut);\
	gen->addTableBegin(yytext);\
		 }
// @table {text: " ... "}
#define	GETTABLETAGBEGIN_1PARAM {\
	gen->addText(yyout, fileOut);\
	gen->addTableBegin(yytext, "tag", 1);\
		 }
// @table {text: " ... "; id: " ... "}
#define	GETTABLETAGBEGIN_2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addTableBegin(yytext, "tag", 1, 2);\
		 }
// @table {id: " ... "; text: " ... "}
#define	GETTABLETAGBEGIN_2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addTableBegin(yytext, "tag", 2, 1);\
		 }

// @table {ref: " ... "}
#define	GETTABLEREF {\
	gen->addText(yyout, fileOut);\
	gen->addTable1Param(yytext);\
		 }
// @table {ref: " ... "; text: " ... "}
#define	GETTABLEREF_2PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addTable2Param(yytext, 1, 2);\
		 }
// @table {text: " ... "; ref: " ... "}
#define	GETTABLEREF_2PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addTable2Param(yytext, 2, 1);\
		 }

// @table {ref: " ... "; text: " ... "; id: " ... "}
#define	GETTABLEREF_3PARAM_1 {\
	gen->addText(yyout, fileOut);\
	gen->addTable3Param(yytext, 1, 2, 3);\
		 }
// @table {ref: " ... "; id: " ... "; text: " ... "}
#define	GETTABLEREF_3PARAM_2 {\
	gen->addText(yyout, fileOut);\
	gen->addTable3Param(yytext, 1, 3, 2);\
		 }
// @table {id: " ... "; ref: " ... "; text: " ... "}
#define	GETTABLEREF_3PARAM_3 {\
	gen->addText(yyout, fileOut);\
	gen->addTable3Param(yytext, 3, 1, 2);\
		 }
// @table {id: " ... "; text: " ... "; ref: " ... "}
#define	GETTABLEREF_3PARAM_4 {\
	gen->addText(yyout, fileOut);\
	gen->addTable3Param(yytext, 3, 2, 1);\
		 }
// @table {text: " ... "; ref: " ... "; id: " ... "}
#define	GETTABLEREF_3PARAM_5 {\
	gen->addText(yyout, fileOut);\
	gen->addTable3Param(yytext, 2, 1, 3);\
		 }
// @table {text: " ... "; id: " ... "; ref: " ... "}
#define	GETTABLEREF_3PARAM_6 {\
	gen->addText(yyout, fileOut);\
	gen->addTable3Param(yytext, 2, 3, 1);\
		 }

// &table {id: " ... "}
#define	GETIDTABLE {\
	gen->addText(yyout, fileOut);\
	gen->addId(yytext, "table");\
		 }
%}
%%  
@[T|t][I|i][T|t][L|l][E|e][ \t\n]+				GETTITLEBEGIN
@[E|e][N|n][D|d][ \t]+[T|t][I|i][T|t][L|l][E|e]			GETTITLEEND
@[T|t][O|o][C|c][ \t]*['\n']*					GETTOC
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]1[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETSECTION1_1PARAM
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]2[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETSECTION2_1PARAM
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]3[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETSECTION3_1PARAM
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]4[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETSECTION4_1PARAM
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]1[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION1_2PARAM_1
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]2[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION2_2PARAM_1
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]3[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION3_2PARAM_1
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]4[ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION4_2PARAM_1
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]1[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION1_2PARAM_2
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]2[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION2_2PARAM_2
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]3[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION3_2PARAM_2
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]4[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETSECTION4_2PARAM_2
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]1[ \t]+			GETSECTION1BEGIN
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]2[ \t]+			GETSECTION2BEGIN
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]3[ \t]+			GETSECTION3BEGIN
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]4[ \t]+			GETSECTION4BEGIN
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE3
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE3
&[S|s][E|e][C|c][T|t][I|i][O|o][N|n]1[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIDSECTION1
&[S|s][E|e][C|c][T|t][I|i][O|o][N|n]2[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIDSECTION2
&[S|s][E|e][C|c][T|t][I|i][O|o][N|n]3[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIDSECTION3
&[S|s][E|e][C|c][T|t][I|i][O|o][N|n]4[ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIDSECTION4
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_1
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_1
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_2
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_2
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_3
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_3
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_4
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_4
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_5
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_5
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_6
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIMAGE2PARAM_6
&[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETIDIMAGE
&[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETIDIMAGE
@[P|p][A|a][G|g][E|e][I|i][D|d][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	CREATPAGEID
&[P|p][A|a][G|g][E|e][I|i][D|d][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETPAGEID
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[L|l][A|a][T|t][E|e][X|x][ \t]*\}[ \t\n]+		GETCODELATEXBEGIN
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*\}[ \t\n]+						GETCODEOTHERBEGIN
@[E|e][N|n][D|d][ \t]+[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[L|l][A|a][T|t][E|e][X|x][ \t]*\}		GETCODELATEX
@[E|e][N|n][D|d][ \t]+[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*\}		GETCODEOTHER
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[L|l][I|i][S|s][T|t][I|i][N|n][G|g][ \t]*\}[ \t\n]+		GETCODELISTINGBEGIN
@[E|e][N|n][D|d][ \t]+[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[L|l][I|i][S|s][T|t][I|i][N|n][G|g][ \t]*\}		GETCODELISTING
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[L|l][A|a][T|t][E|e][X|x][ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETCODELATEXREF
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[L|l][A|a][T|t][E|e][X|x][ \t]*\}		GETCODELATEXREF
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETCODEOTHERREF
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[L|l][I|i][S|s][T|t][I|i][N|n][G|g][ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETCODELISTINGREF
@[C|c][O|o][D|d][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[L|l][I|i][S|s][T|t][I|i][N|n][G|g][ \t]*\}		GETCODELISTINGREF
@[E|e][N|n][U|u][M|m][E|e][R|r][A|a][T|t][E|e][ \t]*\{[ \t]*\}[ \t\n]+		GETENUMERATENOTNUMBEGIN
@[E|e][N|n][U|u][M|m][E|e][R|r][A|a][T|t][E|e][ \t]*\{[ \t]*\"[^"\""]*\"[ \t]*\}[ \t\n]+		GETENUMERATENOTNUMSYMBBEGIN
@[E|e][N|n][U|u][M|m][E|e][R|r][A|a][T|t][E|e][ \t]*\{[ \t]*[N|n][U|u][M|m][E|e][R|r][I|i][C|c][ \t]*\}[ \t\n]+		GETENUMERATEBEGIN
@[E|e][N|n][U|u][M|m][E|e][R|r][A|a][T|t][E|e][ \t]*\{[ \t]*[N|n][U|u][M|m][E|e][R|r][I|i][C|c][ \t]*;[ \t]*\"[^"\""]*\"[ \t]*\}[ \t\n]+		GETENUMERATESYMBBEGIN
@[E|e][N|n][U|u][M|m][E|e][R|r][A|a][T|t][E|e][ \t]*\{[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[N|n][U|u][M|m][E|e][R|r][I|i][C|c][ \t]*\}[ \t\n]+		GETENUMERATESYMBBEGIN
@[E|e][N|n][D|d][ \t]+[E|e][N|n][U|u][M|m][E|e][R|r][A|a][T|t][E|e]			GETENUMEND
@[I|i][N|n][C|c][L|l][U|u][D|d][E|e][ \t]*\{[ \t]*[O|o][D|d][T|t][ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETINCLUDEODT
@[I|i][N|n][C|c][L|l][U|u][D|d][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[O|o][D|d][T|t][ \t]*\}		GETINCLUDEODT
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[L|l][A|a][T|t][E|e][X|x][ \t]*\}[ \t\n]+		GETTABLELATEXBEGIN
@[E|e][N|n][D|d][ \t]+[T|t][A|a][B|b][L|l][E|e]			GETENDTABLE
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*\}[ \t\n]+		GETTABLETAGBEGIN
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLETAGBEGIN_1PARAM
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLETAGBEGIN_2PARAM_1
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLETAGBEGIN_2PARAM_2
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_2PARAM_1
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_2PARAM_2
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_3PARAM_1
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_3PARAM_2
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_3PARAM_3
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_3PARAM_4
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_3PARAM_5
@[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}		GETTABLEREF_3PARAM_6
&[T|t][A|a][B|b][L|l][E|e][ \t]*\{[ \t]*[I|i][D|d][ \t]*:[ \t]*\"[^"\""]*\"[ \t]*\}	GETIDTABLE
%%

// В командной строке пользователь должен указать имя исходного файла, тип документа и формат выходного файла (по умолчанию pdf).
// Форматы: odt, pdf, html, xml (в формате html), tei (формат TEI XML), db (DocBook, в xml) - требуется TexLive
// 	    через pandoc: doc (сначала в html через TexLive, затем в doc через pandoc), docx, markdown
// Тип документа: form - формуляр, text - произвольный документ
// Пример команды: ./gendoc data.txt form odt
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		puts("Не указано имя входного файла!");
		return 1;
	}

	std::string type = argc < 3 ? "text" : argv[2]; // тип генерируемого документа (формуляр, ...)
	for (decltype(type.size()) i=0; i < type.size(); ++i) type[i] = tolower(type[i]); // перевести в нижний регистр
	if (type != "form" && type != "text")
	{
		// TODO: вставить в сообщение типы всех возможных генерируемых документов
		printf ("Тип документа \"%s\" не поддерживается программой. Указывайте следующие форматы:\n text - произвольный текст\n form - формуляр\n", type.c_str());
		return 1;
	}

	std::string stemp("./fileRing ");
	stemp += argv[1];
	if (std::system (stemp.c_str())) // выполнить проверку на корректность включения файлов
		return 1;


	const char * const fSrcIncl = "srcincl.tmp"; // временный файл для выполнения команд @include
	remove(fSrcIncl);
	stemp = std::string("./include ") + argv[1] + " " + fSrcIncl; 
	if (std::system (stemp.c_str())) // выполнить команды @include, результат запишется в файл fSrcIncl
		return 1;
	fileIn = fSrcIncl;



// В файле fSrcIncl заменим последовательности символов \" на \a, a - bell
	FILE *ftemp1;
	stemp.clear();	// буфер файла
	if(!(ftemp1 = fopen(fSrcIncl, "r+b" )))
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", fSrcIncl);
		return 1;
	}
	char ch;
	while(!feof(ftemp1))
	{
		ch=getc(ftemp1);
		if(!feof(ftemp1))
		{
			stemp.push_back (ch);
			if (stemp.size() == 2)
			{
				if (stemp == "\\\"")
				{
					fseek(ftemp1, -1, SEEK_CUR);
					putc('\a', ftemp1);				
				}
				stemp[0] = stemp[1];
				stemp.pop_back();
			}
		}
	}
	fclose(ftemp1);






	if(!(yyin = fopen(fileIn, "rt" )))
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", fileIn);
		return 1;
	}

	remove(fileOut);
	if(!(yyout = fopen(fileOut, "wt" ))) // ненужный выходной поток, генерируемый лексером, сбрасывать во временный файл, из которого считывать произвольный текст
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", fileOut);
		return 1;
	}
	gen = new GenDoc();

	yylex(); // запуск лексера, заполнение дерева
	gen->addText(yyout, fileOut); // записать в дерево произвольный текст после последней команды, или всё содержимое вх. файла, если ни одной команды не встретилось

	fclose(yyin);
	fclose(yyout);
	remove(fileOut); // удалить временный файл
	remove(fSrcIncl); // удалить временный файл	
	fileIn = argv[1];


	std::string result; // имя генерируемого документа
	std::string format; // расширение генерируемого документа. Переменная нужна для преобразования строки формата в нижний регистр

	// получение имени и расширения генерируемого документа
		int i = 0;
		const char *p = fileIn;
		while(*p) // поиск последнего вхождения '.' в имени файла
		{
			if (*p == '.') i = p-fileIn;
			++p;
		}
		if (i) // если имя вх. файла содержит расширение
			result.insert(0, fileIn, i);
		else 
			result += fileIn;

		if (argc > 3) // если пользователь указал формат вых. файла
		{
			format = argv[3];
			for (decltype(format.size()) i=0; i < format.size(); ++i) format[i] = tolower(format[i]); // перевести в нижний регистр
		}
		else
			format = "pdf"; // формат по умолчанию

	remove((result + ".aux").c_str()); // файл сформированный pdflatex
        remove((result + ".log").c_str()); // удалить во избежание ошибок работы pdflatex
	remove((result + ".out").c_str());

	gen->genDocument(result, true, type, format); // запуск генератора документа
	remove((result + ".toc").c_str()); // удалить временный файл .toc

		// сформировать строку result - нужна теперь только для добавления к имени документа "Формуляр " и вывода сообщения пользователю в конце функции main
		if (format == "odt" || format == "pdf" || format == "doc"  || format == "docx")
		{
			std::string stemp2;
			if (type == "form") 
			{
				stemp2 = "Формуляр ";
			}
			stemp = stemp2 + result + "." + format;
			if(rename((result + "." + format).c_str(), stemp.c_str()))
			{
				puts("Внутренняя ошибка приложения.");
				return 1;
			}
			result = stemp;
		}
		else // если формат tex, html, xml и др., не добавлять к имени "Формуляр "
			{
				if (format == "xml")
					result += ".html";
				else
					if (format == "tei" || format == "db")
						result += ".xml";
					else
						if (format == "markdown")
							result += ".md";
						else
							result += ("." + format);
			}
		

	delete gen; // удалить генератор из памяти
	printf("Выполнено. Данные записаны в \"%s\"\n", result.c_str());
	return 0;
}
