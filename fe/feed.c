static char    *rcsid = "$Header: FEED.CV  2.3  95/09/13 16:26:04  Hong.Chen  Exp $";
/*
 $Log: RCS/FEED.CV $
 * Revision 2.3  95/09/13 16:26:04  Hong.Chen
 * Edit line OVER 80 to 256 OK
 * 
 * Revision 2.1  95/06/07 22:31:30  Hong.Chen
 * for released
 * 
 * Revision 1.8  95/06/05 02:47:04  
 * help msg.
 * 
 * Revision 1.7  95/04/27 03:30:28  Hong.Chen
 * for MSVC
 * 
 * Revision 1.6  95/01/20 01:06:00  Hong.Chen
 * debug in mem free lost && line buff up to 2000
 * 
 * Revision 1.5  95/01/10 01:05:00  Hong.Chen
 * For Rel
 * 
 * Revision 1.4  94/12/29 16:55:14  Hong.Chen
 * debug in ed quit menu
 * 
 * Revision 1.3  94/12/21 14:05:42  
 * debug for 0xc code in line case
 * 
 * Revision 1.2  94/12/14 13:44:20  Hong.Chen
 * Append: Find , Jump Function
 * 
 * Revision 1.1  94/11/23 01:00:00  Hong.Chen
 * Initial revision
 *
*/

#include 	"fe.h"
#include 	<mbstring.h>

#define	edit_buffer_size	2000
#define	edit_buffer_len		256

struct EDT_ST
{
    int             start;	/* dat_table start ptr r_total-1) */
    int             point;	/* dat_table current ptr 0..(dir_total-1) */
    int             total;	/* data line count ..dir_total	 */
    int             change;	/* data has change 0..1)	 */
    int             mode;	/* ASCII / BINARY mode */
    int             size;	/* memory avlib line 1..n */
    int             attr;	/* file attrbute */
    struct DAT_BUFF
    {
	int             line;
	char           *ascbuf;
    }               dat[edit_buffer_size];
};

int             tab_length = 8;
int             mode = 1;	/* 0:Overwrite, 1:Insert */
int             xptr = 0;	/* 画面カーソル位置(画面幅無視) */
int		xp = 0;		/* position in memory */
int		xmax;
int		xshift = 0;	/* 画面左端のbaffer位置 */
char           *ed_buf;

/*
#################################################
#	function:				#
#	author	:ChenHong	93/05/12	#
#################################################
*/
draw_cur(int x, int y)
{
    switch (mode)		/* 0:Overwrite, 1:Insert */
    {
	case 1:
	    gcur_set(x - xshift, y, 13, 24);
	    break;
	case 0:
	    gcur_set(x - xshift, y, 0, 24);
	    break;
    }
    return(0);
}


do_cg_f(struct EDT_ST * buf)
{
    int             temp;

    cur_set(24, 24);
    temp = buf->point;
    for (buf->point = buf->start;
	 buf->point < buf->start + disp_end - disp_start + 3;
	 buf->point++)
    {
	show_aline(buf);
    }
    buf->point = temp;
    crt_attr(crt_REVERSE | crt_CYAN);		/* reverce && color */
    crt_putxy(75, 0, "[Ins]");
    crt_printxy(2, disp_end + 1, "%76s", " ");	/* 80->76 at 94/10/01 */
    crt_attr(crt_REVERSE | crt_WHITE);		/* reverce && color */
    crt_putxy(4, disp_end + 1, "1exit ");	/* F1 */
    crt_putxy(11, disp_end + 1, "2opt. ");	/* F2 */
    crt_putxy(18, disp_end + 1, "3print");	/* F3 */

    crt_putxy(33, disp_end + 1, "5find ");	/* F5 */
    crt_putxy(40, disp_end + 1, "6copy ");	/* F6 */
    crt_putxy(47, disp_end + 1, "7cut  ");	/* F7 */

    crt_putxy(62, disp_end + 1, "9paste");	/* F9 */
    crt_putxy(69, disp_end + 1, "10jump ");	/* F10 */
    crt_attr(crt_NOREVERSE);
    mk_cur_posi(buf);
    draw_cur(xptr, buf->point - buf->start + disp_start - 2);
    return(0);
}

