/**********************************************************

                  File for vector math

 **********************************************************/

#include <math.h>
#include "qrt.h"

/* #define MATHDEBUG TRUE */
/* #define MATHMAIN TRUE */


/**********************************************************

    Returns pseudo random number between one and 100

 **********************************************************/

int PsRand() {
  static int val=43;

  val=(val*73+97);
  return(val%CNUM);
}

/**********************************************************
                                    -
                    Vector clear to 0

 **********************************************************/

VectEqZero(v)
  VECT_PTR v;
{
  v->x=v->y=v->z=0.0;
}


/**********************************************************
                   -    -       -       -
     Vector math:  V1 = V1 + k2 V2 + k3 V3

 **********************************************************/

VectAddMult(v1,k2,v2,k3,v3)
  VECT_PTR v1,v2,v3;
  float k2,k3;
{
  v1->x+=k2*(v2->x)+k3*(v3->x);
  v1->y+=k2*(v2->y)+k3*(v3->y);
  v1->z+=k2*(v2->z)+k3*(v3->z);
}

/**********************************************************
                   -      -
     Vector math:  V1 = - V2

 **********************************************************/

VectNegate(v1,v2)
  VECT_PTR v1,v2;
{
  v1->x = -(v2->x);
  v1->y = -(v2->y);
  v1->z = -(v2->z);
}


/**********************************************************
                   -    -    -
     Vector math:  V1 = V2 + V3

 **********************************************************/

VectorAdd(v1,v2,v3)
  VECT_PTR v1,v2,v3;
{
  v1->x=v2->x+v3->x;
  v1->y=v2->y+v3->y;
  v1->z=v2->z+v3->z;
}


/**********************************************************
                   -    -    -
     Vector math:  V1 = V2 * V3

 **********************************************************/

VectorMult(v1,v2,v3)
  VECT_PTR v1,v2,v3;
{
  v1->x=v2->x*v3->x;
  v1->y=v2->y*v3->y;
  v1->z=v2->z*v3->z;
}


/**********************************************************
                   -    -    -
     Vector math:  V1 = V2 - V3

 **********************************************************/

VecSubtract(v1,v2,v3)                              /* 1=2-3 */
  VECT_PTR v1,v2,v3;
{
  v1->x=v2->x-v3->x;
  v1->y=v2->y-v3->y;
  v1->z=v2->z-v3->z;
}


/**********************************************************
                   -      -
     Vector math:  V1 = k V1

 **********************************************************/

VectScale(v1,k,v2)                               /* 1=2*1 */
  VECT_PTR v1,v2;
  float k;
{
  v1->x *= k;
  v1->y *= k;
  v1->z *= k;
}


/**********************************************************

                Color vector assignment

 **********************************************************/

SVectEQ(v1,v2)                    /* short vector v1=v2 */
  SVECT_PTR v1,v2;
{
  v1->r=v2->r;
  v1->g=v2->g;
  v1->b=v2->b;
}


/**********************************************************

                    Vector assignment

 **********************************************************/

VectEQ(v1,v2)                     /* long vector v1=v2 */
  VECT_PTR v1,v2;
{
  v1->x=v2->x;
  v1->y=v2->y;
  v1->z=v2->z;
}


/**********************************************************

                  Normalize vector V

 **********************************************************/

Normalize(v)
  VECT_PTR v;
{
  register float l;
  l=sqrt(DotProd((*v),(*v)));

  v->x/=l;
  v->y/=l;
  v->z/=l;
}


/**********************************************************
                   -    -    -
     Vector math:  V1 = V2 x V3

 **********************************************************/

CrossProd(v1,v2,v3)
  VECT_PTR v1,v2,v3;
{
  v1->x=v2->y*v3->z-v2->z*v3->y;
  v1->y=v2->z*v3->x-v2->x*v3->z;
  v1->z=v2->x*v3->y-v2->y*v3->x;
}


/**********************************************************

      Returns position vector given line structure
      and parameter T

 **********************************************************/

FindPos(pos,line2,t)      /* find position on parametric line 2 */
  VECT_PTR pos;
  OBJ_PTR  line2;
  float t;
{
# ifdef ROBUST
    if (line2->type!=LINE) Error(INTERNAL_ERROR,401);
# endif

  pos->x=line2->loc.x+line2->vect1.x*t;
  pos->y=line2->loc.y+line2->vect1.y*t;
  pos->z=line2->loc.z+line2->vect1.z*t;

# ifdef MATHDEBUG
    printf("FINDPOS: t=%f, Xout=%f, Yout=%f, Zout=%f\n",
            t,pos->x,pos->y,pos->z);
# endif
}

