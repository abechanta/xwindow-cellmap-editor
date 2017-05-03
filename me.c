/*
// me.c ( Map_Editor )
*/

/* compile command */
/* % cc -Aa -I/usr/include/X11R5 -D_HPUX_SOURCE -L/usr/lib/X11R5
   me.c -o me -lX11 */


/* ### Include Files ### */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>


/* ### Constant Define ### */
#define VERSION         "1.01"
                                                           /* define symbols */
#define FALSE           0
#define TRUE            !FALSE
                                                       /* define window size */
#define CELL            8                                       /* cell size */
#define BLOCK           16               /* block size (unit of window size) */
#define SW_X            (16*8)                                /* switch size */
#define SW_Y            (16*4)

#define DEFAULT_WIDTH   (16*(40+16))                  /* default window size */
#define DEFAULT_HEIGHT  (SW_Y*7)

#define MIN_WIDTH       (16*(8+16))                     /* window size limit */
#define MIN_HEIGHT      (SW_Y*7)
#define MAX_WIDTH       (16*(56+16))
#define MAX_HEIGHT      (16*(48+4))

#define MAX_P_BWIDTH    (16*56/2)                    /* pixmap(for map) size */
#define MAX_P_BHEIGHT   (16*48/2)
#define MAX_P_WIDTH     (16*56)                       /* pixmap(for w0) size */
#define MAX_P_HEIGHT    (16*48)

#define W0_M_WIDTH      SW_X                             /* window menu size */
#define W0_M_HEIGHT     (SW_Y*7)
#define MAP_X           (16*(8+4))                       /* map L-U position */
#define MAP_Y           (16*2)

#define W1_WIDTH        (16*36)                             /* window-1 size */
#define W1_HEIGHT       (16*8)

#define W2_WIDTH        (16*16)                             /* window-2 size */
#define W2_HEIGHT       (16*(32+4))
#define W2_M_WIDTH      (16*16)                         /* param-output size */
#define W2_M_HEIGHT     (16*4)
#define W2_P_WIDTH      (16*16)                         /* pixmap(cell) size */
#define W2_P_HEIGHT     (16*256)
                                          /* define maximum symbols for edit */
#define CELL_MAX        256
#define M_MX            256
#define M_MY            256
#define FONT_MAX        2
#define MENU_MAX        3
#define ZOOM_MAX        4
#define PART_MAX        5
#define SCROLL_MAX      24
#define COLOR_MAX       64
#define CURSOR_MAX      2
                                        /* define maximum symbols for system */
#define MES_MAX         3
#define INFO_MAX        8
#define BOX_LEN         10
#define FILE_LEN        48
#define INFO_LEN        32


/* ### Macro Define ### */
#define u_char          unsigned char
#define u_short         unsigned short
#define u_long          unsigned long
#define u_int           unsigned int

#define _in(_x_, _min_, _max_)  ((_min_ <= _x_)&&(_x_ < _max_))


/* ### Data Define ### */
                                                            /* const strings */
const char      box[30][BOX_LEN+1] = {
  "MAP_EDITOR", "INFO",
  " MAP LOAD ", " MAP SAVE ",
  " MAP YANK ", " MAP PASTE",
  "          ", " COMMAND  ",
  "  MENU 1  ", "CELL",

  "CELL LOAD ", "CELL SAVE ",
  "CELL ATTR ", "CELL EDIT ",
  "          ", " COMMAND  ",
  "  MENU 2  ", "  NO_USE  ",
  "   ZOOM   ", "  SCROLL  ",

  "   PART   ", " ENV SAVE ",
  "   QUIT   ", " COMMAND  ",
  "  MENU 3  ", "  NO_USE  ",
  "   O. K.  ", "  NO_USE  ",
  "  NO_USE  ", "  NO_USE  ",
};

                                                                    /* fonts */
static char     *fontname[FONT_MAX] = {
  "-hp-mincho-medium-r-normal--24-240-75-75-c-120-hp-japanese15",
  "fg-16" };
#define FS_0X           12
#define FS_0Y           24
#define FS_1X           10
#define FS_1Y           12

                                                                /* zoom size */
const int       zs[ZOOM_MAX] =
{ 1, 2, 4, 8 };

                                                         /* switch_box lines */
static XSegment seg_sw[] = {
  1,      1,      SW_X-2, 1,
  SW_X-2, 1,      SW_X-2, SW_Y-2,
  SW_X-2, SW_Y-2, 1,      SW_Y-2,
  1,      SW_Y-2, 1,      1,
  3,      5,      SW_X-4, 5,
  3,      SW_Y-6, SW_X-4, SW_Y-6,
};

                                                                  /* cursors */
const int       cursor_name[CURSOR_MAX] =
{ XC_tcross, XC_watch };

                                                      /* default color table */
const char      RGB[COLOR_MAX][3] = {
  { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 2 }, { 0, 0, 3 },
  { 0, 1, 0 }, { 0, 1, 1 }, { 0, 1, 2 }, { 0, 1, 3 },
  { 0, 2, 0 }, { 0, 2, 1 }, { 0, 2, 2 }, { 0, 2, 3 },
  { 0, 3, 0 }, { 0, 3, 1 }, { 0, 3, 2 }, { 0, 3, 3 },
  { 1, 0, 0 }, { 1, 0, 1 }, { 1, 0, 2 }, { 1, 0, 3 },
  { 1, 1, 0 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 },
  { 1, 2, 0 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 2, 3 },
  { 1, 3, 0 }, { 1, 3, 1 }, { 1, 3, 2 }, { 1, 3, 3 },
  { 2, 0, 0 }, { 2, 0, 1 }, { 2, 0, 2 }, { 2, 0, 3 },
  { 2, 1, 0 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 1, 3 },
  { 2, 2, 0 }, { 2, 2, 1 }, { 2, 2, 2 }, { 2, 2, 3 },
  { 2, 3, 0 }, { 2, 3, 1 }, { 2, 3, 2 }, { 2, 3, 3 },
  { 3, 0, 0 }, { 3, 0, 1 }, { 3, 0, 2 }, { 3, 0, 3 },
  { 3, 1, 0 }, { 3, 1, 1 }, { 3, 1, 2 }, { 3, 1, 3 },
  { 3, 2, 0 }, { 3, 2, 1 }, { 3, 2, 2 }, { 3, 2, 3 },
  { 3, 3, 0 }, { 3, 3, 1 }, { 3, 3, 2 }, { 3, 3, 3 } };


/* ### Global Variables ### */

/* for DISPLAY */
Display *dpy;                   /* display id */
Window  w0, w1, w2;             /* window id for "w0", "w1" and "w2" */
XEvent  event;                  /* event id */
u_int   width, height;          /* window size */

Pixmap  pix0, pix1, pix2;                            /* Pixmap for w0 (main) */
Pixmap  info_pix0;                                   /* Pixmap for w1 (info) */
Pixmap  cell_pix0, cell_pix1, cell_pix2, cell_pix3;  /* Pixmap for w2 (cell) */
Pixmap  c_cell, w_cell, sw_pix;                          /* Pixmap for parts */

                     /* Graphic Context id for "normal", "black" and "white" */
GC      gc, gc_b, gc_w;

XGCValues       gv;        /* Graphic Value id */
Font    font[FONT_MAX];    /* font id for "large" and "small" */

GC      gcc[COLOR_MAX];    /* Graphic Context id for color */
XColor  xc[COLOR_MAX];     /* XColor for color-decidion */
Colormap        cmap;      /* colormap for color-output */

Cursor  cursor[CURSOR_MAX];      /* cursor id for "ready" and "wait" */


/* for EDITOR */
int     menu_mode;               /* id for input-wait-mode */
int     info_mode;               /* id for input-wait-mode */
int     cell_mode;               /* number for cell-selection */

int     current;                             /* current cell number */
int     zoom, scroll, part;                  /* id */
int     edit_cell, edit_color;/* number of "editting-cell", "selected-color" */
char    c_work[CELL][CELL];                  /* cell-data of editting cell */

int     bx, by;                              /* map L-U index */
int     bwidth, bheight;                     /* window size index */
int     x_index, y_index;                    /* mouse index */
int     w0_x, w0_y, w1_x, w1_y, w2_x, w2_y;  /* window position */

int     mark_sx, mark_sy, mark_dx, mark_dy;  /* mark point index "s" and "d" */
int     mark_cell;                           /* mark cell number */
int     x_yank, y_yank;                      /* map yank size index */

char    mes_buf[MES_MAX][INFO_LEN];          /* message buffer for window-1 */
char    file_buf[INFO_MAX][INFO_LEN];        /* filename buffer for edit */
int     f_c[INFO_MAX];                       /* filename length */

int     count[CELL_MAX];                     /* count of each cell */
char    attribute[CELL_MAX][INFO_LEN];       /* attribute-name for each cell */
char    cell_data[CELL_MAX][CELL][CELL];     /* cell-data for each cell */

u_char  map_data[M_MX][M_MY];                /* map data */
u_char  yank[M_MX][M_MY];                    /* map yank data */


/* ### Prototype Definition ### */

void    WindowInit();                                       /* Create Window */
void    PixInit();                                /* Draw Pixmap & Character */
void    ColorInit(char *filename);                          /* ColorMap init */
void    Redraw(Pixmap px);                     /* Copy from Pixmap to Window */
void    Destroy();                                         /* Destroy Window */
void    Flush(Pixmap px,
	      int p_x, int p_y, int p_width, int p_height);    /* Flush Area */
void    Wait(int intr);                                        /* Empty Loop */
void    Wait2();                                        /* Wait for No_Evevt */
void    CursorChange(int cursor_ID);                        /* cursor change */

void    EditorInit();                               /* initialize for editor */
void    Box_Check(int menu_ID, int index_ID);           /* box check routine */
void    Command_Check(char *str);                   /* command check routine */

void    SW_Draw(int menu_ID);                            /* draw switch area */
void    Scale_Draw();                                     /* draw scale area */
void    Map_Draw(int mode);                                 /* draw map area */
int     Map_Scroll(int scr_ID, int scr_w, int scr_h);          /* map scroll */
void    Cell_Edit_Draw();                             /* draw cell_edit area */
void    Cell_Copy(Pixmap px, int cell_ID,
		  int zoomsize);               /* choose current cell pixmap */
void    Cell_Draw(Pixmap spx, Pixmap dpx,
		  int x, int y, int zoomsize);   /* draw current cell pixmap */

void    Sub1Redraw(int pxno);                           /* Redraw Sub1Window */
void    Sub2Redraw(int mode, int no);                   /* Redraw Sub2Window */
void    MesInit();                                        /* w1 message init */
void    Cell_Count();                              /* count up for each cell */

                                        /* buffer control for keyboard input */
void    Buf_Clear(int f_no);
void    Buf_Erase(int f_no);
void    Buf_Key(int f_no, char ch);
void    Buf_Enter(int f_no);

                                           /* editorial functions for switch */
void    F_Menu();
void    F_Map_Load(char *filename, int s_x, int s_y);
void    F_Map_Save(char *filename, int s_x, int s_y, int d_x, int d_y);

void    F_Cell_Load(char *filename);
void    F_Cell_Save();
void    F_Cell_Attr(char *str);

void    F_Zoom(int zm);
void    F_Scroll(int scr);
void    F_Partition(int part);
void    F_Env_Save();
void    F_Quit();

void    F_Map_Box(int s_x, int s_y, int d_x, int d_y, int cell_ID);
void    F_Map_Fill(int s_x, int s_y, int d_x, int d_y, int cell_ID);
void    F_Map_Yank(int s_x, int s_y, int d_x, int d_y);
void    F_Map_Paste(int s_x, int s_y);
void    F_Map_Replace(int s_x, int s_y, int d_x, int d_y,
		      int cell_ID1, int cell_ID2);
void    F_Map_Flip(int s_x, int s_y, int d_x, int d_y, char *str);
void    F_Cell_Swap(int cell_ID1, int cell_ID2);
void    F_Jump(int x, int y);
void    F_Cell_Edit(int cell_ID);


