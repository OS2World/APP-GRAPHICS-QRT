
/**********************************************************

                    object loader language

 **********************************************************/

/* #define IODEBUG 1 */

#include "qrt.h"
#include "pattern.h"

OBJ_PTR      new_obj(), Get_Object(), Get_Primitive(),
             Get_Instance_Of();
PATTERN_PTR  Attach_Pattern(), Get_Pattern();
char         *malloc();

# ifdef AMIGA
    float      atof();
# endif

/**********************************************************

             External declarations from lexer

 **********************************************************/

float IsPos();
float Get_Next_Num();
short Get_Color_Val();
char *Get_Next_Name();



/**********************************************************

   Global input line number.  If negative, error rountines
   will not report line number (already reached EOF).

 **********************************************************/

extern int linenumber;

/**********************************************************

          Load the entire world from standard input

 **********************************************************/

int LoadWorld()                      /* load world from stdio */
{
  linenumber = 1;
  if((THEWORLD.stack=Get_Object())==NULL)
    Error(ILLEGAL_OBJECT,201);

  linenumber = -1;
  return(TRUE);
}

/**********************************************************

     Get an object and return a pointer to it. If it is
     not an object, but some other world attribute
     specification, note it, but keep trying until an
     object is found or end of input is encountered.
     This is slighly hacked out, but it works for now.
     It calls itself recursively sometimes.

     Changed 12 Mar 89 to include fix by Paul Balyoz
     to intitialize colorinfo structure (apparently some
     machines have a problem with it otherwise).

 **********************************************************/

OBJ_PTR Get_Object() {
  char str[SLEN], *name;
  OBJ_PTR newobj, queue, temp;
  VECTOR loc,rad,d,v3, upper, lower;
  CINFO cinfo;
  int found;

  def_colorinfo(&cinfo);          /* initialize color info */

  name=NULL; queue=NULL;
  VectEqZero(&loc);
  VectEqZero(&rad);
  VectEqZero(&d);
  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  if (feof(stdin)) return(NULL);
  while (!feof(stdin)) {
    GetToken(str); found = FALSE;

#   ifdef IODEBUG
      printf("GETOBJECT: token=%s\n",str);
#   endif

    if (GetAttrib(str))
      found = TRUE;

    if (strcmp(str,"BEGIN_INSTANCES")==0) {
      found = TRUE;
      THEWORLD.instances = Get_Object();
    }

    if (strcmp(str,"END_INSTANCES")==0) {
#     ifdef ROBUST
        if (queue==NULL) Error(SYNTAX_ERROR,205);
#     endif
      found = TRUE;
      return(queue);
    }

    if (strcmp(str,"END_BBOX")==0) {
#     ifdef ROBUST
        if (queue==NULL) Error(SYNTAX_ERROR,206);
#     endif
      found = TRUE;
      return(queue);
    }

    if (strcmp(str,"BEGIN_BBOX")==0) {

       newobj=new_obj(BBOX,&loc,&rad,&d,&v3,&cinfo,NULL,NULL,name,
                      &upper, &lower,
                      (float)0.0, (float)0.0, (float)0.0);

       newobj->child = Get_Object();
       newobj->nextobj = queue;
       queue = newobj;
       found = TRUE;

    }

    if (strcmp(str,"NAME")==0) {
       name=Get_Next_Name();
       found = TRUE;
    }

    if ((temp=Get_Primitive(str))!=NULL) {
      found = TRUE;
      temp->nextobj=queue;
      queue = temp;
    }

    if (!found && !feof(stdin)) Error(SYNTAX_ERROR,207);

  }

  return(queue);
}


/**********************************************************

                 Load default color info

 **********************************************************/

def_colorinfo(cinfo)
  CINFO_PTR cinfo;
{
  copy_colorinfo(cinfo,&(def.cinfo));
}


/**********************************************************

                    Copy color info

 **********************************************************/

copy_colorinfo(c1,c2)
  CINFO_PTR c1,c2;
{
  if (c1 == NULL) return;

  SVectEQ(&(c1->amb),&(c2->amb));
  SVectEQ(&(c1->diff),&(c2->diff));
  SVectEQ(&(c1->mirror),&(c2->mirror));
  SVectEQ(&(c1->trans),&(c2->trans));
  VectEQ(&(c1->density),&(c2->density));

  c1->fuzz     = c2->fuzz;
  c1->index    = c2->index;
  c1->dither   = c2->dither;
  c1->sreflect = c2->sreflect;
  c1->reflect  = c2->reflect;
}


