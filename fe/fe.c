static char    *rcsid = "$Header: FE.CV  2.3  95/08/19 12:34:28  Hong.Chen  Exp $";
/*
 $Log: RCS/FE.CV $
 * Revision 2.3  95/08/19 12:34:28  Hong.Chen
 * Digit Dispaly Smart -> ###,###,###
 * 
 * Revision 2.1  95/06/07 22:29:08  Hong.Chen
 * for released
 * 
 * Revision 1.9  95/06/07 08:43:50  
 * order, etc.
 * 
 * Revision 1.8  95/04/27 00:39:18  Hong.Chen
 * for MSVC
 * 
 * Revision 1.7  95/04/24 03:25:18  Hong.Chen
 * Add HostDisk Fun. for dos 6.2
 * 
 * Revision 1.6  95/01/20 01:06:00  Hong.Chen
 * move [fpath] buff to far heap
 * etc.
 * 
 * Revision 1.5  95/01/10 01:05:00  Hong.Chen
 * For Rel
 *
 * Revision 1.2  94/12/29 16:59:28  Hong.Chen
 * debug in refmode for reflash & display of diskfree
 *
 * Revision 1.1  94/11/23 01:00:00  Hong.Chen
 * Initial revision
 *
*/

#include	"fe.h"

#define		LL	2
#define		LR	50
#define		RL	54
#define		RR	77
#define		Dir_Mode	0
#define		File_Mode	1
#define		Ref_Mode	2

char            group[10];
static char     orgpath[80];
static struct DIR_ST dir_buff;

/*
#########################################################
#	function:hardware error handler(int 0x24)	#
#	input	:automatic set by turboc		#
#########################################################
*/
/*	--------- AX --------, ------- DI ---------, ------ BP:SI ------- */
void far
handler(unsigned int deverror, unsigned int errcode, unsigned far *devhdr)
{
    /* ﾃﾞﾊﾞｲｽﾗｰ,	DOS ﾌｧﾝｸｼｮﾝ 1 _ 0x0C , 0x59(dosexter)のみが使用可能 */

    harderr_st.flag = 1;
    harderr_st.deverror = deverror;
    harderr_st.errcode = errcode;
    harderr_st.devhdr = devhdr;

    _hardretn(-1);		/* 呼出プログラムに戻る */
}

