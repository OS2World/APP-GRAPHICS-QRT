
/**********************************************************

                HEADER FILE FOR PATTERNS

 **********************************************************/

#define PATT_HEADER       0             /* header    */
#define RECT_PATTERN      1             /* rectangle */
#define CIRCLE_PATTERN    2             /* circle    */
#define POLY_PATTERN      3             /* polygon   */


/**********************************************************

            FUNCTIONS FOR PATTERN INTERSECTIONS

 **********************************************************/

typedef struct patt_data {
  int (*PattHit)();                     /* did we hit the pattern ? */
} PATT_DATA;

extern PATT_DATA PattData[];