/**********************************************************

            Get defaults from input file

 **********************************************************/

int Get_Default()
{
  char        str[SLEN];
  int         end, found;

  end=0;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str);

#   ifdef IODEBUG
      printf("GETDEFAULT : token=%s\n",str);
#   endif

    found = GetOpt(str,&(def.cinfo));

    if (strcmp(str,"NO_SHADOW")==0) {
      def.shadow = FALSE;
      found = TRUE;
    }

    if (strcmp(str,"NO_LAMP")==0) {
      def.vlamp = FALSE;
      found = TRUE;
    }

    if (strcmp(str,"THRESHOLD")==0) {
      def.threshold = IsPos(Get_Next_Num());
      found = TRUE;
    }

    if (strcmp(str,"X_RES")==0) {
      def.x_res    = (short)IsPos(Get_Next_Num());
      found = TRUE;
    }

    if (strcmp(str,"Y_RES")==0) {
      def.y_res    = (short)IsPos(Get_Next_Num());
      found = TRUE;
    }

    if (strcmp(str,"ASPECT")==0) {
      def.aspect = IsPos(Get_Next_Num());
      found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE;}

    if (!found) Error(UNDEFINED_PARAM,209);
  }
  return(TRUE);
}


/**********************************************************

                    Get object options

 **********************************************************/

int GetOpt(str,cinfo)
  char str[];
  CINFO_PTR cinfo;
{
  int found;

  found = FALSE;

  if (strcmp(str,"AMB")==0) {
    GetSVector(&(cinfo->amb));
    found = TRUE;
  }

  if (strcmp(str,"DIFF")==0) {
    GetSVector(&(cinfo->diff));
    found = TRUE;
  }

  if (strcmp(str,"MIRROR")==0) {
    GetSVector(&(cinfo->mirror));
    found = TRUE;
  }

  if (strcmp(str,"TRANS")==0) {
    GetSVector(&(cinfo->trans));
    found = TRUE;
  }

  if (strcmp(str,"DENSITY")==0) {
    GetVector(&(cinfo->density));
    found = TRUE;
  }

  if (strcmp(str,"FUZZ")==0) {
    cinfo->fuzz     = Get_Color_Val();
    found = TRUE;
  }

  if (strcmp(str,"INDEX")==0) {
    cinfo->index    = IsPos(Get_Next_Num());
    found = TRUE;
  }
  if (strcmp(str,"DITHER")==0) {
    cinfo->dither   = (short)IsPos(Get_Next_Num());
    found = TRUE;
  }
  if (strcmp(str,"SREFLECT")==0) {
    cinfo->sreflect = IsPos(Get_Next_Num());
    found = TRUE;
  }
  if (strcmp(str,"REFLECT")==0) {
    cinfo->reflect = Get_Color_Val();
    found = TRUE;
  }

  return(found);
}


/**********************************************************

          Load a sphere and return pointer to it

 **********************************************************/

OBJ_PTR GetSphere()
{
  char        str[SLEN], *name;
  VECTOR      loc, rad, d, v3, upper, lower;
  float       xmult, ymult;
  CINFO       cinfo;
  OBJ_PTR     newobj;
  PATTERN_PTR pattern, remove;
  int         end, f, found;

  end=f=0;
  def_colorinfo(&cinfo);
  xmult=ymult=1;
  pattern = remove = NULL;
  name    = NULL;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str);

#   ifdef IODEBUG
      printf("GETSPHERE : token=%s\n",str);
#   endif

    found = GetOpt(str,&cinfo);

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if (strcmp(str,"RADIUS")==0) {
      rad.x = IsPos(Get_Next_Num());
      f|=2; found = TRUE;
    }
    if (strcmp(str,"PATTERN")==0) {
      pattern = Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"REMOVE")==0) {
      remove = Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"XMULT")==0) {
      xmult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"YMULT")==0) {
      ymult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"NAME")==0) {
       name=Get_Next_Name();
       found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE; }

    if (!found) Error(UNDEFINED_PARAM,211);
  }

  if (f!=3) Error(TOO_FEW_PARMS,212);
  rad.y=rad.z=0;

  VectEqZero(&d);
  VectEqZero(&upper);
  VectEqZero(&lower);

  newobj=new_obj(SPHERE,&loc,&rad,&d,&v3,&cinfo,pattern,
                 remove,name,
                 &upper, &lower, (float)0.0, xmult, ymult);

  THEWORLD.objcount++;
  return(newobj);
}