void online_doc()
{
    char            fpath[_MAX_PATH];

    search_from(fpath);
    strcat(fpath, "FE.HLP");
    do_edit(fpath);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
main(int argc, char *argv[])
{
    void            c_exit();
    int             drivers;
    int             readtree = 1;
    int             status = 1;
    int             page = 0;
    char            choose_name[20];
    struct DIR_ST  *file_buff;
    int             fpoint = 0;	/* file_table current ptr */
    int             fstart = 0;	/* file_table start ptr */

    read_para(argc, argv);
    _harderr(handler);
    signal(SIGINT, c_exit);
    init_screen();

    strcpy(group, "*.*");
    getcwd(orgpath, 78);
    if ((argc > 1) && (strlen(argv[1]) < 3))
	if (-1 == log_disk(argv[1]))
	    page = 0;

    memset(&dir_buff, 0, sizeof(dir_buff));
    dir_buff.mode = Dir_Mode;
    for (;;)
    {
	switch (page)
	{
	    case -2:
		page++;
	    case -1:
		page++;
		switch (correct("Yes No Cancel", "Quit FE, Sure ? "))
		{
		    case -1:
		    case 1:
			break;
		    case 0:
			_dos_setdrive((orgpath[0]) & 0xf, &drivers);
			chdir(orgpath);
		    case 2:
			exit_screen();
			return (0);
		}
	    case 0:		/* Tree Select Mode (left) */
		fpoint = 0;	/* file_table current ptr */
		fstart = 0;	/* file_table start ptr */
		if (readtree == 1)
		{
		    make_dir_tab(&dir_buff);
		    readtree = 0;
		}
		status = choose_select(&dir_buff, LL, LR);
		if (status == 0)
		    readtree = 1;
		if (status == 9)
		{
		    online_doc();
		    status = 0;
		}
		break;
	    case 3:
		page--;
	    case 2:
		do_edit(choose_name);
		page--;
	    case 1:		/* File Select Mode (left) */
		file_buff = malloc(sizeof(struct DIR_ST));
		memset(file_buff, 0, sizeof(*file_buff));
		if (NULL == file_buff)
		{
		    printf("Malloc fault !!\n");
		    exit(0);
		}
		file_buff->point = fpoint;	/* file_table current ptr */
		file_buff->start = fstart;	/* file_table start ptr */
		file_buff->mode = File_Mode;
		make_file_buff(file_buff);
		status = choose_select(file_buff, LL, LR);
		fpoint = file_buff->point;	/* file_table current ptr */
		fstart = file_buff->start;	/* file_table start ptr */
		strcpy(choose_name, file_buff->dat[file_buff->point].packed_name);
		free(file_buff);
		if (status == 9)
		{
		    online_doc();
		    status = 0;
		}
		break;
	}
	page += status;
    }

}

char *long2str(long num, char *str)
{
	char	tmp[12];

	sprintf(tmp, "%9ld", num);
	sprintf(str, "%3.3s,%3.3s,%3.3s", tmp, tmp+3, tmp+6);
	if (str[2] == ' ') str[3] = ' ';
	if (str[6] == ' ') str[7] = ' ';

	return str;
}

file_selected(struct DIR_ST * buf, int para)
{
    int             i;
    long            selected_sum;
    extern char    *mach[];
    extern char    *kjc[];
    char	   tmp1[12], tmp2[12];

    switch (para)
    {
	case -1:
	    if (buf->change)
	    {
		for (i = 0; i < buffer_avail; i++)
		    buf->dat[i].mapc = 0;
		buf->change = 0;
	    }
	    else
	    {
		for (i = 0; i < buf->total; i++)
		    buf->dat[i].mapc = 1;
		buf->change = buf->total;
	    }
	case -2:
	    break;
	default:
	    buf->dat[buf->point].mapc = 1 - buf->dat[buf->point].mapc;
	    if (buf->dat[buf->point].mapc)
		++(buf->change);
	    else
		--(buf->change);
	    break;
    }

    selected_sum = 0L;
    for (i = 0; i < buf->total; i++)
	if (buf->dat[i].mapc)
	    selected_sum += (long) buf->dat[i].fpath;


    crt_attr(crt_REVERSE | crt_CYAN);

    crt_printxy(3, disp_end + 1,
	 "(%3d/%03d)Files (%s/%s)Selected  DOS V%d.%02d %s %s(%2d)",
		buf->change, buf->total, 
		long2str(selected_sum, tmp1), long2str(buf->size, tmp2),
	    _osmajor, _osminor, mach[machine()], kjc[kjcode], disp_end + 2);

    crt_attr(crt_NOREVERSE);

    return(0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
choose_select(struct DIR_ST * buf, int left, int right)
{
    unsigned int    k1;
    int             i, k2;
    int             cg_f, cg_c;

    cur_set(24, 24);
    KEYCLR;

    for (cg_f = 1, cg_c = 1;;)
    {
	k1 = ex_getch();
	switch (k1)
	{
	    default:
		switch (buf->mode)
		{
		    case Dir_Mode:
			k2 = do_dir_menu(k1, buf);
			break;
		    case File_Mode:
			k2 = do_file_menu(k1, buf);
			break;
		    case Ref_Mode:
			k2 = do_ref_menu(k1, buf);
			break;
		};
		switch (k2)
		{
		    case -1:
			continue;	/* 変更無い */
			break;
		    case 0:
			cg_f = 1;	/* 表示変更 */
			break;
		    case 1:
			return (0);	/* 再読む  */
			break;
		    case 999:
			return (999);	/* 終了 */
			break;
		}
		KEYCLR;
		break;
	    case ASC_CR:
		return (1);
	    case ASC_TAB:
		return (1);
	    case ASC_ESC:
	    case ASC_BS:
		return (-1);
	    case '?':
		return(9);
	    case IBM_HOME:	/* GO TOP */
	    case FMR_HOME:
	    case N98_HOME:
		buf->start = 0;
		buf->point = 0;
		++cg_f;
		break;
	    case IBM_END:	/* GO END */
	    case FMR_HELP:
	    case KSH_HELP:
		buf->start = max(0, (buf->total - disp_end + disp_start - 1));
		buf->point = buf->total - 1;
		++cg_f;
		break;
	    case IBM_INS:	/* FLASH */
	    case N98_INS:
/*	    case FMR_INS:	* same as N98_INS */
	    case KSH_INS:
		if (buf->mode == Ref_Mode)
		    continue;
		exit_screen();
		wait_msg(0, 5, 16, "Press any key !");
		KEYCLR;
		init_screen();
		if (left == RL)
		    open_select(buf, LL, LR);
		k1 = 0;
		cg_f = 1;	/* 表示変更 */
		/* break;	 */
	    case 0:
		if (kbhit())
		    continue;
		if (cg_f)
		{
		    cg_f = 0;
		    cg_c = 1;
		    show_waku(buf);
		    open_select(buf, left, right);
		}
		if (cg_c)
		{
		    cg_c = 0;
		    show_a_line(buf, 1, left, right);
		    if (show_current(buf, left, right))
			return (999);	/* No Find Dir. */
		}
		else
		{
		    long            etime;
		    time(&etime);
		    crt_printxy(54, disp_start - 2, "%.24s", ctime(&etime));
#ifdef _DEBUG
		    crt_attr(crt_REVERSE | crt_CYAN);
		    crt_printxy(57, 2, " S:%05u  M:%05u ", stackavail(), _memavl());
		    crt_attr(crt_NOREVERSE);
#endif
		}
		if (harderr_st.flag == 1)
		    check_hard();
		continue;
	    case IBM_UP:
	    case FMR_UP:
	    case N98_UP:
	    case KSH_UP:
		if (buf->point > buf->start)
		{
		    show_a_line(buf, 0, left, right);
		    --buf->point;	/* not scroll */
		}
		else
		{
		    if (buf->start > 0)
		    {
			show_a_line(buf, 0, left, right);
			--buf->point;
			--buf->start;
			if (-1 == movetext(left, disp_start, right, disp_end - 1,
					   left, disp_start + 1))
			    cg_f++;
		    }
		    else
			continue;
		}
		break;
	    case IBM_DEL:	/* Clear Mark */
	    case N98_DEL:
	    case FMR_DEL:
/*	    case KSH_DEL:	*/
		if (buf->mode != File_Mode)
		    continue;
		file_selected(buf, -1);
		cg_f = 1;
		break;
	    case ' ':
		if (buf->mode != File_Mode)
		    continue;
		file_selected(buf, buf->point);
		cg_c = 1;
	    case IBM_DN:
	    case FMR_DN:
	    case N98_DN:
	    case KSH_DN:
		if (buf->point < min(buf->total - 1,
				     buf->start + disp_end - disp_start))
		{
		    show_a_line(buf, 0, left, right);
		    ++buf->point;	/* not scroll */
		}
		else
		{
		    if (buf->total > (buf->start + disp_end - disp_start + 1))
		    {
			show_a_line(buf, 0, left, right);
			++buf->point;
			++buf->start;
			if (-1 == movetext(left, disp_start + 1, right, disp_end,
					   left, disp_start))
			    cg_f++;
		    }
		    else
			continue;
		}
		break;
	    case IBM_LT:
	    case FMR_LT:
/*	    case N98_LT:	*/
	    case KSH_LT:
	    case IBM_RUP:
	    case KSH_RUP:
		for (i = disp_start; i < disp_end; i++)
		{
		    if (buf->start > 0)
		    {
			--buf->start;
			cg_f++;
		    }
		    if (buf->point > min(buf->total - 1,
					 buf->start + disp_end - disp_start))
			--buf->point;
		}
		break;
	    case IBM_RT:
	    case FMR_RT:
	    case N98_RT:
	    case KSH_RT:
	    case IBM_RDN:
	    case KSH_RDN:
		for (i = disp_start; i < disp_end; i++)
		{
		    if (buf->total > (buf->start + disp_end - disp_start))
		    {
			++buf->start;
			++cg_f;
		    }
		    if (buf->point < min(buf->total - 1, buf->start))
			++buf->point;
		}
		break;
	}
	cg_c = 1;
    }
}


/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
show_current(struct DIR_ST * buf, int filel, int filer)
{
    char            orgpath[78];


    switch (buf->mode)
    {
	case Ref_Mode:
	case Dir_Mode:
	    if (buf->point == 0)
		chdir("\\");
	    else
	    {
		sprintf(orgpath, "%Fs", buf->dat[buf->point].fpath);
		chdir(orgpath);
	    }

	    if (buf->mode == Ref_Mode)
	        return (0);
	    show_file_index(buf, filel, filer);
	    break;
	case File_Mode:
	    if (filel != RL)
		show_file_title(buf, RL, RR);
	    break;
    }

    getcwd(orgpath, 78);
    if (3 != strlen(orgpath))
	strcat(orgpath, "\\");
    strcat(orgpath, group);
    strcat(orgpath, ">>");
    if ((buf->mode == File_Mode) && (buf->point != -1))
    {
	strcat(orgpath, (buf->dat[buf->point].packed_name));
    }

    crt_printxy(2, 1, "%-*.*s", 52, 52, orgpath);
    return (0);
}

int
ex_choose_data(WIN * win, struct datast * buf, int ptr, int cnt, int mode)
{
    int             n, yptr;

    for (;;)
    {
	n = choose_data(win, buf, ptr, cnt, mode);
	yptr = -2 - n;
	if (n >= -1)
	    break;
	change_dir(buf[yptr].dat.buf, buf[yptr].w, _A_SUBDIR);
    }
    return(n);
}

char           *
change_dir(char *p, int len, int a)
{
    static char     fname[20];
    char            orgpath[80];
    WIN             win;
    int             status;
    int             drivers;
    int             disk;
    int             fpoint;	/* file_table current ptr */
    int             fstart;	/* file_table start ptr */

    getcwd(orgpath, 78);

    if (p[1] == ':')
	_dos_setdrive(p[0] & 0xf, &drivers);
    chdir(p);

    win = *mkwindow(RL, disp_start, RR, disp_end, " SELECT DIRECTORY ");
    selwindow(&win, crt_RED);

    for (status = 0; status == 0;)
    {
	_dos_getdrive(&disk);
	if ((disk & 0xf) == (dir_buff.dat[0].fpath[0] & 0xf))
	{
	    fpoint = dir_buff.point;	/* file_table current ptr */
	    fstart = dir_buff.start;	/* file_table start ptr */
	    dir_buff.mode = Ref_Mode;
	    status = choose_select(&dir_buff, RL, RR);
	    if (status > 0)
	    {
		sprintf(p, "%-*Fs\0", len, dir_buff.dat[dir_buff.point].fpath);
		strcpy(fname, dir_buff.dat[dir_buff.point].packed_name);
	    }
	    dir_buff.mode = Dir_Mode;
	    dir_buff.point = fpoint;	/* file_table current ptr */
	    dir_buff.start = fstart;	/* file_table start ptr */
	}
	else
	{
	    struct DIR_ST   sdir_buff;

	    memset(&sdir_buff, 0, sizeof(struct DIR_ST));
	    sdir_buff.mode = Ref_Mode;
	    make_dir_tab(&sdir_buff);
	    status = choose_select(&sdir_buff, RL, RR);
	    if (status > 0)
	    {
		sprintf(p, "%-*Fs\0", len, sdir_buff.dat[sdir_buff.point].fpath);
		strcpy(fname, sdir_buff.dat[sdir_buff.point].packed_name);
	    }
	}
    }

    _dos_setdrive((orgpath[0]) & 0xf, &drivers);
    chdir(orgpath);

    rmwindow(&win);
    return (fname);
}

show_file_index(struct DIR_ST * buf, int filel, int filer)
{
    struct DIR_ST   file_buff;

    struct diskfree_t dspc;
    int             disk;
    unsigned long   ut;
	char	tmp1[12];
	char	tmp2[12];
	char	tmp3[12];

    _dos_getdrive(&disk);
    _dos_getdiskfree(disk, &dspc);
    ut = dspc.bytes_per_sector * dspc.sectors_per_cluster;

    memset(&file_buff, 0, sizeof(struct DIR_ST));
    if (filel != RL)
    {
	file_buff.point = 0;	/* file_table current ptr */
	file_buff.start = 0;	/* file_table start ptr */
	file_buff.mode = File_Mode;
	make_file_buff(&file_buff);
	open_select(&file_buff, RL, RR);
    }

    crt_attr(crt_REVERSE | crt_CYAN);
    crt_printxy(3, disp_end + 1,
	      "%3dDir's (%sFree/%sTotal)Bytes  %3dFiles%sBytes",
		buf->total,
		long2str(ut * (unsigned long) dspc.avail_clusters, tmp1),
		long2str(ut * (unsigned long) dspc.total_clusters, tmp2),
		file_buff.total, long2str(file_buff.size, tmp3));
    crt_attr(crt_NOREVERSE);

    return (0);
}

show_file_title(struct DIR_ST * buf, int dirl, int dirr)
{
    FILE           *fp;
    char            dat[80 * 100], *p;
    int             cnt, nr, max;
    int             len;

    box_cls(dirl, disp_start, dirr, disp_end);
    len = dirr - dirl + 1;
    max = (disp_end - disp_start + 1) * len + 1;
    if (buf->point != -1)
	if (NULL != (fp = fopen(buf->dat[buf->point].packed_name, "rb")))
	{
	    max = fread(dat, 1, max, fp);
	    p = dat;
	    for (cnt = 0;
		 ((cnt < (disp_end - disp_start + 1)) && (max > 0));
		 cnt++)
	    {
		nr = 8;
		crt_putxy(dirl, cnt + disp_start,
			  convert_string(p, (max < len ? max : len), &nr));
		p += nr;
		max -= nr;
	    }
	    fclose(fp);
	}

    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
show_a_line(struct DIR_ST * buf, int rev, int dirl, int dirr)
{
    int             wdi, i;
    unsigned int    y;
    extern char    *waku[];
    char            s1[4], s3[4], s4[4];
    char            ff_buf[60];
    char            dt_buf[30];
    char            attr[10];
    struct TIMEST  *timest;
    struct DATEST  *datest;
    static char     week_char[] = "SunMonTurWedThuFriSat";

    strcpy(attr, "RHSVDA__");
    for (i = 0; i < 8; i++)
    {
	if (((buf->dat[buf->point].attrib >> i) & 1) == 0)
	    attr[i] = '_';
    }
    datest = (struct DATEST *) & buf->dat[buf->point].wr_date;
    timest = (struct TIMEST *) & buf->dat[buf->point].wr_time;
    wdi = week_day(1980 + datest->tm_year, datest->tm_mon, datest->tm_mday);
    sprintf(dt_buf, "%02d/%02d/%02d(%.2s)%02d:%02d:%02d\0",
	    80 + datest->tm_year,
	    datest->tm_mon,
	    datest->tm_mday,
	    &week_char[3 * wdi],
	    timest->tm_hour,
	    timest->tm_min,
	    (timest->tm_sec) * 2
	);

    sprintf(s1, "%2.2s\0", &waku[machine()][6]);
    sprintf(s3, "%2.2s\0", &waku[machine()][12]);
    sprintf(s4, "%2.2s\0", &waku[machine()][18]);

    if (rev)
	crt_attr(crt_YELLOW | crt_REVERSE);

    y = buf->point - buf->start + disp_start;

    switch (buf->mode)
    {
	case Dir_Mode:
	case Ref_Mode:
	    ff_buf[0] = 0;
	    for (i = buf->dat[buf->point].mapc >> 1; i > 1; i >>= 1)
	    {
		if ((i & 1) == 1)
		    strcat(ff_buf, s1);
		else
		    strcat(ff_buf, "  ");
	    }
	    if (buf->point != 0)
	    {
		if (buf->dat[buf->point].mape == 0)
		    strcat(ff_buf, s3);
		else
		    strcat(ff_buf, s4);
	    }

	    if (buf->point == 0)
		sprintf(ff_buf, "%FsVolume is ", buf->dat[buf->point].fpath);

	    strcat(ff_buf, buf->dat[buf->point].packed_name);
	    strcat(ff_buf, "                       ");
	    sprintf(&ff_buf[24], "%.3s%c %s\0", attr, attr[5], dt_buf);

	    crt_printxy(dirl, y, "%-*.*s", dirr - dirl + 1, dirr - dirl + 1, ff_buf);
	    break;
	case File_Mode:
	    if (buf->point != -1)
	    {
		char            fname[20];
		char            flen[10];
		char           *sptr, ffile[10], fext[6];

		strcpy(fname, buf->dat[buf->point].packed_name);
		sptr = strrchr(fname, '.');
		strcpy(fext, sptr);
		*sptr = 0;
		strcpy(ffile, fname);
/*		_splitpath(dir_buf->packed_name,fdriver,fdir,ffile,fext);	*/

		switch (buf->dat[buf->point].attrib & (_A_SUBDIR | _A_VOLID))
		{
		    case _A_SUBDIR:
			sprintf(flen, "  <DIR> ");
			break;
		    case _A_VOLID:
			sprintf(flen, "<VOLUME>");
			break;
		    default:
			sprintf(flen, "%8ld", (long) buf->dat[buf->point].fpath);
			break;
		}

		sprintf(ff_buf,
			"%03d%c%-8.8s%-4.4s%s%.3s%c %s\0",
			buf->point + 1,
			(buf->dat[buf->point].mapc ? '*' : ' '),
			ffile, fext, flen,
			attr, attr[5], dt_buf
		    );

		if (buf->dat[buf->point].mapc && (!rev))
		{
		    crt_attr(crt_REVERSE | crt_BLUE);
		    rev = 1;
		}
		if (dirl == RL)
		    crt_printxy(dirl, y, "%.*s", dirr - dirl + 1, &ff_buf[4]);
		else
		    crt_printxy(dirl, y, "%.*s", dirr - dirl + 1, ff_buf);
	    }
	    break;
    }


    if (rev)
    {
	if (dirl == LL)
	{
	    crt_attr(crt_REVERSE | crt_CYAN);
	    for (i = disp_start; i <= disp_end; i++)
		crt_putxy(RL - 2, i, "|");
	    crt_attr(crt_REVERSE | crt_GREEN);
	    if (buf->total > 1)
		crt_putxy(RL - 2, disp_start + (buf->point *
			  (disp_end - disp_start)) / (buf->total - 1), "#");
	}

	crt_attr(crt_NOREVERSE);
	crt_attr(crt_WHITE);
    }
    return (0);
}

show_waku(struct DIR_ST * buf)
{
    int             disk;
    int             i, j, s, c;
    char	tmp[12];

    _dos_getdrive(&disk);

    switch (buf->mode)
    {
	case Ref_Mode:
	    {
		struct diskfree_t dspc;
		unsigned long   ut;

		_dos_getdiskfree(disk, &dspc);
		ut = dspc.bytes_per_sector * dspc.sectors_per_cluster;
		crt_attr(crt_REVERSE | crt_RED);
		crt_printxy(54, disp_end + 1, "%sFree%5ld B/C",
			    long2str(ut * (unsigned long) dspc.avail_clusters, tmp), ut);
		crt_attr(crt_NOREVERSE);
	    }
	    break;
	case File_Mode:
	    file_selected(buf, -2);
	    /* break; */
	case Dir_Mode:
	    crt_attr(crt_REVERSE | crt_CYAN);
	    for (i = 0, j = 0; i < 26; i++)
	    {
		if(s = drv_status(i + 1))
		{

		if (i == disk - 1)
		{
		    crt_attr(crt_REVERSE | crt_GREEN);
		    c = '*';
		}
		else
		    c = '-';

		if (s == 1)
		    crt_printxy(3 + j * 6, disp_start - 1, "(%c%c%c)", c, 'A' + i, c);
		if (s == 2)
		    crt_printxy(3 + j * 6, disp_start - 1, "[%c%c%c]", c, 'A' + i, c);
		if (i == disk - 1)
		    crt_attr(crt_REVERSE | crt_CYAN);

		    j++;
		}
	    }

	    break;
    }

    crt_attr(crt_NOREVERSE);
    return(0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
open_select(struct DIR_ST * buf, int left, int right)
{
    int             temp;


    if (buf->start >= buf->total)
	buf->start = 0;		/* !1! */
    if (buf->point < buf->start)
	buf->point = buf->start;/* !2! */
    if (buf->point > buf->start + disp_end - disp_start)
	buf->point = buf->start + disp_end - disp_start;	/* !4! */
    if (buf->point >= buf->total)
	buf->point = buf->total - 1;	/* !3! */

    temp = buf->point;

    for (buf->point = buf->start;
	 buf->point < buf->start + disp_end - disp_start + 1 &&
	 buf->point < buf->total;
	 buf->point++)
    {
	show_a_line(buf, 0, left, right);
    }
    box_cls(left, disp_start + buf->point - buf->start, right, disp_end);

    buf->point = temp;

    return (0);
}



/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/

make_sub(char *directry, char *wild, struct DIR_ST * dir_buf)
{
    char            path[100];
    int             ans;
    struct find_t   buffer;

    if (dir_buf->mode == Dir_Mode)
	crt_printxy(2, 1, "Search DIR.No:[%03d] %-32.32s", dir_buf->total, directry);

    sprintf(path, "%s\\%s\0", directry, wild);
    ans = _dos_findfirst(path, _A_SUBDIR | _A_HIDDEN | _A_VOLID, &buffer);
    if (ans != 0)
	return (0);

    do
    {
	if (dir_buf->total >= buffer_avail)
	    break;		/* Buffer Full */

	if ((buffer.attrib & _A_SUBDIR) && *(buffer.name) != '.')
	{
	    sprintf(dir_buf->dat[dir_buf->total].packed_name, "%s\0", buffer.name);
	    sprintf(path, "%s\\%s\0", directry, buffer.name);
	    _ffree(dir_buf->dat[dir_buf->total].fpath);
	    dir_buf->dat[dir_buf->total].fpath = _fstrdup(path);
	    dir_buf->dat[dir_buf->total].attrib = buffer.attrib;
	    dir_buf->dat[dir_buf->total].wr_time = buffer.wr_time;
	    dir_buf->dat[dir_buf->total].wr_date = buffer.wr_date;
	    ++(dir_buf->total);
	    make_sub(path, wild, dir_buf);
	}
	else if (buffer.attrib & _A_VOLID)
	{
	    sprintf(dir_buf->dat[0].packed_name, "%s", buffer.name);
	    dir_buf->dat[0].attrib = buffer.attrib;
	    dir_buf->dat[0].wr_time = buffer.wr_time;
	    dir_buf->dat[0].wr_date = buffer.wr_date;
	}
    } while (_dos_findnext(&buffer) == 0);
    return (0);
}

make_dir_tab(struct DIR_ST * dir_buf)
{
    int             disk;
    char            rootpath[4];

    _dos_getdrive(&disk);
    disk += 'A' - 1;
    sprintf(rootpath, "%c:\0", disk);

    memset(&dir_buf->dat[0], 0, sizeof(dir_buf->dat[0]));
    dir_buf->dat[0].fpath = _fstrdup(rootpath);

    dir_buf->total = 1;
    make_sub(rootpath, group, dir_buf);
    make_dir_tree(dir_buf);

    return (dir_buf->total);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
make_dir_tree(struct DIR_ST * dir_buf)
{
    int             i, j;
    char            fpath[128];
    char           *read_ptr;
    char           *new_ptr;
    int             sptr, imgs;

    for (sptr = 0; sptr < dir_buf->total; sptr++)
    {
	sprintf(fpath, "%Fs", dir_buf->dat[sptr].fpath);
	read_ptr = fpath;
	for (i = 0; (NULL != (new_ptr = jstrchr(read_ptr, '\\'))); i++)	/* \の数計算 */
	{
	    read_ptr = ++new_ptr;
	}
	imgs = 1 << i;
	dir_buf->dat[sptr].mapc = imgs;	/* 01 ├─ */
	dir_buf->dat[sptr].mape = imgs;	/* 11 └─	 */
	for (j = sptr - 1; j > 0; j--)
	{
	    if (dir_buf->dat[j].mapc < imgs)
		break;		/* sub level dir. default is└ */

	    if ((dir_buf->dat[j].mapc & imgs) != 0)	/* same level dir. */
	    {
		dir_buf->dat[j].mape = 0;	/* change└ to ├ image */
		break;
	    }
	    dir_buf->dat[j].mapc |= imgs;	/* high level, set │ image */
	}
    }
    return (0);
}
