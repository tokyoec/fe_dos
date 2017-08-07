static char    *rcsid = "$Header: MODIFY.CV  1.4  95/06/05 01:18:00  Hong.Chen  Exp $";
/*
 $Log: RCS/MODIFY.CV $
 * Revision 1.4  95/06/05 01:18:00  Hong.Chen
 * debug in insert mode
 * write over buff. bound
 *
 * Revision 1.3  95/04/27 03:02:44  Hong.Chen
 * for MSVC
 *
 * Revision 1.2  95/01/18 22:13:16  Hong.Chen
 * key [SHOME]->[HELP]
 *
 * Revision 1.1  94/11/25 11:38:02  Hong.Chen
 * Initial revision
 *
*/
#include	<stdlib.h>
#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	<ctype.h>
#include	<jctype.h>
#include	<string.h>
#include	<jstring.h>
#include	<malloc.h>
#include	"choose.h"

static int      ymin, ymax, yptr, xptr;
static unsigned int key;

struct SWITCH_BUF
{
    char            buf[80];
    struct
    {
	int             x;
	char           *dat;
    }               key[20];
    int             cnt;
};
static struct SWITCH_BUF w;

/*
#########################################################
#							#
#	function:	choose data			#
#							#
#	input	:	x,y___start point		#
#				data__data sturucture	#
#	output	:	int___ret			#
#							#
#	note	:	be care for menu structure	#
#							#
#	author	:	ChenHong	90/01/18	#
#						 	#
#########################################################
*/
int
choose_data(WIN * win, struct datast * buf, int ptr, int cnt, int mode)
{
    int             sptr;
    int             cf;			/* change flag  1:move cursor, 2:date
					 * change */

    selwindow(win, crt_YELLOW);
    KEYCLR;

    for (sptr = 0; sptr < cnt; sptr++)
    {
	if (NULL != buf[sptr].msg.buf)
	{
	    wprintxy(buf[sptr].msg.x, buf[sptr].y, buf[sptr].msg.buf);
	}
	switch (buf[sptr].type)
	{
	    case 'W':
	    case 'w':
		show_swit_buf(buf + sptr, 0);
		break;
	    default:
		show_data_buf(buf + sptr);
		break;
	}
    }

    ymax = sptr - 1;
    ymin = 0;
    yptr = ptr;

    if (yptr > ymax)
	yptr = ymin;
    if (yptr < ymin)
	yptr = ymin;

    xptr = 0;
    cf = 1;
    while (1)
    {
	if (cf)				/* Has Change */
	{
	    crt_attr(crt_REVERSE | crt_YELLOW);
	    switch (buf[yptr].type)
	    {
		case 'W':
		case 'w':
		    crt_putxy(win->x2 - 11, win->y2 + 1, " <-Select-> ");
		    crt_attr(crt_NOREVERSE);
		    show_swit_buf(buf + yptr, 1);
		    break;
		case 'p':
		case 'P':
		    crt_putxy(win->x2 - 11, win->y2 + 1, "TAB:ReferDir");
		    crt_attr(crt_NOREVERSE);
		    goto rewrite;
		default:
		    crt_putxy(win->x2 - 11, win->y2 + 1, "TAB:ClearAll");
		    crt_attr(crt_NOREVERSE);
		  rewrite:
		    if (cf == 2)
			show_data_buf(buf + yptr);	/* All Change */
		    wgotoxy(buf[yptr].dat.x + xptr, buf[yptr].y);
		    switch (mode)
		    {
			case 0:
			    cur_set(0, 24);
			    break;
			default:
			    mode = 1;
			case 1:
			    cur_set(15, 24);	/* insert mode */
			    break;
		    }
		    break;
	    }
	    cf = 0;
	}

	key = ex_getch();
	switch (key)
	{
	    default:
		switch (buf[yptr].type)
		{
		    case 'W':
		    case 'w':
			cf = change_swit(buf);
			break;
		    default:
			cf = change_data(win, buf, &mode);
			break;
		}
		break;
	    case 0:
		break;
	    case ASC_TAB:		/* TAB key */
		switch (buf[yptr].type)
		{
		    case 'W':
		    case 'w':
			break;
		    case 'P':
		    case 'p':
			cur_set(24, 24);
			selwindow(win, crt_CYAN);
			return (-2 - yptr);
		    default:
			buf[yptr].dat.buf[0] = 0;
			xptr = 0;
			cf = 2;
		}
		break;
	    case ASC_ESC:
		if (kbhit())
		    break;
		cur_set(24, 24);
		selwindow(win, crt_CYAN);
		return (-1);
	    case ASC_CR:
		cur_set(24, 24);
		selwindow(win, crt_CYAN);
		return (buf[yptr].ret);
	}
    }
}


