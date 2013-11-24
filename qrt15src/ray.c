/************************************************************

                     ray trace module

 ************************************************************/

#include <math.h>
#include "qrt.h"

/* #define RAYDEBUG 1 */

OBJ_PTR new_line();


/**********************************************************

  Diffuse color module. Also compute specular reflections
  here for efficiency sake - only one lamp eneumerator
  needed that way.

  - Changed 12 Mar 89 to fix bug where sreflect instead of
    reflect was being tested.  This caused the ray tracer
    to go much slower than needed!!

 **********************************************************/

DiffColor(color,cinfo,norm,loc,oline)
  SVECT_PTR color;
  VECT_PTR norm,loc;
  CINFO_PTR cinfo;
  OBJ_PTR oline;
{
  OBJ_PTR lamp, line, CurrObj, Ray_Hit();
  float t;
  register float dist, t1,t2;
  VECTOR refl, atten;

  line=new_line();
  VectEQ(&(line->loc),loc);
  lamp=THEWORLD.lamps;

  while (lamp!=NULL) {

#   ifdef ROBUST
      if (lamp->type!=LAMP) Error(INTERNAL_ERROR,601);
#   endif

    /* find dir to lamp */
    VecSubtract(&(line->vect1),&(lamp->loc),loc);

    CurrObj=NULL;
    atten.x = atten.y = atten.z = 1.00;

    if (def.shadow==TRUE) {

      /* set light attenuation factor */

      CurrObj=Ray_Hit(THEWORLD.stack,line,&t,TRUE,TRUE,&atten);

      THEWORLD.to_lamp++;
    }

    if (CurrObj==NULL) {                         /* hit nothing ? */

      dist = DotProd((line->vect1),(line->vect1));
      Normalize(&(line->vect1));

      if (cinfo->diff.r>0 || cinfo->diff.g>0 || cinfo->diff.b>0) {
        t1 = DotProd((*norm),(line->vect1));

        if (t1>0) {
          t2 = lamp->vect1.y*t1/sqrt(dist);

          color->r+=(short)((lamp->cinfo.amb.r) * atten.x *
                    t2*(cinfo->diff.r)/CNUM);
          color->g+=(short)((lamp->cinfo.amb.g) * atten.y *
                    t2*(cinfo->diff.g)/CNUM);
          color->b+=(short)((lamp->cinfo.amb.b) * atten.z *
                    t2*(cinfo->diff.b)/CNUM);
        }
      }

      if (cinfo->reflect>0) {                  /* specular */
        line->vect1.x = -line->vect1.x;        /* reverse line */
        line->vect1.y = -line->vect1.y;
        line->vect1.z = -line->vect1.z;
        Reflect(&refl,&(line->vect1),norm);
        t1 = -DotProd(refl,(oline->vect1));
        if (t1>SMALL) {

          t2 = pow(t1,cinfo->sreflect)*        /* this is slow !! */
               lamp->vect1.y/sqrt(dist);

          t2 *= (float)cinfo->reflect/(float)CNUM;

          color->r+=(short)(t2 * atten.x * (lamp->cinfo.amb.r));
          color->g+=(short)(t2 * atten.y * (lamp->cinfo.amb.g));
          color->b+=(short)(t2 * atten.z * (lamp->cinfo.amb.b));
        }
      }
    }
    lamp=lamp->nextobj;
  }

# ifdef RAYDEBUG
    printf("DIFFCOLOR: r=%d, g=%d, b=%d\n",
            color->r, color->g, color->b);
# endif

  free(line);
}


/**********************************************************

                     Ambient color module

   Really simple - just add the color, no questions asked.

 **********************************************************/

AmbColor(color, cinfo, norm, loc)
  SVECT_PTR color;
  VECT_PTR norm, loc;
  CINFO_PTR cinfo;
{
  color->r+=(short)((int)cinfo->amb.r*(int)cinfo->diff.r/CNUM);
  color->g+=(short)((int)cinfo->amb.g*(int)cinfo->diff.g/CNUM);
  color->b+=(short)((int)cinfo->amb.b*(int)cinfo->diff.b/CNUM);

# ifdef RAYDEBUG
    printf("AMBCOLOR: added r=%d, g=%d, b=%d\n",
            cinfo->amb.r,cinfo->amb.g,cinfo->amb.b);
# endif
}