/**********************************************************

             Load lamp and attach to world

 **********************************************************/

int GetLamp()
{
  char str[SLEN];
  VECTOR loc, rad, d, v3, upper, lower;
  CINFO cinfo;
  int   end, f, found;

  end=f=0;

  cinfo.amb.r=cinfo.amb.g=cinfo.amb.b=CNUM;
  rad.y=150;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str); found = TRUE;

#   ifdef IODEBUG
      printf("GETLAMP : token=%s\n",str);
#   endif

    found = GetOpt(str,&cinfo);

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if (strcmp(str,"RADIUS")==0) {
      rad.x = IsPos(Get_Next_Num());
      f|=2; found = TRUE;
    }
    if (strcmp(str,"DIST")==0) {
      rad.y = IsPos(Get_Next_Num());
      f|=4; found = TRUE;
    }
    if (strcmp(str,")")==0) { end = 1; found = TRUE;}

    if (!found) Error(UNDEFINED_PARAM,214);
  }

  if (f!=7) return(FALSE);

  rad.z=0;

  VectEqZero(&d);
  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  cinfo.diff.r=cinfo.diff.g=cinfo.diff.b=CNUM;

  add_lamp(new_obj(LAMP,&loc,&rad,&d,&v3,&cinfo,NULL,NULL,
                   NULL, &upper, &lower,
                   (float)0.0, (float)0.0, (float)0.0));

  THEWORLD.lampcount++;
  return(TRUE);
}


/**********************************************************

         Load observer and attach him to the world

 **********************************************************/

int GetObserver()
{
  char   str[SLEN];
  VECTOR loc, lk, up, dir, v3, upper, lower;
  CINFO  cinfo;
  int    end, f, found;

  def_colorinfo(&cinfo);

  end=f=0;
  up.x = up.z = 0; up.y = 1;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str); found = FALSE;

#   ifdef IODEBUG
      printf("GETOBSERVER : token=%s\n",str);
#   endif

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if (strcmp(str,"LOOKAT")==0) {
      GetVector(&lk);
      f|=2; found = TRUE;
    }

    if (strcmp(str,"UP")==0) {
      GetVector(&up);
      found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE; }

    if (!found) Error(UNDEFINED_PARAM,216);
  }

  VecSubtract(&dir,&lk,&loc);    /* find view direction */

  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  if (f!=3) Error(TOO_FEW_PARMS,217);

  THEWORLD.observer=
     new_obj(OBSERVER,&loc,&dir,&up,&v3,&cinfo,NULL,NULL,
             NULL, &upper, &lower,
             (float)0.0, (float)0.0, (float)0.0);

  return(TRUE);
}


/**********************************************************

          Load triangle  - not yet implimented

 **********************************************************/

OBJ_PTR GetTriangle()
{
  char        str[SLEN], *name;
  CINFO       cinfo;
  VECTOR      loc, v1, v2, v3, upper, lower;
  OBJ_PTR     newobj;
  PATTERN_PTR pattern, remove;
  int         end, f, found;
  float       xmult, ymult;

  end=f=0;
  xmult=ymult=1;
  def_colorinfo(&cinfo);
  pattern = remove = NULL;
  name    = NULL;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str);

#   ifdef IODEBUG
      printf("GETTRIANGLE : token=%s\n",str);
#   endif

    found = GetOpt(str,&cinfo);

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if ((strcmp(str,"V1")==0)     ||
        (strcmp(str,"VECT1")==0)) {

      GetVector(&v1);
      f|=2; found = TRUE;
    }

    if ((strcmp(str,"V2")==0)     ||
        (strcmp(str,"VECT2")==0)) {

      GetVector(&v2);
      f|=4; found = TRUE;
    }

    if (strcmp(str,"PATTERN")==0) {
      pattern=Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"REMOVE")==0) {
      remove=Attach_Pattern();
      found = TRUE;
    }

    if (strcmp(str,"XMULT")==0) {
      xmult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"YMULT")==0) {
      ymult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"NAME")==0) {
       name=Get_Next_Name();
       found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE;}

    if (!found) Error(UNDEFINED_PARAM,219);
  }

  if (f!=7) Error(TOO_FEW_PARMS,220);

  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  newobj=new_obj(TRIANGLE,&loc,&v1,&v2,&v3,&cinfo,pattern,
                 remove, name,
                 &upper, &lower, (float)0.0, xmult, ymult);

  THEWORLD.objcount++;
  return(newobj);
}