int
change_data(struct win * win, struct datast * buf, int *mode)
{
    int             i;

    switch (key)
    {
	case IBM_INS:
	case FMR_INS:
	case KSH_INS:
	    *mode = 1 - *mode;
	    break;
	case ASC_BS:			/* BS key */
	    if (xptr <= 0)
		return (0);
	    xptr--;
	case IBM_DEL:
	case FMR_DEL:
	case N98_DEL:
	    strcpy(buf[yptr].dat.buf + xptr, buf[yptr].dat.buf + (xptr + 1));
	    xptr = min(xptr, (int) strlen(buf[yptr].dat.buf));
	    return (2);
	case IBM_LT:
	case FMR_LT:
	case KSH_LT:
	    if (xptr > 0)
	    {
		xptr--;
		break;
	    }
	case IBM_UP:
	case FMR_UP:
	case N98_UP:
	case KSH_UP:
	    if (yptr > ymin)
		--yptr;
	    else
		yptr = ymax;
	    xptr = 0;
	    break;
	case IBM_HOME:
	case FMR_HOME:
	case N98_HOME:
	    xptr = 0;
	    break;
	case IBM_END:
	case FMR_HELP:
	case KSH_HELP:
	    for (i = strlen(buf[yptr].dat.buf); i >= 0; i--)
		if (buf[yptr].dat.buf[i - 1] != ' ')
		    break;
	    xptr = min(i, buf[yptr].w - 1);
	    break;
	default:
	    if (inkey_analyze(buf, *mode) == -1)
	    {
		crt_beep();
		return (0);
	    }
	    /* rewrite current line after input a word */
	    show_data_buf(buf + yptr);
	case IBM_RT:
	case FMR_RT:
	case N98_RT:
	case KSH_RT:
	    if (xptr < (buf[yptr].w - 1))
	    {
		xptr++;
		break;
	    }
	case IBM_DN:
	case FMR_DN:
	case N98_DN:
	case KSH_DN:
	    if (yptr < ymax)
		++yptr;
	    else
		yptr = ymin;
	    xptr = 0;
	    break;
    }

    return (1);
}

int
change_swit(struct datast * buf)
{
    int             i;

    switch (key)
    {
	default:
	    for (i = 0; i < w.cnt; i++)
	    {
		if (toupper((int) key) == w.key[i].dat[0])	/* ���������� */
		{
		    buf[yptr].w = i;
		    break;
		}
	    }
	    break;
	case ' ':
	    if (buf[yptr].w < (w.cnt - 1))
		buf[yptr].w++;
	    else
		buf[yptr].w = 0;
	    break;
	case IBM_HOME:
	case FMR_HOME:
	case N98_HOME:
	    buf[yptr].w = 0;
	    break;
	case IBM_END:
	case FMR_HELP:
	case KSH_HELP:
	    buf[yptr].w = w.cnt - 1;
	    break;
	case IBM_RT:
	case FMR_RT:
	case N98_RT:
	case KSH_RT:
	    if (buf[yptr].w < (w.cnt - 1))
		buf[yptr].w++;
	    else
		return (0);
	    break;
	case IBM_LT:
	case FMR_LT:
	case N98_LT:
	case KSH_LT:
	    if (buf[yptr].w > 0)
		buf[yptr].w--;
	    else
		return (0);
	    break;
	case IBM_UP:
	case FMR_UP:
	case N98_UP:
	case KSH_UP:
	    show_swit_buf(buf + yptr, 0);
	    if (yptr > ymin)
		--yptr;
	    else
		yptr = ymax;
	    break;
	case IBM_DN:
	case FMR_DN:
	case N98_DN:
	case KSH_DN:
	    show_swit_buf(buf + yptr, 0);
	    if (yptr < ymax)
		++yptr;
	    else
		yptr = ymin;
	    break;
    }

    return (1);
}