/* =-=-=-=-=-=-=-=-= */
main(int argc, char *argv[])
{
  char    keybuf[16];
  KeySym  key;
  int     intx, d1, d2;

  Window  dummy0;
  int     dummy1;
  u_int    dummy2;

                                                                  /* message */
  fprintf(stderr,
	  "\nMap_Editor(%s) : ver." VERSION " Copyright (c) abechanta 1994\n",
	  argv[0]);


  EditorInit();                                     /* initialize for editor */
  WindowInit();                                     /* initialize for window */

  if (argc >= 2) {
    strcpy(file_buf[1], argv[1]);
  }
  f_c[1] = strlen(file_buf[1]);
  Buf_Enter(1);

  ColorInit(file_buf[1]);                         /* initialize for colormap */
  PixInit();                                        /* initialize for pixmap */

                                                                /* main loop */
  while (TRUE) {
    if (XMaskEvent(dpy,
		   ExposureMask | ButtonPressMask |
		   PointerMotionMask | KeyPressMask,
		   &event)) {


                                                  /* calc index for map_data */
      d1 = (event.xbutton.x - MAP_X)/(CELL*zs[zoom]);
      if (_in(event.xbutton.x, MAP_X, MAP_X + CELL*bwidth)&&
	  _in(bx + d1, 0, M_MX)) {
	x_index = d1;
      } else {
	x_index = -1;
      }

      d2 = (event.xbutton.y - MAP_Y)/(CELL*zs[zoom]);
      if (_in(event.xbutton.y, MAP_Y, MAP_Y + CELL*bheight)&&
	  _in(by + d2, 0, M_MY)) {
	y_index = d2;
      } else {
	y_index = -1;
      }



      if (event.xany.window == w0) {
	switch (event.type) {
                                                      /* w0-Expose -> redraw */
	case Expose:
	  Wait2();
	  CursorChange(1);                                    /* wait cursor */

	  /* もし画面サイズが変っていたら */
	  XGetGeometry(dpy, w0, &dummy0,
		       &dummy1, &dummy1, &width, &height, &dummy2, &dummy2);
	  XFlush(dpy);
	  d1 = (width /BLOCK - 8 - 4 - 4)*2;
	  d2 = (height/BLOCK     - 2 - 2)*2;
	  if ((bwidth != d1) || (bheight != d2)) {
	    bwidth  = d1;
	    bheight = d2;
	    Map_Scroll(15, 0, 0);
	    Scale_Draw();
	    Map_Draw(TRUE);
	  }

	  /* 画面書き直し */
	  Redraw(pix0);
	  Sub1Redraw(info_mode);
	  Sub2Redraw(FALSE, cell_mode);
	  break; /* endswitch Expose */
                                              /* w0-Button1 -> decidion, put */
                                                /* w0-Button2 -> scroll, get */
                                                    /* w0-Button3 -> mark on */
	case ButtonPress:
	  if (info_mode == 0) {
	    switch (event.xbutton.button) {
	    case Button1:
	      if (_in(event.xbutton.x, 0, W0_M_WIDTH )&&
		  _in(event.xbutton.y, 0, W0_M_HEIGHT)&&
		  (event.xbutton.y % SW_Y != 0)) {

		/* スイッチが押された */
		Box_Check(menu_mode, event.xbutton.y/SW_Y);

	      } else if ((x_index >= 0)&&(y_index >= 0)) {

		/* マップにセルをプット */
		map_data[bx + x_index][by + y_index] = (u_char)current;
		Cell_Draw(c_cell, pix2, x_index, y_index, zs[zoom]);
		Map_Draw(FALSE);
		Redraw(pix0);
		MesInit();
		Sub1Redraw(info_mode = 0);
	      }
	      break; /* endswitch Button1 */

	    case Button2:
	      if (event.xbutton.x >= W0_M_WIDTH) {
		intx = (event.xbutton.x - W0_M_WIDTH)/
		  ((width - W0_M_WIDTH)/4) +
		    4*(event.xbutton.y/(height/4));
		if (Map_Scroll(intx, scroll, scroll)) {

		  /* マップのスクロールだ */
		  CursorChange(1);
		  Scale_Draw();
		  Map_Draw(TRUE);
		} else {

		  /* セルのスポイトだ */
		  if ((x_index >= 0)&&(y_index >= 0)) {
		    current = (u_char)map_data[bx + x_index][by + y_index];
		    Cell_Copy(c_cell, current, zs[zoom]);
		  }
		}

		/* 画面書き直し */
		Redraw(pix0);
		Sub1Redraw(info_mode = 0);
		Sub2Redraw(FALSE, cell_mode);
	      }
	      break; /* endswitch Button2 */

	    case Button3:
	      /* マークセット */
	      if ((x_index >= 0)&&(y_index >= 0)) {
		mark_dx = mark_sx;
		mark_dy = mark_sy;
		mark_sx = bx + x_index;
		mark_sy = by + y_index;
		MesInit();
		sprintf(mes_buf[0], "mark s =( %3d %3d)", mark_sx, mark_sy);
		sprintf(mes_buf[1], "mark d =( %3d %3d)", mark_dx, mark_dy);
		Sub1Redraw(info_mode);
	      }
	      break; /* endswitch Button3 */

	    }
	  } else {
	    /* info_mode が 0 じゃない（入力要求があったのに．．．） */
	    MesInit();
	    sprintf(mes_buf[0], "Canselled.");
	    Sub1Redraw(info_mode = 0);
	  }
	  Wait2();
	  break; /* endswitch ButtonPress */

                                                   /* w0-Motion -> redraw w1 */
	case MotionNotify:
	  /* マウス座標の表示 */
	  if (info_mode == 0) {
	    Sub1Redraw(info_mode);
	  }
	  break; /* endswitch MotionNotify */

                                                         /* w0-Key -> scroll */
	case KeyPress:
	  if (info_mode == 0) {
	    CursorChange(1);
	    XLookupString((XKeyEvent*)&event, keybuf, 2, &key, NULL);
	    keybuf[1] = '\0';
	    if ((key == XK_KP_1)||(key == XK_KP_2)||
		(key == XK_KP_3)||(key == XK_KP_4)||
		(key == XK_KP_5)||(key == XK_KP_6)||
		(key == XK_KP_7)||(key == XK_KP_8)||
		(key == XK_KP_9)) {
	      /* キーパットによるスクロールだ */
	      switch (key) {
	      case XK_KP_1:
		intx = 12;
		break; /* endswitch XK_KP_1 */

	      case XK_KP_2:
		intx = 13;
		break; /* endswitch XK_KP_2 */

	      case XK_KP_3:
		intx = 15;
		break; /* endswitch XK_KP_3 */

	      case XK_KP_4:
		intx = 4;
		break; /* endswitch XK_KP_4 */

	      case XK_KP_5:
		intx = 5;
		break; /* endswitch XK_KP_5 */

	      case XK_KP_6:
		intx = 7;
		break; /* endswitch XK_KP_6 */

	      case XK_KP_7:
		intx = 0;
		break; /* endswitch XK_KP_7 */

	      case XK_KP_8:
		intx = 1;
		break; /* endswitch XK_KP_8 */

	      case XK_KP_9:
		intx = 3;
		break; /* endswitch XK_KP_9 */

	      }
	      if (Map_Scroll(intx,
			     bwidth/zs[zoom]/2, bheight/zs[zoom]/2)) {
		Scale_Draw();
		Map_Draw(TRUE);
	      }
	      Redraw(pix0);
	      Sub1Redraw(info_mode);
	    }
	    Wait2();
	    break; /* endswitch KeyPress */

	  }
	}
      }



      if (event.xany.window == w1) {
	switch (event.type) {
                                                      /* w1-Expose -> redraw */
	case Expose:
	  CursorChange(1);
	  Sub1Redraw(info_mode);
	  break; /* endswitch Expose */

                                                /* w1-Button1 -> editor_info */
                                     /* w1-Button2 -> editor_info, buf-clear */
                                                  /* w1-Button3 -> mark_info */
	case ButtonPress:
	  CursorChange(1);
	  switch (info_mode) {
	  case 0:
	    switch (event.xbutton.button) {
	    case Button1:
	    case Button2:
	      MesInit();
	      sprintf(mes_buf[0], "Zoom : x%d", zs[zoom]);
	      sprintf(mes_buf[1], "Scroll : %d cell", scroll);
	      sprintf(mes_buf[2], "Partition : no.%d", part);
	      Sub1Redraw(info_mode);
	      break; /* endswitch Button1, Button2 */

	    case Button3:
	      MesInit();
	      sprintf(mes_buf[0], "mark s=( %3d %3d)", mark_sx, mark_sy);
	      sprintf(mes_buf[1], "mark d=( %3d %3d)", mark_dx, mark_dy);
	      sprintf(mes_buf[2], "mark cell : no.%3d", mark_cell);
	      Sub1Redraw(info_mode);
	      break; /* endswitch Button3 */

	    }
	    break; /* endswitch info_mode = 0 */

	  case 1:
	    if ((event.xbutton.button == Button1)&&
		(event.xbutton.y > BLOCK*5)) {
	      Flush(info_pix0, 0, BLOCK*5, W1_WIDTH, BLOCK*3);
	      F_Quit();
	    }
	    break; /* endswitch info_mode = 1 */

	  case 2:
	  case 3:
	  case 4:
	  case 5:
	  case 6:
	  case 7:
	    if (event.xbutton.button == Button2) {
	      Buf_Clear(info_mode);
	      Sub1Redraw(info_mode);
	    }
	    break; /* endswitch info_mode = 2, 3, 4, 5, 6, 7 */

	  }
	  break; /* endswitch ButtonPress */

                                                      /* w1-Key -> buf-input */
	case KeyPress:
	  if ((info_mode == 2)||
	      (info_mode == 3)||
	      (info_mode == 4)||
	      (info_mode == 5)||
	      (info_mode == 6)||
	      (info_mode == 7)) {
	    XLookupString((XKeyEvent*)&event, keybuf, 2, &key, NULL);
	    keybuf[1] = '\0';
	    if ((!IsCursorKey(key))&&
		(!IsFunctionKey(key))&&
		(!IsMiscFunctionKey(key))&&
		(!IsModifierKey(key))&&
		(!IsPFKey(key))) {
	      switch (key) {
	      case XK_Return:
		Buf_Enter(info_mode);
		Sub1Redraw(info_mode);
		Flush(info_pix0, 0, BLOCK*5, W1_WIDTH, BLOCK*3);
		CursorChange(1);
		switch (info_mode) {
		case 2:
		  F_Map_Load(file_buf[info_mode], 0, 0);
		  Cell_Count();
		  Sub2Redraw(TRUE, cell_mode);
		  strcpy(file_buf[info_mode + 1], file_buf[info_mode]);
		  f_c[info_mode + 1] = f_c[info_mode];
		  break; /* endswitch info_mode = 2 */

		case 3:
		  F_Map_Save(file_buf[info_mode], 0, 0, M_MX - 1, M_MY - 1);
		  break; /* endswitch info_mode = 3 */

		case 4:
		  F_Cell_Load(file_buf[info_mode]);
		  strcpy(file_buf[info_mode + 1], file_buf[info_mode]);
		  f_c[info_mode + 1] = f_c[info_mode];
		  break; /* endswitch info_mode = 4 */

		case 5:
		  F_Cell_Save(file_buf[info_mode]);
		  break; /* endswitch info_mode = 5 */

		case 6:
		  F_Cell_Attr(file_buf[info_mode]);
		  break; /* endswitch info_mode = 6 */

		case 7:
		  Command_Check(file_buf[info_mode]);
		  break; /* endswitch info_mode = 7 */

		}
		Sub1Redraw(info_mode = 0);
		Redraw(pix0);
		break; /* endswitch XK_Return */

	      case XK_BackSpace:
	      case XK_Delete:
		Buf_Erase(info_mode);
		Sub1Redraw(info_mode);
		break; /* endswitch XK_BackSpace, XK_Delete */

	      default:
		Buf_Key(info_mode, keybuf[0]);
		Sub1Redraw(info_mode);
		break; /* endswitch default */

	      }
	    }
	  }
	  Wait2();
	  break; /* endswitch KeyPress */

	}
      }



      if (event.xany.window == w2) {
	switch (event.type) {
                                                      /* w2-Expose -> redraw */
	case Expose:
	  CursorChange(1);
	  Sub2Redraw(FALSE, cell_mode);
	  break; /* endswitch Expose */

                                                 /* w2-Button1 -> count, get */
                                                     /* w2-Button2 -> scroll */
                                                    /* w2-Button3 -> mark on */
	case ButtonPress:
	  if (info_mode == 0) {
	    CursorChange(1);
	    d1 = (event.xbutton.x - W2_WIDTH)/BLOCK;
	    d2 = event.xbutton.y/BLOCK;
	    switch (event.xbutton.button) {
	    case Button1:
	      if (event.xbutton.x < W2_WIDTH) {
		if (event.xbutton.y < W2_M_HEIGHT) {
		  Cell_Count();
		  Sub2Redraw(TRUE, cell_mode);
		} else {
		  current = cell_mode + (event.xbutton.y - W2_M_HEIGHT)/(BLOCK*2);
		  Cell_Copy(c_cell, current, zs[zoom]);
		  Sub2Redraw(FALSE, cell_mode);
		}
	      } else {
		if (_in(d1, 4, 4 + 8)&&
		    _in(d2, 4, 4 + 8)) {
		  c_work[d1 - 4][d2 - 4] = (u_char)edit_color;
		  XFillRectangle(dpy, cell_pix3, gcc[edit_color],
				 BLOCK*d1, BLOCK*d2, BLOCK, BLOCK);
		  Redraw(cell_pix3);
		} else if (_in(d2, 16, 32)) {
		  edit_color = (u_char)(8*((d2 - 16)/2) + d1/2);
		  Cell_Edit_Draw();
		  Redraw(cell_pix3);
		} else if (32 <= d2) {
		  Flush(cell_pix3, 0, BLOCK*32, W2_WIDTH, BLOCK*4);
		  F_Cell_Edit(-1);
		}
	      }
	      break; /* endswitch Button1 */

	    case Button2:
	      if (event.xbutton.x < W2_WIDTH) {
		if (event.xbutton.y >= W2_M_HEIGHT) {
		  if (event.xbutton.y - W2_M_HEIGHT < (W2_HEIGHT - W2_M_HEIGHT)/4){
		    cell_mode = cell_mode == 0 ? 0 : cell_mode - 16;
		  } else if (event.xbutton.y - W2_M_HEIGHT >=
			     (W2_HEIGHT - W2_M_HEIGHT)*3/4) {
		    cell_mode =
		      cell_mode == CELL_MAX - 16 ? CELL_MAX - 16 : cell_mode + 16;
		  }
		  Sub2Redraw(FALSE, cell_mode);
		}
	      } else {
		if (_in(d1, 4, 4 + 8)&&
		    _in(d2, 4, 4 + 8)) {
		  edit_color = (u_char)c_work[d1 - 4][d2 - 4];
		  Cell_Edit_Draw();
		  Redraw(cell_pix3);
		}
	      }
	      break; /* endswitch Button2 */

	    case Button3:
	      if (event.xbutton.x < W2_WIDTH) {
		if (event.xbutton.y >= W2_M_HEIGHT) {
		  mark_cell =cell_mode+(event.xbutton.y - W2_M_HEIGHT)/(BLOCK*2);
		  MesInit();
		  sprintf(mes_buf[0], "mark cell : no.%3d", mark_cell);
		  Sub1Redraw(info_mode);
		}
	      } else {
		if (_in(d1, 4, 4 + 8)&&
		    _in(d2, 4, 4 + 8)) {
		  F_Cell_Edit(mark_cell);
		}
	      }
	      break; /* endswitch Button3 */

	    }
	  }
	  break; /* endswitch ButtonPress */

	}
      }



      CursorChange(0);
    }
  }
}


