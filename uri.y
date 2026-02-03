/**
URI BNF http://www.w3.org/Addressing/URL/5_URI_BNF.html
thanks
*/
%{

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define YYSTYPE double

int yylex(void);
void yyerror(const char* s);

%}
%%
/*
	fragmentaddress        uri [ # fragmentid ]

	uri                    scheme :  path [ ? search ]

	scheme                 ialpha

	path                   void |  xpalphas  [  / path ]

	search                 xalphas [ + search ]

	fragmentid             xalphas

	xalpha                 alpha | digit | safe | extra | escape

	xalphas                xalpha [ xalphas ]

	xpalpha                xalpha | +

	xpalphas               xpalpha [ xpalpha ]

	ialpha                 alpha [ xalphas ]

	alpha                  a | b | c | d | e | f | g | h | i | j | k |
	                      l | m | n | o  | p | q | r | s | t | u | v |
	                      w | x | y | z | A | B | C  | D | E | F | G |
	                      H | I | J | K | L | M | N | O | P |  Q | R |
	                      S | T | U | V | W | X | Y | Z

	digit                  0 |1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

	safe                   $ | - | _ | @ | . | &

	extra                  ! | * | " |  ' | ( | ) | ,

	reserved               = | ; | / | # | ? | : | space

	escape                 % hex hex

	hex                    digit | a | b | c | d | e | f | A | B | C |
	                      D | E | F

	national               { | } | vline | [ | ] | \ | ^ | ~

	punctuation            < | >

	void
*/
hex           : digit | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' |
                        'a' | 'b' | 'c' | 'd' | 'e' | 'f'
alphanum      : alpha | digit
alpha         : lowalpha | upalpha
lowalpha : 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' |
           'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' |
           's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z'
upalpha  : 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
           'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
           'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
digit    : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' |
           '8' | '9'
%%
	
