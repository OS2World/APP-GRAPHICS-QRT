
/**********************************************************

  Relative position module.  This is used for patterns:
  when an object intersection is found, its location is
  passed to one of these routines, which returns two
  object-relative coordinates.

 **********************************************************/


#include "qrt.h"

/* #define RELPOSDEBUG 1 */

/**********************************************************

   Finds the coordinates of a point on a given plane.

   Added 11 Mar 89 to fix bug in finding plane coordinates.

 **********************************************************/

Find_Plane_Coords(v1,v2,delta,pos1,pos2, t1, t2, t3)
  VECT_PTR v1, v2, delta;
  float    *pos1, *pos2, t1, t2, t3;
{

  if (t1!= 0) {
    *pos1 = (delta->x*v2->y-v2->x*delta->y)/t1;
  } else {
    if (t2!=0) {
      *pos1 = (delta->x*v2->z-v2->x*delta->z)/t2;
    } else {
      if (t3!=0) {
        *pos1 = (delta->y*v2->z-v2->y*delta->z)/t3;
      } else Error(ILLEGAL_OBJECT,700);
    }
  }

  if (v2->x != 0) {
    *pos2 = (delta->x-*pos1*v1->x)/v2->x;
    return;
  }

  if (v2->y != 0) {
    *pos2 = (delta->y-*pos1*v1->y)/v2->y;
    return;
  }

  if (v2->z != 0) {
    *pos2 = (delta->z-*pos1*v1->z)/v2->z;
    return;
  }

  Error(ILLEGAL_OBJECT,701);
}


/**********************************************************

   Finds relative coords on plane given position in space.
   object should be parallelogram or ring.
   loc is point in space.
   pos1, pos2 are set to relative coords.

   Changed 11 Mar 88 to use Find_Plane_Coords().

 **********************************************************/

Plane_Pos(obj,loc,pos1,pos2,normalize)
  OBJ_PTR obj;
  VECT_PTR loc;
  float *pos1, *pos2;
  int normalize;
{
    VECTOR delta;

#   ifdef ROBUST
      if (!((obj->type == RING) ||
            (obj->type == PARALLELOGRAM) ||
            (obj->type == RING) ||
            (obj->type == TRIANGLE)))
        Error(INTERNAL_ERROR,702);
#   endif

    VecSubtract(&delta,loc,&(obj->loc));

    Find_Plane_Coords( &(obj->vect1),
                       &(obj->vect2),
                       &delta,
                       pos1,
                       pos2,
                       obj->precomp.sin1,
                       obj->precomp.cos1,
                       obj->precomp.sin2
                     );

    if (!normalize) {
      *pos1 *= sqrt(obj->precomp.len1);
      *pos2 *= sqrt(obj->precomp.len2);
    }

#   ifdef RELPOSDEBUG
      printf("Plane_Pos: pos1,2 = %f %f\n",*pos1,*pos2);
#   endif

}


/**********************************************************

   Finds relative coords on sphere given position in space
     obj->vect1.x = radius of sphere

 **********************************************************/

Sphere_Pos(obj,loc,pos1,pos2)
  OBJ_PTR obj;
  VECT_PTR loc;
  float *pos1, *pos2;
{
    float atan2w();
    VECTOR delta;

#   ifdef ROBUST
      if (obj->type!=SPHERE) Error(INTERNAL_ERROR,703);
#   endif

    VecSubtract(&delta,loc,&(obj->loc));

#   ifdef ROBUST
      if (delta.x==0 && delta.y==0 && delta.z==0)
        Error(INTERNAL_ERROR,704);
#   endif

    *pos1 = atan2w(delta.x,delta.y) * obj->vect1.x;
    *pos2 = atan2w(sqrt(sqr(delta.x)+sqr(delta.y)),delta.z) *
            obj->vect1.x;

#   ifdef RELPOSDEBUG
      printf("SPHEREPOS: pos1,2 = %f %f\n",*pos1,*pos2);
#   endif

}


/**********************************************************

 Finds relative coords on quadratic given position in space

 **********************************************************/

Quadratic_Pos(obj,loc,pos1,pos2)
  OBJ_PTR obj;
  VECT_PTR loc;
  float *pos1, *pos2;
{
   VECTOR newpos;

   VecSubtract(&newpos,loc,&(obj->loc));

   *pos1 = newpos.x;                     /** This isn't right! **/
   *pos2 = newpos.y;                     /** fix it later      **/

#   ifdef RELPOSDEBUG
      printf("QUADRATICPOS: pos1,2 = %f %f\n",*pos1,*pos2);
#   endif
}


