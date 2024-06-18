/* SCREEN.H
*/

/* This file was assumed to exist. A real example on an original Sierra game disk
** has not been found. If this file is currently void of code, then it exists 
** merely as a placeholder to indicate that Siera's AGI interpreter had such a 
** file. If this file contains code, then it is because an attempt has been made
** to reconstruct what it might have looked like using a disassembly tool.
*/

/* Monitor types
*/
#define COMP    0   /* Composite, or CGA (Black-Cyan-Magenta-White palette) */
#define RGB     1   /* RGB (Blue-Green-Red-Brown palette) */
#define MONO    2   /* Hercules Graphics Card */
#define EGA     3   /* Enhanced Graphics Adapter */
#define VGA     4   /* Video Graphics Array */

/* Picture relocation factors used in configure.screen command
*/
#define HGC_RELOC_FACTOR    6
#define IBM_RELOC_FACTOR    8
