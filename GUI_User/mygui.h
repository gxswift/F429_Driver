#ifndef _MYGUI_H
#define _MYGUI_H

extern GUI_CONST_STORAGE GUI_BITMAP bmQQ;

extern GUI_CONST_STORAGE GUI_BITMAP bmgit;

extern GUI_CONST_STORAGE GUI_BITMAP bmmail;

extern GUI_CONST_STORAGE GUI_BITMAP bmtarget;

extern GUI_CONST_STORAGE GUI_BITMAP bmwechat;

extern GUI_CONST_STORAGE GUI_BITMAP bmgame;

extern GUI_CONST_STORAGE GUI_BITMAP bmkeil;

extern GUI_CONST_STORAGE GUI_BITMAP bmapple;

extern GUI_CONST_STORAGE GUI_BITMAP bmQQlogo;

extern GUI_CONST_STORAGE GUI_BITMAP bmbar;

extern GUI_CONST_STORAGE GUI_BITMAP bmlogo;

extern GUI_CONST_STORAGE GUI_BITMAP bmQQ2;

extern GUI_CONST_STORAGE GUI_BITMAP bmreturn;

extern GUI_CONST_STORAGE GUI_BITMAP bmgitlogo;



extern WM_HWIN CreateWinMain(void);
extern WM_HWIN CreateWin1(void);
extern WM_HWIN CreateWin2(void);
extern WM_HWIN CreateWin3(void);
extern WM_HWIN CreateWin4(void);
extern WM_HWIN CreateWin5(void);
extern WM_HWIN CreateWin6(void);
typedef struct
{
	int C1;
	int C2;
}ColorChange;

extern ColorChange C_Change[];
extern void Theme_Paint();


#endif

