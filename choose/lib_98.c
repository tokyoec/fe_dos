static char *rcsid = "$Header: LIB_98.CV  1.1  94/11/25 11:38:24  Hong.Chen  Exp $";
/*
 $Log: RCS/LIB_98.CV $
 * Revision 1.1  94/11/25 11:38:24  Hong.Chen
 * Initial revision
 *
*/
#include 	<stdlib.h>
#include 	<stdio.h>
#include 	<conio.h>
#include 	<ctype.h>
#include 	<jctype.h>
#include	<string.h>
#include	<jstring.h>
#include 	<dos.h>
#include 	"choose.h"

struct PC98_PAGE
{
    unsigned int    data[25][80];
    unsigned int    dummy[48];
};
struct PC98_SCR
{
    struct PC98_PAGE codepage[2];
    struct PC98_PAGE attrpage[2];
} far          *scr;

void
pc98_putfkey(int code, char *key_str)
{
    union REGS      stregs;

    stregs.h.cl = 0x0d;
    stregs.x.ax = code;			/* charactor */
    stregs.x.dx = (unsigned) key_str;	/* charactor */
    int86(0xdc, &stregs, &stregs);
}

void
pc98_crt_putch(int ch)
{
    union REGS      stregs;

    stregs.h.al = ch;			/* charactor */
    int86(0x29, &stregs, &stregs);
}

void
pc98_crt_puts(char *buf)
{
    for (; *buf; buf++)
	pc98_crt_putch(*buf);
}

/*
#################################################
#	function:	flame write  					#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
pc98_box(int left, int top, int right, int bottom, int c)
{
    unsigned int    attr, attrblue;
    int             i, j, color;

    scr = (struct PC98_SCR far *) 0xa0000000;
    if (((left + 2) > right) || ((top + 2) > bottom))
	return (-1);
    color = 0;
    if (c & crt_RED)
	color |= 0x2;
    if (c & crt_GREEN)
	color |= 0x4;
    if (c & crt_BLUE)
	color |= 0x1;
    attr = (color << 5) | 0x04 | 0x01 | 0x00;
    attrblue = (1 << 5) | 0x04 | 0x01 | 0x00;
/* 	|G|R|B|?|?|REVERSE|BLINK|NOSECRET|SECRET */


    for (i = top + 1; i < bottom + 1; i++)
    {
	(scr->codepage[0].data[i][left]) = 0x262b - ' ';
	(scr->attrpage[0].data[i][left]) = attr;
	(scr->codepage[0].data[i][right]) = 0x272b - ' ';
	(scr->attrpage[0].data[i][right]) = attr;
#ifdef DISP_SHADOW
	if (right + 1 <= 79)
	{
	    (scr->codepage[0].data[i][right + 1]) = ' ';
	    (scr->attrpage[0].data[i][right + 1]) = attrblue;
	}
#endif
    }
    for (j = left + 1; j < right + 1; j++)
    {
	(scr->codepage[0].data[top][j]) = 0x242b - ' ';
	(scr->attrpage[0].data[top][j]) = attr;
	(scr->codepage[0].data[bottom][j]) = 0x252b - ' ';
	(scr->attrpage[0].data[bottom][j]) = attr;
#ifdef DISP_SHADOW
	if (bottom + 1 <= 23)
	{
	    (scr->codepage[0].data[bottom + 1][j]) = ' ';
	    (scr->attrpage[0].data[bottom + 1][j]) = attrblue;
	}
#endif
    }
    (scr->codepage[0].data[top][left]) = 0x302b - ' ';
    (scr->attrpage[0].data[top][left]) = attr;
    (scr->codepage[0].data[top][right]) = 0x362b - ' ';
    (scr->attrpage[0].data[top][right]) = attr;
    (scr->codepage[0].data[bottom][left]) = 0x392b - ' ';
    (scr->attrpage[0].data[bottom][left]) = attr;
    (scr->codepage[0].data[bottom][right]) = 0x3f2b - ' ';
    (scr->attrpage[0].data[bottom][right]) = attr;
