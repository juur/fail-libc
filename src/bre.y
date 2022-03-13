%{
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <err.h>
#include <stdlib.h>

#include "bre.tab.h"
#include "bre_priv.h"

extern int yylex(YYSTYPE *, void *, void **);
extern int yyget_lineno (void *yyscanner );

%}

%lex-param {void *scanner} {void **result}
%parse-param {void *scanner} {void **result}
%pure-parser
%token-table

%token  L_ANCHOR R_ANCHOR SPEC_CHAR
%token  Back_open_paren  Back_close_paren
%token  Back_open_brace  Back_close_brace
%token  Open_equal Equal_close Open_dot Dot_close Open_colon Colon_close

%token  <chr> ORD_CHAR 
%token  <str> QUOTED_CHAR 
%token  <val> DUP_COUNT 
%token  <val> BACKREF 
%token  <chr> COLL_ELEM_SINGLE 
%token  <str> COLL_ELEM_MULTI 
%token  <chr> META_CHAR
%token  <str> CLASS_NAME

%type   <str> character_class
%type   <str> equivalence_class
%type	<str> collating_symbol
%type   <node> start_range end_range
%type   <node> range_expression expression_term single_expression 
%type   <node> follow_list bracket_list nonmatching_list matching_list bracket_expression
%type   <node> RE_dupl_symbol nondupl_RE one_char_or_coll_elem_RE
%type   <node> basic_reg_exp RE_expression simple_RE

%start  basic_reg_exp

%union {
	char    chr;
	char   *str;
	long	val;

	struct node *node;
}

%%

basic_reg_exp  :          RE_expression				{ $$=newNode(N_ROOT); $$->arg0=first_node($1); *result=$$;  }
               | L_ANCHOR							{ $$=newNode(N_ROOT); $$->num=1; *result=$$;   }
               |                        R_ANCHOR	{ $$=newNode(N_ROOT); $$->num=2; *result=$$;   }
               | L_ANCHOR               R_ANCHOR	{ $$=newNode(N_ROOT); $$->num=1+2; *result=$$; }
               | L_ANCHOR RE_expression				{ $$=newNode(N_ROOT); $$->arg0=first_node($2); $$->num=1; *result=$$;   }
               |          RE_expression R_ANCHOR	{ $$=newNode(N_ROOT); $$->arg0=first_node($1); $$->num=2; *result=$$;   }
               | L_ANCHOR RE_expression R_ANCHOR	{ $$=newNode(N_ROOT); $$->arg0=first_node($2); $$->num=1+2; *result=$$; }
               ;

RE_expression  :               simple_RE	{ $$=$1; }
               | RE_expression simple_RE	{ $1->next=$2; $2->prev=$1; $$=$2; }
               ;

simple_RE      : nondupl_RE					{ $$=newNode(N_SIMPLE); $$->num=0; $$->arg0=$1; }
               | nondupl_RE RE_dupl_symbol	{ $$=newNode(N_SIMPLE); $$->num=1; $$->arg0=$1; $$->arg1=$2; }
               ;

nondupl_RE     : one_char_or_coll_elem_RE						{ $$=$1; }
               | Back_open_paren RE_expression Back_close_paren	{	
																	$$=newNode(N_MATCH);
																	$$->arg0=first_node($2);
																	$$->num=++backref_cnt;
																	if (backref_cnt>9)
																		errx(EXIT_FAILURE, "%d: Too many back references", yyget_lineno(scanner));
																}
               | BACKREF										{	
																	$$=newNode(N_BACKREF);
																	$$->num=$1;
																	if ($1 < 1 || $1 > 9)
																		errx(EXIT_FAILURE, "%d: Invalid back reference", yyget_lineno(scanner));
																}
               ;

one_char_or_coll_elem_RE : ORD_CHAR		{ $$=newNode(N_CHAR); $$->chr=$1;    $$->num=0; }
               | QUOTED_CHAR			{ $$=newNode(N_CHAR); $$->chr=$1[2]; $$->num=1; }
               | '.'					{ $$=newNode(N_CHAR); $$->chr=0;     $$->num=2; }
               | bracket_expression     { $$=$1; }
               ;

RE_dupl_symbol : '*'														{ $$=newNode(N_DUPL); $$->subtype = N_DUPL_ALL; }
               | Back_open_brace DUP_COUNT               Back_close_brace	{ $$=newNode(N_DUPL); $$->num=$2; $$->subtype = N_DUPL_N;}
               | Back_open_brace DUP_COUNT ','           Back_close_brace	{ $$=newNode(N_DUPL); $$->num=$2; $$->subtype = N_DUPL_NP;}
               | Back_open_brace DUP_COUNT ',' DUP_COUNT Back_close_brace	{ $$=newNode(N_DUPL); $$->num=$2; $$->to=$4; $$->subtype = N_DUPL_N2M; }
			   ;

bracket_expression : '[' matching_list ']'		{ $2->num&=~(2); $$=$2; }
                   | '[' nonmatching_list ']'	{ $2->num|=2;    $$=$2; }
               ;

matching_list  : bracket_list				{ $$=$1; }
               ;

nonmatching_list : '^' bracket_list			{ $$=$2; }
               ;
bracket_list   : follow_list				{ $$=newNode(N_LIST); $$->arg0=first_node($1); $$->num&=~(1); }
               | follow_list '-'			{ $$=newNode(N_LIST); $$->arg0=first_node($1); $$->num|=1; }
               ;

follow_list    :             expression_term { $$=$1; }
               | follow_list expression_term { $1->next=$2; $2->prev=$1; $$=$2; }
               ;

expression_term : single_expression	{ $$=newNode(N_EXP); $$->arg0=$1; }
                | range_expression  { $$=$1; }
                ;

/* TODO: this should wrap in something to indicate it is a class or not vs. raw string */
single_expression : end_range			{ $$=$1; }
                  | character_class		{ $$=newNode(N_CLASS); $$->num=2; $$->str=$1; }
                  | equivalence_class	{ $$=newNode(N_CLASS); $$->num=3; $$->str=$1; }
                  ;
range_expression : start_range end_range { 
											$$=newNode(N_RANGE); 
											$$->arg0 = $1;
											$$->arg1 = $2;
										 }
                 | start_range '-'		 {	
											$$=newNode(N_RANGE); 
											$$->arg0 = $1;
											$$->num|=1;
										 }
                 ;
start_range    : end_range '-'		{ $$=$1; }
               ;
end_range      : COLL_ELEM_SINGLE	{ $$=newNode(N_CHAR);  $$->chr=$1; }
               | collating_symbol	{ $$=newNode(N_CLASS); $$->num=1; $$->str=$1; }
               ;
collating_symbol : Open_dot COLL_ELEM_SINGLE Dot_close	{ $$=strdup((char[2]){$2,0}); }
               | Open_dot COLL_ELEM_MULTI Dot_close		{ $$=$2; }
               | Open_dot META_CHAR Dot_close			{ $$=strdup((char[2]){$2,0}); }
               ;
equivalence_class : Open_equal COLL_ELEM_SINGLE Equal_close { $$=strdup((char[2]){$2,0}); }
                  | Open_equal COLL_ELEM_MULTI Equal_close     { $$=$2; }
                  ;
character_class : Open_colon CLASS_NAME Colon_close { $$=$2; }
               ;

%%

int yywrap()
{
	return 1;
}

void yyerror(void *scanner, void **result, char *const s)
{
	err(EXIT_FAILURE, "grammar error: %d:%s\n", yyget_lineno(scanner), s);
}
