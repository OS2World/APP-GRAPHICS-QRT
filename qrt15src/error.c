
#include "qrt.h"

extern int linenumber;

/**********************************************************

         Here if pointer to fn goes astray
         (from ObjInfo structure)

 **********************************************************/

Err() {
  Error(INTERNAL_ERROR,9999);
}

/**********************************************************

      Prints warning message to stdio.  These are not
      critical; ray tracing continues.

 **********************************************************/

Warning(num,messg)
  int num;
  char *messg;
{
  if (linenumber >=0)
    printf("\nInput line number %d\n",linenumber);

  printf("Warning: ");

  switch (num) {
    case OBSOLETE_OPTION: printf("obsolete option\n");
                          break;

    default             : printf("programmer stupidity error\n");
  }

  if (messg != NULL)
    printf(" (%s)\n",messg);

}

/**********************************************************

      Error reporting routine. Dumps user back DOS

 **********************************************************/

Error(num,code)
  int num,code;
{

  if (linenumber >= 0)
    printf("\nInput line number %d\n",linenumber);

  printf("\nFatal error code %d: ",code);

  switch (num) {
    case ILLEGAL_PARAMETER: printf("illegal parameter\n");
                            break;
    case TOO_FEW_PARMS    : printf("too few parameters\n");
                            break;
    case ILLEGAL_OBJECT   : printf("illegal object type\n");
                            break;
    case MALLOC_FAILURE   : printf("malloc failure\n");
                            break;
    case SYNTAX_ERROR     : printf("syntax error\n");
                            break;
    case INTERNAL_ERROR   : printf("programmer confusion error\n");
                            break;
    case FILE_ERROR       : printf("file error\n");
                            break;
    case PATTERN_NOT_FOUND: printf("pattern not found\n");
                            break;
    case PATTERN_EXISTS   : printf("pattern already defined\n");
                            break;
    case NO_OBSERVER      : printf("no observer defined\n");
                            break;
    case UNDEFINED_PARAM  : printf("undefined parameter\n");
                            break;
    case NON_HOMOGENIOUS  : printf("world contains non-homogenious object\n");
                            break;
    case ZERO_INDEX       : printf("an index of refraction is 0\n");
                            break;
    case COLOR_VALUE_ERR  : printf("illegal color_info value\n");
                            break;
    case LESS_THAN_ZERO   : printf("parameter should be >= 0\n");
                            break;
    case ZERO_MULTIPLIER  : printf("a pattern multiplier is 0\n");
                            break;
    case UNDEFINED_NAME   : printf("undefined name\n");
                            break;
    case LPAREN_EXPECTED  : printf("left paren expected\n");
                            break;
    case RPAREN_EXPECTED  : printf("right paren expected\n");
                            break;
    case ILLEGAL_VECTOR   : printf("illegal vector structure\n");
                            break;
    case ILLEGAL_SVECTOR  : printf("illegal color triple\n");
                            break;
    case ILLEGAL_OPTION   : printf("illegal command line option\n");
                            break;

    default               : printf("programmer stupidity error\n");

  }
  exit(1);
}

