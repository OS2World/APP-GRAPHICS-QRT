
/**********************************************************

   This module loads patterns and sub-patterns for the
   inout.c file, which was getting inconvienently big.

 **********************************************************/


#include "qrt.h"
#include "pattern.h"

char *Get_Next_Name();
float Get_Next_Num(), IsPos();

/**********************************************************

     Attaches a pattern pointer to an object given
     pattern name.

 **********************************************************/

PATTERN_PTR Attach_Pattern()
{
  char *patname;
  PATTERN_PTR find_pat(), pat;

  patname=Get_Next_Name();
  pat = find_pat(patname);
  if (pat==NULL) Error(PATTERN_NOT_FOUND,1101);
  free(patname);
  return(pat);
}


/**********************************************************

   Load a circle sub-pattern and return pointer to it

 **********************************************************/

PATTERN_PTR Get_Circle_Pattern()
{
  char str[SLEN];
  int end, f, found;
  PATTERN_PTR pattern, new_pat();
  CINFO cinfo;

  end=f=0;
  pattern=new_pat();
  pattern->type = CIRCLE_PATTERN;
  def_colorinfo(&cinfo);

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str); found = TRUE;

    if (strcmp(str,",")!=0) {
      found = GetOpt(str,&cinfo);
      if (strcmp(str,"RADIUS")==0) {
        pattern->radius = IsPos(Get_Next_Num());
        f|=1; found = TRUE;
      }
      if (strcmp(str,")")==0) { end=1; found = TRUE; }
    }
    if (!found) Error(UNDEFINED_PARAM,1102);

  }

  if (f!=1) Error(TOO_FEW_PARMS,1103);

  copy_colorinfo(&(pattern->cinfo),&cinfo);

  return(pattern);
}

/**********************************************************

   Load a rectangle sub-pattern and return pointer to it

 **********************************************************/

PATTERN_PTR Get_Rect_Pattern()
{
  char str[SLEN];
  int end, f, found;
  PATTERN_PTR pattern, new_pat();
  CINFO cinfo;

  end=f=0;
  pattern=new_pat();
  pattern->type = RECT_PATTERN;
  def_colorinfo(&cinfo);

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str); found = TRUE;

    if (strcmp(str,",")!=0) {
      found = GetOpt(str,&cinfo);
      if (strcmp(str,"START_X")==0) {
        pattern->startx=Get_Next_Num();
        f|=1; found = TRUE;
      }
      if (strcmp(str,"START_Y")==0) {
        pattern->starty=Get_Next_Num();
        f|=2; found = TRUE;
      }
      if (strcmp(str,"END_X")==0) {
        pattern->endx=Get_Next_Num();
        f|=4; found = TRUE;
      }
      if (strcmp(str,"END_Y")==0) {
        pattern->endy=Get_Next_Num();
        f|=8; found = TRUE;
      }
      if (strcmp(str,")")==0) { end=1; found = TRUE; }
    }
    if (!found) Error(UNDEFINED_PARAM,1104);

  }

  if (f!=15) Error(TOO_FEW_PARMS,1105);

  copy_colorinfo(&(pattern->cinfo),&cinfo);

  return(pattern);
}

/**********************************************************

   Load a polygon sub-pattern and return pointer to it

 **********************************************************/

PATTERN_PTR Get_Poly_Pattern()
{
  char str[SLEN];
  int end, f, found;
  PATTERN_PTR pattern, pointpatt, new_pat();
  CINFO cinfo;

  end=f=0;
  pattern=new_pat();
  pattern->type = POLY_PATTERN;
  def_colorinfo(&cinfo);

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str); found = TRUE;

    if (strcmp(str,",")!=0) {
      found = GetOpt(str,&cinfo);
      if (strcmp(str,"POINT")==0) {
        GetLeftParen();

        pointpatt = new_pat();
        pointpatt->startx = Get_Next_Num();
        pointpatt->starty = Get_Next_Num();
        f+=1; found = TRUE;
        GetToken(str);

        if (strcmp(str,")") != 0) {
          Error(RPAREN_EXPECTED,1106);
        }

        str[0] = '\0';                 /* clear str */

        pointpatt->link = pattern->link;
        pattern->link = pointpatt;
      }
      if (strcmp(str,")")==0) { end=1; found = TRUE; }
    }
    if (!found) Error(UNDEFINED_PARAM,1107);
  }

  if (f<3) Error(TOO_FEW_PARMS,1108);

  copy_colorinfo(&(pattern->cinfo),&cinfo);

  return(pattern);
}


/**********************************************************

       Load a sub-pattern and return a pointer to it,
       or null if not found.

 **********************************************************/

PATTERN_PTR Get_SubPattern(str)
  char *str;
{
  if (strcmp(str,"RECTANGLE")==0)
    return(Get_Rect_Pattern());

  if (strcmp(str,"CIRCLE")==0)
    return(Get_Circle_Pattern());

  if (strcmp(str,"POLYGON")==0)
    return(Get_Poly_Pattern());

  return(NULL);
}


/**********************************************************

        Load pattern and attach it to pattern list

 **********************************************************/

int GetPattern()
{
  char str[SLEN];
  int end, f, found;
  PATTERN_PTR pattern, spat, new_pat();

  end=f=0;
  GetLeftParen();

  pattern = new_pat();

  pattern->type = PATT_HEADER;

  while (!end && !feof(stdin)) {
    GetToken(str); found = TRUE;
    if (strcmp(str,",")!=0) {
      found = FALSE;
      if (strcmp(str,"X_SIZE")==0) {
         pattern->xsize=Get_Next_Num();
         f|=1; found = TRUE;
      }
      if (strcmp(str,"Y_SIZE")==0) {
         pattern->ysize=Get_Next_Num();
         f|=2; found = TRUE;
      }
      if (strcmp(str,"NAME")==0) {
         pattern->name=Get_Next_Name();
         f|=4; found = TRUE;
      }

      if ((spat=Get_SubPattern(str))!=NULL) {
         spat->sibling=pattern->child;
         pattern->child=spat;
         f|=8; found = TRUE;
      }
      if (strcmp(str,")")==0) { end=1; found = TRUE; }
    }
    if (!found) Error(UNDEFINED_PARAM,1109);
  }

  if (f!=15) Error(TOO_FEW_PARMS,1110);

  pattern->sibling=THEWORLD.patlist;
  THEWORLD.patlist=pattern;

  return(TRUE);
}