/**********************************************************

         Load sky data and attach it to the world

 **********************************************************/

int GetSky()
{
  VECTOR loc, v1, v2, v3, upper, lower;
  CINFO  cinfo;
  char   str[SLEN];
  int    end, found;

  end=0;
  GetLeftParen();

  def_colorinfo(&cinfo);

  while (!end && !feof(stdin)) {
    GetToken(str);

    found = GetOpt(str,&cinfo);                    /*** DITHER ONLY ***/

    if (strcmp(str,"ZENITH")==0) {
       GetSVector(&(THEWORLD.skycolor_zenith));
       found = TRUE;
    }

    if ((strcmp(str,"HORIZ")==0) ||
        (strcmp(str,"HORIZON")==0)) {

       GetSVector(&(THEWORLD.skycolor_horiz));
       found = TRUE;
    }
    if (strcmp(str,")")==0) { end=1; found=TRUE; }

    if (!found) Error(UNDEFINED_PARAM,222);
  }

  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  THEWORLD.sky=new_obj(SKY,&loc,&v1,&v2,&v3,&cinfo,NULL,NULL,
                       NULL, &upper, &lower,
                       (float)0.0, (float)0.0, (float)0.0);
  return(TRUE);
}

/**********************************************************

            Load ring and return pointer to it

 **********************************************************/

OBJ_PTR GetRing()
{
  char        str[SLEN], *name;
  CINFO       cinfo;
  VECTOR      loc, v1, v2, v3, upper, lower;
  OBJ_PTR     newobj;
  PATTERN_PTR pattern, remove;
  int         end, f, found;
  float       xmult, ymult;

  end=f=0;
  xmult=ymult=1;
  def_colorinfo(&cinfo);
  pattern = remove = NULL;
  name    = NULL;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str);

#   ifdef IODEBUG
      printf("GETRING : token=%s\n",str);
#   endif

    found = GetOpt(str,&cinfo);

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if ((strcmp(str,"V1")==0)     ||
        (strcmp(str,"VECT1")==0)) {

      GetVector(&v1);
      f|=2; found = TRUE;
    }

    if ((strcmp(str,"V2")==0)     ||
        (strcmp(str,"VECT2")==0)) {

      GetVector(&v2);
      f|=4; found = TRUE;
    }

    if (strcmp(str,"RAD_1")==0) {
       v3.x = IsPos(Get_Next_Num());
       f|=8; found = TRUE;
    }
    if (strcmp(str,"RAD_2")==0) {
       v3.y = IsPos(Get_Next_Num());
       f|=16; found = TRUE;
    }
    if (strcmp(str,"PATTERN")==0) {
       pattern=Attach_Pattern();
       found = TRUE;
    }
    if (strcmp(str,"REMOVE")==0) {
      remove=Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"XMULT")==0) {
      xmult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"YMULT")==0) {
      ymult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"NAME")==0) {
       name=Get_Next_Name();
       found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE; }

    if (!found) Error(UNDEFINED_PARAM,224);
  }

  if (f!=31) Error(TOO_FEW_PARMS,225);
  v3.z = 0;
  Normalize(&v1);
  Normalize(&v2);

  newobj=new_obj(RING,&loc,&v1,&v2,&v3,&cinfo,pattern,
                 remove, name,
                 &upper, &lower, (float)0.0, xmult, ymult);

  THEWORLD.objcount++;
  return(newobj);
}


/**********************************************************

       Load parallelogram and return pointer to it

 **********************************************************/

