static char    *rcsid = "$Header: FECFG.CV  2.3  95/09/13 16:27:12  Hong.Chen  Exp $";
/*
 $Log: RCS/FECFG.CV $
 * Revision 2.3  95/09/13 16:27:12  Hong.Chen
 * Header
 * 
 * Revision 2.1  95/06/07 22:30:48  Hong.Chen
 * for released
 * 
 * Revision 1.8  95/06/02 01:27:22  
 * order, etc.
 * 
 * Revision 1.7  95/04/26 23:50:40  Hong.Chen
 * for MSVC
 *
 * Revision 1.6  95/01/20 01:06:00  Hong.Chen
 * debug for kjcode, etc.
 *
 * Revision 1.5  95/01/10 01:05:00  Hong.Chen
 * Can set System Opt. by env
 *
 * Revision 1.4  95/01/02 14:16:02  Hong.Chen
 * Copyright(C) to 1995
 *
 * Revision 1.3  94/12/28 15:17:12
 * CMD_CNT->15 and USE in NO FE.CFG
 *
 * Revision 1.2  94/11/28 17:36:24  Hong.Chen
 * Title changed
 *
 * Revision 1.1  94/11/23 01:00:00  Hong.Chen
 * Initial revision
 *
*/

#include	"fe.h"

static int      curx, cury;
static WIN      fullscr;
static char     ver_str[] = " Filer&Editor Ver 2.03  Copyright(C) 1989-1995, Hong.Chen ";
char           *mach[] =
{"PC9801", " FM/R ", "AT(US)", "AT(JP)", "AT(CN)", "MS-DOS"};
char           *kjc[] =
{"ASC", "JIS", "GBC"};

struct SYS_CMD
{
    char           *cmd;
    char           *buff;
};
#define		CMD_CNT		15
static struct SYS_CMD usr_cmd[CMD_CNT];
static struct SYS_CMD exe_cmd[CMD_CNT];
static struct SYS_CMD sys_cmd[CMD_CNT];

extern int      machine_flag;		/* 0,pc9801 1,fmr50 2.3.4,ibm-pc/at */
int             disp_end = 22;		/* disp line-2 */
int             kjcode = -1;		/* file code, -1:noset 0:ASCII ,
					 * 1:SJ, 2:GB */
int             order = 1;

set_env_buff(char *tmp, struct SYS_CMD * cmd)
{
    char            cmd0[40], buf0[80];

    if (1 == sscanf(tmp, "%[^ =]%*[ =]%[^\n]", cmd0, buf0))
	buf0[0] = 0;
    cmd->cmd = strdup(cmd0);
    cmd->buff = strdup(buf0);
    return (0);
}

char           *
get_env_buff(char *tmp, struct SYS_CMD * cmd)
{
    int             i;
    static char    *buf;
    char            tmp1[20];

    strcpy(tmp1, tmp);
    /* ä¬ã´ïœêîëÂï∂éöÇÃÇ›Ç…ëŒâûÅAtmpÇëÂï∂éöÇ… */
    if (NULL != (buf = getenv(strupr(tmp1))))
	return (buf);

    for (i = 0; (i < CMD_CNT) && (cmd[i].cmd != NULL); i++)
	if (0 == strcmp(tmp, cmd[i].cmd))
	{
	    if (cmd[i].buff[0] != 0)
		return (cmd[i].buff);
	    else
		return (NULL);
	}

    wait_msg(0, 5, 16, "Error:No define command(%s) !", tmp);
    return (NULL);
}

read_env()
{
    FILE           *fp;
    char            fpath[_MAX_PATH];
    int             usrp, exep, sysp;
    char            tmp[80];

    sysp = 0;
    exep = 0;
    usrp = 0;

    search_from(fpath);
    strcat(fpath, "fe.cfg");

    if (NULL == (fp = fopen(fpath, "r")))
    {
	/*
	 * cprintf("Can't find (%s)", fpath); exit(0);
	 */
    }
    else
    {
	while (fgets(tmp, 79, fp))
	{
	    switch (tmp[0])
	    {
		case '/':
		    break;
		case 'U':
		    set_env_buff(&tmp[1], &usr_cmd[usrp++]);
		    break;
		case 'X':
		    set_env_buff(&tmp[1], &exe_cmd[exep++]);
		    break;
		case 'V':
		case 'E':
		default:
		    set_env_buff(&tmp[0], &sys_cmd[sysp++]);
		    break;
	    }
	}
	fclose(fp);

	{
	    char           *buf;

	    if (NULL != (buf = get_env_buff("DirOrder", sys_cmd)))
		order = atoi(buf);
	    if (NULL != (buf = get_env_buff("Machine", sys_cmd)))
		machine_flag = atoi(buf);
	    if (NULL != (buf = get_env_buff("Kjcode", sys_cmd)))
		kjcode = atoi(buf);
	    if (NULL != (buf = get_env_buff("DispLine", sys_cmd)))
		disp_end = atoi(buf) - 2;
	}
    }
    return (0);
}


