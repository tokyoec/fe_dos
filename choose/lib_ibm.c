static char *rcsid = "$Header: LIB_IBM.CV  1.1  94/11/25 11:38:10  Hong.Chen  Exp $";
/*
 $Log: RCS/LIB_IBM.CV $
 * Revision 1.1  94/11/25 11:38:10  Hong.Chen
 * Initial revision
 *
*/
#include 	<stdlib.h>
#include 	<stdio.h>
#include 	<ctype.h>
#include 	<jctype.h>
#include	<string.h>
#include	<jstring.h>
#include 	<dos.h>
#include 	"choose.h"

#ifndef	MK_FP
#define	MK_FP(s,o) ((void far *)(((unsigned long)(s) << 16) | (unsigned)(o)))
#endif

void far       *
_dosv_getvramadr(void)			/* �^�����޵��ޯ̧ ���ڽ �̎擾	 */
{
    union REGS      iregs;
    struct SREGS    segs;

    segs.es = iregs.x.di = 0;
    iregs.h.ah = 0xfe;			/* �^�����޵��ޯ̧ ���ڽ �̎擾	 */
    int86x(0x10, &iregs, &iregs, &segs);
    return (MK_FP(segs.es, iregs.x.di));/* �^�����޵��ޯ̧ ���ڽ	 */
}

static int      crt_attr_word = 0x07;	/* normal white */
void
ibm_crt_attr(int c)
{
    int             color;

    if (c == 0)
    {
	crt_attr_word = 0x07;		/* normal white */
	return;
    }
    if (c & crt_BLINK)
	crt_attr_word |= 0x80;		/* BLINK */

    if (c & 0xf)
    {
	color = 0;
	if (c & crt_RED)
	    color |= 0x4;
	if (c & crt_GREEN)
	    color |= 0x2;
	if (c & crt_BLUE)
	    color |= 0x1;
	if (c & crt_REVERSE)
	    color <<= 4;
	crt_attr_word &= 0x88;
	crt_attr_word |= color;
    }
    else
    {
	if ((c & crt_REVERSE) && (crt_attr_word & 0x07))
	{
	    color = (crt_attr_word & 0x7) << 4;
	    crt_attr_word &= 0xf8;
	    crt_attr_word |= (color & 0x7f);
	}
    }
}

void
ibm_crt_gotoxy(int x, int y)
{
    union REGS      stregs;

    stregs.h.ah = 0x2;			/* set cursor position */
    stregs.h.bh = 0;			/* page */
    stregs.h.dh = y;
    stregs.h.dl = x;
    int86(0x10, &stregs, &stregs);
}

void
ibm_crt_putch(int ch, int attr, int cnt)
{
    union REGS      stregs;

    stregs.h.ah = 0x9;			/* write charactor */
    stregs.h.bh = 0;			/* page */
    stregs.h.bl = attr;			/* attribute */
    stregs.h.al = ch;			/* charactor */
    stregs.x.cx = cnt;			/* count */
    int86(0x10, &stregs, &stregs);
}

void
ibm_crt_putxy(int x, int y, char *buf)
{
    ibm_crt_gotoxy(x, y);
    for (; *buf; buf++, x++)
    {
	ibm_crt_gotoxy(x, y);
	ibm_crt_putch(*buf, crt_attr_word, 1);
    }
}

void
ibm_boxcls(int x1, int y1, int x2, int y2)
{
    int             y;

    for (y = y1; y <= y2; y++)
    {
	ibm_crt_gotoxy(x1, y);
	ibm_crt_putch(' ', 0, x2 - x1 + 1);
    }
}

void
ibm_crt_clear(void)
{
    union REGS      stregs;

    stregs.h.ah = 0x6;
    stregs.h.al = 0;
    stregs.h.ch = 0;
    stregs.h.cl = 0;
    stregs.h.dh = 25 - 1;
    stregs.h.dl = 80 - 1;
    stregs.h.bh = 0;			/* page */
    int86(0x10, &stregs, &stregs);
}