/**********************************************************

                 Transmitted color module

 This is hairy and slow - bends ray around normal vector
 by ratio of indicies of refraction.  line->flag=TRUE
 if we are inside a piece of glass - this means glass must
 have a simple structure.

 10 Aug 88 - Fixed TransColor so that it works.  The
 original algorithm came from a book, and I had some
 problems getting it to work.  This one came from my head,
 and seems to work ok.

 11 Aug 88 - Added density effects (light is reduced
 more by thicker glass).

 **********************************************************/

/* #define TESTTRANS TRUE */

TransColor(color,cinfo,norm,loc,line,inmult)
  SVECT_PTR color;
  VECT_PTR norm,loc;
  OBJ_PTR line;
  CINFO_PTR cinfo;
  float inmult;
{
  OBJ_STRUCT newline;
  SVECTOR    col1;
  VECTOR     toadd, negvect1, distance;
  float      index2, multiplier, side1;
  int        maxtrans;

  /* these vars are used to keep track of the distance
     traveled through the glass so we can attenuate the
     light accordingly.                                 */

  static     VECTOR prevpos;
  VECTOR     displacement;
  SVECTOR    attenuation;
  float      glassdist;

# ifdef TRANSDEBUG
    printf("TRANSCOLOR:\n");
# endif
# ifdef RAYDEBUG
    printf("TRANSCOLOR:\n");
# endif

  if ((cinfo->trans.r < def.ithreshold) &&
      (cinfo->trans.g < def.ithreshold) &&
      (cinfo->trans.b < def.ithreshold)) return;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,602);
    if ((THEWORLD.globindex == 0) ||
        (cinfo->index == 0))
          Error(ZERO_INDEX,603);
# endif

  if (line->flag) {

    /* inside an object going out ? */
    index2 = cinfo->index/THEWORLD.globindex;

    /* find offset from prev position */
    VecSubtract(&displacement,loc,&prevpos);

    /* measure distance */
    glassdist = sqrt(DotProd(displacement,displacement));

  } else {

    /* outside an object going in ? */
    index2 = THEWORLD.globindex/cinfo->index;
    /* remember this position */
    VectEQ(&prevpos,loc);
  }

  /* doesn't currently use new_line() call */
  newline.type = LINE;
  newline.child = newline.nextobj = NULL;

  /* this ray starts from where we hit glass */
  VectEQ(&(newline.loc),loc);
  VectEqZero(&toadd);

  VectNegate(&negvect1,&(line->vect1));
  side1 = DotProd((*norm),negvect1);

  VectAddMult(&toadd,-side1,norm,(float)1,&negvect1);

  VectScale(&toadd,((float)1.0-index2));

  VectorAdd(&(newline.vect1),&(line->vect1),&toadd);

  Normalize(&(newline.vect1));

  maxtrans = MAX(MAX(cinfo->trans.r,cinfo->trans.g),
                     cinfo->trans.b);

  /* if we were inside, now we're out */
  newline.flag = !(line->flag);

# ifdef TESTTRANS

    if (line->flag) printf("line TRUE\n");
    if (newline.flag) printf("newline TRUE\n");

    printf("Trans = %d %d %d\n",cinfo->trans.r,
                                cinfo->trans.g,
                                cinfo->trans.b);

    printf("index2 = %f\n",index2);
    printf("side1  = %f\n",side1);

    printf("Dot(toadd,norm) = %f\n",
           DotProd(toadd,(*norm)));

    printf("Dot(norm,negvect1) = %f\n",
           DotProd((*norm),negvect1));

    printf("Dot(-norm,newvect) = %f\n",
           -DotProd((*norm),newline.vect1));

    printf("inmult, maxtrans = %f %d\n",inmult,maxtrans);
# endif

  multiplier = inmult * (float)maxtrans/(float)CNUM;
  Ray_Trace(&newline,&col1,multiplier);
  THEWORLD.refl_trans++;

# ifdef TESTTRANS
    printf("Col1 = %d %d %d\n",col1.r,col1.g,col1.b);
# endif

  color->r += col1.r;

  color->g += col1.g;

  color->b += col1.b;

  if (line->flag) {          /* density effects? */

    /* density uses x,y,z not r,g,b cuz its a floating
       point vector.                                   */

    attenuation.r = (int)(cinfo->density.x * glassdist *
                         (float)(color->r));

    attenuation.g = (int)(cinfo->density.y * glassdist *
                         (float)(color->g));

    attenuation.b = (int)(cinfo->density.z * glassdist *
                         (float)(color->b));

    /* don't remove more than original intensity! */

    color->r -= MIN(attenuation.r,color->r);
    color->g -= MIN(attenuation.g,color->g);
    color->b -= MIN(attenuation.b,color->b);

  }
}


