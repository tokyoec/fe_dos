static char    *rcsid = "$Header: FEDISK.CV  2.1  95/04/27 22:44:00  Hong.Chen  Rel $";
/*
 $Log: RCS/FEDISK.CV $
 * Revision 2.1  95/04/27 22:44:00  Hong.Chen
 * for released
 * 
 * Revision 1.8  95/04/27 03:32:02  Hong.Chen
 * for MSVC
 * 
 * Revision 1.7  95/04/24 03:22:40  Hong.Chen
 * Add HostDisk Fun. for dos 6.2
 * 
 * Revision 1.6  95/01/20 01:06:00  Hong.Chen
 * Include io.h for copy error in 64k
 * 
 * Revision 1.5  95/01/10 01:05:00  Hong.Chen
 * For Rel
 *
 * Revision 1.2  94/12/16 17:25:22  Hong.Chen
 * bugfix(fcopy in diskfull case, lost free mem)
 *
 * Revision 1.1  94/11/23 01:00:00  Hong.Chen
 * Initial revision
 *
*/

#include	<errno.h>
#include	<io.h>
#include 	"fe.h"
#ifdef LSI_C
#include	<farstr.h>
#endif

struct HARDERR_ST harderr_st =
{0};

/*
#################################################
#	function:				#
#	author	:ChenHong	89/11/25	#
#################################################
*/
show_disk_info()
{
    struct FAT_ID   sst;
    int             drive;
    WIN             win;

    flush_disk();
    _dos_getdrive(&drive);
    get_disk_data(drive, &sst);

    win = *mkwindow(25, 7, 55, 18, "DISK INFORMATION");
    selwindow(&win, crt_RED);

    wprintf("          Check Driver %c:\n", '@' + drive);
    wprintf("        Bytes / Sector %u\n", sst.bytes_per_sector);
    wprintf("      Sector / Cluster %u\n", sst.sectors_per_cluster + 1);
    wprintf("           Shift Count %u\n", sst.driver_type);
    wprintf("      FAT Start Sector %u\n", sst.fat_start);
    wprintf("            FAT Number %u\n", sst.fat_cnt);
    wprintf("        MAX Root Entry %u\n", sst.max_root_entry);
    wprintf("     Data Start Sector %u\n", sst.data_start);
    wprintf("      Max Data Cluster %u\n", sst.total_data_cluster - 1);
    wprintf("         Sectors / FAT %u\n", sst.sectors_per_fat);
    wprintf(" Root DIR Start Sector %u", sst.root_dir_start);

    wait_msg(0, 5, 19, "Press a key to continue ... ");
    rmwindow(&win);

    return (0);
}

/*
#################################################
#	function:check hard error structure	#
#	output	:-1, not hardware error		#
#		'a',hardware error abort	#
#		'r',hardware error retry	#
#################################################
*/
check_hard()
{
    int             drive;

    if (harderr_st.flag == 1)
    {
	if (harderr_st.deverror & 0x8000)
	{
	    wait_msg(0, 5, 16, "Device(0x%x) Error(0x%x) !!!",
		     *(harderr_st.devhdr + 4), harderr_st.errcode);
	    harderr_st.flag = 0;
	    return (0);
	    /*
	     * c_exit(); exit(1);      /* not disk error,
	     * 呼出プログラムを異常終了させる
	     */
	}
	drive = 'A' + (harderr_st.deverror & 0x00FF);

	switch (harderr_st.errcode)
	{
	    case 0:
		wait_msg(0, 5, 16, "Write protected on driver %c !", drive);
		break;
	    case 1:
		wait_msg(0, 5, 16, "Wrong driver number specified !");
		break;
	    case 2:
		wait_msg(0, 5, 16, "Disk not mounted on driver %c !", drive);
		break;
	    default:
		wait_msg(0, 5, 16, "Disk Access(0x%x) error(0x%x) on driver %c !",
			 harderr_st.deverror, harderr_st.errcode, drive);
		break;
	}
	harderr_st.flag = 0;
	flush_disk();
	return (0);
    }
    else
    {
	return (-1);		/* no hard error */
    }
}

