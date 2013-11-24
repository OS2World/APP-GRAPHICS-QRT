#include "qrt.h"
#include "pattern.h"


/**********************************************************

   Contains actual instantiations of some variables, such
   as the world, and the object data structure array.

 **********************************************************/


WORLD THEWORLD;      /* THE WORLD in the mind of a computer */
DEF     def;         /* defaults */
int     linenumber;  /* line counter */


/**********************************************************

   Table of pointers to functions for objects. There are
   six functions per object (see qrt.h)

 **********************************************************/

int LineSphere(), SphereNorm(), BboxSphere(),
    LineParallelogram(), PlaneNorm(), BboxParallelogram(),
    LineBbox(), BboxBbox(), LineRing(), BboxRing(),
    Plane_Pos(), Sphere_Pos(), LineQuadratic(),
    QuadraticNorm(), BboxQuadratic(), Quadratic_Pos(),
    PreCompPlane(), PreCompSphere(), PreCompQuadratic(),
    PreCompNull(), Standard_Offset(), Offset_Bbox(),
    LineTriangle(), BboxTriangle(), Resize_Bbox(),
    Resize_Sphere(), Resize_Plane(), Resize_Quadratic(),
    Err();

OBJ_DATA ObjData[] = {
  { Err,                 /* nothing       */
    Err,
    Err,
    Err,
    PreCompNull,
    Err,
    Err },

  { Err,                 /* LINE          */
    Err,
    Err,
    Err,
    PreCompNull,
    Err,
    Err },

  { LineSphere,          /* SPHERE        */
    SphereNorm,
    BboxSphere,
    Sphere_Pos,
    PreCompSphere,
    Standard_Offset,
    Resize_Sphere },

  { LineParallelogram,   /* PARALLELOGRAM */
    PlaneNorm,
    BboxParallelogram,
    Plane_Pos,
    PreCompPlane,
    Standard_Offset,
    Resize_Plane },

  { LineTriangle,        /* TRIANGLE      */
    PlaneNorm,
    BboxTriangle,
    Plane_Pos,
    PreCompPlane,
    Standard_Offset,
    Resize_Plane },

  { LineSphere,          /* LAMP          */
    SphereNorm,
    BboxSphere,
    Err,
    PreCompNull,
    Err,
    Err },

  { Err,                 /* OBSERVER      */
    Err,
    Err,
    Err,
    PreCompNull,
    Err,
    Err },

  { Err,                 /* GROUND        */
    Err,
    Err,
    Err,
    PreCompNull,
    Err,
    Err },

  { Err,                 /* SKY           */
    Err,
    Err,
    Err,
    PreCompNull,
    Err,
    Err },

  { LineBbox,            /* BBOX          */
    Err,
    BboxBbox,
    Err,
    PreCompNull,
    Offset_Bbox,
    Resize_Bbox },

  { LineRing,            /* RING          */
    PlaneNorm,
    BboxRing,
    Plane_Pos,
    PreCompPlane,
    Standard_Offset,
    Resize_Plane },

  { LineQuadratic,       /* QUADRATIC     */
    QuadraticNorm,
    BboxQuadratic,
    Quadratic_Pos,
    PreCompQuadratic,
    Standard_Offset,
    Resize_Quadratic }
};


/**********************************************************

   Table of pointers to functions for patterns.
   (see pattern.h)

 **********************************************************/

int Rect_Hit(), Circle_Hit(), Poly_Hit();

PATT_DATA PattData[] = {
  { Err             },
  { Rect_Hit        },
  { Circle_Hit      },
  { Poly_Hit        }
};