/**********************************************************

                   Reflective color module

      Bounce ray off object and recursively ray trace.

 **********************************************************/

ReflectColor(color,cinfo,norm,loc,line,inmult)
  SVECT_PTR color;
  VECT_PTR  norm,loc;
  OBJ_PTR   line;
  CINFO_PTR cinfo;
  float inmult;
{
  SVECTOR col1;
  OBJ_STRUCT newline;
  float multiplier;
  int maxmirror;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,604);
# endif

  if ((cinfo->mirror.r < def.ithreshold) &&
      (cinfo->mirror.g < def.ithreshold) &&
      (cinfo->mirror.b < def.ithreshold)) return;

  newline.type = LINE;

  newline.child = newline.nextobj = NULL;

  VectEQ(&(newline.loc),loc);
  Reflect(&(newline.vect1),&(line->vect1),norm);

  maxmirror = MAX(MAX(cinfo->mirror.r,cinfo->mirror.g),
              cinfo->mirror.b);

  multiplier = inmult * (float)maxmirror/CNUM;

  Ray_Trace(&newline,&col1,multiplier);
  THEWORLD.refl_trans++;

  color->r+=(int)col1.r*(int)cinfo->mirror.r/CNUM;

  color->g+=(int)col1.g*(int)cinfo->mirror.g/CNUM;

  color->b+=(int)col1.b*(int)cinfo->mirror.b/CNUM;

# ifdef RAYDEBUG
    printf("REFLECTCOLOR:\n");
    printf("  col1 = %d %d %d\n",col1.r,col1.g,col1.b);
# endif

}


/**********************************************************

   Computes sky color given line - interpolate between
   horizon and zenith to find color (user should dither
   the explitive out of the sky to compensate for lack
   of color resolution.

 **********************************************************/

SkyColor(line,color)
  OBJ_PTR line;
  SVECT_PTR color;
{
  float length, horiz, zenith;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,605);
# endif

  if (THEWORLD.sky==NULL) return;

# ifdef ROBUST
    if (THEWORLD.sky->type != SKY)
      Error(INTERNAL_ERROR,606);
# endif

  length = DotProd(line->vect1,line->vect1);

  zenith = sqr(line->vect1.y)/length;
  horiz  = (sqr(line->vect1.x)+sqr(line->vect1.z))/length;

  color->r += (short)(zenith*THEWORLD.skycolor_zenith.r+
                      horiz *THEWORLD.skycolor_horiz.r);

  color->g += (short)(zenith*THEWORLD.skycolor_zenith.g+
                      horiz *THEWORLD.skycolor_horiz.g);

  color->b += (short)(zenith*THEWORLD.skycolor_zenith.b+
                      horiz *THEWORLD.skycolor_horiz.b);

  Dither(color,&((THEWORLD.sky)->cinfo));


# ifdef RAYDEBUG
    printf("SKYCOLOR:\n");
# endif

}


/**********************************************************

  Color dithering rountine - negative dither number will
  dither all three colors together - positive with perform
  separate color dithering on all colors.

 **********************************************************/

#define MINCOL 10
Dither(color,cinfo)
  SVECT_PTR color;
  CINFO_PTR cinfo;
{
  register int r,g,b;
  int PsRand();

  if (cinfo->dither==0) return;

  if (cinfo->dither>0) {
    r=PsRand(); g=PsRand(); b=PsRand();

    if (color->r<MINCOL) r=IABS(r);
    if (color->g<MINCOL) g=IABS(g);
    if (color->b<MINCOL) b=IABS(b);

    color->r += (short)(r*cinfo->dither/CNUM);
    color->g += (short)(g*cinfo->dither/CNUM);
    color->b += (short)(b*cinfo->dither/CNUM);
  } else {
    r=PsRand();
    if ((color->r+color->g+color->b)>(3*MINCOL)) r=IABS(r);

    color->r += (short)(r*cinfo->dither/CNUM);
    color->g += (short)(r*cinfo->dither/CNUM);
    color->b += (short)(r*cinfo->dither/CNUM);
  }
}


