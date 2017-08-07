/*
 $Header: CHOOSE.HV  1.3  95/04/25 23:32:14  Hong.Chen  Exp $"
 $Log: RCS/CHOOSE.HV $
 * Revision 1.3  95/04/25 23:32:14  Hong.Chen
 * for MSVC
 *
 * Revision 1.2  95/01/17 10:40:54  Hong.Chen
 * renew prototype define & add MK_FP
 *
 * Revision 1.1  94/11/25 11:39:08  Hong.Chen
 * Initial revision
 *
*/
#define		PC_98 			0
#define 	FMR_50			1
#define 	AT_US			2
#define 	AT_JP			3
#define 	AT_CN			4

/*
#define		DISP_SHADOW
*/

#ifndef MK_FP
#define	MK_FP(s, o)	((void far*)(((unsigned long)(s) << 16)|(unsigned)(o)))
#endif

#define 	KEYCLR			bdos(12,0,0)

#define		crt_beep()		putch(7)
#define 	crt_BLACK       0x0
#define 	crt_RED         0x1
#define 	crt_GREEN       0x2
#define 	crt_YELLOW      0x3
#define 	crt_BLUE        0x4
#define 	crt_MAGENTA     0x5
#define 	crt_CYAN        0x6
#define 	crt_WHITE       0x7

#define 	crt_SECRET		0x00	/* secret bit */
#define 	crt_NOSECRET	0x10	/* No secret bit */
#define 	crt_BLINK		0x20	/* blink bit */
#define 	crt_NOBLINK		0x00	/* No blink bit */
#define 	crt_REVERSE		0x40	/* reverse bit */
#define		crt_NOREVERSE	0x00	/* No reverse bit */

/*****************************************************
        [ESC](^[), [BS](^H), [TAB](^I), [RET](^M)
        [��](^W), [��](^X), [��](^A), [��](^D),
        [Page Down](^B), [Page Up](^F)
        [HOME](^Q), [END]=([SHIFT] + [HOME])(^Z)
        [INS](^V), [DEL](^G), [HELP](^_),
*****************************************************/

#define		ASC_ESC			0x001B
#define		ASC_BS			0x0008
#define		ASC_TAB			0x0009
#define		ASC_CR			0x000D
/*
#define 	ASC_RT 			0x0004
#define 	ASC_LT 			0x0001
#define 	ASC_UP 			0x0017
#define 	ASC_DN 			0x0018
#define		ASC_RUP			0x0006
#define		ASC_RDN			0x0002
#define		ASC_HOME		0x0011
#define		ASC_END			0x001A
#define 	ASC_INS 		0x0016
#define 	ASC_DEL 		0x0007
#define		ASC_HELP		0x001F
*/
#define		IBM_F1			0x803b
#define 	IBM_F2			0x803c
#define 	IBM_F3			0x803d
#define 	IBM_F4			0x803e
#define 	IBM_F5			0x803f
#define 	IBM_F6			0x8040
#define 	IBM_F7			0x8041
#define 	IBM_F8			0x8042
#define 	IBM_F9			0x8043
#define 	IBM_F10			0x8044

#define 	IBM_INS 		0x8052
#define 	IBM_DEL 		0x8053
#define		IBM_RUP			0x8049
#define		IBM_RDN			0x8051
#define 	IBM_RT 			0x804d
#define 	IBM_LT 			0x804b
#define 	IBM_UP 			0x8048
#define 	IBM_DN 			0x8050
#define		IBM_HOME		0x8047
#define		IBM_END			0x804f

#define		N98_F1			0x1b53
#define 	N98_F2			0x1b54
#define 	N98_F3			0x1b55
#define 	N98_F4			0x1b56
#define 	N98_F5			0x1b57
#define 	N98_F6			0x1b45
#define 	N98_F7			0x1b4a
#define 	N98_F8			0x1b50
#define 	N98_F9			0x1b51
#define 	N98_F10			0x1b5A

#define 	N98_INS 		0x1b50
#define 	N98_DEL 		0x1b44
#define 	N98_RT 			0x980c
/* N98_LT == ASC_BS */
#define 	N98_LT 			0x0008
#define 	N98_UP 			0x980b
#define 	N98_DN 			0x980a
#define 	N98_HOME		0x981a
#define 	N98_SHOME		0x981e

#define 	KSH_INS 		0x1b49
#define 	KSH_DEL 		0x1b44
#define 	KSH_RT 			0x1b3e
#define 	KSH_LT 			0x1b3c
#define 	KSH_UP 			0x1b5e
#define 	KSH_DN 			0x1b5f
#define		KSH_RUP			0x1b7b
#define		KSH_RDN			0x1b7d
#define 	KSH_HELP		0x1b3f

#define 	FMR_INS 		0x1b50
#define 	FMR_DEL 		0x1b56
#define		FMR_RUP			0x7816
#define		FMR_RDN			0x7817
#define 	FMR_RT 			0x781c
#define 	FMR_LT 			0x781d
#define 	FMR_UP 			0x781e
#define 	FMR_DN 			0x781f
#define 	FMR_HOME		0x780b
#define 	FMR_SHOME		0x780c
#define 	FMR_HELP		0x780c