OBJ_PTR GetParallelogram()
{
  char        str[SLEN], *name;
  CINFO       cinfo;
  VECTOR      loc, v1, v2, v3, upper, lower;
  OBJ_PTR     newobj;
  PATTERN_PTR pattern, remove;
  int         end, f, found;
  float       xmult, ymult;

  end=f=0;
  xmult=ymult=1;
  def_colorinfo(&cinfo);
  pattern = remove = NULL;
  name    = NULL;

  GetLeftParen();

  while (!end && !feof(stdin)) {
    GetToken(str);

#   ifdef IODEBUG
      printf("GETPARREL : token=%s\n",str);
#   endif

    found = GetOpt(str,&cinfo);

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if ((strcmp(str,"V1")==0)     ||
        (strcmp(str,"VECT1")==0)) {

      GetVector(&v1);
      f|=2; found = TRUE;
    }

    if ((strcmp(str,"V2")==0)     ||
        (strcmp(str,"VECT2")==0)) {

      GetVector(&v2);
      f|=4; found = TRUE;
    }

    if (strcmp(str,"PATTERN")==0) {
      pattern=Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"REMOVE")==0) {
      remove=Attach_Pattern();
      found = TRUE;
    }

    if (strcmp(str,"XMULT")==0) {
      xmult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"YMULT")==0) {
      ymult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"NAME")==0) {
       name=Get_Next_Name();
       found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE;}

    if (!found) Error(UNDEFINED_PARAM,227);
  }

  if (f!=7) Error(TOO_FEW_PARMS,228);

  VectEqZero(&v3);
  VectEqZero(&upper);
  VectEqZero(&lower);

  newobj=new_obj(PARALLELOGRAM,&loc,&v1,&v2,&v3,&cinfo,
                 pattern, remove,name,
                 &upper, &lower, (float)0.0, xmult, ymult);

  THEWORLD.objcount++;
  return(newobj);
}

/**********************************************************

                  Load quadratic surface

 **********************************************************/

OBJ_PTR GetQuadratic()
{
  char        str[SLEN], *name;
  CINFO       cinfo;
  VECTOR      loc, v1, v2, v3, upper, lower;
  OBJ_PTR     newobj;
  PATTERN_PTR pattern, remove;
  int         end, f, found;
  float       cterm, xmult, ymult;

  end=f=0;
  xmult=ymult=1;
  def_colorinfo(&cinfo);
  pattern = remove = NULL;
  name    = NULL;

  GetLeftParen();

  upper.x = upper.y = upper.z = 10;
  lower.x = lower.y = lower.z = -10;

  v1.x = 0; v1.y = 1; v1.z = 0;

  while (!end && !feof(stdin)) {
    GetToken(str);

#   ifdef IODEBUG
      printf("GETQUAD : token=%s\n",str);
#   endif

    found = GetOpt(str,&cinfo);

    if ((strcmp(str,"POS")==0)      ||
        (strcmp(str,"LOC")==0)      ||
        (strcmp(str,"POSITION")==0) ||
        (strcmp(str,"LOCATION")==0)) {

      GetVector(&loc);
      f|=1; found = TRUE;
    }

    if (strcmp(str,"A")==0) {
      v2.x  = Get_Next_Num();
      f|=2; found = TRUE;
    }
    if (strcmp(str,"B")==0) {
      v2.y  = Get_Next_Num();
      f|=4; found = TRUE;
    }
    if (strcmp(str,"C")==0) {
      v2.z  = Get_Next_Num();
      f|=8; found = TRUE;
    }
    if (strcmp(str,"D")==0) {
      cterm = Get_Next_Num();
      f|=16; found = TRUE;
    }

    if (strcmp(str,"XMIN")==0) {
      lower.x  = Get_Next_Num();
      found    = TRUE;
    }
    if (strcmp(str,"XMAX")==0) {
      upper.x  = Get_Next_Num();
      found    = TRUE;
    }
    if (strcmp(str,"YMIN")==0) {
      lower.y  = Get_Next_Num();
      found    = TRUE;
    }
    if (strcmp(str,"YMAX")==0) {
      upper.y  = Get_Next_Num();
      found    = TRUE;
    }
    if (strcmp(str,"ZMIN")==0) {
      lower.z  = Get_Next_Num();
      found    = TRUE;
    }
    if (strcmp(str,"ZMAX")==0) {
      upper.z  = Get_Next_Num();
      found    = TRUE;
    }

    if (strcmp(str,"DIR")==0) {
      GetVector(&v1);
      found = TRUE;
    }

    if (strcmp(str,"PATTERN")==0) {
      pattern=Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"REMOVE")==0) {
      remove=Attach_Pattern();
      found = TRUE;
    }
    if (strcmp(str,"XMULT")==0) {
      xmult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"YMULT")==0) {
      ymult = IsPos(Get_Next_Num());
      found=TRUE;
    }
    if (strcmp(str,"NAME")==0) {
       name=Get_Next_Name();
       found = TRUE;
    }

    if (strcmp(str,")")==0) { end = 1; found = TRUE;}

    if (!found) Error(UNDEFINED_PARAM,230);
  }

  if (f!=31) Error(TOO_FEW_PARMS,231);

  VectEqZero(&v3);

  newobj=new_obj(QUADRATIC,&loc,&v1,&v2,&v3,&cinfo,pattern,
                 remove, name,
                 &upper, &lower, cterm, xmult, ymult);

  THEWORLD.objcount++;
  return(newobj);
}


