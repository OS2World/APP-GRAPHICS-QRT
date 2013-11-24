
/**********************************************************

   ** Normal finder ** - one proceedure here for each
   primitive type.  Functions are pointed at by entries
   in the ObjData structure. Some functions will find
   the normal for >1 object.

 **********************************************************/

#include "qrt.h"

/**********************************************************

           Generates normal to sphere ( or lamp )

 **********************************************************/

SphereNorm(norm,object,position)
  VECT_PTR norm, position;
  OBJ_PTR object;
{
# ifdef ROBUST
    if (!((object->type == SPHERE) ||
          (object->type == LAMP)))
      Error(INTERNAL_ERROR,501);
# endif

  VecSubtract(norm,position,&(object->loc));
  Normalize(norm);
}


/**********************************************************

             Generates normal to planar object

 **********************************************************/

PlaneNorm(norm,object,position)
  VECT_PTR norm, position;
  OBJ_PTR object;
{
# ifdef ROBUST
    if (!((object->type == PARALLELOGRAM) ||
          (object->type == RING)          ||
          (object->type == TRIANGLE)))
      Error(INTERNAL_ERROR,502);
# endif

  VectEQ(norm,&(object->precomp.norm));
}


/**********************************************************

          Generates normal to quadratic surface
                    ^         ^         ^
  Normal to { a*x*x i + b*y*y j + c*z*z k = d } is:
                    ^         ^         ^
            { 2*a*x i + 2*b*y j + 2*c*d k }

 **********************************************************/

QuadraticNorm(norm,object,position)
  VECT_PTR norm, position;
  OBJ_PTR object;
{
  VECTOR newpos, newdir;

# ifdef ROBUST
    if (object->type != QUADRATIC)
      Error(INTERNAL_ERROR,503);
# endif

  /* translate collision point */

  VecSubtract(&newpos,position,&(object->loc));

  if ((object->vect1.x == 0) &&             /* no rotation  */
      (object->vect1.y == 1) &&             /* if aligned   */
      (object->vect1.z == 0))   {

      norm->x = object->vect2.x * newpos.x; /* throw out factor of */
      norm->y = object->vect2.y * newpos.y; /* 2 because of normalization */
      norm->z = object->vect2.z * newpos.z;

      Normalize(norm);

  } else {                                  /* here we must rotate */

    Rot12( &newpos, &newpos,                /* rotate position */
           object->precomp.cos1,
           object->precomp.sin1,
           object->precomp.cos2,
           object->precomp.sin2 );

    newdir.x = object->vect2.x * newpos.x;  /* normal */
    newdir.y = object->vect2.y * newpos.y;
    newdir.z = object->vect2.z * newpos.z;

    Rot21( &newdir, norm,                    /* rotate back */
           object->precomp.cos1,
           object->precomp.sin1,
           object->precomp.cos2,
           object->precomp.sin2 );

  }

}

