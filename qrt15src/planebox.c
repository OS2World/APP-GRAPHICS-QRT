
/************************************************************

                    Plane Bbox Module

 ************************************************************/

#include "qrt.h"


/************************************************************

         Compute plane bbox linked list for object tree

 ************************************************************/

PLANE_BBOX_PTR Init_Plane_Bbox(obj)
  OBJ_PTR obj;
{
  OBJ_PTR line, bbox;
  VECTOR p_000, p_100, p_010, p_110,
         p_001, p_101, p_011, p_111, delta;

  PLANE_BBOX_PTR pbox;

  static PLANE_BBOX_PTR list;

  if (obj == NULL) return(NULL);

  if (obj->type == BBOX) {            /* if BBOX, move on */
    Init_Plane_Bbox(obj->child);
    Init_Plane_Bbox(obj->nextobj);
    return(NULL);
  }
                                      /* if not, compute stuff */

/*  (*(ObjData[node->type].FindBbox))(&p_000,&p_111,node);
*/
    (*(ObjData[obj->type].FindBbox))(&p_000,&p_111,obj);


  /* create all 8 corners of 3d box */

  VecSubtract(&delta, &p_111, &p_000);

  VectEQ(&p_100, &p_000); p_100.x += delta.x;  /* front face */
  VectEQ(&p_010, &p_000); p_010.y += delta.y;
  VectEQ(&p_110, &p_100); p_110.z += delta.y;

  VectEQ(&p_101, &p_111); p_101.y -= delta.y;  /* back face */
  VectEQ(&p_011, &p_111); p_011.x -= delta.x;
  VectEQ(&p_001, &p_011); p_001.y -= delta.y;



}