/**********************************************************

       Load the focal length of the observers lens

 **********************************************************/

int GetFocLength() {
  THEWORLD.flength=IsPos(Get_Next_Num());
  return(TRUE);
}


/**********************************************************

    Load a primitive from input and return pointer to it

 **********************************************************/

OBJ_PTR Get_Primitive(str)                /* get an object */
  char str[];
{
  OBJ_PTR newobj;

# ifdef IODEBUG
    printf("GETPRIMITIVE : token=%s:\n",str);
# endif

  newobj=NULL;
  if (strcmp(str,"SPHERE")==0) {
    if ((newobj=GetSphere())==NULL)
      Error(ILLEGAL_OBJECT,232);
  }
  if (strcmp(str,"PARALLELOGRAM")==0) {
    if ((newobj=GetParallelogram())==NULL)
      Error(ILLEGAL_OBJECT,233);
  }
  if (strcmp(str,"TRIANGLE")==0) {
    if ((newobj=GetTriangle())==NULL)
      Error(ILLEGAL_OBJECT,234);
  }
  if (strcmp(str,"RING")==0) {
    if ((newobj=GetRing())==NULL)
      Error(ILLEGAL_OBJECT,235);
  }
  if (strcmp(str,"QUADRATIC")==0) {
    if ((newobj=GetQuadratic())==NULL)
      Error(ILLEGAL_OBJECT,236);
  }
  if (strcmp(str,"INSTANCE_OF")==0) {
    if ((newobj=Get_Instance_Of())==NULL)
      Error(ILLEGAL_OBJECT,237);
  }
  return(newobj);
}


/**********************************************************

     Load an attribute from input and do stuff with it

 **********************************************************/

int GetAttrib(str)                    /* get an object */
  char str[];
{
  int found, scrap;

  found=FALSE;

# ifdef IODEBUG
    printf("GETATTRIB : token=%s:\n",str);
# endif

  if (strcmp(str,"SKY")==0) {
    if (!GetSky())      Error(INTERNAL_ERROR,238);
    found=TRUE;
  }
  if (strcmp(str,"FOC_LENGTH")==0) {
    if (!GetFocLength()) Error(INTERNAL_ERROR,239);
    found=TRUE;
  }
  if (strcmp(str,"DEFAULT")==0) {
    if (!Get_Default()) Error(INTERNAL_ERROR,240);
    found=TRUE;
  }
  if (strcmp(str,"FILE_NAME")==0) {
    THEWORLD.outfile=Get_Next_Name();
    found=TRUE;
  }
  if (strcmp(str,"LAMP")==0) {
    if (!GetLamp())     Error(INTERNAL_ERROR,241);
    found=TRUE;
  }
  if (strcmp(str,"OBSERVER")==0) {
    if (!GetObserver()) Error(INTERNAL_ERROR,242);
    found=TRUE;
  }
  if (strcmp(str,"PATTERN")==0) {
    if (!GetPattern())  Error(INTERNAL_ERROR,243);
    found=TRUE;
  }
  if (strcmp(str,"FIRST_SCAN")==0) {
    Warning(OBSOLETE_OPTION,"FIRST_SCAN");
    scrap = (int)IsPos(Get_Next_Num());
    found=TRUE;
  }
  if (strcmp(str,"LAST_SCAN")==0) {
    Warning(OBSOLETE_OPTION,"LAST_SCAN");
    scrap = (int)IsPos(Get_Next_Num());
    found=TRUE;
  }

  return(found);
}