#ifdef DISP_SHADOW
    if ((bottom + 1 <= 23) && (right + 1 <= 79))
    {
	(scr->codepage[0].data[bottom + 1][right + 1]) = ' ';
	(scr->attrpage[0].data[bottom + 1][right + 1]) = attrblue;
    }
#endif
    return (0);
}


/*
#############################################################
#	function:�J�[�\���̌`�����A���_�[�o�[�^�ɕς���			#
#	author	:	April/20/1990 	Ver1.0 		By S.I			#
#############################################################
*/
pc98_cur_set(int start, int end)
{
    if ((start == 24) && (end == 24))
    {
	start = 16;
	end = 16;
    }
    else
    {
	if (start > 15)
	    start = 15;
	if (end > 15)
	    end = 15;
    }

    while (0x04 == inp(0x60))
    {;
    }
    outp(0x62, 0x4b);			/* Command send to GDC */
    outp(0x60, (0x80 | 0x0f));
    outp(0x60, (start & 0x1f));
    outp(0x60, (end << 3) | 0x03);
    return (0);
}

/*
#################################################
#	function:	get text   						#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
pc98_gettext(int left, int top, int right, int bottom, void *destin)
{
    int             i, j;
    unsigned int    offs;
    unsigned int   *c_attr;
    unsigned int   *c_code;

    scr = (struct PC98_SCR far *) 0xa0000000;
    c_code = destin;
    c_attr = ((unsigned int *) destin) + (bottom - top + 1) * (right - left + 1);

    for (i = top; i <= bottom; i++)
    {
	for (j = left; j <= right; j++)
	{
	    offs = (j - left) + (right - left + 1) * (i - top);
	    c_code[offs] = scr->codepage[0].data[i][j];
	    c_attr[offs] = scr->attrpage[0].data[i][j];
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
pc98_puttext(int left, int top, int right, int bottom, void *destin)
{
    int             i, j;
    unsigned int    offs;
    unsigned int   *c_attr;
    unsigned int   *c_code;

    scr = (struct PC98_SCR far *) 0xa0000000;
    c_code = destin;
    c_attr = ((unsigned int *) destin) + (bottom - top + 1) * (right - left + 1);

    for (i = top; i <= bottom; i++)
    {
	for (j = left; j <= right; j++)
	{
	    offs = (j - left) + (right - left + 1) * (i - top);
	    scr->codepage[0].data[i][j] = c_code[offs];
	    scr->attrpage[0].data[i][j] = c_attr[offs];
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
pc98_movetext(int left, int top, int right, int bottom, int destleft, int desttop)
{
    int             i;
    unsigned int    soff, doff, dlen;

    dlen = (right - left + 1) * 2;

    if (top <= desttop)			/* move down */
    {
	soff = (bottom * 80 + left) * 2;
	doff = ((desttop + bottom - top) * 80 + destleft) * 2;
	for (i = bottom; i >= top; i--)
	{
	    movedata(0xa000, soff, 0xa000, doff, dlen);
	    movedata(0xa200, soff, 0xa200, doff, dlen);
	    soff -= 160;
	    doff -= 160;
	}
    }
    else
	/* move up */
    {
	soff = (top * 80 + left) * 2;
	doff = (desttop * 80 + destleft) * 2;
	for (i = top; i <= bottom; i++)
	{
	    movedata(0xa000, soff, 0xa000, doff, dlen);
	    movedata(0xa200, soff, 0xa200, doff, dlen);
	    soff += 160;
	    doff += 160;
	}
    }
    return (0);
}

/*
#################################################
#	function:	flame write  					#
#	author	:	ChenHong			90/03/05	#
#################################################
*/
pc98_boxcls(int left, int top, int right, int bottom)
{
    int             i, j;
    unsigned int    attr;

    scr = (struct PC98_SCR far *) 0xa0000000;
    attr = 0x01;

    for (i = top; i <= bottom; i++)
    {
	for (j = left; j <= right; j++)
	{
	    (scr->codepage[0].data[i][j]) = ' ';
	    (scr->attrpage[0].data[i][j]) = attr;
	}
    }

    return (0);
}