ibm_box(int x1, int y1, int x2, int y2, int color)
{
    int             y;
    char           *w;
    extern char    *waku[];

    switch (machine())
    {
	case AT_US:
	    w = waku[AT_US];
	    break;
	case AT_JP:
	    w = waku[AT_JP];
	    break;
	case AT_CN:
	    w = "                        ";	/* 24 Byte [Space] */
	    break;
    }

    ibm_crt_attr(color | crt_REVERSE);

    ibm_crt_gotoxy(x1, y1);
    ibm_crt_putch(w[0], crt_attr_word, 1);
    ibm_crt_gotoxy(x1 + 1, y1);
    ibm_crt_putch(w[1], crt_attr_word, x2 - x1 + 1 - 2);
    ibm_crt_gotoxy(x2, y1);
    ibm_crt_putch(w[5], crt_attr_word, 1);

    for (y = y1 + 1; y < y2; y++)
    {
	ibm_crt_gotoxy(x1, y);
	ibm_crt_putch(w[6], crt_attr_word, 1);
	ibm_crt_gotoxy(x2, y);
	ibm_crt_putch(w[11], crt_attr_word, 1);
    }

    ibm_crt_gotoxy(x1, y2);
    ibm_crt_putch(w[18], crt_attr_word, 1);
    ibm_crt_gotoxy(x1 + 1, y2);
    ibm_crt_putch(w[19], crt_attr_word, x2 - x1 + 1 - 2);
    ibm_crt_gotoxy(x2, y2);
    ibm_crt_putch(w[23], crt_attr_word, 1);

    ibm_crt_attr(crt_WHITE);

    return (0);
}

void
ibm_crt_cursor(int *x, int *y)
{
    union REGS      stregs;

    stregs.h.ah = 0x3;
    stregs.h.bh = 0;			/* page */
    int86(0x10, &stregs, &stregs);
    *y = stregs.h.dh;
    *x = stregs.h.dl;
}

/*
#########################################################
#	function:�J�[�\���̌`�����ݒ�			#
#	author	:ChenHong		92/04/08	#
#########################################################
*/
ibm_cur_set(int start, int end)
{
    union REGS      stregs;

    if ((start == 24) && (end == 24))
    {
	start = -1;
	end = -1;
    }

    stregs.h.ah = 0x1;
    stregs.h.ch = start >> 1;
    stregs.h.cl = end >> 1;
    int86(0x10, &stregs, &stregs);
    return (0);
}

/*
#################################################
#	function:	get text   						#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
ibm_gettext(int left, int top, int right, int bottom, void *destin)
{
    int             i, j;
    unsigned int    offs;
    unsigned int   *c_code;
    union REGS      stregs;

    c_code = destin;

    for (i = top; i <= bottom; i++)
    {
	for (j = left; j <= right; j++)
	{
	    offs = (j - left) + (right - left + 1) * (i - top);

	    ibm_crt_gotoxy(j, i);
	    stregs.h.ah = 0x8;		/* read charactor */
	    stregs.h.bh = 0;		/* page */
	    int86(0x10, &stregs, &stregs);

	    c_code[offs] = stregs.x.ax;
	}
    }
    return (0);
}

/*
#################################################
#	function:	put text   						#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
ibm_puttext(int left, int top, int right, int bottom, void *destin)
{
    int             i, j;
    unsigned int    offs;
    unsigned int   *c_code;

    c_code = destin;

    for (i = top; i <= bottom; i++)
    {
	for (j = left; j <= right; j++)
	{
	    offs = (j - left) + (right - left + 1) * (i - top);

	    ibm_crt_gotoxy(j, i);
	    ibm_crt_putch(c_code[offs], c_code[offs] >> 8, 1);
	}
    }
    return (0);
}

/*
#################################################
#	function:	move text   					#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
ibm_movetext(int left, int top, int right, int bottom, int destleft, int desttop)
{
    union REGS      stregs;

    if (top <= desttop)
    {
	stregs.h.ah = 0x7;
	stregs.h.al = -(top - desttop);
	bottom++;
    }
    else
    {
	stregs.h.ah = 0x6;
	stregs.h.al = (top - desttop);
	top--;
    }
    stregs.h.ch = top;			/* insert line */
    stregs.h.cl = left;
    stregs.h.dh = bottom;
    stregs.h.dl = right;
    stregs.h.bh = 0;			/* attribute */
    int86(0x10, &stregs, &stregs);
    return (0);
}