/**/


void    WindowInit()
{
  u_long   black, white;
  XSizeHints      hint;
  int  i;

  Window  dummy0;
  int     dummy1;
  u_int    dummy2;

                                                             /* display open */
  if ((dpy = XOpenDisplay(NULL) ) == NULL) {
    fprintf(stderr, "Can't open display !\n");
    exit(0);
  }

                                                                /* get pixel */
  black = BlackPixel(dpy, 0);
  white = WhitePixel(dpy, 0);

                                                              /* window open */
  w0 = XCreateSimpleWindow(dpy, RootWindow(dpy, 0),
			   0, 0, width, height,
			   1, white, black);
  w1 = XCreateSimpleWindow(dpy, RootWindow(dpy, 0),
			   0, 0, W1_WIDTH, W1_HEIGHT,
			   1, white, black);
  w2 = XCreateSimpleWindow(dpy, RootWindow(dpy, 0),
			   0, 0, W2_WIDTH, W2_HEIGHT,
			   1, white, black);

                                               /* create pixmap for w0(main) */
  pix0 = XCreatePixmap(dpy, w0,
		       MAX_WIDTH, MAX_HEIGHT,
		       DefaultDepth(dpy, 0));
  pix1 = XCreatePixmap(dpy, w0,
		       MAX_P_WIDTH, MAX_P_HEIGHT,
		       DefaultDepth(dpy, 0));
  pix2 = XCreatePixmap(dpy, w0,
		       MAX_P_WIDTH, MAX_P_HEIGHT,
		       DefaultDepth(dpy, 0));
                                               /* create pixmap for w1(info) */
  info_pix0 = XCreatePixmap(dpy, w0,
			    W1_WIDTH, W1_HEIGHT,
			    DefaultDepth(dpy, 0));
                                               /* create pixmap for w2(cell) */
  cell_pix0 = XCreatePixmap(dpy, w0,
			    W2_M_WIDTH, W2_M_HEIGHT,
			    DefaultDepth(dpy, 0));
  cell_pix1 = XCreatePixmap(dpy, w0,
			    W2_P_WIDTH, W2_P_HEIGHT,
			    DefaultDepth(dpy, 0));
  cell_pix2 = XCreatePixmap(dpy, w0,
			    W2_P_WIDTH, W2_P_HEIGHT,
			    DefaultDepth(dpy, 0));
  cell_pix3 = XCreatePixmap(dpy, w0,
			    W2_P_WIDTH, W2_HEIGHT,
			    DefaultDepth(dpy, 0));
                                                  /* create pixmap for parts */
  sw_pix = XCreatePixmap(dpy, w0,
			 SW_X, SW_Y,
			 DefaultDepth(dpy, 0));
                                                   /* create pixmap for work */
  c_cell = XCreatePixmap(dpy, w0,
			 CELL*zs[ZOOM_MAX-1], CELL*zs[ZOOM_MAX-1],
			 DefaultDepth(dpy, 0));
  w_cell = XCreatePixmap(dpy, w0,
			 CELL*zs[ZOOM_MAX-1], CELL*zs[ZOOM_MAX-1],
			 DefaultDepth(dpy, 0));


                                                             /* select input */
  XSelectInput(dpy, w0,
	       ExposureMask | ButtonPressMask |
	       PointerMotionMask | KeyPressMask );
  XSelectInput(dpy, w1,
	       ExposureMask | KeyPressMask | ButtonPressMask);
  XSelectInput(dpy, w2,
	       ExposureMask | ButtonPressMask);

                                                                /* create gc */
  gc   = XCreateGC(dpy, w0, 0, NULL);
  gc_b = XCreateGC(dpy, w0, 0, NULL);
  gc_w = XCreateGC(dpy, w0, 0, NULL);
  XSetForeground(dpy, gc  , white);
  XSetForeground(dpy, gc_b, black);
  XSetForeground(dpy, gc_w, white);
  XSetFunction(dpy, gc, GXxor);
  gv.graphics_exposures = False;
  XChangeGC(dpy, gc  , GCBackground, &gv);
  XChangeGC(dpy, gc_b, GCForeground, &gv);
  XChangeGC(dpy, gc_w, GCBackground, &gv);
                                                          /* set hint for w0 */
  hint.flags = PMaxSize | PMinSize;
  hint.min_width  = MIN_WIDTH;
  hint.min_height = MIN_HEIGHT;
  hint.max_width  = MAX_WIDTH;
  hint.max_height = MAX_HEIGHT;
  XSetStandardProperties(dpy, w0, box[0], box[0], None, NULL, 0, &hint);
                                                          /* set hint for w1 */
  hint.flags = PMaxSize | PMinSize;
  hint.min_width  = hint.max_width  = W1_WIDTH;
  hint.min_height = hint.max_height = W1_HEIGHT;
  XSetStandardProperties(dpy, w1, box[1], box[1], None, NULL, 0, &hint);
                                                          /* set hint for w2 */
  hint.flags = PMaxSize | PMinSize;
  hint.min_width  = W2_WIDTH;
  hint.max_width  = W2_WIDTH + W2_WIDTH;
  hint.min_height = hint.max_height = W2_HEIGHT;
  XSetStandardProperties(dpy, w2, box[9], box[9], None, NULL, 0, &hint);

                                                                 /* set font */
  for (i = 0; i < FONT_MAX; i++) {
    font[i] = XLoadFont(dpy, fontname[i]);
  }
  XSetFont(dpy, gc  , font[0]);
  XSetFont(dpy, gc_b, font[0]);
  XSetFont(dpy, gc_w, font[0]);

                                                               /* map window */
  XMapWindow(dpy, w2);
  XMoveWindow(dpy, w2, w2_x, w2_y);
  XMapWindow(dpy, w1);
  XMoveWindow(dpy, w1, w1_x, w1_y);
  XMapWindow(dpy, w0);
  XMoveWindow(dpy, w0, w0_x, w0_y);

                                                               /* set cursor */
  for (i = 0; i < CURSOR_MAX; i++) {
    cursor[i] = XCreateFontCursor(dpy, cursor_name[i]);
  }
  XDefineCursor(dpy, w0, cursor[0]);
  XDefineCursor(dpy, w1, cursor[0]);
  XDefineCursor(dpy, w2, cursor[0]);

                                                             /* get geometry */
  XGetGeometry(dpy, w0, &dummy0,
	       &dummy1, &dummy1, &width, &height, &dummy2, &dummy2);
  XFlush(dpy);
  bwidth  = (width /BLOCK - 8 - 4 - 4)*2;
  bheight = (height/BLOCK     - 2 - 2)*2;

} /* end of WindowInit */


void    PixInit()
{
  int  i;

                                                         /* clear all pixmap */
  XFillRectangle(dpy, pix0, gc_b, 0, 0, MAX_WIDTH, MAX_HEIGHT);
  XFillRectangle(dpy, pix1, gc_b, 0, 0, MAX_P_WIDTH, MAX_P_HEIGHT);
  XFillRectangle(dpy, pix2, gc_b, 0, 0, MAX_P_WIDTH, MAX_P_HEIGHT);

  XFillRectangle(dpy, info_pix0, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT);

  XFillRectangle(dpy, cell_pix0, gc_b, 0, 0, W2_M_WIDTH, W2_M_HEIGHT);
  XFillRectangle(dpy, cell_pix1, gc_b, 0, 0, W2_P_WIDTH, W2_P_HEIGHT);
  XFillRectangle(dpy, cell_pix2, gc_b, 0, 0, W2_P_WIDTH, W2_P_HEIGHT);
  XFillRectangle(dpy, cell_pix3, gc_b, 0, 0, W2_P_WIDTH, W2_HEIGHT);
  for (i = 0; i < COLOR_MAX; i++) {
    XFillRectangle(dpy, cell_pix3, gcc[i],
		   BLOCK*2*(i%8), BLOCK*2*(8+i/8), BLOCK*2, BLOCK*2);
  }
  XDrawRectangle(dpy, cell_pix3, gc_w,
		 0, 0, W2_WIDTH-1, BLOCK*16-1);
  XDrawRectangle(dpy, cell_pix3, gc_w,
		 BLOCK*4-1, BLOCK*4-1, BLOCK*8+1, BLOCK*8+1);
  XDrawRectangle(dpy, cell_pix3, gc_w,
		 BLOCK*4-4, BLOCK*4-4, BLOCK*8+7, BLOCK*8+7);
  for (i = 0; i < 4; i++) {
    XDrawRectangle(dpy, cell_pix3, gc_w,
		   i*2, BLOCK*32+i*2, W2_WIDTH-i*4-1, BLOCK*4-i*4-1);
  }
  XSetFont(dpy, gc_w, font[0]);
  XDrawString(dpy, cell_pix3, gc_w,
	      (W2_WIDTH-BOX_LEN*FS_0X)/2, BLOCK*32+(BLOCK*4+FS_0Y)/2,
	      box[26], BOX_LEN);
                                                            /* pixmap switch */
  XFillRectangle(dpy, sw_pix, gc_b, 0, 0, SW_X, SW_Y);
  XDrawSegments(dpy, sw_pix, gc_w, seg_sw, 6);

                                                           /* pixmap current */
  XFillRectangle(dpy, c_cell, gc_b, 0, 0, CELL*zs[zoom], CELL*zs[zoom]);
  Cell_Copy(c_cell, current, zs[zoom]);

                                                              /* pixmap work */
  XFillRectangle(dpy, w_cell, gc_b, 0, 0, CELL*zs[zoom], CELL*zs[zoom]);

  SW_Draw(menu_mode);
  Scale_Draw();
  Map_Draw(TRUE);
  Cell_Count();

  Redraw(pix0);
  Sub1Redraw(info_mode);
  Sub2Redraw(TRUE, cell_mode);

} /* end of PixInit */