/**********************************************************

        Reflects vector IN about NORM to produce OUT

 **********************************************************/

Reflect(out,in,norm)
  VECT_PTR out,in,norm;
{
  float dot;

  dot = -DotProd((*in),(*norm));

# ifdef ROBUST                           /* CHECK THIS OUT */
    /* if (dot<0) Error (INTERNAL_ERROR,402); */
# endif

  VectEQ(out,in);
  VectAddMult(out,dot,norm,dot,norm);

# ifdef MATHDEBUG
    printf("REFLECT: in   x,y,z = %f %f %f\n",in->x,in->y,in->z);
    printf("         norm x,y,z = %f %f %f\n",norm->x,norm->y,norm->z);
    printf("         out  x,y,z = %f %f %f\n",out->x,out->y,out->z);
# endif
}


/**********************************************************

   Arctan of 2 numbers.  This *should* exist in the
   compiler library, but it's not there.

 **********************************************************/

float atan2w(x,y)
  float x,y;
{
  if (x==0) {
    if (y>0) return(PI2);
    return(-PI2);
  }

  if (x>0) return(atan(y/x));

  if (y>0) return(PI-atan(y/(-x)));

  return(-PI+atan(y/x));
}


/**********************************************************

                    DOUBLE ROTATION

   This is a really two multiplied transformation
   matricies, but this form is faster. The if statements
   rotate only if necessary.

 **********************************************************/

Rot12(invect,outvect,cos1,sin1,cos2,sin2)
  VECT_PTR invect, outvect;
  register float cos1, sin1, cos2, sin2;
{
#ifdef ROTATEFINISH

  VECTOR tempdir;

  /** Rotate once to YZ plane **/

  if ((cos1!=0) || (sin1!=0)) {

    tempdir.x = invect->x *  cos1 +
                invect->z * -sin1;

    tempdir.y = invect->y;

    tempdir.z = invect->x *  sin1 +
                invect->z *  cos1;
  } else {
    VectEQ(&tempdir,invect);
  }

  /** Rotate again to positive Y **/

  if ((cos2!=0) || (sin2!=0)) {

    outvect->x = tempdir.x;

    outvect->y = tempdir.y * -cos2 +
                 tempdir.z *  sin2;


    outvect->z = tempdir.y * -sin2 +
                 tempdir.z * -cos2;

  } else {
    VectEQ(outvect,&tempdir);
  }
#endif
}


/**********************************************************

                    DOUBLE ROTATION

       Same as above, but in reverse direction

 **********************************************************/

Rot21(invect,outvect,cos1,sin1,cos2,sin2)
  VECT_PTR invect, outvect;
  float cos1, sin1, cos2, sin2;
{
#ifdef ROTATEFINISH

  VECTOR tempdir;

  /** Rotate once from Y up direction **/

  if ((cos2!=0) || (sin2!=0)) {

    tempdir.x = invect->x;

    tempdir.y = invect->y *  cos2 +
                invect->z * -sin2;


    tempdir.z = invect->y *  sin2 +
                invect->z *  cos2;

  } else {
    VectEQ(&tempdir,invect);
  }

  /** Rotate again to arbitrary direction **/

  if ((cos1!=0) || (sin1!=0)) {

    outvect->x = tempdir.x * -cos1 +
                 tempdir.z *  sin1;

    outvect->y = tempdir.y;

    outvect->z = tempdir.x * -sin1 +
                 tempdir.z * -cos1;
  } else {
    VectEQ(outvect,&tempdir);
  }
#endif
}


/**********************************************************

      Main for mth.c - used ONLY to test functions.

 **********************************************************/

# ifdef MATHMAIN

Error() { }

main() {

  VECTOR in,out;
  float sin1, sin2, cos1, cos2;

  printf("in: ");
  scanf("%f %f %f",&(in.x),&(in.y),&(in.z));

  printf("sin1 cos1 sin2 cos2: ");
  scanf("%f %f %f %f",&sin1, &cos1, &sin2, &cos2);

  Rot12(&in,&out,cos1,sin1,cos2,sin2);

  printf("out12: %f %f %f\n",out.x,out.y,out.z);

  Rot21(&in,&out,cos1,sin1,cos2,sin2);
  printf("out21: %f %f %f\n",out.x,out.y,out.z);
}

# endif

