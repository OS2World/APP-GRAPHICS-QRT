
/**********************************************************

      Small lexical analyser for inout. Also contains
      some bounds checking code.

 **********************************************************/


#include "qrt.h"

extern int linenumber;
char *malloc();

# ifdef AMIGA
    float atof();
# endif

# ifdef UNIX
#   include <string.h>
# endif

/**********************************************************

           Transforms all white spaces to blanks

    - changed 13 Mar 89 to handle tabs in input file

 **********************************************************/

char towhite(c) char c; {
  if (c=='\n') linenumber++;

  if ((c=='\n') ||
      (c=='\t') ||
      (c=='=')  ||
      (c==',')  ||
      (c==';'))     return(' ');

  return(c);
}


/**********************************************************

        Removes blank space before next token

 **********************************************************/

rmspace() {
  char c;
  while (((c=towhite(fgetc(stdin)))==' ') && !feof(stdin));
  ungetc(c,stdin);
}

/**********************************************************

 Comment Killer - Added 16 Jun 88 to handle nested comments

 **********************************************************/

Comment_Killer()
{
  char c;

  c='\0';
  while (c != '}' && !feof(stdin)) {
    c = towhite(fgetc(stdin));
    if (c == '{') Comment_Killer();
  }
}

/**********************************************************

               Get next token from stdin

   - Changed 12 Mar 89 to fix compatibility problem with
     toupper() across different versions of UN*X.
     Aparently some versions change a letter even if it is
     already upper case.

 **********************************************************/

GetToken(s)                          /* get a token from stdio */
  char s[];
{                                    /* */
  char c; int x;

  x=0; s[0]=c='\0';                       /* char count */

  rmspace();

  while (!feof(stdin) && x<(SLEN-1)) {

    c = towhite(fgetc(stdin));
    if ('a'<=c && c<='z') c=toupper(c);
    s[x++]=c;

    if (c==' ') { s[--x]='\0'; break; }
    if (c=='(' || c==')') {
      if (x==1) { s[x]='\0'; break; }
      else {
        ungetc(c,stdin); s[--x]='\0'; break;
      }
    }
    if (c=='{') {
      x--;
      Comment_Killer();
      rmspace();
    }
  }
}


/**********************************************************

      Return value if color is in range 0<=cnum<=CNUM
      otherwise call error routine.

 **********************************************************/

float InRange(cnum)
  float cnum;
{
  if (cnum>=0.0 && cnum<=1.00) return(cnum);
  Error(COLOR_VALUE_ERR,1501);

  return(0);   /* this is to keep lint happy! */
}


/**********************************************************

         Return value if value is >=0.
         otherwise call error routine.

 **********************************************************/

float IsPos(val)
  float val;
{
  if (val >= 0.0) return(val);
  Error(LESS_THAN_ZERO,1502);

  return(0);   /* this is to keep lint happy! */
}

/**********************************************************

    Reads next number and converts to float from string

 **********************************************************/

float Get_Next_Num() {
  char str[SLEN];
  float val;

  GetToken(str);

  val=atof(str);

# ifdef IODEBUG
    printf("GETNEXTNUM: token=%s, val=%f\n",str,val);
# endif

  return(val);
}


/**********************************************************

      Reads a name from input, and allocates some space
      for it. Returns a pointer to space.

 **********************************************************/

char *Get_Next_Name() {
  char str[SLEN], *s;

  GetToken(str);

  if ((s=malloc(strlen(str)+1))==NULL)
    Error(MALLOC_FAILURE,1503);

  strcpy(s,str);

# ifdef IODEBUG
    printf("GETNEXTNAME: token=%s\n",str);
# endif

  return(s);
}

/**********************************************************

     Reads a number 0..1 and returns a color value
     0..CNUM;

 **********************************************************/

short Get_Color_Val() {
  return((short)(InRange(Get_Next_Num())*(float)CNUM));
}


/**********************************************************

       Returns true if the next token is a left paren

 **********************************************************/

GetLeftParen() {
  char str[SLEN];

  GetToken(str);
  if (strcmp(str,"(")!=0) Error(LPAREN_EXPECTED,1504);
  return;
}


/**********************************************************

       Returns true if the next token is a left paren

 **********************************************************/

int GetRightParen() {
  char str[SLEN];

  GetToken(str);
  if (strcmp(str,")")!=0) return(FALSE);
  return(TRUE);
}


/**********************************************************

           Gets a VECTOR structure of the form
           (num1, num2, num3)

 **********************************************************/

GetVector(vector)
  VECT_PTR vector;
{
  GetLeftParen();

  vector->x = Get_Next_Num();
  vector->y = Get_Next_Num();
  vector->z = Get_Next_Num();

  if(!GetRightParen()) Error(ILLEGAL_VECTOR,1505);
}


/**********************************************************

           Gets a SVECTOR structure of the form
           (num1, num2, num3)

 **********************************************************/

GetSVector(svector)
  SVECT_PTR svector;
{
  GetLeftParen();

  svector->r = Get_Color_Val();
  svector->g = Get_Color_Val();
  svector->b = Get_Color_Val();

  if(!GetRightParen()) Error(ILLEGAL_SVECTOR,1506);
}