/**********************************************************

   Returns pointer to object hit by ray.
   Parameters:
     CurrObj = root of object tree
     line    = light ray
     MinT    = parameter T for line/obj intersection
     sflag   = if TRUE, stop on first intersection
     fflag   = ALWAYS pass TRUE here.
     pos1    = position vector for object relative coords
     pos2    = position vector for object relative coords

  - changed 11 jun 88 to fix shadow routine -

  - changed 12 aug 88 to add light attenuation by glass.
            this is not done correctly, but is better
            than nothing.

 **********************************************************/

OBJ_PTR Ray_Hit(CurrObj,line,MinT,sflag,fflag,atten)
  OBJ_PTR line, CurrObj;
  float *MinT;
  short sflag,fflag;
  VECT_PTR atten;           /* light attenuation by glass */
{
  static OBJ_PTR MinObj;
  VECTOR loc;
  OBJ_PTR obj;
  short collision;
  static short stop;
  float t;

  obj = CurrObj;

  if (fflag) {

#   ifdef ROBUST
      if (line->type!=LINE) Error(INTERNAL_ERROR,607);
#   endif

    *MinT=BIG;
    MinObj=NULL;
    stop=FALSE;
  }

  while (obj!=NULL && !stop) {         /* check for object collisions */

    collision =
      (*(ObjData[obj->type].ColTest))(line,obj,&t);

    if (collision && (obj->remove != NULL)) {
      FindPos(&loc,line,t);
      if (Find_Color(obj,obj->remove,&loc,NULL, 1.0, 1.0))
        collision = FALSE;
    }

    THEWORLD.intersect_tests++;

    if (collision && (t>SMALL)) {      /* did we hit something ? */

#     ifdef RAYDEBUG
        printf("RAY_HIT: Collision t=%f\n",t);
#     endif

      if (obj->type!=BBOX) {           /* if not BBOX */
        THEWORLD.ray_intersects++;
        if (sflag && t<1) {

          /* did we hit a transparent object? */
          /* PS - this is not right - fix it later */

          if ((obj->cinfo.trans.r < def.ithreshold) &&
              (obj->cinfo.trans.g < def.ithreshold) &&
              (obj->cinfo.trans.b < def.ithreshold)) {

            stop   = TRUE;
            MinObj = obj;
            return(obj);

          } else {

            /* attenuate light if transparent object.  This is
               REALLY screwy and not at all correct, but it is
               the only simple way to do it.
            */

            if (atten != NULL) {
              atten->x *= sqr(((float)obj->cinfo.trans.r) /
                          (float)CNUM);
              atten->y *= sqr(((float)obj->cinfo.trans.g) /
                          (float)CNUM);
              atten->z *= sqr(((float)obj->cinfo.trans.b) /
                          (float)CNUM);
            }
          }
        }
        if ((!sflag) && (t<*MinT)) {   /* nearest collision ? */
          *MinT  = t;                  /* if so, save it */
          MinObj = obj;
        }
      } else {                         /* is bbox hit */
        THEWORLD.bbox_intersects++;
        Ray_Hit(obj->child,line,MinT,sflag,FALSE,atten);
      }
    }
    obj=obj->nextobj;
  }

  return(MinObj);
}


/**********************************************************

  Performs ray tracing in line, fills color structure.
  Multiplier is a number by which the color output will
  be mulplied (0..1) so that we can tell when its useless
  to continue recursivly tracing rays.

 **********************************************************/

int Ray_Trace(line,color,multiplier)
  OBJ_PTR line;
  SVECT_PTR color;
  float multiplier;
{
  float MinT, divisor;
  OBJ_PTR MinObj;
  CINFO cinfo;
  VECTOR MinLoc, MinNorm;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,608);
