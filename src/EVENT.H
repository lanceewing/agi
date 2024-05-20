/* EVENT.H
*/


/* Structure for event codes from input devices
*/
typedef	struct {
	WORD		type;	/* type of code (direction, controller, key, etc) */
	WORD		val;		/* value of code */
	} EVENT;


/* Keymap structure
*/
typedef	struct {
	WORD		key;		/* character key */
	WORD		keyval;	/* translated value */
	} KEYMAP;

#define	KEYMAPSIZE	40		/* size of keymap array */


/* Event types
*/
#define	NULLEV	0
#define	CHAR		1
#define	DIR		2
#define	CONTROL	3


/* Joystick button character definitions
*/
#define	B0_CLICK		0x0101
#define	B1_CLICK		0x0201
#define	B0_DBLCLICK	0x0301
#define	B1_DBLCLICK	0x0401