void    ColorInit(char *filename)
{
  FILE *fp;
  int  i;

                                                          /* create colormap */
  cmap = XDefaultColormap(dpy, 0);

                                                         /* open color_table */
  fp = fopen(filename, "r");
  if (fp == NULL) {
                                               /* failed to open color_table */
    for (i = 0; i < COLOR_MAX; i++) {
      xc[i].red   = RGB[i][0]*COLOR_MAX*256;
      xc[i].green = RGB[i][1]*COLOR_MAX*256;
      xc[i].blue  = RGB[i][2]*COLOR_MAX*256;
    }
    strcpy(file_buf[1], "NONE");
  } else {
                                            /* successed to open color_table */
    for (i = 0; i < COLOR_MAX; i++) {
      xc[i].red   = (u_char)fgetc(fp)*COLOR_MAX*256;
      xc[i].green = (u_char)fgetc(fp)*COLOR_MAX*256;
      xc[i].blue  = (u_char)fgetc(fp)*COLOR_MAX*256;
    }
    fclose(fp);

    MesInit();
    sprintf(mes_buf[0], "Load private color-");
    sprintf(mes_buf[1], "file from");
    sprintf(mes_buf[2], "[%s].", filename);
  }

                                                          /* create color_gc */
  for (i = 0; i < COLOR_MAX; i++) {
    gcc[i] = XCreateGC(dpy, w0, 0, NULL);
    gv.graphics_exposures = False;
    XChangeGC(dpy, gcc[i] , GCBackground, &gv);
    XAllocColor(dpy, cmap, &xc[i]);
    XSetForeground(dpy, gcc[i], xc[i].pixel);
  }

} /* end of ColorInit */


void    Redraw(Pixmap px)
{
  if ((px == pix0)||
      (px == pix1)||
      (px == pix2)) {
    XCopyArea(dpy, px, w0, gc_b, 0, 0, width, height, 0, 0);
  } else if (px == info_pix0) {
    XCopyArea(dpy, px, w1, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT, 0, 0);
  } else if (px == cell_pix0) {
    XCopyArea(dpy, px, w2, gc_b, 0, 0, W2_M_WIDTH, W2_M_HEIGHT, 0, 0);
  } else if ((px == cell_pix1)||
	     (px == cell_pix2)) {
    XCopyArea(dpy, px, w2, gc_b, 0, 0, W2_P_WIDTH, W2_P_HEIGHT, 0,W2_M_HEIGHT);
  } else if (px == cell_pix3) {
    XCopyArea(dpy, px, w2, gc_b, 0, 0, W2_P_WIDTH, W2_P_HEIGHT, W2_WIDTH, 0);
  }
  XFlush(dpy);

} /* end of Redraw */


void    Destroy()
{
  int  i;

  XFreeGC(dpy, gc  );
  XFreeGC(dpy, gc_b);
  XFreeGC(dpy, gc_w);
  for (i = 0; i < COLOR_MAX; i++) {
    XFreeGC(dpy, gcc[i]);
  }
  XUninstallColormap(dpy, cmap);
  XUndefineCursor(dpy, w0);
  XDestroyWindow(dpy, w2);
  XDestroyWindow(dpy, w1);
  XDestroyWindow(dpy, w0);
  XCloseDisplay(dpy);
  XFlush(dpy);

} /* end of Destroy */


void    Flush(Pixmap px, int p_x, int p_y, int p_width, int p_height)
{
  int  i;

  for (i = 0; i < 6; i++) {
    XFillRectangle(dpy, px, gc, p_x, p_y, p_width, p_height);
    Redraw(px);
    Wait(1);
  }

} /* end of Flush */


void    Wait(int intr)
{
  int  i, j;

  for (j = 0; j < intr; j++) {
    for (i = 0; i < 1000; i++) {
      XCheckMaskEvent(dpy, ExposureMask | ButtonPressMask, &event);
    }
  }

} /* end of Wait */


void    Wait2()
{
  while (XCheckMaskEvent(dpy,
			 ExposureMask | ButtonPressMask |
			 PointerMotionMask | KeyPressMask,
			 &event))
    ;

} /* end of Wait2 */


void    CursorChange(int cursor_ID)
{
  XDefineCursor(dpy, w0, cursor[cursor_ID]);
  XDefineCursor(dpy, w1, cursor[cursor_ID]);
  XDefineCursor(dpy, w2, cursor[cursor_ID]);

} /* end of CursorChange */


void    EditorInit()
{
  FILE *fp;
  char buf[INFO_LEN];
  int  i, j, k, flag;

                                              /* initialize editor_variables */
  menu_mode = info_mode = cell_mode = 0;
  edit_cell = edit_color = 0;

  current = 0;
  part = 0;
  zoom = 0;
  scroll = 4;

  bx = by = 0;
  bwidth = bheight = 0;

  mark_sx = mark_sy = mark_dx = mark_dy = mark_cell = 0;
  x_yank = y_yank = 0;


  width  = DEFAULT_WIDTH;
  height = DEFAULT_HEIGHT;
  w0_x = 279;
  w0_y = 128;
  w1_x =   1;
  w1_y = 128;
  w2_x =   1;
  w2_y = 305;

  for (i = 0; i < INFO_MAX; i++) {
    Buf_Clear(i);
  }

                                                  /* initialize data_section */
  for (j = 0; j < M_MY; j++) {
    for (i= 0; i < M_MX; i++) {
      map_data[i][j] = 0;
    }
  }

  for (i = 0; i < CELL_MAX; i++) {
    count[i] = 0;
    attribute[i][0] = '\0';
    for (j = 0; j < CELL; j++) {
      for (k = 0; k < CELL; k++) {
	cell_data[i][j][k] = i % COLOR_MAX;
      }
    }
  }

  MesInit();

                            /* open env-file for initialize editor_variables */
  sprintf(buf, "%s/.me_env", getenv("HOME"));
  fp = fopen(buf, "r");
  if (fp != NULL) {
    flag = FALSE;
    while (TRUE) {
      if (fscanf(fp, "%s", buf) == EOF) {
	break;
      }
      if (!strcmp(buf, "ZOOM")) {
	fscanf(fp, "%d", &zoom);
      } else if (!strcmp(buf, "SCROLL")) {
	fscanf(fp, "%d", &scroll);
      } else if (!strcmp(buf, "PART")) {
	fscanf(fp, "%d", &part);
      } else if (!strcmp(buf, "COLOR_FILE")) {
	fscanf(fp, "%s", file_buf[1]);
      } else if (!strcmp(buf, "SIZE")) {
	fscanf(fp, "%d %d", &width, &height);
      } else if (!strcmp(buf, "WINDOW_0")) {
	fscanf(fp, "%d %d", &w0_x, &w0_y);
      } else if (!strcmp(buf, "WINDOW_1")) {
	fscanf(fp, "%d %d", &w1_x, &w1_y);
      } else if (!strcmp(buf, "WINDOW_2")) {
	fscanf(fp, "%d %d", &w2_x, &w2_y);
      } else if (buf[0] == '#') {
	while (fgetc(fp) != '\n')
	  ;
      } else if (!strcmp(buf, "END")) {
	flag = TRUE;
	break;
      } else {
	fprintf(stderr,
		"Illigal declaration '%s' in env-file[$HOME/.me_env]\n",
		buf);
	break;
      }
    }
    fclose(fp);

    zoom   = zoom   % ZOOM_MAX;
    scroll = scroll % SCROLL_MAX;
    part   = part   % PART_MAX;
    width  = (width  - MIN_WIDTH ) % (MAX_WIDTH  - MIN_WIDTH  + 1) + MIN_WIDTH;
    height = (height - MIN_HEIGHT) % (MAX_HEIGHT - MIN_HEIGHT + 1) + MIN_HEIGHT;
    if (flag) {
      sprintf(mes_buf[0], "Load env-file from");
      sprintf(mes_buf[1], "[$HOME/.me_env].");
    } else {
      sprintf(mes_buf[0], "Error #Load env#");
      sprintf(mes_buf[1], "[$HOME/.me_env].");
    }
  }

} /* end of EditorInit */


void    Box_Check(int menu_ID, int index_ID)
{
  Flush(pix0, 0, index_ID*SW_Y, SW_X, SW_Y);
  CursorChange(1);
  switch (menu_ID) {
                                                     /* menu 0 (map control) */
  case 0:
    switch (index_ID) {
    case 0:
      MesInit();
      sprintf(mes_buf[0], " ### MAP LOAD ###");
      sprintf(mes_buf[1], "Input file-name.");
      Sub1Redraw(info_mode = 2);
      break;
    case 1:
      MesInit();
      sprintf(mes_buf[0], " ### MAP SAVE ###");
      sprintf(mes_buf[1], "Input file-name.");
      Sub1Redraw(info_mode = 3);
      break;
    case 2:
      Command_Check("map-yank");
      Redraw(pix0);
      Sub1Redraw(info_mode = 0);
      break;
    case 3:
      Command_Check("map-paste");
      Redraw(pix0);
      Sub1Redraw(info_mode = 0);
      break;
    case 4:
      MesInit();
      break;
    case 5:
      MesInit();
      sprintf(mes_buf[0], "  ### COMMAND ###");
      sprintf(mes_buf[1], "Input edit command.");
      Sub1Redraw(info_mode = 7);
      break;
    case 6:
      F_Menu(menu_ID);
      break;
    }
    break; /* endswitch menu_mode = 0 */

                                                    /* menu 1 (cell control) */
  case 1:
    switch (index_ID) {
    case 0:
      MesInit();
      sprintf(mes_buf[0], " ### CELL LOAD ###");
      sprintf(mes_buf[1], "Input file-name.");
      Sub1Redraw(info_mode = 4);
      break;
    case 1:
      MesInit();
      sprintf(mes_buf[0], " ### CELL SAVE ###");
      sprintf(mes_buf[1], "Input file-name.");
      Sub1Redraw(info_mode = 5);
      break;
    case 2:
      MesInit();
      sprintf(mes_buf[0], " ### CELL ATTR ###");
      sprintf(mes_buf[1], "Input attribute.");
      info_mode = 6;
      strcpy(file_buf[info_mode], attribute[current]);
      f_c[info_mode] = strlen(attribute[current]);
      Sub1Redraw(info_mode);
      break;
    case 3:
      MesInit();
      F_Cell_Edit(edit_cell = current);
      break;
    case 4:
      MesInit();
      break;
    case 5:
      MesInit();
      sprintf(mes_buf[0], "  ### COMMAND ###");
      sprintf(mes_buf[1], "Input edit command.");
      Sub1Redraw(info_mode = 7);
      break;
    case 6:
      F_Menu(menu_ID);
      break;
    }
    break; /* endswitch menu_mode = 1 */

                                                  /* menu 2 (editor control) */
  case 2:
    switch (index_ID) {
    case 0:
      F_Zoom(-1);
      break;
    case 1:
      F_Scroll(-1);
      break;
    case 2:
      F_Partition(-1);
      break;
    case 3:
      F_Env_Save();
      break;
    case 4:
      MesInit();
      sprintf(mes_buf[1], "   ### QUIT ###");
      Sub1Redraw(info_mode = 1);
      break;
    case 5:
      MesInit();
      sprintf(mes_buf[0], "  ### COMMAND ###");
      sprintf(mes_buf[1], "Input edit command.");
      Sub1Redraw(info_mode = 7);
      break;
    case 6:
      F_Menu(menu_ID);
      break;
    }
    break; /* endswitch menu_mode = 2 */

  }

} /* end of Box_Check */