gcur_set(int x0, int y0, int sy, int ey)
{
    cur_set(sy, ey);
    crt_putxy(x0, y0, "");
    return (0);
}

gmovetext(int left, int top, int right, int bottom, int destleft, int desttop)
{
    return movetext(left, top, right, bottom, destleft, desttop);
}

void
bputxy(int x, int y, char *str)
{
    crt_putxy(x, y, str);
}

do_edit(char *choose_name)
{
    do_myedit(choose_name);
    cur_set(24, 24);
    selwindow(&fullscr, crt_CYAN);
    wclrscr();
    box(0, disp_start - 1, 79, disp_end + 1, crt_CYAN);	/* under flame */
    selwindow(&fullscr, crt_CYAN);
    return (0);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
comp_file(const struct DIR_BUF * buf1, const struct DIR_BUF * buf2)
{
    switch (order)
    {
	case 1:
	    return (strcmp(buf1->packed_name, buf2->packed_name));
	    break;
	case 2:
	    return (strcmp(jstrchr(buf1->packed_name, '.'),
			   jstrchr(buf2->packed_name, '.')));
	    break;
	case 3:
	    if (((unsigned long) buf1->fpath) >
		((unsigned long) buf2->fpath))
		return 1;
	    else if (((unsigned long) buf1->fpath) <
		     ((unsigned long) buf2->fpath))
		return -1;
	    else
		return 0;
	    break;
	case 4:
	    if ((((unsigned long) buf1->wr_date << 16) + buf1->wr_time) >
		(((unsigned long) buf2->wr_date << 16) + buf2->wr_time))
		return -1;
	    else if ((((unsigned long) buf1->wr_date << 16) + buf1->wr_time) <
		     (((unsigned long) buf2->wr_date << 16) + buf2->wr_time))
		return 1;
	    else
		return 0;
	    break;
    }
}
make_file_buff(struct DIR_ST * buf)
{
    struct find_t   buff;
    struct DIR_BUF *dir_buf;

    buf->attr = _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH;
    buf->total = 0;
    buf->size = 0;

    if (0 == _dos_findfirst(group, buf->attr, &buff))
    {
	do
	{
	    dir_buf = buf->dat + buf->total;
	    dir_buf->attrib = buff.attrib;
	    dir_buf->wr_time = buff.wr_time;
	    dir_buf->wr_date = buff.wr_date;
	    (long) dir_buf->fpath = buff.size;
	    strcpy(dir_buf->packed_name, buff.name);
	    buf->size += buff.size;
	    ++(buf->total);
	}
	while ((0 == _dos_findnext(&buff)) && (buf->total < buffer_avail));
    }

    if (order)
	qsort(buf->dat, buf->total, sizeof(struct DIR_BUF), comp_file);

    return (buf->total);
}


sys_set()
{
    static struct datast sys_menu[] =
    {
	{0, 0, 1, "Dir. Order  :", 1, 'W', 17, "Org Name Ext Size Date"},
	{1, 1, 1, "Display line:", 2, 'D', 17, "  "},
	{2, 2, 1, "Kanji code  :", 1, 'W', 17, "ASCII SJIS EUC(GB)"},
    };
    WIN             win;
    int             ln;

    sys_menu[0].w = order;
    sprintf(sys_menu[1].dat.buf, "%2.2d", disp_end + 2);
    sys_menu[2].w = kjcode;

    win = *mkwindow(20, disp_end - 13, 60, disp_end - 11, " System Option ");

    if (-1 != choose_data(&win, sys_menu, 0, 3, 0))
    {
	kjcode = sys_menu[2].w;
	order = sys_menu[0].w;

	ln = atoi(sys_menu[1].dat.buf) - 2;
	if (ln != disp_end)
	{
	    exit_screen();
	    disp_end = ln;
	    init_screen();
	}
    }

    rmwindow(&win);
    return (0);
}


read_para(int argc, char *argv[])
{
    int             i, j;
    void            pc98_putfkey(int code, char *key_str);

    switch (machine())
    {
	case PC_98:
	    cputs("\33[>1h");			/* Erase function keys */
/*	    pc98_putfkey(0x19, "\x1b^");	/*	Å™	*/
	    pc98_putfkey(0x1a, "\x1b<");	/* Å©	 */
/*	    pc98_putfkey(0x1b, "\x1b>");	/*	Å®	*/
/*	    pc98_putfkey(0x1c, "\x1b_");	/*	Å´	*/
	    pc98_putfkey(0x15, "\x1b{");	/* ROLL UP	 */
	    pc98_putfkey(0x16, "\x1b}");	/* ROLL DOWN	 */
	    pc98_putfkey(0x1e, "\x1b?");	/* HELP		 */
	    disp_end = (*((char far *) 0x00600112 + 0L)) - 2;
	    break;
	case FMR_50:			/* fmr */
	    break;
	case AT_US:			/* ibm */
	case AT_JP:			/* ibm */
	case AT_CN:			/* ibm */
	    disp_end = (*((char far *) 0x00400084 + 0L)) - 1;
	    break;
    }

    read_env();

    for (i = 1; i < argc;)
    {
	if (argv[i][0] == '-' || argv[i][0] == '/')
	{
	    for (j = 1; argv[i][j] != 0; j++)
	    {
		switch (toupper(argv[i][j]))
		{
		    case '?':
			puts("\n");
			puts(ver_str);
			cprintf(" Status : DOS V%d.%02d %s %s(%2d)\n\r",
				_osmajor, _osminor, mach[machine()], kjc[kjcode], disp_end + 2);
			puts(" Syntax : fe [options] [Disk Name]");
			puts("");
			puts("     -? Print this message");
			puts("     -M[0..4]   Machine [98|FMR|AT.US|AT.JP|AT.CN]");
			puts("     -K[0..2]   Kjcode  [AC|SJ|GB]");
			puts("     -L[25..99] Display Line Number");
			exit(0);
		    case 'K':
			kjcode = argv[i][j + 1] - '0';
			j++;
			break;
		    case 'M':
			machine_flag = argv[i][j + 1] - '0';
			j++;
			break;
		    case 'L':
			disp_end = atoi(argv[i] + j + 1) - 3;
			j += 2;
			break;
		    default:
			break;
		}
	    }
	    for (j = i + 1; j < argc; j++)
		argv[j - 1] = argv[j];
	    argc--;
	}
	else
	    i++;
    }

    if (kjcode == -1)
	kjcode = (machine_flag < 2) ? 1 : (machine_flag - AT_US);
    return (0);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/27	#
#################################################
*/
do_environ(char *cmd, char *fn1, char *fn2)
{
    char           *buf;

    if (NULL != (buf = get_env_buff(cmd, sys_cmd)))
    {
	run_system(0, buf, fn1, fn2, "", "");
	return (0);
    }
    else
	return (-1);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	93/05/27	#
#################################################
*/
do_execute(char *fn1)
{
    char            cmd[4];
    int             i, j;
    char            message[51];

    sscanf(fn1, "%*[^.]%*[.]%s", cmd);	/* get extension name */
    if (cmd)
	for (i = 0; (i < CMD_CNT) && (exe_cmd[i].cmd != NULL); i++)
	{
	    for (j = 0; j < 3; j++)
		if (exe_cmd[i].cmd[j] == '?')
		    cmd[j] = '?';
	    if (0 == strcmp(cmd, exe_cmd[i].cmd))
	    {
		run_system(1, exe_cmd[i].buff, fn1, fn1);
		return (0);
	    }
	}

    if (-1 == question(message, 51, fn1, ">"))
	return (-1);
    run_system(1, "%s", message);
    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	90/11/29	#
#################################################
*/
init_screen()
{
    crt_cursor(&curx, &cury);
    fullscr = *mkwindow(1, 1, 78, disp_end, ver_str);

    cur_set(24, 24);

    box(0, disp_start - 1, 79, disp_end + 1, crt_CYAN);	/* under flame */
/*  box( 0,disp_start-1,56,disp_end+1,crt_CYAN);	/* left flame */
/*  box(56,disp_start-1,79,disp_end+1,crt_CYAN);	/* right flame */
    selwindow(&fullscr, crt_CYAN);
    return (0);
}

/**/
/*
#################################################
#	function:				#
#	author	:ChenHong	88/11/19	#
#################################################
*/
void
c_exit(int sig)
{
    exit_screen();
    exit(0);
}

/**/
/*
#################################################
#	function:				#
#	author	:ChenHong	88/11/19	#
#################################################
*/
exit_screen()
{
    cur_set(22, 24);
    crt_attr(crt_NOREVERSE);
    rmwindow(&fullscr);
    crt_putxy(curx, cury, "");
    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	93/05/24	#
#################################################
*/
do_hist(int idx, char *choose_name)
{
    struct menust   hist_stru[CMD_CNT];
    WIN             win;
    int             i;
    static int      ret = 0;
    struct SYS_CMD *cmd;
    char           *ptr, buf[80];

    switch (idx)
    {
	case 0:
	    cmd = usr_cmd;
	    break;
	case 1:
	    cmd = exe_cmd;
	    break;
    }

    for (i = 0; (i < CMD_CNT) && (cmd[i].buff); i++)
    {
	hist_stru[i].ret = i;
	hist_stru[i].y = i;
	hist_stru[i].msg.x = 2;
	hist_stru[i].msg.buf = cmd[i].buff;
    }

    if (i)
    {
	win = *mkwindow(15, disp_end - 17, 65,
			disp_end - 17 + (i - 1), " USER DEFINED COMMAND ");
	ret = choose_menu(&win, hist_stru, ret, i, 0);
	rmwindow(&win);

	if (-1 == ret)
	    return (-1);

	strcpy(buf, cmd[ret].buff);
	if (ptr = strchr(buf, ';'))
	    *ptr = 0;
	run_system(atoi(cmd[ret].cmd), buf, choose_name);
    }
    return (0);
}


/*
#################################################
#	function:				#
#################################################
*/
run_system(int wait, char *frm,...)
{
    char            str[256];

    vsprintf(str, frm, (va_list) (&frm + 1));

    exit_screen();
    if (system(str) == -1)
	wait_msg(0, 5, 16, "Error: Shell failed !");
    if (wait)
	wait_msg(0, 5, 16, "Press a key...");
    KEYCLR;
    init_screen();
    return (0);
}


/*   */
/*
#################################################
#	function:do_ppc				#
#	author	:ChenHong	90/03/12	#
#################################################
*/
do_ppc(char *filename)
{
    static struct datast ppc_menu[] =
    {
	{1, 0, 1, "Lines per page :", 2, 'D', 18, "66"},
	{2, 1, 1, "Words per line :", 3, 'D', 18, "128"},
	{3, 2, 1, "Hard_TAB       :", 2, 'D', 18, "8 "},
	{4, 3, 1, "Left_Margin    :", 2, 'D', 18, "0 "},
	{5, 4, 1, "Output_device  :", 12, 'T', 18, "PRN          "},
	{6, 5, 1, "Line_number    :", 1, 'W', 18, "No Yes"},
    };
    char            out_device[20];
    struct PPC_ST   s;
    WIN             win, win_menu;

    win_menu = *mkwindow(25, disp_end - 15, 60,
			 disp_end - 10, " Text File Print ");
    if (-1 == choose_data(&win_menu, ppc_menu, 0, 6, 1))
    {
	rmwindow(&win_menu);
	return (-1);
    }

    s.line_perpage = atoi(ppc_menu[0].dat.buf);
    s.word_perline = atoi(ppc_menu[1].dat.buf);
    s.tab_length = atoi(ppc_menu[2].dat.buf);
    s.left_margin = atoi(ppc_menu[3].dat.buf);
    sscanf(ppc_menu[4].dat.buf, "%s", out_device);	/* space[' '] kill */
    s.line_number = 10 * ppc_menu[5].w;	/* No Yes */

    if (NULL == (s.fp0 = fopen(filename, "r")))
    {
	wait_msg(0, 5, 16, "Error: Cannot open file :%s", filename);
	rmwindow(&win_menu);
	return (1);
    }
    if (NULL == (s.fp1 = fopen(out_device, "w")))
    {
	wait_msg(0, 5, 16, "Error: Cannot open file :%s", out_device);
	rmwindow(&win_menu);
	return (1);
    }

    win = *mkwindow(5, disp_end - 10, 74, disp_end - 10, " P.P.C ");
    selwindow(&win, crt_YELLOW);
    wprintf("P.P.C %s;  <ESC> to quit. ", filename);
    ppc(filename, &s);
    rmwindow(&win);

    rmwindow(&win_menu);

    fclose(s.fp0);
    fclose(s.fp1);
    return (0);
}

/*  */
/*
#################################################
#	function:main				#
#	author	:ChenHong	90/03/12	#
#################################################
*/
ppc(char *filename, struct PPC_ST * s)
{
    struct tm      *newtime;
    long            ltime;
    int             line_count, page_count, word_count, margin_count, avil_count;
    int             line_of_page, accept_dat;
    char            line_buf[250];

    time(&ltime);
    newtime = localtime(&ltime);

    accept_dat = 12;			/* first, change to new page */
    page_count = 1;
    line_count = 1;
    word_count = 0;			/* nessary */
    line_of_page = 0;

    do
    {
	switch (accept_dat)
	{
	    case 9:			/* TAB code */
		do
		{
		    line_buf[word_count] = ' ';
		    word_count++;
		} while (word_count % s->tab_length != 0);
		break;
	    default:
		line_buf[word_count] = accept_dat;
		word_count++;
		if (iskanji(accept_dat))
		{
		    accept_dat = fgetc(s->fp0);
		    line_buf[word_count] = accept_dat;
		    word_count++;
		}
		avil_count = s->word_perline - s->left_margin - s->line_number - word_count;
		if (avil_count > 1)
		    break;
		if (avil_count == 1)
		{
		    accept_dat = fgetc(s->fp0);
		    if (iskanji(accept_dat))
			ungetc(accept_dat, s->fp0);
		    else
		    {
			line_buf[word_count] = accept_dat;
			word_count++;
		    }
		}
		/*
		 * fgets(temp_buf,(word_perline - left_margin -
		 * line_number),fp0);	/* Kill data end
		 */
		/* next to newline */
	    case 10:			/* 'cr' 0ah code */
		if (0x1b == ex_getch())
		    return (-1);	/* ESC,CTRL_C ? */
		for (margin_count = 0; margin_count < s->left_margin; margin_count++)
		    fprintf(s->fp1, " ");
		if (s->line_number == 10)
		    fprintf(s->fp1, "%5d:   ", line_count);
		line_buf[word_count] = 0;
		fprintf(s->fp1, "%s\n", line_buf);
		line_of_page++;
		line_count++;
		word_count = 0;
		if (line_of_page + 2 < s->line_perpage)
		    break;
		/* next to newpage */
	    case 12:			/* 'ff' 0ch code */
		if ((accept_dat == 12) && (word_count != 0))
		{
		    for (margin_count = 0; margin_count < s->left_margin; margin_count++)
			fprintf(s->fp1, " ");
		    if (s->line_number == 10)
			fprintf(s->fp1, "%5d:   ", line_count);
		    line_buf[word_count] = 0;
		    fprintf(s->fp1, "%s\n", line_buf);
		}
		fprintf(s->fp1, "\f");
		line_of_page = 0;
		word_count = 0;
		for (margin_count = 0; margin_count < s->left_margin; margin_count++)
		    fprintf(s->fp1, " ");
		fprintf(s->fp1, " P.P.C. BY:C.H    FILENAME:%.12s    PAGE:%d    %24.24s\n\n",
			filename, page_count++, asctime(newtime));
		break;
	    case 13:			/* 'lf' 0dh code */
		break;
	}
    } while ((accept_dat = fgetc(s->fp0)) != EOF);

    fprintf(s->fp1, "\f");		/* change to new page */
    return (0);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
disk_msg(int cur_sec, int step_sec, int all_sec, int mode)
{
    int             x1;

    x1 = (int) (cur_sec * 50L / all_sec);

    switch (mode)
    {
	case 0:
	    crt_attr(crt_WHITE | crt_REVERSE);
	    break;
	case 1:
	    crt_attr(crt_BLINK);
	    wgotoxy(0, 0);
	    wprintf("%04dÅ`%04d Sector reading   ", cur_sec, cur_sec + step_sec - 1);
	    crt_attr(crt_CYAN | crt_REVERSE);
	    break;
	case 2:
	    crt_attr(crt_BLINK);
	    wgotoxy(0, 0);
	    wprintf("%04dÅ`%04d Sector writting  ", cur_sec, cur_sec + step_sec - 1);
	    crt_attr(crt_YELLOW | crt_REVERSE);
	    break;
    }
    wgotoxy(x1, 1);
    wprintf("%*s", (int) ((step_sec * 50L) / all_sec) + 1, " ");

    crt_attr(crt_NOREVERSE);
    crt_attr(crt_NOBLINK);
    return (0);
}



disk_duplicator()
{
    unsigned int    seg, max;
    int             cur_sec, step_sec, all_sec;
    int             src, dst;
    struct FAT_ID   stype, dtype;
    void far       *cp_buf;
    WIN             win;

    _dos_getdrive(&src);
    if ((dst = choose_disk("Copy Disk to: ")) == -1)
	return (-1);
    if (src == dst)
    {
	wait_msg(0, 5, 16, "Must copy to another disk !");
	return (-1);
    }

    get_disk_data(src, &stype);
    get_disk_data(dst, &dtype);
    if (harderr_st.flag == 1)
    {
	check_hard();
	return (-1);
    }

    if ((stype.total_data_cluster != dtype.total_data_cluster) ||
	(stype.bytes_per_sector != dtype.bytes_per_sector))
    {
	wait_msg(0, 5, 16, "Different disk type !");
	return (-1);
    }

    _dos_allocmem(0xFFFF, &max);
    if (_dos_allocmem(max, &seg) != 0)
    {
	wait_msg(0, 5, 16, "Not enough mem area !");
	return (-1);
    }
    cp_buf = MK_FP(seg, 0);

    step_sec = (int) ((max * 16L) / dtype.bytes_per_sector);
    all_sec = dtype.data_start + (dtype.sectors_per_cluster + 1) * (dtype.total_data_cluster - 1);

    win = *mkwindow(15, disp_end - 11, 65, disp_end - 10, "DISKCOPY");
    selwindow(&win, crt_RED);
    wgotoxy(0, 1);
    wprintf("++++++++++++++++++++++++++++++++++++++++++++++++++");
    for (cur_sec = 0; cur_sec < all_sec;)
    {
	step_sec = min(step_sec, all_sec - cur_sec);

	disk_msg(cur_sec, step_sec, all_sec, 1);
	if (abs_read(cp_buf, src - 1, cur_sec, step_sec))
	{
	    wait_msg(0, 5, 16, "%04d Sector read error ! Abort.", cur_sec);
	    break;
	}
	disk_msg(cur_sec, step_sec, all_sec, 2);
	if (abs_write(cp_buf, dst - 1, cur_sec, step_sec))
	{
	    if (harderr_st.flag == 1)
		check_hard();
	    else
		wait_msg(0, 5, 16, "%04d Sector write error ! Abort.", cur_sec);
	    break;
	}

	disk_msg(cur_sec, step_sec, all_sec, 0);
	cur_sec += step_sec;
    }

    if (cur_sec >= all_sec)
    {
	selwindow(&win, crt_RED);
	wgotoxy(0, 0);
	wprintf("Complate ! Press a key...   ");
	getch();
	crt_attr(crt_NOBLINK);
    }

    rmwindow(&win);

    _dos_freemem(seg);
    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
week_day(int year, int month, int day)
{
    int             r1, r2;
    static int      r22[] =
    {2, 5, 0, 3, 5, 1, 4, 6, 2, 4, 0, 3};

    if (month >= 3)
	month -= 2;			/* ïœà◊ 11,12,1,2,3,4,5,6,7,8,9,10 */
    else
    {
	year--;
	month += 10;
    }

    r1 = year + (year / 4) - (year / 100) + (year / 400) + day;
    r2 = r22[month - 1];
/*	r2 = 2.6 * month - 0.2;	*/

    return ((r1 + r2) % 7);
}
