static char *rcsid = "$Header: lib_fmr.cv  1.2  95/01/13 17:41:30  Exp $";
/*
 $Log: RCS/lib_fmr.cv $
 * Revision 1.2  95/01/13 17:41:30
 * Fun. setcur() up for 16 Dot machine
 *
 * Revision 1.1  94/11/25 11:38:16  Hong.Chen
 * Initial revision
 *
*/
#include 	<stdlib.h>
#include 	<stdio.h>
#include 	<ctype.h>
#include 	<jctype.h>
#include 	<memory.h>
#include 	<dos.h>
#include 	"choose.h"


struct FMR_CODE {
	char		text;
};
struct FMR_ATTR {
	unsigned char	txtf;
	unsigned char	attr;
	int				color;
};
struct FMR_CELL	{
	struct FMR_CODE far	*code;
	struct FMR_ATTR far	*attr;
};

/*
#################################################
#	function:	put text   						#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
fmr_puttext(int left,int top,int right,int bottom,void *destin)
{
union REGS			stregs;
struct SREGS		stsregs;
int					b_size;
struct FMR_CELL		sa;
struct FMR_CELL	far	*s=&sa;

	b_size 	= (right-left+1)*(bottom-top+1);
	s->code = (struct FMR_CODE far *)destin;
	s->attr = (struct FMR_ATTR far *)(&(s->code->text) + b_size);

	stregs.h.ah = 0x15;
	stregs.h.al = 1;
	stregs.h.dh = top+1;
	stregs.h.dl = left+1;
	stregs.h.bh = bottom+1;
	stregs.h.bl = right+1;
	stsregs.ds 	= FP_SEG(s);
	stregs.x.di = FP_OFF(s);
	int86x(0x91, &stregs, &stregs,&stsregs);

	return(stregs.h.ah);
}


/*
#################################################
#	function:	get text   						#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
fmr_gettext(int left,int top,int right,int bottom,void *destin)
{
union REGS			stregs;
struct SREGS		stsregs;
int					b_size;
struct FMR_CELL		sa;
struct FMR_CELL	far	*s=&sa;

	b_size 	= (right-left+1)*(bottom-top+1);
	s->code = (struct FMR_CODE far *)destin;
	s->attr = (struct FMR_ATTR far *)(&(s->code->text) + b_size);
/*
printf("save text size = %0x(%0x+%0x) \n",5*b_size,4*b_size,b_size);
printf("distin=%8lx \n",destin);
printf("s->code=%8lx, s->attr=%8lx \n",s->code,s->attr);
getch();
*/
	stregs.h.ah = 0x16;
	stregs.h.al = 1;
	stregs.h.dh = top+1;
	stregs.h.dl = left+1;
	stregs.h.bh = bottom+1;
	stregs.h.bl = right+1;
	stsregs.ds  = FP_SEG(s);
	stregs.x.di = FP_OFF(s);
	int86x(0x91, &stregs, &stregs,&stsregs);

	return(stregs.h.ah);
}


/*
#################################################
#	function:	move text   					#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
fmr_movetext(int left,int top,int right,int bottom,int destleft,int desttop)
{
union REGS		stregs;

	stregs.h.ah = 0x17;
	stregs.h.al = 1;
	stregs.h.dh = top+1;
	stregs.h.dl = left+1;
	stregs.h.bh = bottom+1;
	stregs.h.bl = right+1;
	stregs.h.ch = desttop+1;
	stregs.h.cl = destleft+1;
	int86(0x91, &stregs, &stregs);

	return(stregs.h.ah);
}

/*
#################################################
#	function:	move text   					#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
fmr_boxcls(int left,int top,int right,int bottom)
{
union REGS		stregs;

	stregs.h.ah = 0x18;
	stregs.h.al = 1;
	stregs.h.dh = top+1;
	stregs.h.dl = left+1;
	stregs.h.bh = bottom+1;
	stregs.h.bl = right+1;
	int86(0x91, &stregs, &stregs);

	return(stregs.h.ah);
}

/*
#########################################################
#	function:�J�[�\���̌`�����ݒ�			#
#	author	:ChenHong		92/04/08	#
#########################################################
*/
fmr_cur_set(int start,int end)
{
union REGS	stregs;

	stregs.h.ah = 0x9;
	stregs.h.al = start==end ? 0x2f:0x7f;
	stregs.h.dh = (start>15 && start<24) ? 15:start; /* For 16 dot machine */
	stregs.h.dl = end;
	int86(0x91, &stregs, &stregs);

	return(stregs.h.ah);
}

fmr_box(int x1, int y1, int x2, int y2, int color)
{
int		y;

	crt_attr(crt_REVERSE | color);
	crt_printxy(x1, y1, "%*s", x2-x1+1, "");
	fmr_movetext(x1,y1,x2,y1,x1,y2);

	for(y=y1+1;y<y2;y++)
	{
		crt_putxy(x1, y, " ");
		crt_putxy(x2, y, " ");
	}

	crt_attr(crt_NOREVERSE);
	crt_attr(crt_WHITE);

	return(0);
}
