/*******************************************************

                  stacks and queues

 *******************************************************/

#include "qrt.h"
#include "pattern.h"

/* #define STACKDEBUG 1 */

char *malloc();

/**********************************************************

    Calls the pre-computing routine for each object in
    the object tree by calling Do_Precomp_Tree().  Also,
    does a bit of general housekeeping, like finding the
    center of the screen, etc.

 **********************************************************/

Do_Precomp(node)
  OBJ_PTR node;
{
  def.x_center = def.x_res/2; 
  def.y_center = def.y_res/2; 

  THEWORLD.x_divisor = THEWORLD.flength * def.x_res *
                       def.aspect / 60;

  THEWORLD.y_divisor = THEWORLD.flength * def.y_res / 70;

  Do_Precomp_Tree(node);
}


/**********************************************************

    Calls the pre-computing routine for each object in
    the object tree.  The pre-computing routines figure
    out some sub-expressions that don't change with
    different calls to the intersection routines.

 **********************************************************/

Do_Precomp_Tree(node)
  OBJ_PTR node;
{

# ifdef STACKDEBUG
    printf("DOPRECOMP: type = %d\n", node->type);
# endif

  (*(ObjData[node->type].PreComp))(node);  /* precompute */

  if (node->child != NULL) {               /* node has children ? */

#   ifdef ROBUST
      if (node->type != BBOX) Error(INTERNAL_ERROR,802);
#   endif

    Do_Precomp(node->child);
  }

  if (node->nextobj != NULL) {
    Do_Precomp(node->nextobj);
  }
}

/**********************************************************

    Assigns bounding box values for entire object tree.
    This must be called once before any ray-tracing is done.

 **********************************************************/

Make_Bbox(node)
  OBJ_PTR node;
{
  OBJ_PTR tnode;
  VECTOR  v1,v2;

# ifdef STACKDEBUG
    printf("MAKEBBOX\n");
# endif

  if (node->child!=NULL) {                 /* node has children ? */

#   ifdef ROBUST
      if (node->type!=BBOX) Error(INTERNAL_ERROR,801);
#   endif

    Make_Bbox(node->child);
  }

  if (node->nextobj!=NULL) {               /* node has siblings ? */
    Make_Bbox(node->nextobj);
  }

  if (node->type==BBOX) {
    tnode=node->child;

    node->lower.x=node->lower.y=node->lower.z=  BIG;
    node->upper.x=node->upper.y=node->upper.z= -BIG;

    while (tnode!=NULL) {
      (*(ObjData[tnode->type].FindBbox))(&v1,&v2,tnode);
      node->lower.x=MIN(node->lower.x,v1.x);
      node->lower.y=MIN(node->lower.y,v1.y);
      node->lower.z=MIN(node->lower.z,v1.z);

      node->upper.x=MAX(node->upper.x,v2.x);
      node->upper.y=MAX(node->upper.y,v2.y);
      node->upper.z=MAX(node->upper.z,v2.z);

#     ifdef STACKDEBUG
        printf("MAKEBBOX: v1 x,y,z = %f %f %f\n",
               node->lower.x,node->lower.y,node->lower.z);
        printf("          v2 x,y,z = %f %f %f\n",
               node->upper.x,node->upper.y,node->upper.z);
#     endif

      tnode=tnode->nextobj;
    }
  }
}


/**********************************************************

    Returns pointer to pattern structure given name, or
    null if not found.

 **********************************************************/

PATTERN_PTR find_pat(name)
  char *name;
{
  PATTERN_PTR pat;

  pat=THEWORLD.patlist;

  while (pat!=NULL) {
    if (strcmp(name,pat->name)==0) return(pat);
    pat=pat->sibling;
  }

  return(NULL);
}


/**********************************************************

     Allocates a new pattern structure and returns a
     pointer to it.

 **********************************************************/

PATTERN_PTR new_pat()
{
  PATTERN_PTR pat;

  if ((pat=(PATTERN_PTR)malloc(sizeof(PATTERN)))==NULL)
    Error(MALLOC_FAILURE,802);

  pat->name    = NULL;
  pat->child   =
  pat->sibling =
  pat->link    = NULL;

  return(pat);
}


/**********************************************************

     Allocates a new object structure, stuffs most of
     its information fields, and returns a pointer to it.

 **********************************************************/

OBJ_PTR new_obj(type,loc,v1,v2,v3,cinfo,pattern,remove,name,
                upper, lower, cterm, xmult, ymult)

        short        type;
        VECT_PTR     loc, v1, v2, v3, upper, lower;
        CINFO_PTR    cinfo;
        PATTERN_PTR  pattern, remove;
        float        cterm, xmult, ymult;
        char         *name;
{
  OBJ_PTR obj;

  if ((obj=(OBJ_PTR)malloc(sizeof(OBJ_STRUCT)))==NULL)
    Error(MALLOC_FAILURE,803);

  obj->type=type;                            /* copy info */
  VectEQ(&(obj->loc),loc);
  VectEQ(&(obj->vect1),v1);
  VectEQ(&(obj->vect2),v2);
  VectEQ(&(obj->vect3),v3);
  VectEQ(&(obj->lower),lower);
  VectEQ(&(obj->upper),upper);

  obj->cterm = cterm;
  obj->xmult = xmult;
  obj->ymult = ymult;

  obj->name = name;

  copy_colorinfo(&(obj->cinfo),cinfo);       /* colorinfo */

  obj->nextobj = obj->child = NULL;          /* no relatives */
  obj->pattern = pattern;
  obj->remove  = remove;

  return(obj);
}


