
/**********************************************************

  Line/Object intersection routines.  Routine returns
  TRUE if object is hit, along with the nearest of the
  possibly multiple intersections.  First parameter is
  line, second is object, and third is pointer to parameter
  on line for intersection (filled by routine).  Functions
  are pointed at by entries in ObjData structure.

 **********************************************************/

#include "qrt.h"

/* #define INTERSECTDEBUG */

/**********************************************************

          Line/bounding box intersection test.

   Looks bad, but its pretty fast.  Many times we don't
   even have to go through the whole routine if a miss
   can be detected early.

 **********************************************************/

int LineBbox(line, bbox, t)
  OBJ_PTR line, bbox;
  float *t;
{
  register float tminx, tmaxx, tminy, tmaxy,
                 tminz, tmaxz, tmin, tmax, t1,t2;

  /* this number is not really important, since nothing
     cares about the exact location of the intersection */

  *t=10;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,301);
    if (bbox->type!=BBOX) Error(INTERNAL_ERROR,302);
# endif

  if (fabs(line->vect1.x) < SMALL) {
    if ((bbox->lower.x < line->loc.x) &&
        (bbox->upper.x > line->loc.x)) {
          tminx = -BIG; tmaxx=BIG;
        } else return(FALSE);

  } else {

    t1 = (bbox->lower.x-line->loc.x)/line->vect1.x;
    t2 = (bbox->upper.x-line->loc.x)/line->vect1.x;
    tminx = MIN(t1,t2);
    tmaxx = MAX(t1,t2);
    if (tmaxx<0) return(FALSE);
  }

  if (fabs(line->vect1.y) < SMALL) {
    if ((bbox->lower.y < line->loc.y) &&
        (bbox->upper.y > line->loc.y)) {
          tminy = -BIG; tmaxy=BIG;
        } else return(FALSE);

  } else {

    t1 = (bbox->lower.y-line->loc.y)/line->vect1.y;
    t2 = (bbox->upper.y-line->loc.y)/line->vect1.y;
    tminy = MIN(t1,t2);
    tmaxy = MAX(t1,t2);
    if (tmaxy<0) return(FALSE);
  }

  if (fabs(line->vect1.z) < SMALL) {
    if ((bbox->lower.z < line->loc.z) &&
        (bbox->upper.z > line->loc.z)) {
          tminz = -BIG; tmaxz=BIG;
        } else return(FALSE);

  } else {

    t1 = (bbox->lower.z-line->loc.z)/line->vect1.z;
    t2 = (bbox->upper.z-line->loc.z)/line->vect1.z;
    tminz = MIN(t1,t2);
    tmaxz = MAX(t1,t2);
    if (tmaxz<0) return(FALSE);
  }

  tmin = MAX(MAX(tminx,tminy),tminz);
  tmax = MIN(MIN(tmaxx,tmaxy),tmaxz);

# ifdef INTERSECTDEBUG
    printf("LINEBBOX: dir = %f %f %f\n",line->vect1.x,
                                        line->vect1.y,
                                        line->vect1.z);

    printf("  tminx=%7.2f, tmaxx=%7.2f, tminy=%7.2f, tmaxy=%7.2f\n",
            tminx, tmaxx, tminy, tmaxy);
    printf("  tminz=%7.2f, tmaxz=%7.2f, tmin=%7.2f,  tmax=%f\n",
            tminz, tmaxz, tmin, tmax);
# endif

  if (tmax<0) return(FALSE);

  if (tmax<tmin) return(FALSE);

# ifdef INTERSECTDEBUG
    printf("  HIT:\n");
# endif

  return(TRUE);
}

/**********************************************************

               Line/ring intersection test

     Similar to, but slower than parallelogram test

     Changed 11 Mar 89 to fix bug with planar position
     computation.  Now uses Plane_Pos(), which is a
     little slower than before.

 **********************************************************/

int LineRing(line, ring, t)
  OBJ_PTR line, ring;
  float *t;
{
  VECTOR         loc;
  register float dot, rad;
  float          pos1, pos2;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,303);
    if (ring->type!=RING) Error(INTERNAL_ERROR,304);
# endif

  dot  = DotProd((ring->precomp.norm),line->vect1);

  if (fabs(dot)<SMALL) return(FALSE);

  pos1 = ring->precomp.n1;
  pos2 = DotProd((ring->precomp.norm),line->loc);

  *t=(pos1-pos2)/dot;

# ifdef INTERSECTDEBUG
    printf("LINERING: t=%f\n", *t);
# endif

  FindPos(&loc,line,*t);
  Plane_Pos(ring,&loc,&pos1,&pos2,TRUE);

  rad = sqrt(sqr(pos1)+sqr(pos2));

# ifdef INTERSECTDEBUG
    printf("LINERING  pos1,2 = %f %f\n",pos1,pos2);
    printf("          radius = %f\n",rad);
    printf("          R1,2   = %f %f\n",ring->vect3.x,ring->vect3.y);
# endif

  if (rad<(ring->vect3.x) || rad>(ring->vect3.y))
     return(FALSE);