do_myedit(char *filename)
{
    struct EDT_ST   edt_buf;
    struct EDT_ST  *buf;
    FILE           *fp;
    int             i, status;
    char            tmp[256], *tp;

    mode = 1;			/* 0:Overwrite, 1:Insert */
    xptr = 0;
    xp = 0;
    if (NULL == (fp = fopen(filename, "r")))
    {
	wait_msg(0, 5, 16, "Error: Cannot open file :%s", filename);
	return (-1);
    }

    buf = &edt_buf;
    status = 0;
    for (i = 0; fgets(tmp, 255, fp); i++)
    {
	if (i >= edit_buffer_size)
	{
	    status = -1;
	    break;
	}
	tp = buf->dat[i].ascbuf = strdup(tmp);

#ifdef LSI_C
	if (NULL == tp)
#else
	if (_memavl() < (size_t) 100)
#endif
	{
	    status = -2;
	    break;
	}
    }

    fclose(fp);

    if (!status)
    {
    	buf->dat[i].ascbuf = "=================== End of File ===================\r\n";

	buf->start = 0;
	buf->total = i;
	buf->point = 0;
	crt_attr(crt_REVERSE | crt_CYAN);	/* reverce && color */
	crt_printxy(0, 0, "%s", filename);
	crt_attr(crt_NOREVERSE);

	ed_buf = NULL;

	do_cg_f(buf);
	do
	    status = edit_proc(buf, filename);
	while (status == 2);

	if (1 == status)
	{
	    sscanf(filename, "%[^.]", tmp);
	    strcat(tmp, ".bak");
	    remove(tmp);
	    rename(filename, tmp);
	    fp = fopen(filename, "w");
	    for (i = 0; i < buf->total; i++)
		fputs(buf->dat[i].ascbuf, fp);
	    fclose(fp);
	}
	i = buf->total;
    }

    for (i; i >= 0; i--)
	free(buf->dat[i].ascbuf);

    if(status == -2)    wait_msg(0, 5, 16, "Edit: File too big(avail %u byte) !",_memavl());
    if(status == -1)    wait_msg(0, 5, 16, "Edit: File too big(avail %d line) !",edit_buffer_size);

    return (0);
}



insert_menber(struct EDT_ST * buf, int posi, char *ptr)
{
    memmove(buf->dat + posi + 1, buf->dat + posi,
	    sizeof(struct DAT_BUFF) * (edit_buffer_size - 1 - posi));
    buf->dat[posi].ascbuf = strdup(ptr);
    ++buf->total;
    return(0);
}

delete_menber(struct EDT_ST * buf, int posi)
{
    free(buf->dat[posi].ascbuf);
    memmove(buf->dat + posi, buf->dat + posi + 1,
	    sizeof(struct DAT_BUFF) * (edit_buffer_size - posi));
    --buf->total;
    return(0);
}

mk_cur_posi(struct EDT_ST * buf)
{
    unsigned int    dsp_wide;
    int             x, x1, xshiftm;
    char           *c;
    static int      xpm, xpo;

    if (buf->total <= 0)
    {
	xp = xpm = xpo = xptr = xmax = 0;
	return(0);
    }

    if (xp != xpm)
	xpm = xpo = xp;
    else
	xp = xpm = xpo;

    c = buf->dat[buf->point].ascbuf;
    for (x = 0, x1 = 0; x <= edit_buffer_len; x++, x1++, c++)
    {
	if (xp == x)
	{
	    xptr = x1;
/*	    crt_attr(crt_REVERSE | crt_WHITE);	/* reverce && color */
	    crt_attr(crt_REVERSE | crt_CYAN);	/* reverce && color */
#ifdef _DEBUG
	    crt_printxy(25, disp_end + 1, "S%05u M%05u ", stackavail(), _memavl());
#else
	    crt_printxy(25, disp_end + 1, "%02XH,%02XH", 0xff&(*c), 0xff&(*(c + 1)));	/* F4 */
#endif
	    crt_attr(crt_NOREVERSE);
	}
	switch (*c)
	{
	    default:
		continue;
	    case 9:		/* TAB code */
		while (x1 % tab_length != (tab_length - 1))
		    x1++;
		continue;
	    case 0xd:
	    case 0xa:
/*	    case 0xc:	*/
	    case 0:
		xmax = x;
		if (xp > xmax)
		{
		    xp = xpm = xmax;
		    xptr = x1;
		}
		break;
	}
	break;
    }

    dsp_wide = (x > 80) ? 76 : 80;
    xshiftm = xshift;
    xshift = 0;
    if((dsp_wide == 76) && (xptr >76)) 
    {
	xshift = ((xptr - 76) / 8 + 1) * 8;
    }
    if(xshift != xshiftm)
    	show_aline(buf);

    crt_attr(crt_REVERSE | crt_CYAN);	/* reverce && color */
    crt_printxy(54, disp_end + 1, "%03d/%03d", buf->point + 1, xp);	/* F8 */
    crt_attr(crt_NOREVERSE);
    return(0);
}


