/**********************************************************


             Header file for Quick Ray Trace

                     Steve Koren

 **********************************************************/

#include <stdio.h>
#include <math.h>

#define TRUE  1
#define FALSE 0

#define BOOL short


/**********************************************************

                     MACHINE TYPES

 **********************************************************/

#undef  AMIGA
#undef  MAC_II
#undef  ATARI_ST
#define UNIX             TRUE
#undef  IBM_OR_CLONE
#undef  OS_2
#undef  XENIX
#undef  VMS


/**********************************************************

                   MISC MATH CONSTANTS

 **********************************************************/

#define PI  3.141592
#define PI2 1.570796


/**********************************************************

                     OBJECT  NUMBERS

 **********************************************************/

#define LINE               1
#define SPHERE             2
#define PARALLELOGRAM      3
#define TRIANGLE           4
#define LAMP               5
#define OBSERVER           6
#define GROUND             7
#define SKY                8
#define BBOX               9
#define RING               10
#define QUADRATIC          11


/**********************************************************

                   PROGRAM CONSTANTS

 **********************************************************/

#define SMALL    1.0e-3      /* a sorta small number    */
#define BIG      1.0e15      /* a sorta big number      */
#define CNUM     100         /* this many shades/color  */
#define SLEN     64          /* max string length       */
#define MAX_IX   4           /* maximum x interpolation */
#define MAX_IY   4           /* maximum y interpolation */
#define MAX_XRES 2500        /* maximum X resolution    */
#define XYZZY    42          /* this is here for no     */
                             /* reason whatsoever       */


/**********************************************************

                      VECTOR STRUCTURES

 **********************************************************/

typedef struct vector {        /* a vector in 3 space   */
  float x,y,z;
} VECTOR, *VECT_PTR;

typedef struct svector {       /* an r,g,b color vector */
  short r,g,b;
} SVECTOR, *SVECT_PTR;

typedef struct cinfo_struct {  /* color information     */

  SVECTOR       amb,           /* ambient lighting      */
                diff,          /* diffuse lighting      */
                mirror,        /* % light reflected     */
                trans;         /* % light transmitted   */

  VECTOR        density;       /* density */

  float         sreflect,      /* specular refl coefficient */
                index;         /* index if refraction */

  short         fuzz,          /* currently unused */
                reflect,       /* percent specularly reflected */
                dither;        /* color dithering. 3..6 look ok */

} CINFO, *CINFO_PTR;


/**********************************************************

                PRECOMPUTED INFO FOR OBJECTS

 These fields can be used by object routines however
 they wish.  They just make object/line intersections
 faster.

 **********************************************************/

typedef struct _pre {

  float    sin1, cos1,         /* sin and cos */
           sin2, cos2,
           n1,                 /* misc number */
           len1, len2;         /* lengths of vectors */

  VECTOR   vect1,              /* misc vector */
           norm;               /* norm for planar objs */

} PRECOMP, PRECOMP_PTR;


/**********************************************************

                      PATTERN STRUCTURE

 **********************************************************/

typedef struct patt {
  short    type;               /* type of pattern */
  float    xsize,              /* pattern size */
           ysize,
           startx,
           starty,             /* x,y positions */
           endx,
           endy,
           radius;             /* rad for circles */

  CINFO    cinfo;              /* color information */

  char     *name;              /* pattern name */

  struct patt *child, *sibling, *link;

} PATTERN, *PATTERN_PTR;


/**********************************************************

                    OBJECT STRUCTURE

 **********************************************************/

typedef struct obj_struct {

  short         type,         /* object type */
                flag;         /* misc boolean flag */

  char          *name;        /* object name */

  VECTOR        loc,          /* object location */
                vect1,        /* three vectors */
                vect2,
                vect3,
                lower,        /* lower and upper bounds */
                upper;

  float         cterm,        /* for quadratic surfaces only */
                xmult,        /* x and y multipliers for patterns */
                ymult;

  CINFO         cinfo;        /* color information */

  PRECOMP       precomp;      /* precomputed information */

  struct obj_struct *nextobj, /* next obj in list */
                    *child;   /* child for bounding boxes only */

  PATTERN_PTR   pattern,      /* pointer to pattern structure */
                remove;       /* remove section of object */

} OBJ_STRUCT, *OBJ_PTR;


