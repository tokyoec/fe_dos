static char *rcsid = "$Header: pc_model.cv  1.2  95/01/13 17:41:32  Exp $";
/*
 $Log: RCS/pc_model.cv $
 * Revision 1.2  95/01/13 17:41:32
 * comment for set_cur
 *
 * Revision 1.1  94/11/25 11:37:58  Hong.Chen
 * Initial revision
 *
*/
#include 	<stdlib.h>
#include 	<stdio.h>
#include 	<ctype.h>
#include 	<jctype.h>
#include	<string.h>
#include	<jstring.h>
#include 	<conio.h>
#include 	<dos.h>
#include 	"choose.h"

int             machine_flag = -1;	/* 0,pc9801 1,ibm-pc/at 2,fmr50
					 * 3,fmr60 */

/*
#################################################
#	function:				#
#	author	:ChenHong	90/03/07	#
#################################################
*/

char           *waku[] =
{
 /* pc98 */
    "������"
    "��  ��"
    "������"
    "������",
 /* fmr */
    "������"
    "��  ��"
    "������"
    "������",
 /* ibm  English mode */
    "����Ŀ"
    "�    �"
    "����Ĵ"
    "������",
 /* DOS/V Japaness Mode */
    ""
    "    "
    ""
    "",
 /* CCBIOS 2.13 Mode */
    "����ͻ"
    "��   �"
    "����͹"
    "����ͼ"
};


