
#include "qrt.h"

/**********************************************************

            Generates bounds for bounding box
            v1 = min x,y,z; v2 = max x,y,z

 **********************************************************/

BboxBbox(v1,v2,bbox)
  VECT_PTR v1,v2;
  OBJ_PTR bbox;
{
# ifdef ROBUST
    if (bbox->type!=BBOX) Error(INTERNAL_ERROR,101);
# endif

  VectEQ(v1,&(bbox->lower));
  VectEQ(v2,&(bbox->upper));
}


/**********************************************************

               Generates bounds for sphere
               v1 = min x,y,z; v2 = max x,y,z

 **********************************************************/

BboxSphere(v1,v2,sphere)
  VECT_PTR v1,v2;
  OBJ_PTR sphere;
{
# ifdef ROBUST
    if (!(sphere->type==SPHERE || sphere->type==LAMP))
      Error(INTERNAL_ERROR,102);
# endif

  v1->x=sphere->loc.x-sphere->vect1.x;
  v2->x=sphere->loc.x+sphere->vect1.x;

  v1->y=sphere->loc.y-sphere->vect1.x;
  v2->y=sphere->loc.y+sphere->vect1.x;

  v1->z=sphere->loc.z-sphere->vect1.x;
  v2->z=sphere->loc.z+sphere->vect1.x;

}


/**********************************************************

               Generates bounds for parallelogram
               v1 = min x,y,z; v2 = max x,y,z

 **********************************************************/

BboxParallelogram(v1,v2,para)
  VECT_PTR v1,v2;
  OBJ_PTR para;
{
  VECTOR point2,point3,point4;

# ifdef ROBUST
    if (para->type!=PARALLELOGRAM) Error(INTERNAL_ERROR,103);
# endif

  VectorAdd(&point2,&(para->loc),&(para->vect1));
  VectorAdd(&point3,&(para->loc),&(para->vect2));
  VectorAdd(&point4,&point3,&(para->vect1));

  v1->x=MIN(point2.x,point3.x);
  v1->x=MIN(v1->x,point4.x);
  v1->x=MIN(v1->x,para->loc.x);

  v1->y=MIN(point2.y,point3.y);
  v1->y=MIN(v1->y,point4.y);
  v1->y=MIN(v1->y,para->loc.y);

  v1->z=MIN(point2.z,point3.z);
  v1->z=MIN(v1->z,point4.z);
  v1->z=MIN(v1->z,para->loc.z);

  v2->x=MAX(point2.x,point3.x);
  v2->x=MAX(v2->x,point4.x);
  v2->x=MAX(v2->x,para->loc.x);

  v2->y=MAX(point2.y,point3.y);
  v2->y=MAX(v2->y,point4.y);
  v2->y=MAX(v2->y,para->loc.y);

  v2->z=MAX(point2.z,point3.z);
  v2->z=MAX(v2->z,point4.z);
  v2->z=MAX(v2->z,para->loc.z);

}


/**********************************************************

               Generates bounds for Triangle
               v1 = min x,y,z; v2 = max x,y,z

 **********************************************************/

BboxTriangle(v1,v2,obj)
  VECT_PTR v1,v2;
  OBJ_PTR obj;
{
  VECTOR point2,point3;

# ifdef ROBUST
    if (obj->type!=TRIANGLE) Error(INTERNAL_ERROR,104);
# endif

  VectorAdd(&point2,&(obj->loc),&(obj->vect1));
  VectorAdd(&point3,&(obj->loc),&(obj->vect2));

  v1->x=MIN(point2.x,point3.x);
  v1->x=MIN(v1->x,obj->loc.x);

  v1->y=MIN(point2.y,point3.y);
  v1->y=MIN(v1->y,obj->loc.y);

  v1->z=MIN(point2.z,point3.z);
  v1->z=MIN(v1->z,obj->loc.z);

  v2->x=MAX(point2.x,point3.x);
  v2->x=MAX(v2->x,obj->loc.x);

  v2->y=MAX(point2.y,point3.y);
  v2->y=MAX(v2->y,obj->loc.y);

  v2->z=MAX(point2.z,point3.z);
  v2->z=MAX(v2->z,obj->loc.z);

}

/**********************************************************

  Generates bounds for ring. This routine is not quite
  right, and will generate a bounding box bigger than is
  really needed, but it will have to do until i have time
  to do it right.

               v1 = min x,y,z; v2 = max x,y,z

 **********************************************************/

BboxRing(v1,v2,ring)
  VECT_PTR v1,v2;
  OBJ_PTR ring;
{

# ifdef ROBUST
    if (ring->type!=RING) Error(INTERNAL_ERROR,105);
# endif

  v1->x = ring->loc.x-ring->vect3.y;
  v2->x = ring->loc.x+ring->vect3.y;

  v1->y = ring->loc.y-ring->vect3.y;
  v2->y = ring->loc.y+ring->vect3.y;

  v1->z = ring->loc.z-ring->vect3.y;
  v2->z = ring->loc.z+ring->vect3.y;
}

/**********************************************************

               Generates bounds for Quadratic
               v1 = min x,y,z; v2 = max x,y,z

     *** THIS IS NOT RIGHT FOR ROTATED QUADRATICS ***

 **********************************************************/

BboxQuadratic(v1,v2,quad)
  VECT_PTR v1,v2;
  OBJ_PTR quad;
{

# ifdef ROBUST
    if (quad->type!=QUADRATIC) Error(INTERNAL_ERROR,106);
# endif

  VectEQ(v1,&(quad->lower));
  VectEQ(v2,&(quad->upper));

  VectorAdd(v1,v1,&(quad->loc));
  VectorAdd(v2,v2,&(quad->loc));
}