show_aline(struct EDT_ST * buf)
{
    int             y, tab;
    unsigned int    dsp_wide;
    char            tmp[82], *str;

    y = buf->point - buf->start + disp_start - 2;
    tab = tab_length;

    if (buf->point > buf->total)
	str = "~\0";
    else
	str = convert_string(buf->dat[buf->point].ascbuf,
			     edit_buffer_len, &tab);

    dsp_wide = (strlen(str) > 80) ? 76 : 80;
    xshift = 0;
    if((dsp_wide == 76) && (xptr >76)) 
	xshift = ((xptr - 76) / 8 + 1) * 8;

/*  sprintf(tmp, "%-*s\0", edit_buffer_len, str);	*/
    sprintf(tmp, "%-80.80s\0", &str[xshift]);

    bputxy(0, y, tmp);

    if(xshift > 0)
    {
    	crt_attr(crt_REVERSE | crt_CYAN);	/* reverse && color */
    	crt_putxy(0, y, "<<");
    	crt_attr(crt_NOREVERSE);
    }

    if(strlen(str) > dsp_wide + xshift)
    {
    	crt_attr(crt_REVERSE | crt_CYAN);	/* reverse && color */
    	crt_putxy(78, y, ">>");
    	crt_attr(crt_NOREVERSE);
    }

    return (0);
}


ins_rpl_mode()
{
    mode = 1 - mode;		/* 0:Overwrite, 1:Insert */
    crt_attr(crt_REVERSE | crt_CYAN);	/* reverce && color */
    switch (mode)
    {
	case 1:
	    crt_printxy(75, 0, "[Ins]");
	    break;
	case 0:
	    crt_printxy(75, 0, "[Rep]");
	    break;
    }
    crt_attr(crt_NOREVERSE);
    return(0);
}

jump_fun(struct EDT_ST * buf)
{
    int             ret = -1;
    static struct menust menu[] =
    {
	{0, 0, 1, "Top    "},
	{1, 1, 1, "Line   "},
	{2, 2, 1, "Bottom "},
    };
    static char     str[11];
    WIN             win;

    win = *mkwindow(20, disp_end - 10, 28, disp_end - 8, " Jump ");
    switch (choose_menu(&win, menu, 0, 3, 0))
    {
	case 1:
	    if (-1 != question(str, 10, NULL, "Jump To :"))
	    {
		ret = atoi(str);
		break;
	    }
	    /* else */
	case -1:
	    ret = -1;
	    break;
	case 0:
	    ret = 1;
	    break;
	case 2:
	    ret = buf->total;
	    break;
    }

    rmwindow(&win);
/*  draw_cur(xptr, yptr);	*/
    buf->point = max(ret - 1, 0);
    buf->start = buf->point - 10;
    if (buf->start < 0)
	buf->start = 0;

    return (ret);
}