# ifdef INTERSECTDEBUG
    printf("  HIT:\n");
# endif

  return(TRUE);
}


/**********************************************************

        Line/Parallelogram intersection test
        Returns Parameter T for intersection

        Changed 11 Mar 89 to fix bug with planar position
        computation.  Now uses Plane_Pos(), which is a
        little slower than before.

 **********************************************************/

int LineParallelogram(line, para, t)
  OBJ_PTR line, para;
  float *t;
{
  VECTOR  loc;
  float   dot, in1, in2;

# ifdef ROBUST
    if (line->type!=LINE)          Error(INTERNAL_ERROR,305);
    if (para->type!=PARALLELOGRAM) Error(INTERNAL_ERROR,306);
# endif

  dot = DotProd((para->precomp.norm),line->vect1);

  if (fabs(dot)<SMALL) return(FALSE);

  in1 = para->precomp.n1;
  in2 = DotProd((para->precomp.norm),line->loc);

  *t=(in1-in2)/dot;

# ifdef INTERSECTDEBUG
    printf("LINEPARALL: t=%f\n", *t);
# endif

  FindPos(&loc,line,*t);
  Plane_Pos(para,&loc,&in1,&in2,TRUE);

# ifdef INTERSECTDEBUG
    printf("LINEPARALL: in1,2 = %f %f\n",in1,in2);
# endif

  if (!((in1>=0) && (in2>=0) && (in1<=1) && (in2<=1)))
    return(FALSE);

# ifdef INTERSECTDEBUG
    printf("  HIT:\n");
# endif

  return(TRUE);
}


/**********************************************************

        Line/Triangle intersection test
        Returns Parameter T for intersection

        Changed 11 Mar 89 to fix bug with planar position
        computation. Now uses Plane_Pos(), which is a
        little slower than before.

 **********************************************************/

int LineTriangle(line, obj, t)
  OBJ_PTR line, obj;
  float *t;
{
  VECTOR         loc;
  register float dot;
  float          in1, in2;

# ifdef ROBUST
    if (line->type != LINE)      Error(INTERNAL_ERROR,307);
    if (obj->type  != TRIANGLE)  Error(INTERNAL_ERROR,308);
# endif

  dot = DotProd((obj->precomp.norm),line->vect1);

  if (fabs(dot)<SMALL) return(FALSE);

  in1 = obj->precomp.n1;
  in2 = DotProd((obj->precomp.norm),line->loc);

  *t=(in1-in2)/dot;

# ifdef INTERSECTDEBUG
    printf("LINETRIANGLE: t=%f\n", *t);
# endif

  FindPos(&loc,line,*t);
  Plane_Pos(obj,&loc,&in1,&in2,TRUE);

# ifdef INTERSECTDEBUG
    printf("LINETRIANGLE: in1,2 = %f %f\n",in1,in2);
# endif

  if (!((in1>=0) && (in2>=0) && (in1+in2<=1)))
    return(FALSE);

# ifdef INTERSECTDEBUG
    printf("  HIT:\n");
# endif

  return(TRUE);
}


/**********************************************************

           Line/sphere intersection test
        Returns parameter T for intersection

 **********************************************************/

int LineSphere(line, sph, t)
  OBJ_PTR line, sph;
  float *t;
{
  register float a,b,c,d,t1, tmpx,tmpy,tmpz;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,309);
    if (!(sph->type==SPHERE || sph->type==LAMP))
      Error(INTERNAL_ERROR,310);
# endif

  tmpx = sph->loc.x-line->loc.x;
  tmpy = sph->loc.y-line->loc.y;
  tmpz = sph->loc.z-line->loc.z;

  c = sqr(tmpx)+ sqr(tmpy)+ sqr(tmpz) - (sph->precomp.n1);

  b = -2*(line->vect1.x*tmpx+                      /* find b */
          line->vect1.y*tmpy+
          line->vect1.z*tmpz);

  a = sqr(line->vect1.x)+                          /* find a */
      sqr(line->vect1.y)+
      sqr(line->vect1.z);

  d = sqr(b)-4.0*a*c;

# ifdef INTERSECTDEBUG
    printf("LINESPHERE: a=%f, b=%f, c=%f, d=%f\n",a,b,c,d);
# endif

  if (d<=0) return(FALSE);                          /* does sphere hit? */

  d=sqrt(d); *t=(-b+d)/(a+a);
             t1=(-b-d)/(a+a);

  if (t1<*t && t1>SMALL) *t=t1;                     /* find 1st collision */

  if (*t > SMALL) {

#   ifdef INTERSECTDEBUG
      printf("LINESPHERE: collision @ t=%f\n",*t);
#   endif

    return(TRUE);
  }

  return(FALSE);
}


/**********************************************************

        Line/quadratic intersection test
        Returns parameter T for intersection

  newline is the input line translated and rotated so that
  the quadratic is @ 0,0,0 and pointed up.

 **********************************************************/