/**********************************************************

                   Plane Bbox structure

 **********************************************************/

typedef struct _PlaneBox {
  int      min_x, min_y,
           max_x, max_y;

  OBJ_PTR  object;

  struct _PlaneBox *next;
} PLANE_BBOX, *PLANE_BBOX_PTR;


/**********************************************************

                       WORLD STRUCTURE

 **********************************************************/

typedef struct world {
  OBJ_PTR   stack,            /* here are the objects */
            observer,         /* the observer */
            sky,              /* sky */
            lamps,            /* a lamp list */
            instances;        /* instance list */

  int       objcount,         /* # objects and lamps */
            lampcount;

  long      ray_intersects,   /* statistics */
            primary_traced,
            to_lamp,
            refl_trans,
            bbox_intersects,
            intersect_tests,
            pixels_hit,
            pattern_matches;

  VECTOR    obsright,         /* obs up dir */
            obsup;

  SVECTOR   skycolor_horiz,   /* skycolors */
            skycolor_zenith;

  PATTERN_PTR patlist;        /* the pattern stack */

  float     flength,          /* focal length */
            x_divisor,        /* used to find ray direction */
            y_divisor,
            globindex;        /* global index of refraction */

  char      *outfile;         /* output file name */
  FILE      *filept;          /* output file pointer */
} WORLD;


/**********************************************************

                  FUNCTIONS FOR OBJECT TYPES

 **********************************************************/

typedef struct obj_data {
  int (*ColTest)();           /* collision test function ptr */
  int (*FindNorm)();          /* normal finding function ptr */
  int (*FindBbox)();          /* object bound function ptr   */
  int (*RelPos)();            /* object relative position    */
  int (*PreComp)();           /* info pre-computing routine  */
  int (*Offset)();            /* offset object by dx, dy, dz */
  int (*Resize)();            /* resize object by a multiple */
} OBJ_DATA;


/**********************************************************

                      MATH DEFINES

 **********************************************************/

#define sqr(x) ((x)*(x))
#define DotProd(v1,v2) (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z)
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define IABS(x)  ((x)>0   ? (x) : (-(x)))


/**********************************************************

                      Default structure

 **********************************************************/

typedef struct def_struct {

  CINFO cinfo;             /* default colorinfo */

  short shadow,            /* shadows ? */
        vlamp,             /* lamps visible (not yet implimented) */
        int_x,             /* interpolate (def=1) */
        int_y,

        x_res,             /* X resolution of image */
        y_res,             /* Y resolution of image */
        x_center,          /* X center of image */
        y_center;          /* Y center of image */

  float threshold,         /* cutoff pt for min refl, refl rays */
        aspect;            /* aspect ratio for image            */


  short ithreshold;        /* integer version of above          */

} DEF, *DEF_PTR;


/**********************************************************

                     EXTERNAL DECLARATIONS

 **********************************************************/

extern DEF def;

extern OBJ_DATA ObjData[];

extern WORLD THEWORLD;


/**********************************************************

                       ERROR CODES

 **********************************************************/

#define ILLEGAL_PARAMETER 1
#define TOO_FEW_PARMS     2
#define ILLEGAL_OBJECT    3
#define MALLOC_FAILURE    4
#define SYNTAX_ERROR      5
#define INTERNAL_ERROR    6
#define FILE_ERROR        7
#define PATTERN_NOT_FOUND 8
#define PATTERN_EXISTS    9
#define NO_OBSERVER       10
#define UNDEFINED_PARAM   11
#define NON_HOMOGENIOUS   12
#define ZERO_INDEX        13
#define COLOR_VALUE_ERR   14
#define LESS_THAN_ZERO    15
#define ZERO_MULTIPLIER   16
#define UNDEFINED_NAME    17
#define LPAREN_EXPECTED   18
#define RPAREN_EXPECTED   19
#define ILLEGAL_VECTOR    20
#define ILLEGAL_SVECTOR   21
#define ILLEGAL_OPTION    22


/**********************************************************

                      WARNING CODES

 **********************************************************/

#define OBSOLETE_OPTION   1


/**********************************************************

 Define this flag for more robust code (HIGHLY recommended)

 **********************************************************/

#define ROBUST TRUE