edit_proc(struct EDT_ST * buf, char *fname)
{
    static int      cg_f = 0;
    static char     find_str[31];
    unsigned int    k1;
    int             i, yptr;

    yptr = buf->point - buf->start + disp_start - 2;
    switch (k1 = ex_getch())
    {
	case IBM_F1:
	case N98_F1:
	    switch (correct("Yes No Continue", "Save File, Sure ? "))
	    {
		case 0:
		    return (1);	/* Save */
		case -1:
		case 2:
		    return (2);	/* Continue */
		case 1:	/* Quit */
		    break;
	    }
	case ASC_ESC:
	    if (ed_buf != NULL)
		free(ed_buf);
	    return (0);		/* Quit */
	    break;
	case IBM_F2:
	case N98_F2:
	    sys_set();
	    cg_f = 1;
	    break;
	case IBM_F3:
	case N98_F3:
	    do_ppc(fname);
	    cg_f = 1;
	    break;
	case IBM_F9:		/* pest */
	case N98_F9:
	    if (ed_buf == NULL)
		break;
	    insert_menber(buf, buf->point, ed_buf);
	    cg_f = 1;
	    goto do_down;
	    break;
	case IBM_F6:		/* copy */
	case N98_F6:
	    if (ed_buf != NULL)
		free(ed_buf);
	    ed_buf = strdup(buf->dat[buf->point].ascbuf);
	    break;
	case IBM_F7:		/* cut */
	case N98_F7:
	    draw_cur(xptr, yptr);
	    if (ed_buf != NULL)
		free(ed_buf);
	    ed_buf = strdup(buf->dat[buf->point].ascbuf);
	    delete_menber(buf, buf->point);
	    if (buf->total == 0)
		insert_menber(buf, buf->point, "");
	    if (buf->point > buf->total - 1)
		--buf->point;
	    cg_f = 1;
	    break;
	case IBM_F5:
	case N98_F5:
	    if (-1 != question(find_str, 30, find_str, "Find String:"))
	    {
	    	for(i = buf->point + 1; i <= buf->total; i++)
		{
		    char           *new_ptr;

		    if(new_ptr = strstr(buf->dat[i].ascbuf, find_str))
		    {
		/*      draw_cur(xptr, yptr);	*/
		    	xp = (int)(new_ptr - buf->dat[i].ascbuf);
			buf->point = i;
			buf->start = buf->point - 10;
			if (buf->start < 0)
			    buf->start = 0;
		    	cg_f = 1;
			break;
		    }
		}
	    }
	    break;
	case IBM_F10:		/* Jump */
	case N98_F10:
	    if (-1 != (i = jump_fun(buf)))
	    {
		cg_f = 1;
	    }
	    break;
	case 0:
	    if (cg_f)
	    {
		do_cg_f(buf);
		cg_f = 0;
		break;
	    }
	    else
	    {
	    }
	    return (2);		/* Continue */
	case IBM_HOME:
	case FMR_HOME:
	case N98_HOME:
	    draw_cur(xptr, yptr);
	    xp = 0;
	    break;
	case IBM_END:
	case FMR_HELP:
	case KSH_HELP:
	    draw_cur(xptr, yptr);
	    xp = xmax;
	    break;
	case ASC_BS:
	case IBM_LT:
	case FMR_LT:
/*	case N98_LT:	same as ASC_BS */
	case KSH_LT:
	    draw_cur(xptr, yptr);
	    if (xp > 0)
	    {
		xp--;
		break;
	    }
	    if (buf->point > buf->start)
		xp = strlen(buf->dat[buf->point - 1].ascbuf) - 1;
	    goto do_up;
	case IBM_UP:
	case FMR_UP:
	case N98_UP:
	case KSH_UP:
	    draw_cur(xptr, yptr);
	  do_up:
	    if(xshift)
	    {
	    	int	tmp;
	    	tmp = xptr;
	    	xptr = 0;
	    	show_aline(buf);
	    	xptr = tmp;
	    }
	    if (buf->point > buf->start)
		--buf->point;	/* not scroll */
	    else
	    {
		if (buf->start > 0)
		{
		    --buf->point;
		    --buf->start;
		    if (-1 == gmovetext(0, disp_start - 2, 79, disp_end - 1,
					0, disp_start - 2 + 1))
			cg_f++;
		    show_aline(buf);
		}
	    }
	    break;
	default:
/*	    if (!isprint(k1))	return (2);	/* Continue */
	    if (mode || (xp >= xmax))	/* 0:Overwrite, 1:Insert */
	    {			        /* ~~~~~~~~ */
		char	tmp[256];
		
		sprintf(tmp, "%.*s%c%s", xp,
			buf->dat[buf->point].ascbuf, k1,
			buf->dat[buf->point].ascbuf + xp);
		free(buf->dat[buf->point].ascbuf);
		buf->dat[buf->point].ascbuf = strdup(tmp);
		xp++;
		show_aline(buf);
		break;
	    }
	    *(buf->dat[buf->point].ascbuf + xp) = k1;
	    show_aline(buf);
	case IBM_RT:
	case FMR_RT:
	case N98_RT:
	case KSH_RT:
	    draw_cur(xptr, yptr);
	    if (xp < xmax)
	    {
		xp++;
		break;
	    }
	    if (buf->point < buf->total - 1)
		xp = 0;
	    goto do_down;
	    break;
	case ASC_CR:
	    draw_cur(xptr, yptr);
	    if (ed_buf != NULL)
		free(ed_buf);
	    ed_buf = strdup(buf->dat[buf->point].ascbuf + xp);
	    strcpy(buf->dat[buf->point].ascbuf + xp, "\n");
	    xp = 0;
	    insert_menber(buf, buf->point + 1, ed_buf);
	    cg_f = 1;
	    goto do_down;
	    break;
	case IBM_DN:
	case FMR_DN:
	case N98_DN:
	case KSH_DN:
	    draw_cur(xptr, yptr);
	  do_down:
	    if(xshift)
	    {
	    	int	tmp;
	    	tmp = xptr;
	    	xptr = 0;
	    	show_aline(buf);
	    	xptr = tmp;
	    }
	    if (buf->point < min(buf->total - 1,
				 buf->start + disp_end - disp_start + 2))
		++buf->point;	/* not scroll */
	    else
	    {
		if (buf->total > (buf->start + disp_end - disp_start + 2 + 1))
		{
		    ++buf->point;
		    ++buf->start;
		    if (-1 == gmovetext(0, disp_start - 2 + 1, 79, disp_end,
					0, disp_start - 2))
			cg_f++;
		    show_aline(buf);
		}
	    }
	    break;
	case IBM_RUP:
	case KSH_RUP:
	    for (i = 0; i < disp_end; i++)
	    {
		if (buf->start > 0)
		{
		    --buf->start;
		    cg_f++;
		}
		if (buf->point > min(buf->total - 1, buf->start + disp_end - 1))
		    --buf->point;
	    }
	    break;
	case IBM_RDN:
	case KSH_RDN:
	    for (i = 0; i < disp_end; i++)
	    {
		if (buf->total > (buf->start + disp_end))
		{
		    ++buf->start;
		    ++cg_f;
		}
		if (buf->point < min(buf->total - 1, buf->start))
		    ++buf->point;
	    }
	    break;
	case IBM_F8:
/*	case N98_F8:	*/
	case IBM_INS:
	case FMR_INS:
/*	case N98_INS:		/* 	same as FMR_INS */
	case KSH_INS:
	    draw_cur(xptr, yptr);
	    ins_rpl_mode();
	    break;
	case IBM_F4:
/*	case N98_F4:	*/
	case IBM_DEL:
	case FMR_DEL:
	case N98_DEL:
/*	case KSH_DEL:		/* same as N98_del */
	    k1 = IBM_DEL;
	    break;
    }

#ifndef NOT_USE_BS
    if (k1 == IBM_DEL || k1 == ASC_BS)
#else
    if (k1 == IBM_DEL)
#endif
    {
	if (xp >= (int)(strlen(buf->dat[buf->point].ascbuf) - 1))
	{
	    if (buf->point + 1 < buf->total)
	    {
	    	char	tmp[256];
		
		sprintf(tmp, "%.*s%s",
			strlen(buf->dat[buf->point].ascbuf) - 1,
			buf->dat[buf->point].ascbuf,
			buf->dat[buf->point + 1].ascbuf);
		free(buf->dat[buf->point + 1].ascbuf);
		buf->dat[buf->point + 1].ascbuf = strdup(tmp);
		delete_menber(buf, buf->point);
		cg_f++;
	    }
	}
	else
	{
	    strcpy(buf->dat[buf->point].ascbuf + xp,
		   buf->dat[buf->point].ascbuf + (xp + 1));
	    show_aline(buf);
	}
    }

    mk_cur_posi(buf);
    draw_cur(xptr, buf->point - buf->start + disp_start - 2);

    return (2);			/* Continue */
}


