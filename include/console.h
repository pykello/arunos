#ifndef CONSOLE_H
#define CONSOLE_H


#define MAX_SPECIAL_KEY_SEQ_LEN 6

/* codes for special keys */
typedef enum SpecialKeyType
{
	SPECIAL_KEYS_START = 0x1000,

	KEY_ARROW_UP,
	KEY_ARROW_DOWN,
	KEY_ARROW_RIGHT,
	KEY_ARROW_LEFT,

	KEY_BACKSPACE

} SpecialKeyType;

/* character sequence for a sepcial key */
typedef struct SpecialKeySequence
{
	SpecialKeyType code;
	char sequence[MAX_SPECIAL_KEY_SEQ_LEN];

} SpecialKeySequence;


/* null-terminated character sequences for special keys */
static const SpecialKeySequence SPECIAL_KEY_SEQUENCES[] = {

	/* arrow keys */
	{KEY_ARROW_UP,    {0x1b, 0x5b, 0x41, 0x00}},
	{KEY_ARROW_DOWN,  {0x1b, 0x5b, 0x42, 0x00}},
	{KEY_ARROW_RIGHT, {0x1b, 0x5b, 0x43, 0x00}},
	{KEY_ARROW_LEFT,  {0x1b, 0x5b, 0x44, 0x00}},

	/* control keys */
	{KEY_BACKSPACE,   {0x7f, 0x00}}
};

#define SPECIAL_KEY_COUNT (sizeof(SPECIAL_KEY_SEQUENCES) / sizeof(SpecialKeySequence))


/* function declarations for interfacing with concole */
extern void console_init(void);
extern void putch(int c);
extern int getch(void);

#endif
