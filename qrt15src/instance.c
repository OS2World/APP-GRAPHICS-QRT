
/**********************************************************

    Routines for user-defined primitives (INSTANCE_OF)

 **********************************************************/

#include "qrt.h"

float Get_Next_Num(), IsPos();
char  *Get_Next_Name();

/* #define INSTANCEDEBUG TRUE */


/**********************************************************

     Returns a pointer to the object in the object
     tree with the given name.

 **********************************************************/


OBJ_PTR Name_Find(obj,name)
  OBJ_PTR obj;
  char *name;
{
  OBJ_PTR temp;

  if (obj==NULL) return(NULL);

  if (obj->name!=NULL)
    if (strcmp(name,obj->name)==0) return(obj);

  if (obj->child != NULL)
    if ((temp=Name_Find(obj->child,name))!=NULL)
      return(temp);

  if (obj->nextobj != NULL)
    if ((temp=Name_Find(obj->nextobj,name))!=NULL)
      return(temp);

  return(NULL);
}


/**********************************************************

    Copies a given sub-tree and returns a pointer to
    the copy.    Always pass fflag=TRUE

 **********************************************************/

OBJ_PTR Subtree_Copy(obj, fflag)
  OBJ_PTR obj;
  int fflag;
{
  OBJ_PTR newobj, new_obj();

  if (obj==NULL) return(NULL);

# ifdef INSTANCEDEBUG
    printf("SUBTREECOPY: name = %s\n",obj->name);
# endif

  newobj = new_obj( obj->type,         /* make new obj */
                    &(obj->loc),
                    &(obj->vect1),
                    &(obj->vect2),
                    &(obj->vect3),
                    &(obj->cinfo),
                    obj->pattern,
                    obj->remove,
                    NULL,              /* no name here */
                    &(obj->upper),
                    &(obj->lower),
                    obj->cterm,
                    obj->xmult,
                    obj->ymult );

  newobj->child   = Subtree_Copy(obj->child,FALSE);

  if (!fflag)
    newobj->nextobj = Subtree_Copy(obj->nextobj,FALSE);
  else
    newobj->nextobj = NULL;

  return(newobj);
}

/**********************************************************

          Offsets sub-tree by offset distance units
          ALWAYS pass fflag = TRUE

 **********************************************************/

Subtree_Offset(obj,offset,fflag)
  OBJ_PTR obj;
  VECT_PTR offset;
  int fflag;
{
  if (obj==NULL) return;

# ifdef INSTANCEDEBUG
    printf("SUBTREEOFFSET: type = %d offset = %f %f %f\n",
           obj->type,
           offset->x,
           offset->y,
           offset->z);
# endif

  (*(ObjData[obj->type].Offset))(obj,offset);

  Subtree_Offset(obj->child,offset,FALSE);

  if (!fflag)
    Subtree_Offset(obj->nextobj,offset,FALSE);
}

/**********************************************************

          Scales a sub-tree by mult distance units
          ALWAYS pass fflag = TRUE

 **********************************************************/

Subtree_Scale(obj,mult,fflag)
  OBJ_PTR obj;
  VECT_PTR mult;
  int fflag;
{
  if (obj==NULL) return;

# ifdef INSTANCEDEBUG
    printf("SUBTREESCALE: type = %d mult = %f %f %f\n",
           obj->type,
           mult->x,
           mult->y,
           mult->z);
# endif

  (*(ObjData[obj->type].Resize))(obj,mult);

  Subtree_Scale(obj->child,mult,FALSE);

  if (!fflag)
    Subtree_Scale(obj->nextobj,mult,FALSE);
}

/**********************************************************

  Load an "Instance_of" structure.  This really makes a
  copy of another part of the tree, offsets it, and returns
  a pointer to this copy.  All "name" fields for the copy
  are set to null to avoid duplicate names.  Also, a new
  scale factor can be specified (default 1).

 **********************************************************/

OBJ_PTR Get_Instance_Of()
{
  char str[SLEN], *name;
  int end, f, found;
  VECTOR offset, mult;
  OBJ_PTR source, dest;

  end=f=0;
  mult.x = mult.y = mult.z = 1.00;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str);

    found = FALSE;
    if (strcmp(str,"NAME")==0) {
      name = Get_Next_Name();
      f|=1; found = TRUE;
    }

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&offset);
      f|=2; found = TRUE;
    }

    if (strcmp(str,"SCALE")==0) {
      GetVector(&mult);
      found = TRUE;
    }

    if (strcmp(str,")")==0) { end=1; found = TRUE; }

    if (!found) Error(UNDEFINED_PARAM,1203);

  }

  if (f!=3) Error(TOO_FEW_PARMS,1204);

# ifdef INSTANCEDEBUG
    printf("GET_INSTANCE: offset = %f %f %f\n",
           offset.x,
           offset.y,
           offset.z);
# endif

  if ((source = Name_Find(THEWORLD.instances,name))==NULL)
    Error(UNDEFINED_NAME,1205);

  if ((dest = Subtree_Copy(source,TRUE))==NULL)
    Error(INTERNAL_ERROR,1206);

  /* scale the subtree */

# ifdef INSTANCEDEBUG
    printf("scaling...\n");
# endif

  Subtree_Scale(dest,&mult,TRUE);

# ifdef INSTANCEDEBUG
    printf("Offsetting...\n");
# endif

  /* move the subtree */

  Subtree_Offset(dest,&offset,TRUE);

# ifdef INSTANCEDEBUG
    printf("returning\n");
# endif

  return(dest);
}


