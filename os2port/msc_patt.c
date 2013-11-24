/**********************************************************

                Pattern and texture module

 **********************************************************/

#include "qrt.h"
#include "pattern.h"

 /* #define PATTERNDEBUG 1 */


/**********************************************************

    Given two coordinates on the surface of an object,
    and a pointer to a colorinfo structure, this routine
    fills the structure with the objects color info at
    this location. If the object does not have an associated
    pattern, its default colorinfo is returned.  The
    routine returns TRUE if the pattern is hit, FALSE
    otherwise.

 **********************************************************/

int Find_Color(obj, pattern, loc, cinfo, xmult, ymult)
  OBJ_PTR     obj;
  PATTERN_PTR pattern;
  VECT_PTR    loc;
  CINFO_PTR   cinfo;
  float       xmult, ymult;

{
  int           temp;         /* added to get around MSC 5.1 bug */
  PATTERN_PTR   patt;
  float         modpos1, modpos2, pos1, pos2;
  int           modx, mody;


  if (pattern==NULL) {                      /* no pattern */
    copy_colorinfo(cinfo,&(obj->cinfo));
    return(FALSE);
  }

# ifdef ROBUST
    if (pattern->type != PATT_HEADER)
      Error(INTERNAL_ERROR,1001);

    if ((xmult==0) || (ymult==0))
      Error(ZERO_MULTIPLIER,1002);
# endif

  /* find object relative position */

  (*(ObjData[obj->type].RelPos))(obj,loc,&pos1,&pos2,FALSE);

  pos1 /= xmult;                            /* x and y multipliers */
  pos2 /= ymult;                            /* for pattern sizing  */

  modx = (int)(pos1 / (pattern->xsize));
  if (pos1<0) modx--;

  mody = (int)(pos2 / (pattern->ysize));
  if (pos2<0) mody--;

  modpos1 = pos1 - ((float)modx * pattern->xsize);
  modpos2 = pos2 - ((float)mody * pattern->ysize);

# ifdef PATTERNDEBUG
    printf("FINDCOLOR: modpos1,2 = %f %f\n",modpos1, modpos2);
    printf("           mod x,y   = %d %d\n",modx, mody);
    printf("           mult1,2   = %f %f\n",xmult,ymult);
# endif

  patt = pattern->child;

  while (patt!=NULL) {

    THEWORLD.pattern_matches++;

    temp = (*(PattData[patt->type].PattHit))(modpos1, modpos2, patt);
    if (temp) {
      copy_colorinfo(cinfo,&(patt->cinfo));
      return(TRUE);
    }

    patt = patt->sibling;
  }

  copy_colorinfo(cinfo,&(obj->cinfo));
  return(FALSE);
}


/**********************************************************

         Determines if point is inside rectangle

 **********************************************************/

int Rect_Hit(pos1, pos2, patt)
  float pos1, pos2;
  PATTERN_PTR patt;
{

# ifdef PATTERNDEBUG
  printf("RECT_HIT: sx,y = %f %f\n",patt->startx,patt->starty);
  printf("          ex,y = %f %f\n",patt->endx,patt->endy);
# endif

# ifdef ROBUST
    if (patt->type != RECT_PATTERN)
      Error(INTERNAL_ERROR,1003);
# endif

  if ( (pos1 > patt->startx) &&
       (pos1 < patt->endx)   &&
       (pos2 > patt->starty) &&
       (pos2 < patt->endy) )      return(TRUE);

  return(FALSE);
}


/**********************************************************

         Determines if point is inside circle

 **********************************************************/

int Circle_Hit(pos1, pos2, patt)
  float pos1, pos2;
  PATTERN_PTR patt;
{
  float rad, a, b;

# ifdef ROBUST
    if (patt->type != CIRCLE_PATTERN)
      Error(INTERNAL_ERROR,1004);
# endif

  a = (pos1 - patt->radius);      /* bug in damn compiler  */
  b = (pos2 - patt->radius);      /* we have to break up   */
                                  /* long float operations */
  rad = sqr(a)+sqr(b);

# ifdef PATTERNDEBUG
    printf("CIRC_HIT\n");
    printf("  p1, p2 = %f %f\n",pos1,pos2);
    printf("  rad    = %f\n",patt->radius);
    printf(" crad    = %f\n",rad);
# endif

  if (rad <= sqr(patt->radius)) return(TRUE);

  return(FALSE);
}


/**********************************************************

   Determines if point px, py intersects line x1,y1,x2,y2.
   Returns true if it does.

 **********************************************************/

int line_intersect(px, py, x1, y1, x2, y2)
  float px, py, x1, y1, x2, y2;
{
  float t;

  x1 -= px; x2 -= px;           /* translate line */
  y1 -= py; y2 -= py;

  if ((y1 > 0.0) && (y2 > 0.0)) return(FALSE);
  if ((y1 < 0.0) && (y2 < 0.0)) return(FALSE);
  if ((x1 < 0.0) && (x2 < 0.0)) return(FALSE);

  if (y1 == y2) {
    if (y1 != 0.0) return(FALSE);
    if ((x1 > 0.0) || (x2 > 0.0)) return(FALSE);
    return(TRUE);
  }

  t = (-y1) / (y2 - y1);

  if ((x1 + t * (x2 - x1)) > 0.0) return(TRUE);
  return(FALSE);
}

/**********************************************************

         Determines if point is inside Polygon

 **********************************************************/

int Poly_Hit(pos1, pos2, patt)
  float pos1, pos2;
  PATTERN_PTR patt;
{
  float xpos, ypos, nxpos, nypos;
  PATTERN_PTR lseg;
  int count;

# ifdef ROBUST
    if (patt->type != POLY_PATTERN)
      Error(INTERNAL_ERROR,1005);
# endif

  lseg = patt->link;

  if (lseg == NULL) Error(INTERNAL_ERROR,1006);

  count = 0;

  while (lseg->link != NULL) {

     if (line_intersect( pos1, pos2,
                         lseg->startx, lseg->starty,
                         lseg->link->startx,
                         lseg->link->starty) )
       count++;

     lseg = lseg->link;
  }

  if ((count % 2) == 0) return(FALSE);
  return(TRUE);
}



