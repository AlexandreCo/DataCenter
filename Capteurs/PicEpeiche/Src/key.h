#ifndef __KEY_H
#define __KEY_H
void Key_vInit(void);
unsigned char Key_ucDown(void);
unsigned char Key_ucRight(void);
char Key_bDownPress(void);
char Key_bRightPress(void);

#define FUNC_INT		0
#define FUNC_T			1
#define FUNC_P			2
#define FUNC_HISTO_PRESS	3
#define FUNC_HISTO_TEMP		4
#define FUNC_RAZ		5
#define FUNC_CONT		6

#define NB_FUNC_RIGHT		7


#endif
