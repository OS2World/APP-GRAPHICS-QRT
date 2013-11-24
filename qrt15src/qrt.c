#include "header.h"

int GetObject();
int RayTrace();


/**********************************************************

               Test routine - unused

 **********************************************************/

#ifdef TESTS

Tree_Walker(obj,num)
  OBJ_PTR obj;
  int num;
{
  int x;

  for (x=0; x<num; x++) printf(" ");
  printf("%d\n",obj->type);

  if (obj->child!=NULL)
    Tree_Walker(obj->child,num+2);

  if (obj->nextobj!=NULL)
    Tree_Walker(obj->nextobj,num);
}

#endif


/**********************************************************

    Initialize some observer data, like up and right
    vectors, focal length, etc.

 **********************************************************/

Setup_Observer() {

  if (THEWORLD.observer==NULL) Error(NO_OBSERVER,1);

  Normalize(&((THEWORLD.observer)->vect1));
  VectEQ(&(THEWORLD.obsup),&((THEWORLD.observer)->vect2));

  CrossProd(&(THEWORLD.obsright),             /* right = up x dir */
            &(THEWORLD.obsup),
            &((THEWORLD.observer)->vect1));

  CrossProd(&(THEWORLD.obsup),                /* up = dir x right */
            &((THEWORLD.observer)->vect1),
            &(THEWORLD.obsright));

  Normalize(&(THEWORLD.obsup));
  Normalize(&(THEWORLD.obsright));

}


/**********************************************************

         Initialize the world and assign defaults

 **********************************************************/

init_world() {                     /* make a universe */
  THEWORLD.stack      =
  THEWORLD.observer   =
  THEWORLD.instances  =
  THEWORLD.sky        =
  THEWORLD.lamps      = NULL;
  THEWORLD.patlist    = NULL;
  THEWORLD.outfile    = NULL;
  THEWORLD.objcount   = 0;
  THEWORLD.lampcount  = 0;
  THEWORLD.flength    = 50;

  THEWORLD.skycolor_zenith.r =
  THEWORLD.skycolor_zenith.b =
  THEWORLD.skycolor_zenith.g =
  THEWORLD.skycolor_horiz.r  =
  THEWORLD.skycolor_horiz.g  =
  THEWORLD.skycolor_horiz.b  = 0;

  THEWORLD.ray_intersects  =
  THEWORLD.pixels_hit      =
  THEWORLD.primary_traced  =
  THEWORLD.to_lamp         =
  THEWORLD.refl_trans      =
  THEWORLD.bbox_intersects =
  THEWORLD.pattern_matches =
  THEWORLD.intersect_tests = 0;

  THEWORLD.globindex = 1.00;   /* global index of refraction */

  def.cinfo.trans.r  =         /* default transmittion */
  def.cinfo.trans.g  =
  def.cinfo.trans.b  = 0;

  def.cinfo.mirror.r =         /* default reflection */
  def.cinfo.mirror.g =
  def.cinfo.mirror.b = 0;

  def.cinfo.amb.r    =         /* default ambiant light */
  def.cinfo.amb.g    =
  def.cinfo.amb.b    = 25;

  def.cinfo.diff.r   =         /* default diffuse light */
  def.cinfo.diff.g   =
  def.cinfo.diff.b   = CNUM;

  def.cinfo.density.x=
  def.cinfo.density.y=
  def.cinfo.density.z= .01;    /* default glass density */

  def.cinfo.fuzz     = 0;
  def.cinfo.index    = CNUM;
  def.cinfo.dither   = 3;
  def.cinfo.reflect  = 0;
  def.cinfo.sreflect = 10;

  def.shadow        = TRUE;    /* shadows */
  def.vlamp         = FALSE;   /* no visible lamps */
  def.int_x         =          /* no interpolation */
  def.int_y         = 1;
  def.threshold     = .1;      /* threshold at 10 percent */
  def.ithreshold    = def.threshold * CNUM;

# ifdef AMIGA
    def.x_res       = 320;     /* AMIGA 4096 color mode */
    def.y_res       = 400;
    def.aspect      = .56;
# endif

# ifdef UNIX
    def.x_res       = 768;     /* UNIX medium sized window */
    def.y_res       = 640;
    def.aspect      = .89;
# endif

# ifdef UNIX_HIRES
    def.x_res       = 1280;    /* HP hi-res workstation */
    def.y_res       = 1024;
    def.aspect      = .89;
# endif

# ifdef MAC_II
    def.x_res       = 640;     /* MAC II full screen */
    def.y_res       = 480;
    def.aspect      = 1.0;     /* is this right? */
# endif

# ifdef ATARI_ST
    def.x_res       = 640;     /* ATARI ST full screen */
    def.y_res       = 200;     /* don't know aspect */
# endif

# ifdef MSDOS
    def.x_res       = 320;	/* VGA mode 13h */
    def.y_res       = 200;
    def.aspect      = 0.565;
# endif

# ifdef MSDOS
    def.x_res       = 800;	/* SVGA mode */
    def.y_res       = 600;
    def.aspect      = 1.0667;
# endif

# ifdef MSDOS
    def.x_res       = 640;	/* SVGA mode */
    def.y_res       = 480;
    def.aspect      = 1.0667;
# endif

}