struct	win	{
	int		x1;
	int		y1;
	int		x2;
	int		y2;
	void	*vram_buf;		/*	win_st	*/
	char	*msg;
};

#define 	WIN 	struct win

struct	dat_st{
	int		x;
	char	*buf;
};

struct	menust{
	int		ret;		/*	return_code(field ptr)	*/
	int		y;
	struct 	dat_st msg;
};

struct	datast{
	int		ret;		/*	return_code(field ptr)	*/
	int		y;
	struct 	dat_st msg;
	int		w;		/*	wide */
	int		type;		/*	data type 'D', 'H', 'T', 'W' */
	struct 	dat_st dat;
};

/* cl /Zg /c *.c	*/

extern  int window(int x1,int y1,int x2,int y2);
static  char *sperate(char *tmp,char *str,int xx);
extern  int wprintf(char *text,...);
extern  int wclreol(void );
extern  int wprintxy(int x,int y,char *text);
extern  int wgotoxy(int x,int y);
extern  int wclrscr(void );
extern  struct win *mkwindow(int x1,int y1,int x2,int y2,char *msg);
extern  int selwindow(struct win *win,int color);
extern  int rmwindow(struct win *win);
extern  int show_menu_msg(struct menust *buf,int rev);
extern  int choose_menu(struct win *win,struct menust *buf,int ptr,int cnt,int mode);
extern  void pc98_putfkey(int code,char *key_str);
extern  void pc98_crt_putch(int ch);
extern  void pc98_crt_puts(char *buf);
extern  int pc98_box(int left,int top,int right,int bottom,int c);
extern  int pc98_cur_set(int start,int end);
extern  int pc98_gettext(int left,int top,int right,int bottom,void *destin);
extern  int pc98_puttext(int left,int top,int right,int bottom,void *destin);
extern  int pc98_movetext(int left,int top,int right,int bottom,int destleft,int desttop);
extern  int pc98_boxcls(int left,int top,int right,int bottom);
extern  int fmr_puttext(int left,int top,int right,int bottom,void *destin);
extern  int fmr_gettext(int left,int top,int right,int bottom,void *destin);
extern  int fmr_movetext(int left,int top,int right,int bottom,int destleft,int desttop);
extern  int fmr_boxcls(int left,int top,int right,int bottom);
extern  int fmr_cur_set(int start,int end);
extern  int fmr_box(int x1,int y1,int x2,int y2,int color);
extern  void far *_dosv_getvramadr(void );
extern  void ibm_crt_attr(int c);
extern  void ibm_crt_gotoxy(int x,int y);
extern  void ibm_crt_putch(int ch,int attr,int cnt);
extern  void ibm_crt_putxy(int x,int y,char *buf);
extern  void ibm_boxcls(int x1,int y1,int x2,int y2);
extern  void ibm_crt_clear(void );
extern  int ibm_box(int x1,int y1,int x2,int y2,int color);
extern  void ibm_crt_cursor(int *x,int *y);
extern  int ibm_cur_set(int start,int end);
extern  int ibm_gettext(int left,int top,int right,int bottom,void *destin);
extern  int ibm_puttext(int left,int top,int right,int bottom,void *destin);
extern  int ibm_movetext(int left,int top,int right,int bottom,int destleft,int desttop);
extern  int choose_data(struct win *win,struct datast *buf,int ptr,int cnt,int mode);
extern  int change_data(struct win *win,struct datast *buf,int *mode);
extern  int change_swit(struct datast *buf);
extern  int show_swit_buf(struct datast *buf,int blink);
extern  int show_data_buf(struct datast *buf);
extern  int inkey_analyze(struct datast *buf,int mode);
extern  int machine(void );
extern  void crt_clear(void );
extern  void crt_attr(int c);
extern  void crt_printxy(int x,int y,char *text,...);
extern  void crt_putxy(int x,int y,char *buf);
extern  void crt_cursor(int *x,int *y);
extern  int ex_getch(void );
extern  int puttext(int left,int top,int right,int bottom,void *destin);
extern  int gettext(int left,int top,int right,int bottom,void *destin);
extern  int movetext(int left,int top,int right,int bottom,int destleft,int desttop);
extern  int cur_set(int start,int end);
extern  int box_cls(int x1,int y1,int x2,int y2);
extern  int box(int x1,int y1,int x2,int y2,int color);
extern  int question(char *buf,unsigned int n,char *std_answ,char *frm,...);
extern  int correct(char *item,char *frm,...);
extern  int wait_msg(int mode,int x1,int y1,char *text,...);
extern  int sjis2jis(int c);
extern  int jis2sjis(int c);
extern  char *search_from(char *fpath);
extern  int get_fullpath(char *patial_name,char *full_name);
#if (_MSC_VER < 800)
extern  char far *_fstrdup(char far *str);
#endif