/**********************************************************

                Generates an empty line

   - changed 13 Mar 89 to include fix by Paul Balyoz to
     intialize cinfo structure.

 **********************************************************/

OBJ_PTR new_line() {
  CINFO   cinfo;
  VECTOR  loc,v1,v2,v3, upper, lower;
  OBJ_PTR line;

  def_colorinfo(&cinfo);      /* initialize cinfo structure */

  VectEqZero(&loc);
  VectEqZero(&v1);
  VectEqZero(&v2);
  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  line=new_obj(LINE,&loc,&v1,&v2,&v3,&cinfo,NULL,NULL,NULL,
               &upper,&lower,(float)0.0,(float)0.0,(float)0.0);

  line->flag = FALSE;
  return(line);
}


/**********************************************************

                 Adds a lamp to the world

 **********************************************************/

add_lamp(object)                   /* add a lamp to the world */
  OBJ_PTR object;
{
   object->nextobj=THEWORLD.lamps;
   THEWORLD.lamps=object;
}


/**********************************************************

       Debugging routine to print object structure
                 MAY BE WRONG BY NOW

 **********************************************************/

#ifdef STACKDEBUG

  print_obj(obj)                     /* display object */
    OBJ_PTR obj;
  {
    printf("OBJECT :  type: ");
    switch (obj->type) {
      case LINE:           printf("LINE\n");          break;
      case SPHERE:         printf("SPHERE\n");        break;
      case PARALLELOGRAM:  printf("PARALLELOGRAM\n"); break;
      case TRIANGLE:       printf("TRIANGLE\n");      break;
      case LAMP:           printf("LAMP\n");          break;
      case OBSERVER:       printf("OBSERVER\n");      break;
      case GROUND:         printf("GROUND\n");        break;
      case SKY:            printf("SKY\n");           break;
      case BBOX:           printf("BBOX\n");          break;
      case RING:           printf("RING\n");          break;
      case QUADRATIC:      printf("QUADRATIC\n");     break;
      default:             printf("Unknown!\n");      break;
    }

    printf("          loc   : %f, %f, %f\n",
           (obj->loc.x),
           (obj->loc.y),
           (obj->loc.z));

    printf("          vect1 : %f, %f, %f\n",
           (obj->vect1.x),
           (obj->vect1.y),
           (obj->vect1.z));

    printf("          vect2 : %f, %f, %f\n",
           (obj->vect2.x),
           (obj->vect2.y),
           (obj->vect2.z));

    printf("          vect3 : %f, %f, %f\n",
           (obj->vect3.x),
           (obj->vect3.y),
           (obj->vect3.z));

  }
#endif


/**********************************************************

            Prints some interesting statistics

 **********************************************************/

World_Stats() {
  printf("World Statistics:\n");
  printf("  Objects:      %4d\n",THEWORLD.objcount);
  printf("  Lamps:        %4d\n",THEWORLD.lampcount);

  printf("  Intersect tests         : %-8ld\n", THEWORLD.intersect_tests);
  printf("  Total Intersections     : %-8ld\n",(THEWORLD.ray_intersects+
                                                THEWORLD.bbox_intersects));
  printf("     Object intersections : %-8ld\n", THEWORLD.ray_intersects);
  printf("     Bbox   intersections : %-8ld\n", THEWORLD.bbox_intersects);
  printf("  Rays traced             : %-8ld\n",(THEWORLD.primary_traced+
                                                THEWORLD.to_lamp+
                                                THEWORLD.refl_trans));
  printf("     Primary              : %-8ld\n", THEWORLD.primary_traced);
  printf("     To lamps             : %-8ld\n", THEWORLD.to_lamp);
  printf("     Refl. or Trans.      : %-8ld\n", THEWORLD.refl_trans);
  printf("  Pattern match checks    : %-8ld\n", THEWORLD.pattern_matches);

  printf("  Image statistics\n");
  printf("     X Resolution         : %-8d\n",  def.x_res);
  printf("     Y Resolution         : %-8d\n",  def.y_res);
  printf("     Aspect Ratio         : %-8.4f\n",def.aspect);

  printf("\n  Data sent to: %s\n\n",THEWORLD.outfile);
}

/**********************************************************

            Open file, and send image size

 **********************************************************/

Open_File() {
#ifdef MSDOS
  if ((THEWORLD.filept=fopen(THEWORLD.outfile,"wb"))==NULL)
    Error(FILE_ERROR,804);
#else
  if ((THEWORLD.filept=fopen(THEWORLD.outfile,"w"))==NULL)
    Error(FILE_ERROR,804);
#endif
  fputc((unsigned char)(def.x_res &  0xff),THEWORLD.filept);
  fputc((unsigned char)(def.x_res >> 8),   THEWORLD.filept);
  fputc((unsigned char)(def.y_res &  0xff),THEWORLD.filept);
  fputc((unsigned char)(def.y_res >> 8),   THEWORLD.filept);
}


/**********************************************************

                      Close file

 **********************************************************/

Close_File() {
  if (fclose(THEWORLD.filept)) Error(FILE_ERROR,805);
}