get_disk_data(int drive, struct FAT_ID * fat_id)
{
    union REGS      regs;
    struct SREGS    sregs;
    struct FAT_ID far *buf;

    regs.h.ah = 0x32;
    regs.h.dl = drive & 0xf;	/* disk no */
    intdosx(&regs, &regs, &sregs);
    buf = MK_FP(sregs.ds, regs.x.bx);

#ifdef LSI_C
    far_memcpy(fat_id, buf, sizeof(struct FAT_ID));
#else
    *fat_id = *buf;
#endif

    /* (dosv4) fat_id.sectors_per_fat:  char-->short */
    if (_osmajor < 4)
    {
	memmove(((char *) (&fat_id->sectors_per_fat) + 1),
		&fat_id->sectors_per_fat, 10);
	fat_id->sectors_per_fat &= 0xff;
    }

    return (regs.h.al);
}

log_disk(char *msg)
{
    int             alldrv, orgdrv;
    char            path[80];
    int             newdrv;

    if (*msg == ' ')
	newdrv = choose_disk(msg);
    else
    {
	newdrv = (*msg) & 0xf;
	msg = " Log Disk To:";
    }
    _dos_getdrive(&orgdrv);

    while (-1 != newdrv)
    {
	_dos_setdrive(newdrv, &alldrv);
	getcwd(path, 78);
	if (harderr_st.flag == 1)
	{
	    check_hard();
	    _dos_setdrive(orgdrv, &alldrv);
	    newdrv = choose_disk(msg);
	}
	else
	    return (1);		/* 変化 */
    }

    return (-1);

}


drv_status(int drv)
{
    union REGS      regs;

    regs.h.ah = 0x44;
    regs.h.al = 0x08;
    regs.h.bl = drv & 0x1f;
    intdos(&regs, &regs);

    if (!(regs.x.ax == 15 && (regs.x.cflag & 1) == 1))
	return (regs.x.ax + 1);
    else
	return (0);
}

choose_disk(char *msg)
{
    WIN             wp;
    int             status;
    char	    DriveList[80];
    static struct datast qdat =
    {1, 0, 0, NULL, 0, 'W', 14, NULL};
    int             drv, drv0, src;

    _dos_getdrive(&src);

    qdat.dat.buf = DriveList;
    for (drv = 0, drv0 = 0; drv < 26; drv++)
    {
	if(drv_status(drv + 1))
	{
	    DriveList[drv0*2] = drv + 'A';
	    DriveList[drv0*2 + 1] = ' ';
	    if(drv + 1 == src)    qdat.w = drv0;
	    drv0++;
	}
    }
    DriveList[drv0*2 - 1] = 0;

    qdat.msg.buf = msg;
    qdat.dat.x = strlen(msg) + 2;
    wp = *mkwindow(20, disp_end - 10, 55, disp_end - 10, " Choice Disk ");
    status = choose_data(&wp, &qdat, 0, 1, 0);
    rmwindow(&wp);

    if (status != -1)
	status = DriveList[qdat.w*2]-'A' + 1;
    return (status);
}

/*
#################################################
#	function:disk abs read            	#
#	author	:ChenHong	92/04/07	#
#################################################
*/
abs_read(void far *buf, int dn, unsigned short sec, unsigned short cnt)
{
    union REGS      regs;
    struct SREGS    sregs;

    regs.h.al = dn;		/* disk no */
    regs.x.dx = sec;
    regs.x.cx = cnt;
    regs.x.bx = FP_OFF(buf);
    sregs.ds = FP_SEG(buf);
    int86x(0x25, &regs, &regs, &sregs);

    return (regs.x.cflag);
}

/*
#################################################
#	function:disk abs write 		#
#	author	:ChenHong	92/04/07	#
#################################################
*/
abs_write(void far *buf, int dn, unsigned short sec, unsigned short cnt)
{
    union REGS      regs;
    struct SREGS    sregs;

    regs.h.al = dn;		/* disk no */
    regs.x.dx = sec;
    regs.x.cx = cnt;
    regs.x.bx = FP_OFF(buf);
    sregs.ds = FP_SEG(buf);
    int86x(0x26, &regs, &regs, &sregs);

    return (regs.x.cflag);
}