int
show_swit_buf(struct datast * buf, int blink)
{
    char           *p;

    strcpy(w.buf, buf->dat.buf);
    p = strtok(w.buf, " ");
    for (w.cnt = 0; *p; w.cnt++)
    {
	w.key[w.cnt].dat = p;
	w.key[w.cnt].x = buf->dat.x + (int) (p - w.buf);
	p = strtok(NULL, " ");
    }

    cur_set(24, 24);			/* not cursor at display */
    wprintxy(buf->dat.x, buf->y, buf->dat.buf);

    if (blink)
	crt_attr(crt_REVERSE | crt_BLINK);
    else
	crt_attr(crt_REVERSE);
    wprintxy(w.key[buf->w].x, buf->y, w.key[buf->w].dat);
    crt_attr(crt_NOREVERSE);

    return (0);
}

int
show_data_buf(struct datast * buf)
{
    cur_set(24, 24);			/* not cursor at display */
    wgotoxy(buf->dat.x, buf->y);
    wprintf("%-*s", buf->w, buf->dat.buf);
    return (0);
}


/*
#########################################################
#							#
#	function:	inkey analyze			#
#							#
#	input	:	mode _inkey char type		#
#			line _current input line	#
#			key __inkey data     		#
#			dx _edit position   		#
#							#
#	output	:	int __ret			#
#							#
#	note	:	be care for menu structure	#
#							#
#	author	:	ChenHong	90/05/08	#
#						 	#
#########################################################
*/
inkey_analyze(struct datast * buf, int mode)
{
    char            temp[80];
    int             len;
    int             real_key = key & 0x00ff;

    switch (buf[yptr].type)
    {
	default:
	    printf("choose_data error !!!\n");
	    exit(1);
	case 'd':
	case 'D':
	    if (!isdigit(real_key))
		return (-1);
	case 'h':
	case 'H':
	    if (!isxdigit(real_key))
		return (-1);
	case 't':
	case 'T':
	case 'p':
	case 'P':
	    if (!(isprint(real_key) || iskanji(real_key) || iskanji2(real_key)))
	    {
		wait_msg(1, 5, 5, "realkey(%x),isp(%d),isk(%d),isk2(%d)",
			 real_key, isprint(real_key),
			 iskanji(real_key), iskanji2(real_key));
		return (-1);
	    }
	    len = strlen(buf[yptr].dat.buf);
	    if (xptr >= len)		/* over string end , full with ' ' */
	    {
		memset(&buf[yptr].dat.buf[len], ' ', xptr - len);
		buf[yptr].dat.buf[xptr + 1] = 0;
	    }
	    if (mode)			/* insert mode */
	    {
		strcpy(temp, &buf[yptr].dat.buf[xptr]);
		strncpy(&buf[yptr].dat.buf[xptr + 1], temp, buf[yptr].w - xptr - 1);
		buf[yptr].dat.buf[(buf[yptr].w)] = 0;
	    }
	    buf[yptr].dat.buf[xptr] = real_key;
	    return (0);
    }
}