void    Command_Check(char *str)
{
  char buf[5][INFO_LEN];
  int  i, j, k, flag;
  int  x1, x2, y1, y2;

                                                          /* string separate */
  flag = TRUE;
  i = -1;
  j = k = 0;
  buf[0][0] = buf[1][0] = buf[2][0] = buf[3][0] = buf[4][0] = '\0';

  while (k < 5) {
    if (flag) {
      while (++i < INFO_LEN) {
	if (*str == '\0') {
	  flag = FALSE;
	  break;
	} else if (isalnum(str[i])) {
	  break;
	}
      }
    } else {
      break;
    }
    if ((i < INFO_LEN)&&(flag)) {
      for (j = 0; j < INFO_LEN; j++) {
	buf[k][j] = str[j + i];
	if (buf[k][j] == ' ') {
	  buf[k][j] = '\0';
	  break;
	} else if (buf[k][j] == '\0') {
	  flag = FALSE;
	  break;
	}
      }
      k++;
      i += j;
    } else {
      break;
    }
  }

                                                              /* limit check */
  if (mark_sx > mark_dx) {
    x1 = mark_dx;
    x2 = mark_sx;
  } else {
    x1 = mark_sx;
    x2 = mark_dx;
  }
  if (mark_sy > mark_dy) {
    y1 = mark_dy;
    y2 = mark_sy;
  } else {
    y1 = mark_sy;
    y2 = mark_dy;
  }
  if ((x1 == x2)&&(y1 == y2)) {
    x1 = y1 = 0;
    x2 = M_MX-1;
    y2 = M_MY-1;
  }

                                                          /* string matching */
  if (!strcmp(buf[0], "map-load")||
      !strcmp(buf[0], "mld")) {
    F_Map_Load(buf[1], mark_sx, mark_sy);
    Cell_Count();
    Sub2Redraw(TRUE, cell_mode);
  } else if (!strcmp(buf[0], "map-save")||
	     !strcmp(buf[0], "msv")) {
    F_Map_Save(buf[1], x1, y1, x2, y2);
  } else if (!strcmp(buf[0], "map-box")||
	     !strcmp(buf[0], "mbox")) {
    F_Map_Box(x1, y1, x2, y2, current);
  } else if (!strcmp(buf[0], "map-clear")||
	     !strcmp(buf[0], "mcls")) {
    F_Map_Fill(x1, y1, x2, y2, 0);
  } else if (!strcmp(buf[0], "map-fill")||
	     !strcmp(buf[0], "mfill")) {
    F_Map_Fill(x1, y1, x2, y2, current);
  } else if (!strcmp(buf[0], "map-yank")||
	     !strcmp(buf[0], "mget")) {
    F_Map_Yank(x1, y1, x2, y2);
  } else if (!strcmp(buf[0], "map-paste")||
	     !strcmp(buf[0], "mput")) {
    F_Map_Paste(mark_sx, mark_sy);
  } else if (!strcmp(buf[0], "map-replace")||
	     !strcmp(buf[0], "mrep")) {
    F_Map_Replace(x1, y1, x2, y2, current, mark_cell);
  } else if (!strcmp(buf[0], "map-flip")||
	     !strcmp(buf[0], "mflip")) {
    F_Map_Flip(x1, y1, x2, y2, buf[1]);
  } else if (!strcmp(buf[0], "cell-swap")||
	     !strcmp(buf[0], "cswap")) {
    F_Cell_Swap(mark_cell, current);
  } else if (!strcmp(buf[0], "zoom")||
	     !strcmp(buf[0], "zm")) {
    F_Zoom(atoi(buf[1]));
  } else if (!strcmp(buf[0], "scroll")||
	     !strcmp(buf[0], "scr")) {
    F_Scroll(atoi(buf[1]));
  } else if (!strcmp(buf[0], "part")||
	     !strcmp(buf[0], "pt")) {
    F_Partition(atoi(buf[1]));
  } else if (!strcmp(buf[0], "jump")||
	     !strcmp(buf[0], "jp")) {
    F_Jump(atoi(buf[1]), atoi(buf[2]));
  } else if (!strcmp(buf[0], "color-file")||
	     !strcmp(buf[0], "col")) {
    strcpy(file_buf[1], buf[1]);
    MesInit();
    sprintf(mes_buf[0], "Set color-file");
    sprintf(mes_buf[1], "[%s]", buf[1]);
  } else if (!strcmp(buf[0], "window-main")||
	     !strcmp(buf[0], "w0")) {
    w0_x = atoi(buf[1]);
    w0_y = atoi(buf[2]);
    XMoveWindow(dpy, w0, w0_x, w0_y);
    Redraw(pix0);
    Sub1Redraw(info_mode);
    Sub2Redraw(FALSE, cell_mode);
    MesInit();
  } else if (!strcmp(buf[0], "window-info")||
	     !strcmp(buf[0], "w1")) {
    w1_x = atoi(buf[1]);
    w1_y = atoi(buf[2]);
    XMoveWindow(dpy, w1, w1_x, w1_y);
    Redraw(pix0);
    Sub1Redraw(info_mode);
    Sub2Redraw(FALSE, cell_mode);
    MesInit();
  } else if (!strcmp(buf[0], "window-cell")||
	     !strcmp(buf[0], "w2")) {
    w2_x = atoi(buf[1]);
    w2_y = atoi(buf[2]);
    XMoveWindow(dpy, w2, w2_x, w2_y);
    Redraw(pix0);
    Sub1Redraw(info_mode);
    Sub2Redraw(FALSE, cell_mode);
    MesInit();
  } else if (!strcmp(buf[0], "help")||
	     !strcmp(buf[0], "h")) {
    fprintf(stderr, "Command                            "
	    " Execute\n");
    fprintf(stderr, "map-load   [mld]   filename.map  ->"
	    " map load to mark_s\n");
    fprintf(stderr, "map-save   [msv]   filename.map  ->"
	    " map save in mark_s-mark_d\n");
    fprintf(stderr, "map-box    [mbox]                ->"
	    " draw box in mark_s-mark_d\n");
    fprintf(stderr, "map-clear  [mcls]                ->"
	    " clear in mark_s-mark_d\n");
    fprintf(stderr, "map-fill   [mfill]               ->"
	    " fill current in mark_s-mark_d\n");
    fprintf(stderr, "map-yank   [mget]                ->"
	    " yank to work-area in mark_s-mark_d\n");
    fprintf(stderr, "map-paste  [mput]                ->"
	    " paste from work-area to mark_s\n");
    fprintf(stderr, "map-replace[mrep]                ->"
	    " replace current->mark_cell in mark_s-mark_d\n");
    fprintf(stderr, "map-flip   [mflip] 'h'or'v'or'hv'->"
	    " flip in mark_s-mark_d\n");
    fprintf(stderr, "cell-swap  [cswap]               ->"
	    " swap cell data mark_cell<->current\n");
    fprintf(stderr, "zoom       [zm]    integer       ->"
	    " zoom\n");
    fprintf(stderr, "scroll     [scr]   integer       ->"
	    " scroll\n");
    fprintf(stderr, "part       [pt]    integer       ->"
	    " part\n");
    fprintf(stderr, "jump       [jp]    jump_x jump_y ->"
	    " jump to (jump_x, jump_y)\n");
    fprintf(stderr, "color-file [col]   filename.col  ->"
	    " define color-file to save to env-file\n");
    fprintf(stderr, "window-main[w0]    pos_x pos_y   ->"
	    " move main-window to (pos_x, pos_y)\n");
    fprintf(stderr, "window-info[w1]    pos_x pos_y   ->"
	    " move info-window to (pos_x, pos_y)\n");
    fprintf(stderr, "window-cell[w2]    pos_x pos_y   ->"
	    " move cell-window to (pos_x, pos_y)\n");
    fprintf(stderr, "help       [h]                   ->"
	    " command list\n");
    MesInit();
  } else {
    MesInit();
    sprintf(mes_buf[0], "Illigal command.");
    sprintf(mes_buf[1], "[%s]", buf[0]);
  }

} /* end of Command_Check */


void    SW_Draw(int menu_ID)
{
  int  i;

                                                          /* set font for SW */
  XSetFont(dpy, gc_w, font[0]);

                                                      /* draw segment for SW */
  for (i = 0; i < 7; i++) {
    XCopyArea(dpy, sw_pix, pix0, gc_b, 0, 0, SW_X, SW_Y, 0, i*SW_Y);
    XDrawString(dpy, pix0, gc_w,
		(SW_X-BOX_LEN*FS_0X)/2, i*SW_Y+(SW_Y+FS_0Y)/2,
		box[i+menu_ID*8+2], BOX_LEN);
  }

                                                        /* fill for SW[menu] */
  XFillRectangle(dpy, pix0, gc, 0, SW_Y*6, SW_X, SW_Y);

} /* end of SW_Draw */


void    Scale_Draw()
{
  char buf[4];
  int  i;

                                                       /* set font for scale */
  XSetFont(dpy, gc_w, font[1]);

                                                          /* clear for scale */
  XFillRectangle(dpy, pix0, gc_b,
		 BLOCK*8, 0,
		 BLOCK*(4 + 4 + 1) + CELL*bwidth, BLOCK*(2 + 2 + 1) + CELL*bheight);

                                                      /* copy from pix1(map) */
  XCopyArea(dpy, pix1, pix0, gc_b,
	    0, 0, CELL*bwidth, CELL*bheight, MAP_X, MAP_Y);

                                                      /* draw line for scale */
  XDrawLine(dpy, pix0, gc_w,
	    MAP_X,               BLOCK,
	    MAP_X + CELL*bwidth, BLOCK);
  XDrawLine(dpy, pix0, gc_w,
	    MAP_X,               MAP_Y + BLOCK + CELL*bheight,
	    MAP_X + CELL*bwidth, MAP_Y + BLOCK + CELL*bheight);
  XDrawLine(dpy, pix0, gc_w,
	    BLOCK*(8 + 2),       MAP_Y,
	    BLOCK*(8 + 2),       MAP_Y + CELL*bheight);
  XDrawLine(dpy, pix0, gc_w,
	    MAP_X + BLOCK*2 + CELL*bwidth, MAP_Y,
	    MAP_X + BLOCK*2 + CELL*bwidth, MAP_Y + CELL*bheight);

                                              /* draw string for scale width */
  for (i = 0; i <= bwidth; i += 8*zs[zoom]) {
    sprintf(buf, "%03d", bx + i/zs[zoom]);
    XDrawString(dpy, pix0, gc_w,
		MAP_X + CELL*i - 3*FS_1X/2, BLOCK + FS_1X/2,
		buf, strlen(buf));
    XDrawString(dpy, pix0, gc_w,
		MAP_X + CELL*i - 3*FS_1X/2, MAP_Y + BLOCK + CELL*bheight + FS_1X/2,
		buf, strlen(buf));
  }
  for (i = 0; i <= bheight; i += 4*zs[zoom]) {
    sprintf(buf, "%03d", by + i/zs[zoom]);
    XDrawString(dpy, pix0, gc_w,
		SW_X + BLOCK*2 - 3*FS_1X/2, MAP_Y + CELL*i + FS_1X/2,
		buf, strlen(buf));
    XDrawString(dpy, pix0, gc_w,
		MAP_X + BLOCK*2 + CELL*bwidth - 3*FS_1X/2, MAP_Y + CELL*i + FS_1X/2,
		buf, strlen(buf));
  }

} /* end of Scale_Draw */