/*
#################################################
#	Function:Convert A String For Display	#
#	Paramete:c,   Source String		#
#		 cnt, Max String len		#
#		 tab, TAB length		#
#	Return  :RET, Return String 		#
#		 tab, Return String Len		#
#	Author	:ChenHong	94/04/12	#
#################################################
*/
char           *
convert_string(char *c, int cnt, int *tab)
{
    static char     tmp[edit_buffer_len + 2];
    int             dx, c2, nr;

    cnt = min(edit_buffer_len, cnt);

    for (dx = 0, nr = 0;
	 dx < cnt;
	 dx++, nr++, c++)
    {
	if (*c == 9)
	{
	    /* TAB code */
	    for (; ((dx + 1) % *tab != 0) && ((dx + 1) < cnt); dx++)
		tmp[dx] = ' ';
	    tmp[dx] = ' ';
	    continue;
	}
	if (*c == 0xd)
	{
	    tmp[dx] = ' ';
	    continue;
	}
	if ((*c == 0xa) || (*c == 0x1a) || (*c == 0))
	{
	    tmp[dx] = ' ';
	    nr++;
	    break;
	}
	if (kjcode == 0)
	{
	    /* ASCII code  */
	    if (isprint(0x00ff & *c))
		tmp[dx] = *c;
	    else
		tmp[dx] = '.';
	    continue;
	}
	if (kjcode == 1)
	{
	    /* sjis code */
	    if (isprkana(0x00ff & *c))
	    {
		tmp[dx] = *c;
		continue;
	    }
	    else
	    {
		if (iskanji(*c))
		{
		    if (dx + 2 > cnt)
		    {
			tmp[dx++] = '~';
			break;
		    }
		    if (iskanji2(c2 = *(c + 1)))
		    {
			tmp[dx++] = *c++;
			nr++;
			tmp[dx] = c2;
			if (machine() == AT_CN)	/* CHINA/GB code */
			{
			    unsigned int   *k;
			    unsigned int    tk;

			    k = (unsigned int *) (tmp + dx - 1);
			    tk = *k;
			    tk = _mbcjmstojis((tk >> 8) | (tk << 8));
			    *k = ((tk >> 8) | (tk << 8)) + 0x8080;

			    k = (unsigned int *) (tmp + dx - 1);
			    *k = _mbcjmstojis(*k) + 0x8080;
			}
			continue;
		    }
		}
	    }
	}
	if (kjcode == 2)
	{
	    /* gb code */

	    if (isprint(0x00ff & (*c)))
	    {
		tmp[dx] = *c;
		continue;
	    }
	    else
	    {
		if (ishanzi(*c))
		{
		    if (dx + 2 > cnt)
		    {
			tmp[dx++] = '~';
			break;
		    }
		    if (ishanzi((c2 = *(c + 1))))
		    {
			tmp[dx++] = *c++;
			nr++;
			tmp[dx] = c2;
			if (machine() == AT_JP)	/* JAPAN/SJ code */
			{
			    unsigned int   *k;
			    unsigned int    tk;

			    k = (unsigned int *) (tmp + dx - 1);
			    tk = *k - 0x8080;
			    tk = _mbcjistojms((tk >> 8) | (tk << 8));
			    *k = (tk >> 8) | (tk << 8);
			}
			continue;
		    }
		}
	    }
	}
	tmp[dx] = '.';
	continue;
    }
    tmp[dx] = 0;
    *tab = nr;
    return (tmp);
}