flush_disk()
{
    union REGS      regs;

    regs.h.ah = 0x0d;
    intdos(&regs, &regs);
    return (regs.x.cflag);
}

/*
#################################################
#	function:High Speed Inside File Copy	#
#	return 	:				#
#		0:	Copy succesful		#
#		1:	Read Only File		#
#		2:	Up-to-date 		#
#		3:	No enough workarea	#
#		4:	Write error(Disk Full)  #
#		5:	Mistake path		#
#	author	:ChenHong	90/01/11	#
#################################################
*/
fcopy(char *sf, char *df, int tf)
{
#ifdef LSI_C
    char far       *buf;
#else
    char huge      *buf;
#endif
    int             rfp, wfp;
    unsigned int    seg, max, date, time, attr, dd, dt, t_errno;
    unsigned long   b, r, w;
    unsigned int    b1, r1, w1;
    char	    sf0[80],df0[80];

    _fullpath(sf0, sf, 80);
    _fullpath(df0, df, 80);
    if (0 == strcmp(sf0, df0))
	return (5);		/* same file ?? */

    _dos_open(sf, O_RDONLY, &rfp);
    _dos_getftime(rfp, &date, &time);

    if (tf & 0x1)			/* time compear */
    {
	if (0 == _dos_open(df, O_RDONLY, &wfp))
	{
	    _dos_getftime(wfp, &dd, &dt);
	    _dos_close(wfp);
	    if ((dd > date) || (dd == date && dt >= time))
	    {
		_dos_close(rfp);
		return (2);
	    }
	}
    }

    _dos_getfileattr(sf, &attr);/* create obj file */
    if (0 != _dos_creat(df, attr, &wfp))
    {
	t_errno = errno;
	_dos_close(rfp);
	if (t_errno == EACCES)
	    return (1);		/* object file readonly */
	return (5);		/* other open error */
    }

    if (filelength(rfp) != 0)
    {
	/* 1994/10/25 APPEND for RDISK */

/*    max = 0x800;	/* LSIC-86 */

	_dos_allocmem(0xFFFF, &max);	/* alloc mem */
	max &= 0xfc00;
	b = ((unsigned long) max) << 4;	/* seg -> size */
	if (b == 0 || _dos_allocmem(max, &seg) != 0)
	{
	    _dos_close(rfp);
	    _dos_close(wfp);
	    remove(df);
	    return (3);
	}

	b1 = 0x4000;		/* 1 block = 0x4000 = 16kbyte */
	buf = MK_FP(seg, 0L);	/* copy... */
	do
	{
	    r1 = b1;
	    for (r = 0L; (r < b) && (r1 == b1); r += r1)
	    {			/* read */
		_dos_read(rfp, buf + r, r1, &r1);
	    }
	    w1 = b1;
	    for (w = 0L; (w < b) && (w1 == b1); w += w1)
	    {			/* write */
		w1 = (unsigned int)(min(r, (unsigned long)b1));
		_dos_write(wfp, buf + w, w1, &w1);
		r -= w1;
	    	if(tf & 0x20) wprintf("o");
	    }
	    if (r != 0L)
	    {
		_dos_freemem(seg);
		_dos_close(rfp);
		_dos_close(wfp);
		remove(df);
		return (4);	/* no enough area */
	    }
	} while (w == b);

	_dos_freemem(seg);
    }
    _dos_close(rfp);
    _dos_setftime(wfp, date, time);
    _dos_close(wfp);

    return (0);

}

#define		MIN_SEC_SIZE	512
#define		MAX_SEC_SIZE	(1024*4)
#define		MAX_ENTRY	192
#define		MAX_SEC_NO	(MAX_ENTRY*32/MIN_SEC_SIZE)
#define		DSORT_ERROR	0xffff

static unsigned short avail_sec_cnt, entry_per_sec;

