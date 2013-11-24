
/**********************************************************

      Routines to resize primitives by mx, my, mz
      (used for user defined primitive types).  The
      routines must also scale the position vector by
      that amount.  The scale factors are passed in as
      a vector (mult).

 **********************************************************/

#include "qrt.h"

/* #define RESIZEDEBUG TRUE */


/**********************************************************

   Resizes sphere.  If !(mult.x==mult.y==mult.z) will resize
   by smallest amount.

 **********************************************************/

Resize_Sphere(obj,mult)
  OBJ_PTR obj;
  VECT_PTR mult;
{
  float size;

# ifdef ROBUST
    if (obj->type!=SPHERE) Error(INTERNAL_ERROR,1401);
# endif

  size = MIN(MIN(mult->x,mult->y),mult->z);

  VectorMult(&(obj->loc),&(obj->loc),mult);
  obj->vect1.x *= size;

# ifdef RESIZEDEBUG
    printf("RESIZESPHERE: newloc = %f %f %f\n",
           obj->loc.x,
           obj->loc.y,
           obj->loc.z);
    printf("   new rad = %f\n",obj->vect1.x);
# endif

}


/**********************************************************

                 Resizes Planar object

 **********************************************************/

Resize_Plane(obj,mult)
  OBJ_PTR obj;
  VECT_PTR mult;
{
  float size;

# ifdef ROBUST
    if (!((obj->type != PARALLELOGRAM) ||
          (obj->type != TRIANGLE) ||
          (obj->type != RING)))
       Error(INTERNAL_ERROR,1402);
# endif

  size = MIN(MIN(mult->x,mult->y),mult->z);

  VectorMult(&(obj->vect1),&(obj->vect1),mult);
  VectorMult(&(obj->vect2),&(obj->vect2),mult);
  VectorMult(&(obj->loc),  &(obj->loc),  mult);

  obj->vect3.x *= size;      /* for ring */
  obj->vect3.y *= size;

# ifdef RESIZEDEBUG
    printf("RESIZEPLANE: newloc = %f %f %f\n",
           obj->loc.x,
           obj->loc.y,
           obj->loc.z);
# endif

}


/**********************************************************

                 Resizes Quadratic object


   16 Mar 89 - fixed bug where mult was declared as float
               instead of VECT_PTR.
 **********************************************************/

Resize_Quadratic(obj,mult)
  OBJ_PTR obj;
  VECT_PTR mult;
{

# ifdef ROBUST
    if (obj->type!=QUADRATIC)
       Error(INTERNAL_ERROR,1403);
# endif

  VectorMult(&(obj->upper),&(obj->upper),mult);
  VectorMult(&(obj->lower),&(obj->lower),mult);
  VectorMult(&(obj->loc),  &(obj->loc),  mult);

# ifdef RESIZEDEBUG
    printf("RESIZEQUAD: newloc = %f %f %f\n",
           obj->loc.x,
           obj->loc.y,
           obj->loc.z);
# endif

}


/**********************************************************

    Resizes Bbox - doesn't actually do anything, since
    bbox values are filled after the tree is created.

 **********************************************************/

Resize_Bbox(obj,mult)
  OBJ_PTR obj;
  VECT_PTR mult;
{

# ifdef ROBUST
    if (obj->type!=BBOX)
       Error(INTERNAL_ERROR,1404);
# endif

# ifdef RESIZEDEBUG
    printf("RESIZEBBOX:\n");
# endif

}


