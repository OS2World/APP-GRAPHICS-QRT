/**********************************************************

   Contains pre-computing routines for objects.  This
   is to get as much as possible out of the way so that
   the line/object intersection routines don't have to
   do it.  Pre-computed stuff is stored in the objects
   precomp sub-structure.

 **********************************************************/

#include "qrt.h"

/* #define PRECOMPDEBUG */


/**********************************************************

                   Precompute sphere

  Computes : n1 = sqr(radius);

 **********************************************************/

PreCompSphere(obj)
  OBJ_PTR obj;
{

# ifdef ROBUST
    if (obj->type != SPHERE) Error(INTERNAL_ERROR,901);
# endif

  obj->precomp.n1 = sqr(obj->vect1.x);

}


/**********************************************************

                   Precompute Planar object

  Computes : norm   = vect1 X vect2
             n1     = Dotprod (norm,loc)
             len1   = sqr( | VECT1 | )
             len2   = sqr( | VECT2 | )

  Changed 12 Mar 89 to add:

             sin1   = v1->x * v2->y - v1->y * v2->x;
             cos1   = v1->x * v2->z - v1->z * v2->x;
             sin2   = v1->y * v2->z - v1->z * v2->y;

 **********************************************************/

PreCompPlane(obj)
  OBJ_PTR obj;
{

# ifdef ROBUST
    if (!((obj->type == PARALLELOGRAM) ||
          (obj->type == TRIANGLE)      ||
          (obj->type == RING)))
      Error(INTERNAL_ERROR,902);
# endif

  CrossProd(&(obj->precomp.norm),&(obj->vect1),&(obj->vect2));
  Normalize(&(obj->precomp.norm));

  obj->precomp.n1 = DotProd((obj->precomp.norm),(obj->loc));

  obj->precomp.len1 = DotProd(obj->vect1,obj->vect1);
  obj->precomp.len2 = DotProd(obj->vect2,obj->vect2);

  /* these precompute fields are not really sine and cos;
     they are just used to hold stuff for planes          */

  obj->precomp.sin1 = obj->vect1.x * obj->vect2.y -
                      obj->vect1.y * obj->vect2.x;

  obj->precomp.cos1 = obj->vect1.x * obj->vect2.z -
                      obj->vect1.z * obj->vect2.x;

  obj->precomp.sin2 = obj->vect1.y * obj->vect2.z -
                      obj->vect1.z * obj->vect2.y;

}


/**********************************************************

                   Precompute Quadratic

 **********************************************************/

PreCompQuadratic(obj)
  OBJ_PTR obj;
{

  VECTOR newdir;

# ifdef ROBUST
    if (obj->type != QUADRATIC) Error(INTERNAL_ERROR,903);
# endif

  Normalize(&(obj->vect1));

  if ((obj->vect1.x!=0) || (obj->vect1.z !=0)) {
    obj->precomp.cos1 = obj->vect1.z /
                        sqrt(sqr(obj->vect1.x) + sqr(obj->vect1.z));

    obj->precomp.sin1 = sqrt(1 - sqr(obj->precomp.cos1));
  } else {
    obj->precomp.cos1 =
    obj->precomp.sin1 = 0;
  }

  /* find new direction after first rotation */

  newdir.x = obj->vect1.x *  obj->precomp.cos1 +
             obj->vect1.z * -obj->precomp.sin1;

  newdir.y = obj->vect1.y;

  newdir.z = obj->vect1.x * obj->precomp.sin1 +
             obj->vect1.z * obj->precomp.cos1;

  /* now do second rotation */

  if ((newdir.y!=0) || (newdir.z !=0)) {
    obj->precomp.cos2 = newdir.y /
                        sqrt(sqr(newdir.y) + sqr(newdir.z));

    obj->precomp.sin2 = sqrt(1 - sqr(obj->precomp.cos2));
  } else {
    obj->precomp.cos2 =
    obj->precomp.sin2 = 0;
  }

# ifdef PRECOMPDEBUG
  printf("PRECOMPQUADRATIC: cos1,sin1 = %f %f\n",
          obj->precomp.cos1,obj->precomp.sin1);

  printf("                  cos2,sin2 = %f %f\n",
          obj->precomp.cos2,obj->precomp.sin2);
# endif

}


/**********************************************************

      Null precomputing routine for all other objects

 **********************************************************/

PreCompNull(obj)
  OBJ_PTR obj;
{
}