struct DIR_ENT
{
    unsigned char   name[11];
    unsigned char   attr;
    unsigned char   reserve[10];
    unsigned short  time;
    unsigned short  date;
    unsigned short  entry;
    unsigned long   size;
};
struct SEC_NO
{
    unsigned short  sec_no;
    struct DIR_ENT *buff;
};

comp_ent(const struct DIR_ENT * buf1, const struct DIR_ENT * buf2)
{
    if (buf1->name[0] == 0xe5)
	return (1);
    if (buf2->name[0] == 0xe5)
	return (-1);

    if ((buf1->attr & _A_SUBDIR) && (buf2->attr & _A_SUBDIR))
	return (strcmp(buf1->name, buf2->name));

    if (buf2->attr & _A_SUBDIR)
	return (1);
    if (buf1->attr & _A_SUBDIR)
	return (-1);

    return (strcmp(buf1->name, buf2->name));
}


unsigned short
read_root(struct SEC_NO * sec, struct FAT_ID * fat_id)
{
    unsigned short  sec_no, sec_cnt, total_sec;
    unsigned char  j;

    total_sec = fat_id->max_root_entry * 32 + fat_id->bytes_per_sector - 1;
    total_sec /= fat_id->bytes_per_sector;
    sec_no = fat_id->root_dir_start;
    for (sec_cnt = 0; sec_cnt < total_sec;)
    {
	for (j = 0; j < (fat_id->sectors_per_cluster + 1); j++)
	{
	    if (0 != abs_read((void far *) sec[sec_cnt].buff, fat_id->driver, sec_no, 1))
		return (DSORT_ERROR);
	    sec[sec_cnt++].sec_no = sec_no++;
	    if (sec[sec_cnt - 1].buff[entry_per_sec - 1].name[0] == 0)
		return (sec_cnt);
	    if (sec_cnt > avail_sec_cnt)
		return (DSORT_ERROR);
	}
    }

    return (sec_cnt);
}

unsigned short
read_subd(struct SEC_NO * sec, struct FAT_ID * fat_id, int cluster)
{
    unsigned short  sec_no, sec_cnt;
    unsigned short  fat_offset, fat_entsec;
    unsigned char  j;
    unsigned short  cur_sec;
    char            sec_buff[MAX_SEC_SIZE];

    cur_sec = 0xf000;
    for (sec_cnt = 0; cluster <= 0xff6;)
    {
	sec_no = (cluster - 2) * (fat_id->sectors_per_cluster + 1) + fat_id->data_start;
	for (j = 0; j < (fat_id->sectors_per_cluster + 1); j++)
	{
	    if (0 != abs_read((void far *) sec[sec_cnt].buff, fat_id->driver, sec_no, 1))
		return (DSORT_ERROR);
	    sec[sec_cnt++].sec_no = sec_no++;
	    if (sec[sec_cnt - 1].buff[entry_per_sec - 1].name[0] == 0)
		return (sec_cnt);
	    if (sec_cnt > avail_sec_cnt)
		return (DSORT_ERROR);
	}

	fat_offset = (cluster >> 1) * 3;
	fat_entsec = fat_offset / fat_id->bytes_per_sector + fat_id->fat_start;
	fat_offset %= fat_id->bytes_per_sector;
	if (cur_sec == 0xf000 || cur_sec != fat_entsec)
	{
	    cur_sec = fat_entsec;
	    if (0 != abs_read((void far *) sec_buff, fat_id->driver, cur_sec, 1))
		return (DSORT_ERROR);
	}
	if (cluster & 1)
	    cluster = (*(unsigned short *) (sec_buff + fat_offset + 1)) >> 4;
	else
	    cluster = (*(unsigned short *) (sec_buff + fat_offset)) & 0x0fff;
    }

    if (cluster <= 0xff7)
	return (DSORT_ERROR);
    return (sec_cnt);
}


