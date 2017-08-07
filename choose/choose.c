static char *rcsid = "$Header: CHOOSE.CV  1.3  95/04/25 22:35:12  Hong.Chen  Exp $";
/*
 $Log: RCS/CHOOSE.CV $
 * Revision 1.3  95/04/25 22:35:12  Hong.Chen
 * for MSVC
 *
 * Revision 1.2  95/01/18 22:15:08  Hong.Chen
 * key [SHOME]->[HELP]
 *
 * Revision 1.1  94/11/25 11:38:40  Hong.Chen
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


/* structure for text position */
struct TEXT_WIN
{
    short           x1;
    short           y1;
    short           x2;
    short           y2;
    short           xx;			/* curent x */
    short           yy;			/* curent y */
};
static struct TEXT_WIN tw;
static struct TEXT_WIN ow[15];
static          ow_p = 0;

window(int x1, int y1, int x2, int y2)
{
    tw.x1 = x1;
    tw.y1 = y1;
    tw.x2 = x2;
    tw.y2 = y2;
    tw.xx = x1;				/* curent x */
    tw.yy = y1;				/* curent y */
    crt_putxy(tw.xx, tw.yy, "");
    return (0);
}

static char    *
sperate(char *tmp, char *str, int xx)
{
    for (;;)
    {
        switch (*str)
        {
            default:
        if (iskanji(*str))
        {
            *tmp++ = *str++;
            xx++;
        }
        *tmp++ = *str++;
        xx++;
        if (xx <= tw.x2)
            continue;
            case 0:
            case '\n':
            case '\r':
        *tmp = 0;
        str--;
        return (str);
        }
    }
}

wprintf(char *text,...)
{
    unsigned char   buf[256], *str = buf, tmp[80];

    vsprintf(buf, text, (va_list) (&text + 1));
    for (; *str; str++)
    {
        switch (*str)
        {
            default:
        str = sperate(tmp, str, tw.xx);
        crt_putxy(tw.xx, tw.yy, tmp);
        tw.xx += strlen(tmp);
        if (tw.xx <= tw.x2)
            continue;
        tw.xx = tw.x1;
            case '\n':
        if (tw.yy < tw.y2)
            tw.yy++;
        else
        {
            movetext(tw.x1, tw.y1 + 1, tw.x2, tw.y2, tw.x1, tw.y1);
            tw.xx = tw.x1;
            wclreol();
        }
            case '\r':
        tw.xx = tw.x1;
        crt_putxy(tw.xx, tw.yy, "");
        break;
        }
    }
    return (0);
}

wclreol()
{
    crt_printxy(tw.xx, tw.yy, "%*s", tw.x2 - tw.xx + 1, "");
    crt_putxy(tw.xx, tw.yy, "");
    return (0);
}

wprintxy(int x, int y, char *text)
{
    crt_putxy(tw.x1 + x, tw.y1 + y, text);
    tw.xx = tw.x1 + x + strlen(text);
    tw.yy = tw.y1 + y;
    return (0);
}

wgotoxy(int x, int y)
{
    crt_putxy(tw.x1 + x, tw.y1 + y, "");
    tw.xx = tw.x1 + x;
    tw.yy = tw.y1 + y;
    return (0);
}