# endif

  color->r = color->g = color->b = 0;

  /* check here if so little light is added that it doesn't matter */
  if (multiplier < def.threshold) return(FALSE);

  MinObj=Ray_Hit(THEWORLD.stack,line,&MinT,FALSE,TRUE,NULL);

  if (MinObj!=NULL) {
    FindPos(&MinLoc,line,MinT);

#   ifdef RAYDEBUG
      printf("RAY_TRACE: Collision, x,y,z = %f %f %f\n",
             MinLoc.x, MinLoc.y, MinLoc.z);
#   endif

    /*  Find object normal vector */

    (*(ObjData[MinObj->type].FindNorm))(&MinNorm,MinObj,&MinLoc);

    if (DotProd(MinNorm,(line->vect1)) >0) {   /* reverse   */
      MinNorm.x = -MinNorm.x;                  /* normal if */
      MinNorm.y = -MinNorm.y;                  /* necessary */
      MinNorm.z = -MinNorm.z;
    }
                                               /* Find colinfo */
    Find_Color(MinObj,
               MinObj->pattern,
               &MinLoc,
               &cinfo,
               MinObj->xmult,
               MinObj->ymult
              );

    AmbColor(color,&cinfo,&MinNorm,&MinLoc);
    DiffColor(color,&cinfo,&MinNorm,&MinLoc,line);
    TransColor(color,&cinfo,&MinNorm,&MinLoc,line,multiplier);
    ReflectColor(color,&cinfo,&MinNorm,&MinLoc,line,multiplier);
    Dither(color,&cinfo);

  } else {
    SkyColor(line,color);
  }

  if (color->r > CNUM || color->g > CNUM || color->b > CNUM) {
    divisor = (float)MAX(MAX(color->r,color->g),color->b) /
              (float)CNUM;

    color->r = (float)color->r/divisor;
    color->g = (float)color->g/divisor;
    color->b = (float)color->b/divisor;
  }

  return(MinObj!=NULL);
}


/**********************************************************

     Generates line for a given x,y pixel position
     This fn needs a little work, as currently it
     produces some distortion around the edge of
     the screen.

 **********************************************************/

PixelLine(x, y, line)           /* generate equation for a line */
  int x,y;
  OBJ_PTR line;
{
  register float xf, yf;

# ifdef ROBUST
    if (line->type!=LINE) Error(INTERNAL_ERROR,609);
# endif

  xf=((float)(def.x_center-x))/(THEWORLD.x_divisor);
  yf=((float)(def.y_center-y))/(THEWORLD.y_divisor);

  VectEQ(&(line->loc),&(THEWORLD.observer->loc));
  VectEQ(&(line->vect1),&(THEWORLD.observer->vect1));
  VectAddMult(&(line->vect1),xf,&(THEWORLD.obsright),
                             yf,&(THEWORLD.obsup));

  line->flag = FALSE;           /* redundant? */
}


/**********************************************************

   Sends scan line color data to file - file format is:

   BYTE 0 : low  byte of x resolution
   BYTE 1 : high byte of x resolution
   BYTE 2 : low  byte of y resolution
   BYTE 3 : high byte of y resolution

   Then, for each scan line, the line number is listed
   (2 bytes), followed by the r,g, and b bytes.

 **********************************************************/

Dump_Line(lineno,r,g,b)
  short r[],g[],b[];
  int lineno;
{
  int x;

#ifndef RAYDEBUG

  /** line number **/

  fputc(((unsigned char)(lineno&(0xff))),THEWORLD.filept);
  fputc(((unsigned char)(lineno>>8)),    THEWORLD.filept);

  for (x=0; x<def.x_res; x++) {
    fputc((unsigned char)r[x],THEWORLD.filept);
  }
  for (x=0; x<def.x_res; x++) {
    fputc((unsigned char)g[x],THEWORLD.filept);
  }
  for (x=0; x<def.x_res; x++) {
    fputc((unsigned char)b[x],THEWORLD.filept);
  }
#endif
}


/**********************************************************

                  Ray trace whole screen

 **********************************************************/

Screen_Trace() {
  int x,y;
  short rbyte[MAX_XRES], gbyte[MAX_XRES], bbyte[MAX_XRES];
  SVECTOR color;
  OBJ_PTR line;

  line=new_line();

# ifdef RAYDEBUG
    printf("SCREEN_TRACE: line:\n");
# endif

  for (y=0; y<def.y_res; y++) {

    for (x=0; x<def.x_res; x++) {

      PixelLine(x,y,line);

      Ray_Trace(line,&color,(float)1.0);
      THEWORLD.primary_traced++;

#     ifdef RAYDEBUG
        printf("SCREEN_TRACE: r,g,b = %d %d %d\n",
               color.r,color.g,color.b);
#     endif

      rbyte[x] = color.r;
      gbyte[x] = color.g;
      bbyte[x] = color.b;

#     ifdef RAYDEBUG
        printf("SCREEN_TRACE: r=%d, g=%d, b=%d\n",
               color.r,color.g,color.b);
#     endif
    }

    Dump_Line(y,rbyte,gbyte,bbyte);
  }
  free(line);
}
