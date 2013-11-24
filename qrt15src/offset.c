/**********************************************************

      Routines to offset primitives by dx, dy, dz
      (used for user defined primitive types)

 **********************************************************/

#include "qrt.h"

/* #define OFFSETDEBUG TRUE */

/**********************************************************

    This will offset most types of objects that use
    the 'loc' vector for their location.

 **********************************************************/

Standard_Offset(obj,offset)
  OBJ_PTR obj;
  VECT_PTR offset;
{

# ifdef ROBUST
    if (!((obj->type == SPHERE)        ||
          (obj->type == PARALLELOGRAM) ||
          (obj->type == RING)          ||
          (obj->type == TRIANGLE)      ||
          (obj->type == QUADRATIC)))
      Error(INTERNAL_ERROR,1301);
# endif

  /** now add the offset (this is a tough one) **/

# ifdef OFFSETDEBUG
    printf("STANDARDOFFSET: %f %f %f\n",
           offset->x,
           offset->y,
           offset->z);
# endif

  VectorAdd(&(obj->loc),&(obj->loc),offset);
}


/**********************************************************

  This will offset BBOX type objects.  It actually
  does nothing, since bbox values are filled after
  the tree is built.

 **********************************************************/

Offset_Bbox(obj,offset)
  OBJ_PTR obj;
  VECT_PTR offset;
{

# ifdef ROBUST
    if (obj->type != BBOX) Error(INTERNAL_ERROR,1302);
# endif

# ifdef OFFSETDEBUG
    printf("OFFSETBBOX: %f %f %f\n",
           offset->x,
           offset->y,
           offset->z);
# endif
}