unsigned short
read_subd16(struct SEC_NO * sec, struct FAT_ID * fat_id, int cluster)
{
    unsigned short  sec_no, sec_cnt;
    unsigned short  fat_offset, fat_entsec;
    unsigned char  j;
    unsigned short  cur_sec;
    char            sec_buff[MAX_SEC_SIZE];

    cur_sec = 0xf000;
    for (sec_cnt = 0; cluster <= 0xfff6;)
    {
	sec_no = (cluster - 2) * (fat_id->sectors_per_cluster + 1) + fat_id->data_start;
	for (j = 0; j < (fat_id->sectors_per_cluster + 1); j++)
	{
	    if (0 != abs_read((void far *) sec[sec_cnt].buff, fat_id->driver, sec_no, 1))
		return (DSORT_ERROR);
	    sec[sec_cnt++].sec_no = sec_no++;
	    if (sec[sec_cnt - 1].buff[entry_per_sec - 1].name[0] == 0)
		return (sec_cnt);
	    if (sec_cnt > avail_sec_cnt)
		return (DSORT_ERROR);
	}

	fat_offset = cluster * 2;
	fat_entsec = fat_offset / fat_id->bytes_per_sector + fat_id->fat_start;
	fat_offset %= fat_id->bytes_per_sector;
	if (cur_sec == 0xf000 || cur_sec != fat_entsec)
	{
	    cur_sec = fat_entsec;
	    if (0 != abs_read((void far *) sec_buff, fat_id->driver, cur_sec, 1))
		return (DSORT_ERROR);
	}
	cluster = *(unsigned short *) (sec_buff + fat_offset);
    }

    if (cluster <= 0xfff7)
	return (DSORT_ERROR);
    return (sec_cnt);
}


dsort(char *path)
{
    unsigned short  i, j, cluster, file_cnt, total_sec;
    struct find_t   dir_ent;
    struct FAT_ID   fat_id;
    struct SEC_NO   sec_ptr[MAX_SEC_NO];
    struct DIR_ENT  entry_buf[MAX_ENTRY];

    flush_disk();

    _dos_findfirst("*.*", 0x3f, &dir_ent);
    get_disk_data(dir_ent.reserved[0], &fat_id);

    if ((fat_id.bytes_per_sector > MAX_SEC_SIZE) ||
	(fat_id.bytes_per_sector < MIN_SEC_SIZE) ||
      fat_id.total_data_cluster > 0xffff / (fat_id.sectors_per_cluster + 1))
    {
	wait_msg(0, 5, 16, "Not Expectant FAT Structure...");
	return (-1);
    }

    entry_per_sec = fat_id.bytes_per_sector / 32;
    avail_sec_cnt = MAX_ENTRY / entry_per_sec;

    for (i = 0; i < avail_sec_cnt; i++)
	sec_ptr[i].buff = &entry_buf[i * entry_per_sec];

    cluster = *(unsigned int *) &dir_ent.reserved[15];
    switch (cluster)
    {
	case 0:
	    total_sec = read_root(sec_ptr, &fat_id);
	    break;
	case 1:
	    wait_msg(0, 5, 16, "Bad FAT Map...");
	    return(-1);
	default:
	    if (fat_id.total_data_cluster < 0xff6)
	    {
		/* data cluster < 0xff6 = 4086 */
		total_sec = read_subd(sec_ptr, &fat_id, cluster);
	    }
	    else
	    {
		/* data cluster >= 0xff6 = 4086 16 bit FAT */
		total_sec = read_subd16(sec_ptr, &fat_id, cluster);
	    }
    }

    if (total_sec == DSORT_ERROR)
    {
	wait_msg(0, 5, 16, "Can't Read FAT Map or FAT too Big...");
	    return(-1);
    }

    for (file_cnt = 0; file_cnt < total_sec * entry_per_sec; file_cnt++)
    {
	if (entry_buf[file_cnt].name[0] == 0)
	    break;
    }

    qsort(&entry_buf[2], file_cnt - 2, sizeof(struct DIR_ENT), comp_ent);

    for (j = 0; j < total_sec; j++)
    {
	if (0 != abs_write(sec_ptr[j].buff, fat_id.driver, sec_ptr[j].sec_no, 1))
	{
	    if (harderr_st.flag == 1)
		check_hard();
	    else
		wait_msg(0, 5, 16, "Can't Write FAT Map...");
	    return(-1);
	}
    }

    flush_disk();
    return(0);
}