void    Map_Draw(int mode)
{
  char check[MAX_P_BWIDTH][MAX_P_BHEIGHT];
  int  i, j, i2, j2, k;

                                                         /* map_draw anyway? */
  if (mode) {
                                                         /* clear check_flag */
    for (j = 0; j < bheight; j += zs[zoom]) {
      for (i = 0; i < bwidth; i += zs[zoom]) {
	check[i][j] = FALSE;
      }
    }
                                          /* draw map loop to pix2(map work) */
    for (j = 0; j < bheight; j += zs[zoom]) {
      for (i = 0; i < bwidth; i += zs[zoom]) {
	if (!check[i][j]) {
	  k = (u_char)map_data[bx + i/zs[zoom]][by + j/zs[zoom]];
	  Cell_Copy(w_cell, k, zs[zoom]);
	  for (j2 = j; j2 < bheight; j2 += zs[zoom]) {
	    for (i2 = 0; i2 < bwidth; i2 += zs[zoom]) {
	      if ((u_char)map_data[bx + i2/zs[zoom]][by + j2/zs[zoom]]
		  == k) {
		Cell_Draw(w_cell, pix2, i2/zs[zoom], j2/zs[zoom], zs[zoom]);
		check[i2][j2] = TRUE;
	      }
	    }
	  }
	}
      }
    }
  }

                                            /* draw map to pix1(editor work) */
  XCopyArea(dpy, pix2, pix1, gc_b,
	    0, 0, CELL*bwidth, CELL*bheight, 0, 0);

                                                   /* draw part to pix1 */
  switch (part) {
  case 0:
    {
      XPoint pts[56*2*2];
      for (j = 0; j < bheight; j += zs[zoom]) {
	k = 0;
	for (i = 0; i < bwidth; i += zs[zoom]) {
	  pts[k  ].x = CELL*i;
	  pts[k++].y = CELL*j;
	  pts[k  ].x = CELL*(i+zs[zoom]) - 1;
	  pts[k++].y = CELL*(j+zs[zoom]) - 1;
	}
	XDrawPoints(dpy, pix1, gc_w, pts, k, CoordModeOrigin);
      }
    }
    break;
  case 1:
    {
      XSegment seg[56*2*4];
      k = 0;
      for (i = 0; i < bheight; i += zs[zoom]) {
	seg[k  ].x1 = 0;
	seg[k  ].y1 = CELL*i*2;
	seg[k  ].x2 = CELL*bwidth;
	seg[k++].y2 = CELL*i*2;
	seg[k  ].x1 = 0;
	seg[k  ].y1 = CELL*(i+zs[zoom])*2 - 1;
	seg[k  ].x2 = CELL*bwidth;
	seg[k++].y2 = CELL*(i+zs[zoom])*2 - 1;
      }
      for (i = 0; i < bwidth; i += zs[zoom]) {
	seg[k  ].x1 = CELL*i*2;
	seg[k  ].y1 = 0;
	seg[k  ].x2 = CELL*i*2;
	seg[k++].y2 = CELL*bheight;
	seg[k  ].x1 = CELL*(i+zs[zoom])*2 - 1;
	seg[k  ].y1 = 0;
	seg[k  ].x2 = CELL*(i+zs[zoom])*2 - 1;
	seg[k++].y2 = CELL*bheight;
      }
      XDrawSegments(dpy, pix1, gc_w, seg, k);
    }
    break;
  case 2:
    {
      XSegment seg[56*2*4];
      k = 0;
      for (i = 0; i < bheight; i += 2*zs[zoom]) {
	seg[k  ].x1 = 0;
	seg[k  ].y1 = CELL*i*2;
	seg[k  ].x2 = CELL*bwidth;
	seg[k++].y2 = CELL*i*2;
	seg[k  ].x1 = 0;
	seg[k  ].y1 = CELL*(i + 2*zs[zoom])*2 - 1;
	seg[k  ].x2 = CELL*bwidth;
	seg[k++].y2 = CELL*(i + 2*zs[zoom])*2 - 1;
      }
      for (i = 0; i < bwidth; i += 2*zs[zoom]) {
	seg[k  ].x1 = CELL*i*2;
	seg[k  ].y1 = 0;
	seg[k  ].x2 = CELL*i*2;
	seg[k++].y2 = CELL*bheight;
	seg[k  ].x1 = CELL*(i + 2*zs[zoom])*2 - 1;
	seg[k  ].y1 = 0;
	seg[k  ].x2 = CELL*(i + 2*zs[zoom])*2 - 1;
	seg[k++].y2 = CELL*bheight;
      }
      XDrawSegments(dpy, pix1, gc_w, seg, k);
    }
    break;
  case 3:
    {
      char buf[4];
      XSetFont(dpy, gc_w, font[1]);
      switch (zoom) {
      case 0:
      case 1:
	k = 10;
	break;
      case 2:
      case 3:
      default:
	k = 1000;
	break;
      }
      for (j = 0; j < bheight; j += zs[zoom]) {
	for (i = 0; i < bwidth; i += zs[zoom]) {
	  sprintf(buf, "%d",
		  (u_char)map_data[bx + i/zs[zoom]][by + j/zs[zoom]] % k);
	  XDrawString(dpy, pix1, gc_w,
		      CELL*i, CELL*j + FS_0Y/2,
		      buf, strlen(buf));
	}
      }
    }
    break;
  case 4:
    break;
  }

                                             /* draw map to pix0(for output) */
  XCopyArea(dpy, pix1, pix0, gc_b,
	    0, 0, CELL*bwidth, CELL*bheight, MAP_X, MAP_Y);

} /* end of Map_Draw */


int     Map_Scroll(int scr_ID, int scr_w, int scr_h)
{
  int  retflag;

  retflag = FALSE;
  MesInit();

  switch (scr_ID % 4) {
  case 0:
                                                           /* scroll to left */
    if (bx == 0) {
      sprintf(mes_buf[0], "Width limit.");
      break;
    }
    if ((bx > 0)&&(scr_w == 0)) {
      break;
    }
    retflag = TRUE;
    bx -= scr_w;
    if (bx < 0) {
      bx = 0;
      sprintf(mes_buf[0], "Width limit.");
    }
    break;
  case 3:
                                                          /* scroll to right */
    if (bx == M_MX - bwidth/zs[zoom]) {
      sprintf(mes_buf[0], "Width limit.");
      break;
    }
    if ((bx < M_MX - bwidth/zs[zoom])&&(scr_w == 0)) {
      break;
    }
    retflag = TRUE;
    bx += scr_w;
    if (bx + bwidth/zs[zoom] > M_MX) {
      bx = M_MX - bwidth/zs[zoom];
      sprintf(mes_buf[0], "Width limit.");
    }
    break;
  }

  switch (scr_ID / 4) {
  case 0:
                                                             /* scroll to up */
    if (by == 0) {
      sprintf(mes_buf[1], "Height limit.");
      break;
    }
    if ((by > 0)&&(scr_h == 0)) {
      break;
    }
    retflag = TRUE;
    by -= scr_h;
    if (by < 0) {
      by = 0;
      sprintf(mes_buf[1], "Height limit.");
    }
    break;
  case 3:
                                                           /* scroll to down */
    if (by == M_MY - bheight/zs[zoom]) {
      sprintf(mes_buf[1], "Height limit.");
      break;
    }
    if ((by < M_MY - bheight/zs[zoom])&&(scr_h == 0)) {
      break;
    }
    retflag = TRUE;
    by += scr_h;
    if (by + bheight/zs[zoom] > M_MY) {
      by = M_MY - bheight/zs[zoom];
      sprintf(mes_buf[1], "Height limit.");
    }
    retflag = TRUE;
    break;
  }

  return retflag;

} /* Map_Scroll */


void    Cell_Edit_Draw()
{
  char buf[INFO_LEN];

  XFillRectangle(dpy, cell_pix3, gc_b,
		 BLOCK, CELL, BLOCK*14, CELL*5);
  XFillRectangle(dpy, cell_pix3, gc_b,
		 BLOCK, BLOCK*13, BLOCK*14, CELL*5);
  XSetFont(dpy, gc_w, font[0]);
  sprintf(buf, "edit cell : %3d", edit_cell);
  XDrawString(dpy, cell_pix3, gc_w,
	      BLOCK, CELL + (CELL*5 + FS_0Y)/2,
	      buf, strlen(buf));
  sprintf(buf, "color : %2d", edit_color);
  XDrawString(dpy, cell_pix3, gc_w,
	      BLOCK, BLOCK*13 + (CELL*5 + FS_0Y)/2,
	      buf, strlen(buf));
  XFillRectangle(dpy, cell_pix3, gcc[edit_color],
		 BLOCK*10, BLOCK*13 + CELL, CELL*3, CELL*3);

} /* end of Cell_Edit_Draw */


void    Cell_Copy(Pixmap px, int cell_ID, int zoomsize)
{
  int  i, j, k;

  for (j = 0; j < CELL; j++) {
    for (i = 0; i < CELL; i++) {
      k = cell_data[cell_ID][i][j];
      XFillRectangle(dpy, px, gcc[k],
		     i*zoomsize, j*zoomsize, zoomsize, zoomsize);
    }
  }

} /* end of Cell_Copy */


void    Cell_Draw(Pixmap spx, Pixmap dpx, int x, int y, int zoomsize)
{
  XCopyArea(dpy, spx, dpx, gc_b, 0, 0,
	    CELL*zoomsize, CELL*zoomsize,
	    CELL*x*zoomsize, CELL*y*zoomsize);

} /* end of Cell_Draw */


void    Sub1Redraw(int pxno)
{
  char buf0[INFO_LEN], buf1[INFO_LEN];
  int  i;

                                                          /* set font for w1 */
  XSetFont(dpy, gc_w, font[0]);
                                                                /* info_mode */
  switch (pxno) {
  case 0:
                                           /* info_mode=0 -> output messages */
    XFillRectangle(dpy, info_pix0, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT);
    sprintf(buf1, "cell :");
    if (x_index >= 0) {
      sprintf(buf0, "point=( %3d", bx + x_index);
    } else {
      sprintf(buf0, "point=( xxx");
    }
    if (y_index >= 0) {
      sprintf(buf0, "%s %3d)", buf0, by + y_index);
    } else {
      sprintf(buf0, "%s xxx)", buf0);
    }
    if ((x_index >= 0)&&(y_index >= 0)) {
      sprintf(buf1, "cell : no.%3d",
	      (u_char)map_data[bx + x_index][by + y_index]);
    } else {
      sprintf(buf1, "cell :");
    }

    XDrawString(dpy, info_pix0, gc_w,
		FS_0X, FS_0Y,
		buf0, strlen(buf0));
    XDrawString(dpy, info_pix0, gc_w,
		FS_0X, 2*FS_0Y,
		buf1, strlen(buf1));
    for (i = 0; i < MES_MAX; i++) {
      XDrawString(dpy, info_pix0, gc_w,
		  FS_0X, (i + 3)*FS_0Y,
		  mes_buf[i], strlen(mes_buf[i]));
    }
    XCopyArea(dpy, info_pix0, w1, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT, 0, 0);
    break;
  case 1:
                                                /* info_mode=1 -> o.k. click */
    XFillRectangle(dpy, info_pix0, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT);
    for (i = 0; i < 4; i++) {
      XDrawRectangle(dpy, info_pix0, gc_w,
		     i*2, BLOCK*5 + i*2, W1_WIDTH - i*4 - 1, BLOCK*3 - i*4 - 1);
    }
    for (i = 0; i < MES_MAX; i++) {
      XDrawString(dpy, info_pix0, gc_w,
		  FS_0X, (i + 1)*FS_0Y,
		  mes_buf[i], strlen(mes_buf[i]));
    }
    XDrawString(dpy, info_pix0, gc_w,
		(W1_WIDTH - BOX_LEN*FS_0X)/2, BLOCK*5 + (BLOCK*3 + FS_0Y)/2,
		box[26], BOX_LEN);
    XCopyArea(dpy, info_pix0, w1, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT, 0, 0);
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
                                /* info_mode=2 -> input string from keyboard */
    XFillRectangle(dpy, info_pix0, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT);
    XDrawRectangle(dpy, info_pix0, gc_w,
		   0, BLOCK*5, W1_WIDTH - 1, BLOCK*3 - 1);
    XDrawString(dpy, info_pix0, gc_w,
		FS_0X, BLOCK*5 + (BLOCK*3 + FS_0Y)/2,
		file_buf[pxno], strlen(file_buf[pxno]));
    for (i = 0; i < MES_MAX; i++) {
      XDrawString(dpy, info_pix0, gc_w,
		  FS_0X, (i + 1)*FS_0Y,
		  mes_buf[i], strlen(mes_buf[i]));
    }
    XFillRectangle(dpy, info_pix0, gc_w,
		   (strlen(file_buf[pxno]) + 1)*FS_0X, BLOCK*5 + BLOCK,
		   FS_0X, FS_0Y);
    XCopyArea(dpy, info_pix0, w1, gc_b, 0, 0, W1_WIDTH, W1_HEIGHT, 0, 0);
    break;
  }

} /* end of Sub1Redraw */