/**********************************************************

     Call other stuff to load world and generate image

     Changed 12 Mar 88 to add command line arguments.

 **********************************************************/

main(argc,argv)
  int argc; char *argv[];
{

  printf("\nQuick Ray Trace: Copyright 1988, 1989 Steve Koren\nVersion 1.5\n");

  init_world();

  if (!LoadWorld()) Error(SYNTAX_ERROR,2);

  Parse_CL_Args(argc,argv);

  Make_Bbox(THEWORLD.stack);     /* make bboxes */
  Do_Precomp(THEWORLD.stack);    /* precompute stuff */

  fclose(stdin);

  Setup_Observer();

  Open_File();
    Screen_Trace();
  Close_File();

  World_Stats();
  fclose(stdout);
  return(NULL);
}


/**********************************************************

  Added 12 Mar 88 to parse command line arguments.

  Command line arguments are optional as follows:

   -xres int -yres int -aspect float -foclen float

  The command line arguments take precidence over the
  DEFAULT() command values.

 **********************************************************/

Parse_CL_Args(argc,argv)
  int argc;
  char *argv[];
{
  int x, atoi(), found;

  for (x=1; x<argc; x++) {       /* parse command line args */
    found = FALSE;

    if (strcmp(argv[x],"-xres")==0) {

      if (++x >= argc)
        Error(TOO_FEW_PARMS,3);

      if ((def.x_res = atoi(argv[x])) <= 0)
        Error(LESS_THAN_ZERO,4);

      found        = TRUE;
    }

    if (strcmp(argv[x],"-yres")==0) {

      if (++x >= argc)
        Error(TOO_FEW_PARMS,5);

      if ((def.y_res = atoi(argv[x])) <= 0)
        Error(LESS_THAN_ZERO,6);

      found        = TRUE;
    }

    if (strcmp(argv[x],"-aspect")==0) {

      if (++x >= argc)
        Error(TOO_FEW_PARMS,7);

      if ((def.aspect = atof(argv[x])) <= 0)
        Error(LESS_THAN_ZERO,8);

      found        = TRUE;
    }

    if (strcmp(argv[x],"-foclen")==0) {

      if (++x >= argc)
        Error(TOO_FEW_PARMS,9);

      if ((THEWORLD.flength = atoi(argv[x])) <=0)
        Error(LESS_THAN_ZERO,010);

      found        = TRUE;
    }

    if (!found) {
      printf("Usage: %s [-xres n] [-yres n] [-aspect n] [-foclen n]\n\n",
             argv[0]);
      Error(ILLEGAL_OPTION,011);
    }

  }
}
