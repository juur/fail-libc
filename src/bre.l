%{
#define _XOPEN_SOURCE 700
#include "bre.tab.h"
%}

%x BRACKET MID_BRACKET END_BRACKET DUP CHR_CLASS EQ_CLASS COL_CLASS
%x DUP_STATE

%option noyywrap warn stack debug reentrant bison-bridge yylineno

%%

<BRACKET>{
	"^"		{ 
				yy_pop_state(yyscanner);
				yy_push_state(MID_BRACKET, yyscanner);
				return '^'; 
			}
	.		{
				yy_pop_state(yyscanner);
				yy_push_state(MID_BRACKET, yyscanner);
				yyless(0);
			}
}

<MID_BRACKET>{
	
	"-"					{ return '-';		}
	"."|"\\"|"["|"*"	{ 
							yylval->chr = *yytext; 
							return ORD_CHAR;	
						}
	"]"					{ 
							yy_pop_state(yyscanner);
							return ']';	
						}
}

<MID_BRACKET>{
	"[="	{ yy_push_state(EQ_CLASS, yyscanner);  return Open_equal;  }
	"[."	{ yy_push_state(COL_CLASS, yyscanner); return Open_dot;    }
	"[:"	{ yy_push_state(CHR_CLASS, yyscanner); return Open_colon;  }
}

<MID_BRACKET,COL_CLASS,EQ_CLASS>{
	.		{
				yylval->chr = *yytext;
				return COLL_ELEM_SINGLE;
			}
}


<CHR_CLASS>[a-zA-Z]+	{ 
						yylval->str = strdup(yytext);
						return CLASS_NAME; 
					}


<EQ_CLASS>{
	"=]"	{ yy_pop_state(yyscanner); return Equal_close; }
}
<COL_CLASS>{
	".]"	{ yy_pop_state(yyscanner); return Dot_close;   }
}
<CHR_CLASS>{
	":]"	{ yy_pop_state(yyscanner); return Colon_close; }
}


^"^"	{ return L_ANCHOR; }
"$"$	{ return R_ANCHOR; }

"\\("	{ return Back_open_paren;  }
"\\)"	{ return Back_close_paren; }
"\\{"	{ yy_push_state(DUP, yyscanner); return Back_open_brace;  }
<DUP>"\\}"	{ yy_pop_state(yyscanner); return Back_close_brace; }

"."		{ return '.'; }
"\\"	{ return SPEC_CHAR; }
"*"		{ return '*'; }
"["		{ 
			yy_push_state(BRACKET, yyscanner);
			return '['; 
		}

<DUP>[0-9]+	{ yylval->val = atol(yytext); return DUP_COUNT; }
<DUP>","	{ return ',';       }

\\[0-9]	{ yylval->val = atol(yytext+1); return BACKREF;     }


"\\^"	{ return QUOTED_CHAR; }
"\\."	{ return QUOTED_CHAR; }
"\\*"	{ return QUOTED_CHAR; }
"\\["	{ return QUOTED_CHAR; }
"\\$"	{ return QUOTED_CHAR; }
"\\\\"	{ return QUOTED_CHAR; }


.	{ 
		yylval->chr = *yytext;
		return ORD_CHAR;    
	}

	/*
<*>"\n"	{
	return 0;
}

<*><<EOF>> {
			return 0;
		}
*/