void    Sub2Redraw(int mode, int no)
{
  char buf[INFO_LEN];
  int  i, dummy;

                                                          /* set font for w2 */
  XSetFont(dpy, gc_w, font[1]);
                                                        /* w2_redraw anyway? */
  if (mode) {
                                                       /* clear cell section */
    XFillRectangle(dpy, cell_pix1, gc_b, 0, 0, W2_P_WIDTH, W2_P_HEIGHT);
    XFillRectangle(dpy, cell_pix2, gc_b, 0, 0, W2_P_WIDTH, W2_P_HEIGHT);

                                                     /* draw cell_infomation */
    for (i = 0; i < CELL_MAX/2; i++) {
      sprintf(buf, "%3d", i);
      XDrawString(dpy, cell_pix1, gc_w,
		  FS_1X, BLOCK*2*i + (BLOCK*2 + FS_1Y)/2,
		  buf, strlen(buf));
      Cell_Copy(w_cell, i, 4);
      XCopyArea(dpy, w_cell, cell_pix1, gc_w, 0, 0, CELL*4, CELL*4,
		BLOCK*3, CELL*4*i);
      sprintf(buf, "%5d %s", count[i], attribute[i]);
      XDrawString(dpy, cell_pix1, gc_w,
		  BLOCK*6, BLOCK*2*i + (BLOCK*2 + FS_1Y)/2,
		  buf, strlen(buf));
    }
    for (i = CELL_MAX/2; i < CELL_MAX; i++) {
      sprintf(buf, "%3d", i);
      XDrawString(dpy, cell_pix2, gc_w,
		  FS_1X, BLOCK*2*(i - 128)+(BLOCK*2 + FS_1Y)/2,
		  buf, strlen(buf));
      Cell_Copy(w_cell, i, 4);
      XCopyArea(dpy, w_cell, cell_pix2, gc_w, 0, 0, CELL*4, CELL*4,
		BLOCK*3, CELL*4*(i - 128));
      sprintf(buf, "%5d %s", count[i], attribute[i]);
      XDrawString(dpy, cell_pix2, gc_w,
		  BLOCK*6, BLOCK*2*(i - 128) + (BLOCK*2 + FS_1Y)/2,
		  buf, strlen(buf));
    }
  }

                                                  /* current cell infomation */
  XFillRectangle(dpy, cell_pix0, gc_b, 0, 0, W2_M_WIDTH, W2_M_HEIGHT);
  sprintf(buf, "cell : no.%3d", current);
  XDrawString(dpy, cell_pix0, gc_w,
	      BLOCK*4 + FS_1X, FS_1Y,
	      buf, strlen(buf));
  sprintf(buf, "attr : %s", attribute[current]);
  XDrawString(dpy, cell_pix0, gc_w,
	      FS_1X, 5*FS_1Y,
	      buf, strlen(buf));
  Cell_Copy(w_cell, current, 4);
  XCopyArea(dpy, w_cell, cell_pix0, gc_w, 0, 0, CELL*4, CELL*4,
	    CELL, CELL);
  XDrawLine(dpy, cell_pix0, gc_w,
	    0, W2_M_HEIGHT - 1, W2_M_WIDTH - 1, W2_M_HEIGHT - 1);

                                                       /* draw w2 for output */
  XCopyArea(dpy, cell_pix0, w2, gc_b, 0, 0, W2_M_WIDTH, W2_M_HEIGHT, 0, 0);
  if (no < CELL_MAX/2) {
    XCopyArea(dpy, cell_pix1, w2, gc_b, 0, BLOCK*2*no,
	      W2_P_WIDTH, W2_P_HEIGHT, 0, W2_M_HEIGHT);
  } else {
    XCopyArea(dpy, cell_pix2, w2, gc_b, 0, BLOCK*2*(no-128),
	      W2_P_WIDTH, W2_P_HEIGHT, 0, W2_M_HEIGHT);
  }
  XCopyArea(dpy, cell_pix3, w2, gc_b, 0, 0,
	    W2_P_WIDTH, W2_HEIGHT, W2_WIDTH, 0);

} /* end of Sub2Redraw */


void    MesInit()
{
  int  i;

  for (i = 0; i < MES_MAX; i++) {
    mes_buf[i][0] = '\0';
  }

} /* end of MesInit */


void    Cell_Count()
{
  int  i, j;

  for (i = 0; i < CELL_MAX; i++) {
    count[i] = 0;
  }

  for (j = 0; j < M_MY; j++) {
    for (i = 0; i < M_MX; i++) {
      count[(u_char)map_data[i][j]]++;
    }
  }

} /* end of Cell_Count */


void    Buf_Clear(int f_no)
{
  file_buf[f_no][f_c[f_no] = 0] = '\0';

} /* end of Buf_Clear */


void    Buf_Erase(int f_no)
{
  if (f_c[f_no] > 0) {
    file_buf[f_no][--f_c[f_no]] = '\0';
  }

} /* end of Buf_Erase */


void    Buf_Key(int f_no, char ch)
{
  if (f_c[f_no] < FILE_LEN-1) {
    file_buf[f_no][f_c[f_no]++] = ch;
    file_buf[f_no][f_c[f_no]  ] = '\0';
  }

} /* end of Buf_Key */


void    Buf_Enter(int f_no)
{
  char buf[INFO_LEN];
  int  i, j;

  file_buf[f_no][f_c[f_no]] = file_buf[f_no][INFO_LEN - 1] = '\0';
  buf[0] = '\0';

  i = -1;
  while (++i < INFO_LEN) {
    if ((buf[i] = file_buf[f_no][i]) == '.') {
      if ((file_buf[f_no][i + 1] != '.')&&(file_buf[f_no][i + 1] != '/')) {
	buf[i] = '\0';
	break;
      }
    }
  }
  buf[INFO_LEN - 1] = '\0';

                                                              /* auto suffix */
  switch (f_no) {
  case 1:
    sprintf(file_buf[f_no], "%s.col", buf);
    break;
  case 2:
  case 3:
    sprintf(file_buf[f_no], "%s.map", buf);
    break;
  case 4:
  case 5:
    sprintf(file_buf[f_no], "%s.cel", buf);
    break;
  }
  file_buf[f_no][INFO_LEN - 1] = '\0';
  f_c[f_no] = strlen(file_buf[f_no]);

} /* end of Buf_Enter */


void    F_Menu()
{
  menu_mode = (menu_mode + 1) % MENU_MAX;
  SW_Draw(menu_mode);

  Redraw(pix0);

} /* end of F_Menu */


void    F_Map_Load(char *filename, int s_x, int s_y)
{
  FILE *fp;
  int  w, h, i, j;
  char data;

  MesInit();
  fp = fopen(filename, "r");
  if (fp != NULL) {
    fscanf(fp, "%d %d", &w, &h);
    while (fgetc(fp) != '\n')
      ;
    for (j = s_y; j < s_y + h; j++) {
      for (i = s_x; i < s_x + w; i++) {
	data = fgetc(fp);
	if ((i < M_MX)&&(j < M_MY)) {
	  map_data[i][j] = (u_char)data;
	}
      }
    }
    fclose(fp);
    Map_Draw(TRUE);

    sprintf(mes_buf[0], "Load map-file from");
    sprintf(mes_buf[1], "[%s].", filename);
  } else {
    sprintf(mes_buf[0], "Can't open map-file");
    sprintf(mes_buf[1], "[%s].", filename);
  }

} /* end of F_Map_Load */


void    F_Map_Save(char *filename, int s_x, int s_y, int d_x, int d_y)
{
  FILE *fp;
  int  x, y, w, h, i, j;

  MesInit();
  fp = fopen(filename, "w");
  if (fp != NULL) {
    fprintf(fp, "%d %d\n", d_x - s_x + 1, d_y - s_y + 1);
    for (j = s_y; j <= d_y; j++) {
      for (i = s_x; i <= d_x; i++) {
	fprintf(fp, "%c", (u_char)map_data[i][j]);
      }
    }
    fprintf(fp, "\n");
    fclose(fp);

    sprintf(mes_buf[0], "Save map-file to");
    sprintf(mes_buf[1], "[%s].", filename);
  } else {
    sprintf(mes_buf[0], "Can't open map-file");
    sprintf(mes_buf[1], "[%s].", filename);
  }

  Sub1Redraw(info_mode);

} /* end of F_Map_Save */


void    F_Cell_Load(char *filename)
{
  FILE *fp;
  char buf[INFO_LEN];
  int  i, j, k, data;

  MesInit();
  fp = fopen(filename, "r");
  if (fp != NULL) {
    for (i = 0; i < CELL_MAX; i++) {
      for (k = 0; k < CELL; k++) {
	for (j = 0; j < CELL; j++) {
	  data = fgetc(fp);
	  cell_data[i][j][k] = data % COLOR_MAX;
	}
      }
    }
    for (i = 0; i < CELL_MAX; i++) {
      attribute[i][0] = '\0';
    }
    fclose(fp);
    Map_Draw(TRUE);

    sprintf(mes_buf[0], "Load cell-file from");
    sprintf(mes_buf[1], "[%s].", filename);

    sprintf(buf, "%s^", filename);
    fp = fopen(buf, "r");
    if (fp != NULL) {
      while (TRUE) {
	fscanf(fp, "%s", buf);
	if (!strcmp(buf, "ATTR")) {
	  fscanf(fp, "%d", &k);
	  fgetc(fp);
	  for (i = 0; i < INFO_LEN; i++) {
	    if ((attribute[k][i] = fgetc(fp)) == '\n') {
	      attribute[k][i] = '\0';
	      break;
	    }
	  }
	} else if (buf[0] == '#') {
	  while (fgetc(fp) != '\n')
	    ;
	} else if (!strcmp(buf, "END")) {
	  break;
	}
      }
      fclose(fp);
    }
    Cell_Copy(c_cell, current = 0, zs[zoom]);
    Sub2Redraw(TRUE, cell_mode);
  } else {
    sprintf(mes_buf[0], "Can't open cell-file");
    sprintf(mes_buf[1], "[%s].", filename);
  }

} /* end of F_Cell_Load */