int LineQuadratic(line, quad, t)
  OBJ_PTR line, quad;
  float *t;
{
  register   float a,b,c,d, t1;
  VECTOR     loc, loc1, tempdir;
  OBJ_STRUCT newline;

# ifdef ROBUST
    if (line->type!=LINE)      Error(INTERNAL_ERROR,311);
    if (quad->type!=QUADRATIC) Error(INTERNAL_ERROR,312);
# endif

  newline.type=LINE;

  /*** translation transformation to newpos for line ***/

  VecSubtract(&(newline.loc),&(line->loc),&(quad->loc));

  if ((quad->vect1.x == 0) &&                  /* no rotation  */
      (quad->vect1.y == 1) &&                  /* if aligned   */
      (quad->vect1.z == 0))   {

      VectEQ(&(newline.vect1),&(line->vect1));

  } else {                                     /* here we must rot */

#   ifdef INTERSECTDEBUG

      printf("LINEQUADRATIC 0\n");
      printf("  inline.loc  = %f %f %f\n",    line->loc.x,
                                              line->loc.y,
                                              line->loc.z);

      printf("  inline.dir  = %f %f %f\n",    line->vect1.x,
                                              line->vect1.y,
                                              line->vect1.z);

      printf("  newline.loc = %f %f %f\n",    newline.loc.x,
                                              newline.loc.y,
                                              newline.loc.z);
#   endif

    Rot12( &(line->vect1),&(newline.vect1),    /* rot view direction */
           quad->precomp.cos1,
           quad->precomp.sin1,
           quad->precomp.cos2,
           quad->precomp.sin2 );

    Rot12( &(newline.loc),&(newline.loc),      /* rotate view location */
           quad->precomp.cos1,
           quad->precomp.sin1,
           quad->precomp.cos2,
           quad->precomp.sin2 );

#   ifdef INTERSECTDEBUG

      printf("LINEQUADRATIC 1\n");
      printf("  inline.loc  = %f %f %f\n",    line->loc.x,
                                              line->loc.y,
                                              line->loc.z);

      printf("  inline.dir  = %f %f %f\n",    line->vect1.x,
                                              line->vect1.y,
                                              line->vect1.z);

      printf("  newline.loc = %f %f %f\n",    newline.loc.x,
                                              newline.loc.y,
                                              newline.loc.z);

      printf("  newline.dir = %f %f %f\n",    newline.vect1.x,
                                              newline.vect1.y,
                                              newline.vect1.z);
#   endif

  }

  c = -(quad->cterm) + quad->vect2.x * sqr(newline.loc.x) +
                       quad->vect2.y * sqr(newline.loc.y) +
                       quad->vect2.z * sqr(newline.loc.z);

  b = 2*( quad->vect2.x * newline.loc.x * newline.vect1.x +
          quad->vect2.y * newline.loc.y * newline.vect1.y +
          quad->vect2.z * newline.loc.z * newline.vect1.z);

  a = quad->vect2.x * sqr(newline.vect1.x) +
      quad->vect2.y * sqr(newline.vect1.y) +
      quad->vect2.z * sqr(newline.vect1.z);

  d = sqr(b)-4.0*a*c;

# ifdef INTERSECTDEBUG
    printf("LINEQUADRATIC 2: a=%f, b=%f, c=%f, d=%f\n",a,b,c,d);

    printf("               newpos = %f %f %f\n",newline.loc.x,
                                                newline.loc.y,
                                                newline.loc.z);

    printf("               newdir = %f %f %f\n",newline.vect1.x,
                                                newline.vect1.y,
                                                newline.vect1.z);
# endif

  if (d<0) return(FALSE);                           /* we missed it */


  d=sqrt(d); *t=(-b+d)/(a+a);
             t1=(-b-d)/(a+a);

  FindPos(&loc,&newline,*t);                        /* find locations */
  FindPos(&loc1,&newline,t1);


  if ((loc.x < quad->lower.x) ||                    /* 1st in range ? */
      (loc.x > quad->upper.x) ||
      (loc.y < quad->lower.y) ||
      (loc.y > quad->upper.y) ||
      (loc.z < quad->lower.z) ||
      (loc.z > quad->upper.z))    *t = -1;

  if ((loc1.x < quad->lower.x) ||                   /* 2nd in range ? */
      (loc1.x > quad->upper.x) ||
      (loc1.y < quad->lower.y) ||
      (loc1.y > quad->upper.y) ||
      (loc1.z < quad->lower.z) ||
      (loc1.z > quad->upper.z))   t1 = -1;

# ifdef INTERSECTDEBUG
    printf("    t,t1 = %f %f\n",*t,t1);
# endif

  if (*t<=SMALL && t1 <=SMALL) return(FALSE);

  if (*t<=SMALL && t1>SMALL) *t=t1;
  if (t1<*t && t1>SMALL)     *t=t1;   /* find 1st collision */

  if (*t>SMALL) {

#   ifdef INTERSECTDEBUG
      printf("LINEQUADRATIC: collision @ t=%f\n",*t);
#   endif

    return(TRUE);
  }

  return(FALSE);
}