#if	0
struct MACHINE_TYPE
{
    int             type;
    char            rom[14];
};
machine()
{
    int             i;
    char            buf[16];
    static struct MACHINE_TYPE machine[] =
    {
	{PC_98, 0xea, 0x00, 0x00, 0x80, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	/* 98 */
	{FMR_50, 0xea, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	/* fmr50 */
	{FMR_50, 0xea, 0x50, 0x00, 0x00, 0xfc, 0x01, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	/* fmr60 */
	{FMR_50, 0xea, 0x75, 0x00, 0x00, 0xfc, 0x21, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	/* fmr70 */
	{FMR_50, 0xea, 0x00, 0x00, 0x00, 0xfc, 0x21, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	/* fmrhx */
	{AT_US, 0xea, 0x5b, 0xe0, 0x00, 0xf0, 0x30, 0x35, 0x2f, 0x30, 0x37, 0x2f, 0x38, 0x37, 0x00},	/* XT */
	{AT_US, 0xea, 0x0c, 0x81, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	/* AT */
    };
    char far       *bufp = buf;

    if (machine_flag != -1)
	return (machine_flag);

    movedata(0xffff, 0, FP_SEG(bufp), FP_OFF(bufp), 14);
    for (i = 0; i < 5; i++)
    {
	if (0 == memcmp(machine[i].rom, buf, 14))
	    return (machine_flag = machine[i].type);
    }
    return (machine_flag = AT_US);	/* unknow machine == ibm */
}
#else
machine()
{
    int             i;

    if (machine_flag != -1)
	return (machine_flag);
    i = *((char far *) (0xffff0000) + 4L);
    switch (0xff & i)
    {
	case 0xfd:
	    machine_flag = PC_98;
	    break;
	case 0xfc:
	    machine_flag = FMR_50;
	    break;
	case 0xf0:
	default:
	    if (_dosv_getvramadr() != NULL)
		machine_flag = AT_JP;	/* DOS/V ���޵�Ӱ��		 */
	    else
		machine_flag = AT_US;	/* IBM-PC �݊��@ ���޵�Ӱ��	 */
	    break;
    }
    return (machine_flag);
}
#endif

void
crt_clear(void)
{
    if (machine() >= AT_US)
	ibm_crt_clear();
    else
	cputs("\033*");			/* clear */
}

void
crt_attr(int c)
{
    char            a[3][4];
    int             i;

    if (machine() >= AT_US)
	ibm_crt_attr(c);
    else
    {
	i = 0;
	if (c & 0xf)
	{
	    sprintf(a[i], "3%1d\0", c & 0xf);	/* set color */
	    i++;
	}
	if (c & crt_REVERSE)
	{
	    strcpy(a[i], "7");		/* REVERSE */
	    i++;
	}
	if (c & crt_BLINK)
	{
	    strcpy(a[i], "5");		/* BLINK */
	    i++;
	}
	switch (i)
	{
	    case 0:
		cputs("\033[0m");	/* NORMAL */
		break;
	    case 1:
		cprintf("\033[%sm", a[0]);
		break;
	    case 2:
		cprintf("\033[%s;%sm", a[0], a[1]);
		break;
	    case 3:
		cprintf("\033[%s;%s;%sm", a[0], a[1], a[2]);
		break;
	}
    }
}

void
crt_printxy(int x, int y, char *text,...)
{
    unsigned char   buf[256];

    vsprintf(buf, text, (va_list) (&text + 1));
    crt_putxy(x, y, buf);
}

/*  buf��%s�����̂܂܏o�͂��邽�߂̊֐�(crt_printxy�ȊO) */
void
crt_putxy(int x, int y, char *buf)
{
    unsigned char   toxy[64];

    switch (machine())
    {
	default:
	    ibm_crt_putxy(x, y, buf);
	    break;
	case PC_98:
	    sprintf(toxy, "\033[%d;%dH", y + 1, x + 1);
	    pc98_crt_puts(toxy);
	    pc98_crt_puts(buf);
	    break;
	case FMR_50:
	    cprintf("\033[%d;%dH%s", y + 1, x + 1, buf);
	    break;
    }
}

void
crt_cursor(int *x, int *y)
{
    int             i;
    char            buf[8];

    switch (machine())
    {
	case AT_US:
	case AT_JP:
	case AT_CN:
	    ibm_crt_cursor(x, y);
	    break;
	default:
	    cputs("\x1b[6n");
	    for (i = 0; i < 8; i++)
		if ('R' == (buf[i] = getch()))
		    break;
	    *y = atoi(buf + 2) - 1;
	    *x = atoi(strchr(buf, ';') + 1) - 1;
	    KEYCLR;
	    break;
    }
}


/*
#################################################
#	function:				#
#	author	:ChenHong	90/03/07	#
#################################################
*/
ex_getch()
{
    unsigned        ch;

    if (!kbhit())
	return (0);

    ch = getch();
    if (kbhit())
    {
	switch (ch)
	{
	    case ASC_ESC:
		ch = 0x1b00 | getch();
		return (ch);
	    case 0:
		ch = 0x8000 | getch();
		return (ch);
	}
    }

    switch (ch)
    {
	case ASC_ESC:
	case ASC_BS:
	case ASC_TAB:
	case ASC_CR:
	    break;
	default:
	    if (!isprint(ch))
	    {
		switch (machine())
		{
		    case PC_98:
			ch |= 0x9800;
			break;
		    case FMR_50:
			ch |= 0x7800;
			break;
		    default:
			ch |= 0x8800;
			break;
		}
	    }
	    break;
    }
    return (ch);
}


puttext(int left, int top, int right, int bottom, void *destin)
{
    switch (machine())
    {
	case PC_98:
	    pc98_puttext(left, top, right, bottom, destin);
	    break;
	case FMR_50:
	    fmr_puttext(left, top, right, bottom, destin);
	    break;
	default:
	    ibm_puttext(left, top, right, bottom, destin);
	    break;
    }
    return (0);
}

gettext(int left, int top, int right, int bottom, void *destin)
{
    switch (machine())
    {
	case PC_98:
	    pc98_gettext(left, top, right, bottom, destin);
	    break;
	case FMR_50:
	    fmr_gettext(left, top, right, bottom, destin);
	    break;
	default:
	    ibm_gettext(left, top, right, bottom, destin);
	    break;
    }
    return (0);
}

movetext(int left, int top, int right, int bottom, int destleft, int desttop)
{
    switch (machine())
    {
	case PC_98:
	    pc98_movetext(left, top, right, bottom, destleft, desttop);
	    break;
	case FMR_50:
	    fmr_movetext(left, top, right, bottom, destleft, desttop);
	    break;
	default:
	    ibm_movetext(left, top, right, bottom, destleft, desttop);
	    break;
    }
    return (0);
}

/*
#################################################
#	function:				#
#	        :0 ,24 INS	22,24 Normal	#
#	        :15,24 REP	24,24 CursorOff	#
#	author	:ChenHong	90/03/07	#
#################################################
*/
cur_set(int start, int end)
{
    switch (machine())
    {
	case PC_98:
	    pc98_cur_set(start, end);
	    break;
	case FMR_50:
	    fmr_cur_set(start, end);
	    break;
	default:
	    ibm_cur_set(start, end);
	    break;
    }
    return (0);
}

box_cls(int x1, int y1, int x2, int y2)
{
    switch (machine())
    {
	case PC_98:
	    pc98_boxcls(x1, y1, x2, y2);
	    break;
	case FMR_50:
	    fmr_boxcls(x1, y1, x2, y2);
	    break;
	default:
	    ibm_boxcls(x1, y1, x2, y2);
	    break;
    }
    return (0);
}


box(int x1, int y1, int x2, int y2, int color)
{
    switch (machine())
    {
	case PC_98:
	    pc98_box(x1, y1, x2, y2, color);
	    break;
	case FMR_50:
	    fmr_box(x1, y1, x2, y2, color);
	    break;
	case AT_US:
	case AT_JP:
	case AT_CN:
	default:
	    ibm_box(x1, y1, x2, y2, color);
	    break;
    }
    return (0);
}