void    F_Cell_Save(char *filename)
{
  FILE *fp;
  char buf[INFO_LEN];
  int  i, j, k;

  MesInit();
  fp = fopen(filename, "w");
  if (fp != NULL) {
    for (i = 0; i < CELL_MAX; i++) {
      for (k = 0; k < CELL; k++) {
	for (j = 0; j < CELL; j++) {
	  fprintf(fp, "%c", cell_data[i][j][k]);
	}
      }
    }
    fprintf(fp, "\n");
    fclose(fp);

    sprintf(mes_buf[0], "Save cell-file to");
    sprintf(mes_buf[1], "[%s].", filename);

    i = -1;
    while (++i < CELL_MAX) {
      if (attribute[i][0] != 0) {
	break;
      }
    }
    if (i < CELL_MAX) {
      sprintf(buf, "%s^", filename);
      fp = fopen(buf, "w");
      if (fp != NULL) {
	fprintf(fp, "#\n");
	fprintf(fp, "# cell-attribute-file for Map_Editor (me)\n");
	fprintf(fp, "#\n");
	fprintf(fp, "\n");
	for (; i < CELL_MAX; i++) {
	  if (attribute[i][0] != '\0') {
	    fprintf(fp, "ATTR\t%3d\t%s\n", i, attribute[i]);
	  }
	}
	fprintf(fp, "END\n");
	fprintf(fp, "\n");
	fclose(fp);
      }
    }
  } else {
    sprintf(mes_buf[0], "Can't open cell-file");
    sprintf(mes_buf[1], "[%s].", filename);
  }

  Redraw(pix0);
  Sub1Redraw(info_mode);

} /* end of F_Cell_Save */


void    F_Cell_Attr(char *str)
{
  strcpy(attribute[current], str);
  MesInit();

  Sub1Redraw(info_mode);
  Sub2Redraw(TRUE, cell_mode);

} /* end of F_Cell_Attr */


void    F_Zoom(int zm)
{
  if (zm < 0) {
    zoom = (zoom + 1) % ZOOM_MAX;
  } else {
    zoom = zm % ZOOM_MAX;
  }
  bwidth  = (width /BLOCK - 8 - 4 - 4)*2;
  bheight = (height/BLOCK     - 2 - 2)*2;
  if (zoom == 0) {
    Map_Scroll(15, 0, 0);
  }
  Scale_Draw();
  Map_Draw(TRUE);
  Cell_Copy(c_cell, current, zs[zoom]);

  MesInit();
  sprintf(mes_buf[0], "Zoom : x%d", zs[zoom]);

  Redraw(pix0);
  Sub1Redraw(info_mode);

} /* end of F_Zoom */


void    F_Scroll(int scr)
{
  if (scr < 0) {
    scroll = (scroll % SCROLL_MAX) + 4;
  } else {
    scroll = scr % SCROLL_MAX;
  }
  MesInit();
  sprintf(mes_buf[0], "Scroll : %d cell", scroll);

  Sub1Redraw(info_mode);

} /* end of F_Scroll */


void    F_Partition(int part)
{
  if (part < 0) {
    part = (part + 1) % PART_MAX;
  } else {
    part = part % PART_MAX;
  }
  Map_Draw(FALSE);

  MesInit();
  sprintf(mes_buf[0], "Partition : no.%d", part);

  Redraw(pix0);
  Sub1Redraw(info_mode);

} /* end of F_Partition */


void    F_Env_Save()
{
  FILE *fp;
  char buf[INFO_LEN];

  MesInit();

  sprintf(buf, "%s/.me_env", getenv("HOME"));
  fp = fopen(buf, "w");
  if (fp != NULL) {
    fprintf(fp, "#\n");
    fprintf(fp, "# env-file for Map_Editor (me)\n");
    fprintf(fp, "#\n");
    fprintf(fp, "\n");
    fprintf(fp, "ZOOM\t\t%d\n",    zoom);
    fprintf(fp, "SCROLL\t\t%d\n",  scroll);
    fprintf(fp, "PART\t%d\n", part);
    fprintf(fp, "\n");
    fprintf(fp, "COLOR_FILE\t%s\n", file_buf[1]);
    fprintf(fp, "\n");
    fprintf(fp, "SIZE\t\t%d %d\n",  width, height);
    fprintf(fp, "WINDOW_0\t%d %d\n",  w0_x, w0_y);
    fprintf(fp, "WINDOW_1\t%d %d\n",  w1_x, w1_y);
    fprintf(fp, "WINDOW_2\t%d %d\n",  w2_x, w2_y);
    fprintf(fp, "\n");
    fprintf(fp, "END\n");
    fclose(fp);

    sprintf(mes_buf[0], "Save env-file to");
    sprintf(mes_buf[1], "[$HOME/.me_env].");
  } else {
    sprintf(mes_buf[0], "Can't open env-file");
    sprintf(mes_buf[1], "[$HOME/.me_env].");
  }

  Sub1Redraw(info_mode);

} /* end of F_Env_Save */


void    F_Quit()
{
  Destroy();
  fprintf(stderr, "Map_Editor : end.\n\n");
  exit(0);

} /* end of F_Quit */


void    F_Map_Fill(int s_x, int s_y, int d_x, int d_y, int cell_ID)
{
  int  i, j;

  for (j = s_y; j <= d_y; j++) {
    for (i = s_x; i <= d_x; i++) {
      map_data[i][j] = (u_char)cell_ID;
    }
  }
  Map_Draw(TRUE);

  MesInit();
  sprintf(mes_buf[0], "Map-Fill in");
  sprintf(mes_buf[1], "(%3d %3d)-(%3d %3d)", s_x, s_y, d_x, d_y);
  sprintf(mes_buf[2], "Cell : no.%3d", cell_ID);

} /* end of F_Map_Fill */


void    F_Map_Box(int s_x, int s_y, int d_x, int d_y, int cell_ID)
{
  int  i, j;

  for (i = s_x; i <= d_x; i++) {
    map_data[i][s_y] = (u_char)cell_ID;
    map_data[i][d_y] = (u_char)cell_ID;
  }
  for (j = s_y; j <= d_y; j++) {
    map_data[s_x][j] = (u_char)cell_ID;
    map_data[d_x][j] = (u_char)cell_ID;
  }
  Map_Draw(TRUE);

  MesInit();
  sprintf(mes_buf[0], "Map-Box in");
  sprintf(mes_buf[1], "(%3d %3d)-(%3d %3d)", s_x, s_y, d_x, d_y);
  sprintf(mes_buf[2], "Cell : no.%3d", cell_ID);

} /* end of F_Map_Box */


void    F_Map_Yank(int s_x, int s_y, int d_x, int d_y)
{
  int  i, j;

  x_yank = d_x - s_x + 1;
  y_yank = d_y - s_y + 1;
  for (j = 0; j < y_yank; j++) {
    for (i = 0; i < x_yank; i++) {
      yank[i][j] = (u_char)map_data[i + s_x][j + s_y];
    }
  }

  MesInit();
  sprintf(mes_buf[0], "Map-Yank in");
  sprintf(mes_buf[1], "(%3d %3d)-(%3d %3d)", s_x, s_y, d_x, d_y);

} /* end of F_Map_Yank */


void    F_Map_Paste(int s_x, int s_y)
{
  int  i, j;

  for (j = 0; j < y_yank; j++) {
    for (i = 0; i < x_yank; i++) {
      if ((i < M_MX)&&(j < M_MY)) {
	map_data[i + s_x][j + s_y] = (u_char)yank[i][j];
      }
    }
  }
  Map_Draw(TRUE);

  MesInit();
  sprintf(mes_buf[0], "Map-Paste to");
  sprintf(mes_buf[1], "          (%3d %3d)", s_x, s_y);

} /* end of F_Map_Paste */


void    F_Map_Replace(int s_x, int s_y, int d_x, int d_y,
		      int cell_ID1, int cell_ID2)
{
  int  i, j;

  for (j = s_y; j <= d_y; j++) {
    for (i = s_x; i <= d_x; i++) {
      if ((u_char)map_data[i][j] == cell_ID1) {
	map_data[i][j] = (u_char)cell_ID2;
      }
    }
  }
  Map_Draw(TRUE);

  MesInit();
  sprintf(mes_buf[0], "Map-Replace");
  sprintf(mes_buf[1], "(%3d %3d)-(%3d %3d)", s_x, s_y, d_x, d_y);
  sprintf(mes_buf[2], "Cell : no.%3d->%3d", cell_ID1, cell_ID2);

} /* end of F_Map_Replace */


void    F_Map_Flip(int s_x, int s_y, int d_x, int d_y, char *str)
{
  char dummy;
  int  i, j, flag_v, flag_h;

  flag_v = flag_h = FALSE;
  i = -1;
  while (str[++i] != '\0') {
    if (str[i] == 'v') {
      flag_v = TRUE;
    } else if (str[i] == 'h') {
      flag_h = TRUE;
    }
  }

  if (flag_v) {
    for (j = 0; j < d_y - s_y - j; j++) {
      for (i = s_x; i <= d_x; i++) {
	dummy                = (u_char)map_data[i][s_y + j];
	map_data[i][s_y + j] = (u_char)map_data[i][d_y - j];
	map_data[i][d_y - j] = (u_char)dummy;
      }
    }
  }
  if (flag_h) {
    for (i = 0; i < d_x - s_x - i; i++) {
      for (j = s_y; j <= d_y; j++) {
	dummy                = (u_char)map_data[s_x + i][j];
	map_data[s_x + i][j] = (u_char)map_data[d_x - i][j];
	map_data[d_x - i][j] = (u_char)dummy;
      }
    }
  }
  MesInit();
  if (flag_v||flag_h) {
    Map_Draw(TRUE);

    sprintf(mes_buf[0], "Map-Flip");
    if (flag_v) {
      sprintf(mes_buf[0], "%s -v", mes_buf[0]);
    }
    if (flag_h) {
      sprintf(mes_buf[0], "%s -h", mes_buf[0]);
    }
    sprintf(mes_buf[1], "(%3d %3d)-(%3d %3d)", s_x, s_y, d_x, d_y);
  }

} /* end of F_Map_Flip */


void    F_Cell_Swap(int cell_ID1, int cell_ID2)
{
  char dummy, buf[INFO_LEN];
  int  i, j;

  for (j = 0; j < CELL; j++) {
    for (i = 0; i < CELL; i++) {
      dummy                     = cell_data[cell_ID1][i][j];
      cell_data[cell_ID1][i][j] = cell_data[cell_ID2][i][j];
      cell_data[cell_ID2][i][j] = dummy;
    }
  }
  strcpy(buf,                 attribute[cell_ID1]);
  strcpy(attribute[cell_ID1], attribute[cell_ID2]);
  strcpy(attribute[cell_ID2], buf);
  Map_Draw(TRUE);
  Cell_Count();
  Sub2Redraw(TRUE, cell_mode);

  MesInit();
  sprintf(mes_buf[0], "Cell-Swap");
  sprintf(mes_buf[1], "Cell : no.%3d<->%3d", cell_ID1, cell_ID2);

} /* end of F_Cell_Swap */


void    F_Jump(int x, int y)
{
  bx = x;
  by = y;
  Map_Scroll(0, 0, 0);
  Map_Scroll(15, 0, 0);
  Scale_Draw();
  Map_Draw(TRUE);
  MesInit();
  sprintf(mes_buf[0], "Jump to");
  sprintf(mes_buf[1], "          (%3d %3d)", bx, by);

} /* end of F_Jump */


void    F_Cell_Edit(int cell_ID)
{
  int  i, j;

  MesInit();
  if (cell_ID >= 0) {
    XResizeWindow(dpy, w2, W2_WIDTH+W2_WIDTH, W2_HEIGHT);
    for (i = 0; i < CELL; i++) {
      for (j = 0; j < CELL; j++) {
	c_work[i][j] = cell_data[cell_ID][i][j];
	XFillRectangle(dpy, cell_pix3, gcc[(u_char)c_work[i][j]],
		       BLOCK*(4+i), BLOCK*(4 + j), BLOCK, BLOCK);
      }
    }
    Cell_Edit_Draw();
    Sub2Redraw(FALSE, cell_mode);

  } else {
    XResizeWindow(dpy, w2, W2_WIDTH, W2_HEIGHT);
    for (i = 0; i < CELL; i++) {
      for (j = 0; j < CELL; j++) {
	cell_data[edit_cell][i][j] = c_work[i][j];
      }
    }
    if (current == edit_cell) {
      Cell_Copy(c_cell, current, zs[zoom]);
    }
    Map_Draw(TRUE);
    Redraw(pix0);
    Sub2Redraw(TRUE, cell_mode);
    sprintf(mes_buf[0], "End cell-edit mode.");
  }

} /* end of F_Cell_Edit */
