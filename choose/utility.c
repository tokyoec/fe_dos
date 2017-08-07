static char *rcsid = "$Header: UTILITY.CV  1.5  95/04/26 22:45:24  Hong.Chen  Exp $";
/*
 $Log: RCS/UTILITY.CV $
 * Revision 1.5  95/04/26 22:45:24  Hong.Chen
 * for MSVC
 *
 * Revision 1.4  95/04/24 02:13:50  Hong.Chen
 * Add get_fullpath
 *
 * Revision 1.3  95/01/17 13:38:32
 * debug fstrdup()
 *
 * Revision 1.2  95/01/17 10:36:12  Hong.Chen
 * Add _fstrdup search_from Func.
 *
 * Revision 1.1  94/11/25 11:37:44  Hong.Chen
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
#include	<direct.h>
#include	"choose.h"



/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
question(char *buf, unsigned int n, char *std_answ, char *frm,...)
{
    struct datast   qdat;
    char            tmp[80], str[80];
    int             i, status = 0;
    WIN             win;

    vsprintf(str, frm, (va_list) (&frm + 1));
    strcpy(tmp, std_answ);
    n = min(n, 65 - strlen(str));

    qdat.ret = 1;
    qdat.y = 0;
    qdat.w = n;
    qdat.type = 'T';
    qdat.msg.x = 0;
    qdat.msg.buf = str;
    qdat.dat.x = strlen(str);
    qdat.dat.buf = tmp;

    win = *mkwindow(5, 16, 5 + strlen(str) + n, 16, " Text Dialog ");
    if (-1 == choose_data(&win, &qdat, 0, 1, 1))
	status = -1;
    rmwindow(&win);

    for (i = strlen(tmp) - 1; i >= 0 && tmp[i] == ' '; i--)
	tmp[i] = 0;			/* kill ' ' */
    strcpy(buf, tmp);
    return (status);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
correct(char *item, char *frm,...)
{
    static struct datast qdat =
    {1, 1, 0, NULL, 0, 'W', 0, NULL};
    char            str[60];
    WIN             wp;
    int             curx, cury, status, w;

    crt_cursor(&curx, &cury);
    vsprintf(str, frm, (va_list) (&frm + 1));

    w = 2 + strlen(str);
    if (w < 20)
	w = 20;
    qdat.w = 0;
    qdat.dat.x = (w - strlen(item)) / 2;
    qdat.dat.buf = item;

    wp = *mkwindow(5, 14, 5 + w, 15, " Question Dialog ");
    wprintxy((w - strlen(str)) / 2, 0, str);
    status = choose_data(&wp, &qdat, 0, 1, 0);
    rmwindow(&wp);

    if(status != -1)
    	status = qdat.w;
    crt_putxy(curx, cury, "");

    return (status);
}

/*
 *
 *
 */

wait_msg(int mode, int x1, int y1, char *text,...)
{
    WIN             wp;
    unsigned int    key;
    char            str[80];
    int             curx, cury, len;

    vsprintf(str, text, (va_list) (&text + 1));
    crt_cursor(&curx, &cury);
    x1 |= 0x1;
    len = (strlen(str) + 1) & 0xfffe;
    for (;;)
    {
	wp = *mkwindow(x1, y1, x1 + len + 1, y1, " Message Dialog ");
	if (NULL == &wp)
	    return (-1);
	selwindow(&wp, crt_YELLOW);
	crt_putxy(x1, y1, str);

	for (;;)
	{
	    switch (key = ex_getch())
	    {
		case 0:
		    continue;
		default:
		    rmwindow(&wp);
		    crt_putxy(curx, cury, "");
		    return (key);
		case IBM_UP:
		case FMR_UP:
		case N98_UP:
		case KSH_UP:
		    if (y1 < 2)
			continue;
		    --y1;
		    break;
		case IBM_DN:
		case FMR_DN:
		case N98_DN:
		case KSH_DN:
		    if (y1 > 20)
			continue;
		    ++y1;
		    break;
		case IBM_LT:
		case FMR_LT:
		case N98_LT:
		case KSH_LT:
		    if (x1 < 2)
			continue;
		    x1 -= 2;
		    break;
		case IBM_RT:
		case FMR_RT:
		case N98_RT:
		case KSH_RT:
		    if (x1 + len + 1 > 75)
			continue;
		    x1 += 2;
		    break;
	    }
	    break;
	}
	rmwindow(&wp);
    }

}


/*
######################################################
#  function     :       search file form rundir      #
#  author       :       ChenHong       90/11/29      #
######################################################
*/
char           *
search_from(char *fpath)
{
    unsigned int far *envp;
    unsigned char far *env;
    extern unsigned int _psp;
    char           *tmp;

    envp = MK_FP(_psp, 0x2c);
    env = MK_FP(*envp, 0L);

    for (; (*env) || (*(env + 1)); env++);
    env += 4;

    tmp = fpath;
    while (*tmp++ = *env++);

/*
    {
    char            fdriver[_MAX_DRIVE], fdir[_MAX_DIR];
    char            ffile[_MAX_FNAME], fext[_MAX_EXT];

	_splitpath(fpath, fdriver, fdir, ffile, fext);
	if((0 != strcmp(ffile,"FE"))||(0 != strcmp(fext,".EXE")))
	{
		cprintf("Owner:��,  Filename:FE.EXE,  rename:�֎~ !!!\n");
		exit(1);
	}
	_makepath(fpath, fdriver, fdir, NULL, NULL);
    }
*/
    *(strrchr(fpath, '\\') + 1) = 0;

    return (fpath);

}

#if (_MSC_VER < 800)

sjis2jis(c)
    int             c;
{
    int             hi, lo;

    lo = c >> 8 & 0xff;
    hi = c & 0xff;
    hi -= (hi <= 0x9f) ? 0x71 : 0xb1;
    hi = hi * 2 + 1;
    if (lo > 0x7f)
	lo -= 1;
    if (lo >= 0x9e)
    {
	lo -= 0x7d;
	hi += 1;
    }
    else
	lo -= 0x1f;
    return (lo << 8 | hi);
}

jis2sjis(c)
    int             c;
{
    int             hi, lo;

    lo = (c >> 8) & 0xff;
    hi = c & 0xff;

    lo += (hi & 1) ? 0x1f : 0x7d;
    if ((0xff & lo) >= 0x7d)
	lo++;
    hi = (hi - 0x21 >> 1) + 0x81;
    if ((0xff & hi) > 0x9f)
	hi += 0x40;
    return (lo << 8 | hi);
}


_fullpath(char *full_name, char *patial_name, int buflen)
{
    union REGS      regs;
    struct SREGS    sregs;

    regs.h.ah = 0x60;
    regs.x.si = FP_OFF(patial_name);
    sregs.ds = FP_SEG(patial_name);
    regs.x.di = FP_OFF(full_name);
    sregs.es = FP_SEG(full_name);
    intdosx(&regs, &regs, &sregs);

    return (regs.x.cflag);
}

char far *_fstrdup(char far *str)
{
char far * tmp;
char far * tmp1;

    tmp = _fmalloc(strlen(str)+1);
    if(tmp != NULL)
    {
    	tmp1 = tmp;
       while(*tmp1++ = *str++);
    }

    return(tmp);
}
#endif