wclrscr()
{
    box_cls(tw.x1, tw.y1, tw.x2, tw.y2);
    tw.xx = tw.x1;			/* curent x */
    tw.yy = tw.y1;			/* curent y */
    crt_putxy(tw.xx, tw.yy, "");
    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/16	#
#################################################
*/
WIN            *
mkwindow(int x1, int y1, int x2, int y2, char *msg)
{
    int             b_size, tx;
    static WIN      win;
    int             TEXT_UNIT = 5;

    switch (machine())
    {
        case PC_98:
            TEXT_UNIT = 4;
            break;
        case AT_US:
        case AT_JP:
        case AT_CN:
            TEXT_UNIT = 2;
            break;
        case FMR_50:
            TEXT_UNIT = 5;
            break;
        default:
            break;
    }

    win.x1 = x1;
    win.y1 = y1;
    win.x2 = x2;
    win.y2 = y2;
    win.msg = strdup(msg);

#ifdef DISP_SHADOW
    tx = (x2 - x1 + 4 + 1) & 0xfe;
    b_size = tx * (y2 - y1 + 3 + 1) * TEXT_UNIT;
#else
    tx = (x2 - x1 + 4) & 0xfe;
    b_size = tx * (y2 - y1 + 3) * TEXT_UNIT;
#endif
    if (NULL == (win.vram_buf = malloc((size_t) b_size)))
    {
        /****************
        printf("Malloc Error !!!");
        exit(1);
        *****************/
        return ((WIN *) NULL);
    }
#ifdef DISP_SHADOW
    gettext(x1 - 1, y1 - 1, x2 + 1 + 1, y2 + 1 + 1, win.vram_buf);
#else
    gettext(x1 - 1, y1 - 1, x2 + 1, y2 + 1, win.vram_buf);
#endif

    ow[ow_p++] = tw;
    window(x1, y1, x2, y2);
    wclrscr();

    return (&win);
}

/*
#################################################
#	function:selwindow			#
#	author	:ChenHong	90/03/05	#
#################################################
*/
selwindow(WIN * win, int color)
{
    box(win->x1 - 1, win->y1 - 1, win->x2 + 1, win->y2 + 1, color);
    window(win->x1, win->y1, win->x2, win->y2);
    crt_attr(crt_REVERSE);
    crt_putxy((win->x1 + win->x2 - strlen(win->msg)) / 2 + 1, win->y1 - 1, win->msg);
    crt_attr(crt_NOREVERSE);
    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/16	#
#################################################
*/
int
rmwindow(WIN * win)
{
#ifdef DISP_SHADOW
    puttext(win->x1 - 1, win->y1 - 1, win->x2 + 1 + 1, win->y2 + 1 + 1, win->vram_buf);
#else
    puttext(win->x1 - 1, win->y1 - 1, win->x2 + 1, win->y2 + 1, win->vram_buf);
#endif
    free(win->vram_buf);
    free(win->msg);
    tw = ow[--ow_p];
    return (0);
}

/*
#################################################
#	function:menu show			#
#	input	:x,y___start point		#
#		 menu__menu sturucture		#
#	output	:int___ret of choose menu	#
#	note	:be care for menu structure	#
#	author	:ChenHong	90/10/10	#
#################################################
*/
int
show_menu_msg(struct menust * buf, int rev)
{
    if (rev)
        crt_attr(crt_REVERSE);
    wprintxy(buf->msg.x, buf->y, buf->msg.buf);
    crt_attr(crt_NOREVERSE);
    return (0);
}

/*
#################################################
#	function:menu choose			#
#	input	:x,y___start point		#
#		 menu__menu sturucture		#
#	output	:int___ret of choose menu	#
#	note	:be care for menu structure	#
#	author	:ChenHong	90/01/18	#
#################################################
*/
int
choose_menu(WIN * win, struct menust * buf, int ptr, int cnt, int mode)
{
    int             sptr, ymin, ymax, i;
    unsigned int    k;

    selwindow(win, crt_YELLOW);
    KEYCLR;

    for (sptr = 0; sptr < cnt; sptr++)
        show_menu_msg(buf + sptr, 0);

    ymax = cnt - 1;
    ymin = 0;

    if (ptr > ymax)
        ptr = ymin;
    if (ptr < ymin)
        ptr = ymin;

    show_menu_msg(buf + ptr, 1);

    for (;;)
    {
        switch (k = ex_getch())
        {
            default:
        for (i = 0; i < cnt; i++)
        {
            if (toupper((int)k) == buf[i].msg.buf[0])	/* ���������� */
            {
        show_menu_msg(buf + ptr, 0);
        ptr = i;
        show_menu_msg(buf + ptr, 1);
        break;
            }
        }
            case 0:
        break;
            case ASC_ESC:
        selwindow(win, crt_CYAN);
        return (-1);
        break;
            case ASC_CR:
        selwindow(win, crt_CYAN);
        return (buf[ptr].ret);
        break;
            case IBM_UP:
            case FMR_UP:
            case N98_UP:
            case KSH_UP:
        show_menu_msg(buf + ptr, 0);
        if (ptr > ymin)
            --ptr;
        else
            ptr = ymax;
        show_menu_msg(buf + ptr, 1);
        break;
            case IBM_DN:
            case FMR_DN:
            case N98_DN:
            case KSH_DN:
        show_menu_msg(buf + ptr, 0);
        if (ptr < ymax)
            ++ptr;
        else
            ptr = ymin;
        show_menu_msg(buf + ptr, 1);
        break;
            case IBM_HOME:
            case FMR_HOME:
            case N98_HOME:
        show_menu_msg(buf + ptr, 0);
        ptr = ymin;
        show_menu_msg(buf + ptr, 1);
        break;
            case IBM_END:
            case FMR_HELP:
            case KSH_HELP:
        show_menu_msg(buf + ptr, 0);
        ptr = ymax;
        show_menu_msg(buf + ptr, 1);
        break;
        }
    }
}
