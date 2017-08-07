static char *rcsid = "$Header: FEAPP.CV  2.1  95/06/10 15:56:30  Hong.Chen  Rel $";
/*
 $Log: RCS/FEAPP.CV $
 * Revision 2.1  95/06/10 15:56:30  Hong.Chen
 * for released
 * 
 * Revision 1.8  95/06/05 01:35:20  
 * order, etc.
 * 
 * Revision 1.7  95/04/26 23:47:12  Hong.Chen
 * for MSVC
 * 
 * Revision 1.6  95/01/20 01:06:00  Hong.Chen
 * Menu Change && Move [fpath] buff to far heap, etc.
 * 
 * Revision 1.5  95/01/10 01:05:00  Hong.Chen
 * For Rel
 * 
 * Revision 1.2  94/11/28 13:41:24  Hong.Chen
 * Find: Page Len Chg. with Disp_end
 * 
 * Revision 1.1  94/11/23 01:00:00  Hong.Chen
 * Initial revision
 * 
*/

#include	"fe.h"

#define			dir_menu_count		14

static struct menust dir_menu[] =
{
    {'L', 0, 2, "L:Log Disk          "},
    {'F', 1, 2, "F:Findfile under dir"},
    {'M', 2, 2, "M:Make new dir      "},
    {'D', 3, 2, "D:Delete            "},
    {'C', 4, 2, "C:Copy dir tree     "},
    {'I', 5, 2, "I:disk Information  "},
    {'W', 6, 2, "W:disk copyWrite    "},
    {'R', 7, 2, "R:Rename  (Ext.cmd) "},
    {'S', 8, 2, "X:eXecute (Ext.Cmd) "},
    {'S', 9, 2, "S:dir Sort(Ext.Cmd)*"},
    {'U',10, 2, "U:User def(Ext.Cmd)*"},
    {'B',11, 2, "B:kill[.Bak,%]file *"},
    {'O',12, 2, "O:system Option    *"},
    {'G',13, 2, "G:set   fileGroup  *"},
};

#define			file_menu_count		15

static struct menust file_menu[] =
{
    {'P', 0, 2, "P:Print text        "},
    {'R', 1, 2, "R:Rename            "},
    {'A', 2, 2, "A:Attribute change  "},
    {'C', 3, 2, "C:Copy .or. move   +"},
    {'T', 4, 2, "T:Touch file       +"},
    {'D', 5, 2, "D:Delete           +"},
    {'L', 6, 2, "L:Lha pack(Ext.Cmd)+"},
    {'E', 7, 2, "E:Edit    (Ext.Cmd) "},
    {'V', 8, 2, "V:Viewer  (Ext.Cmd) "},
    {'X', 9, 2, "X:eXecute (Ext.Cmd) "},
    {'S',10, 2, "S:dir Sort(Ext.Cmd)*"},
    {'U',11, 2, "U:User def(Ext.Cmd)*"},
    {'B',12, 2, "B:kill[.Bak,%]file *"},
    {'O',13, 2, "O:system Option    *"},
    {'G',14, 2, "G:set   fileGroup  *"},
};

do_file_menu(int k1, struct DIR_ST * buf)
{
    int             k2, status = 1, drv, qes;
    char            message[51];
    WIN             win;
    char           *choose_name;

    _dos_getdrive(&drv);
    choose_name = buf->dat[buf->point].packed_name;

    if (k1 == '/')
    {
	win = *mkwindow(29, disp_end - 18, 52,
		   disp_end - 18 + file_menu_count - 1, " FILE OPERATION ");
	k1 = 0;
	k2 = choose_menu(&win, file_menu, 0, file_menu_count, 0);
    }
    else
	k2 = k1;

    k2 = toupper(k2);
    switch (k2)
    {
	default:
	    status = -1;		/* é∏îs */
	    break;
	case 'C':
	    if (-1 == do_copy(buf, 0))
		status = -1;
	    break;
	case 'M':
	    if (-1 == do_copy(buf, 2))
		status = -1;
	    break;
	case 'R':
	    if (-1 == question(message, 51, choose_name, "RENAME %s ", choose_name))
		status = -1;
	    else
	    {
		if (-1 == rename(choose_name, message))
		{
		    if (harderr_st.flag == 1)
			check_hard();
		    else
			wait_msg(0, 5, 16, "Error:rename failed (NO:%d)!", errno);
		    status = -1;	/* é∏îs */
		}
	    }
	    break;
	case 'D':
	    if (-1 == do_removefile(buf))
		status = -1;		/* é∏îs */
	    break;
	case 'A':
	    if (-1 == do_setfattr(choose_name))
		status = -1;
	    break;
	case 'P':
	    if (-1 == do_ppc(choose_name))
		status = -1;
	    break;
	case 'V':
	    if (-1 != do_environ("View", choose_name, ""))
		break;
	case 'E':
	    if (-1 != do_environ("Edit", choose_name, ""))
		break;
	    do_edit(choose_name);
	    break;
	case 'S':
	    if(-1 == do_environ("Dsort", "*.*", "")) dsort("*.*");
	    break;
	case 'L':
	    if (-1 == do_packfile(buf))
		status = -1;
	    break;
	case 'X':
	    do_execute(choose_name);
	    break;
	case 'T':
	    do_setftime(buf);
	    break;
	case 'B':
	    qes = 0;
	    remove_file("*.bak", &qes);
	    remove_file("%*.", &qes);
	    break;
	case 'O':
	    sys_set();
	    status = 1;
	    break;
	case 'G':
	    if (-1 == question(group, 10, "*.*", "File Group:"))
		status = -1;
	    else
		status = 1;		/* çƒì«ÇﬁïKóv */
	    break;
	case 'U':
	    if (-1 == do_hist(0, choose_name))
		status = -1;
	    else
		status = 1;		/* ï\é¶ïœçX */
	    break;
    }

    if (!k1)
	rmwindow(&win);

    return (status);
}

do_dir_menu(int k1, struct DIR_ST * buf)
{
    WIN             win;
    int             qes, k2, status = 0;/* ï\é¶ïœçX */
    char            choose_dir[128];

    sprintf(choose_dir, "%Fs", buf->dat[buf->point].fpath);

    if (k1 == '/')
    {
	win = *mkwindow(29, disp_end - 18, 52,
		     disp_end - 18 + dir_menu_count - 1, " DIR OPERATION ");
	k1 = 0;
	k2 = choose_menu(&win, dir_menu, 0, dir_menu_count, 0);
    }
    else
	k2 = k1;

    k2 = toupper(k2);
    switch (k2)
    {
	default:
	    status = -1;		/* é∏îs */
	    break;
	case 'C':
	    dir_copy(choose_dir);
	    break;
	case 'M':
	    if (-1 == make_dir(buf))
		status = -1;		/* é∏îs */
	    break;
	case 'D':
	    if (-1 == remove_dir(buf))
		status = -1;		/* é∏îs */
	    break;
	case 'X':
	    run_system(0, "%s", getenv("COMSPEC"));
	    break;
	case 'S':
	    if (-1 == do_environ("Dsort", "*.*", ""))
		dsort("*.*");
	    break;
	case 'B':
	    qes = 0;
	    remove_file("*.bak", &qes);
	    remove_file("%*.", &qes);
	    break;
	case 'R':
	    if (-1 == ren_dir(buf))
		status = -1;		/* é∏îs */
	    break;
	case 'F':
	    find_file(choose_dir);
	    break;
	case 'L':
	    status = log_disk(" Log Disk To:");
	    break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	    {
	    	char tmp[2];
		tmp[0] = k2;
		tmp[1] = 0;
	    	status = log_disk(tmp);
	    }
	    break;
	case 'O':
	    sys_set();
	    status = 0;
	    break;
	case 'I':
	    show_disk_info();
	    break;
	case 'W':
	    disk_duplicator();
	    break;
	case 'G':
	    if (-1 == question(group, 10, "*.*", "File Group:"))
		status = -1;
	    break;
	case 'P':
    	    if (-1 == question(choose_dir, 51, "DIR_PACK", "LHA pack name :"))
		status = -1;
	    else
	    {
	        if (-1 == do_environ("Lha", choose_dir, group))
		    status = -1;
	    }
	    break;
	case 'U':
	    if (-1 == do_hist(0, choose_dir))
		status = -1;
	    else
		status = 0;		/* ï\é¶ïœçX */
	    break;
    }

    if (!k1)
	rmwindow(&win);

    return (status);
}

do_ref_menu(int k1, struct DIR_ST * buf)
{
    int             k2, status = 0;	/* ï\é¶ïœçX */

    k2 = k1;

    k2 = toupper(k2);
    switch (k2)
    {
	default:
	    status = -1;		/* é∏îs */
	    break;
	case 'M':
	    if (-1 == make_dir(buf))
		status = -1;		/* é∏îs */
	    break;
	case 'D':
	    if (-1 == remove_dir(buf))
		status = -1;		/* é∏îs */
	    break;
	case 'L':
	    status = log_disk(" Log Disk To:");
	    break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	    {
	    	char tmp[2];
		tmp[0] = k2;
		tmp[1] = 0;
	    	status = log_disk(tmp);
	    }
	    break;
    }

    return (status);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
dir_copy(char *choose_dir)
{
    static struct datast cpy_menu[] =
    {
	{1, 0, 1, "Object Dir :", 30, 'P', 14, ""},
	{2, 1, 1, "Copy Method:", 1, 'W', 14, "OverWrite Update"},
	{3, 2, 1, "Copy Range :", 1, 'W', 14, "Current   Tree"},
    };
    WIN             win_menu, win;
    char            obj_dir_str[32];
    char            obj_dir[32];

    cpy_menu[0].dat.buf = obj_dir_str;
    sprintf(obj_dir_str, "%-28.28s", choose_dir);
    win_menu = *mkwindow(15, disp_end - 8, 64, disp_end - 6, " DIR COPY ");
    if (-1 == ex_choose_data(&win_menu, cpy_menu, 0, 3, 0))
    {
	rmwindow(&win_menu);
	return (-1);
    }
    sscanf(obj_dir_str, "%s", obj_dir);	/* space[' '] kill */
    if (obj_dir[strlen(obj_dir) - 1] == '\\')
	obj_dir[strlen(obj_dir) - 1] = '\0';

    win = *mkwindow(34, 5, 74, disp_end - 2, " DIRECTORY COPY ");
    selwindow(&win, crt_YELLOW);

    if (0 == cpygrpfile(choose_dir, group, obj_dir, cpy_menu[1].w))
    {
	if (cpy_menu[2].w)
	    cpygrpsub(choose_dir, group, obj_dir, cpy_menu[1].w);
    }

    selwindow(&win, crt_CYAN);
    if (harderr_st.flag == 1)
	check_hard();
    else
	wait_msg(0, 44, disp_end - 1, "Press a key to continue ... ");
    rmwindow(&win);

    rmwindow(&win_menu);
    return (0);
}


cpygrpfile(char *src_dir, char *wild, char *obj_dir, int tf)
{
    char            path[100], newname[100];
    int             ans;
    struct find_t   buffer;

    mkdir(obj_dir);
    sprintf(path, "%s\\%s\0", src_dir, wild);
    ans = _dos_findfirst(path,
		  (_A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH),
			 &buffer);
    if (ans != 0)
	return (0);

    wprintf("copy to path=%s\n", obj_dir);
    do
    {
	if (kbhit() && getch() == 0x1b)
	{
	    if (0 == correct("Yes No", "Interrupt, Sure ? "))
		return (-1);		/* Quit */
	}
	wprintf("    %-12s ", buffer.name);
	sprintf(newname, "%s\\%s\0", src_dir, buffer.name);
	sprintf(path, "%s\\%s\0", obj_dir, buffer.name);
	switch (fcopy(newname, path, (tf | 0x20)))
	{
	    case 0:
		wprintf("\n");
		break;
	    case 1:
		wprintf("Read only file.  \n");
		break;
	    case 2:
		wprintf("Up-to-date. \n");
		break;
	    case 3:
		wprintf("Not enough workarea ! \n");
		return (-1);
		break;
	    case 4:
		wprintf("Write Error ! \n");
		return (-1);
		break;
	    case 5:
		wprintf("Mistake path ! \n");
		return (-1);
		break;
	}
    } while (_dos_findnext(&buffer) == 0);
    return (0);
}

cpygrpsub(char *src_dir, char *wild, char *obj_dir, int tf)
{
    char            path[100], newdir[100];
    int             ans;
    struct find_t   buffer;

    sprintf(path, "%s\\*.*\0", src_dir);
    ans = _dos_findfirst(path, _A_SUBDIR, &buffer);
    if (ans != 0)
	return (0);

    do
    {
	if ((buffer.attrib & _A_SUBDIR) && *(buffer.name) != '.')
	{
	    sprintf(newdir, "%s\\%s\0", src_dir, buffer.name);
	    sprintf(path, "%s\\%s\0", obj_dir, buffer.name);
/*	    mkdir(path);	*/
	    if (-1 == cpygrpfile(newdir, wild, path, tf))
		return (-1);
	    if (-1 == cpygrpsub(newdir, wild, path, tf))
		return (-1);
	}
    } while (_dos_findnext(&buffer) == 0);
    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
remove_file(char *group, int *qes)
{
    int             ans;
    struct find_t   buffer;
    char            path[80];

    ans = _dos_findfirst(group,
		  (_A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH),
			 &buffer);

    if (ans != 0)
	return (0);

    if (*qes)
    {
	getcwd(path, 78);
	switch (correct("Yes No All", "Delete %s\\%s, Sure ?", path, group))
	{
	    case 0:
		break;
	    case 2:
		*qes = 0;
		break;
	    case -1:
	    case 1:
		return (-1);
		break;
	}
    }

    do
    {
	if (kbhit() && getch() == 0x1b)
	{
	    if (0 == correct("Yes No", "Interrupt, Sure ? "))
		return (-1);		/* Quit */
	}
	if (-1 == remove(buffer.name))
	{
	    flush_disk();
	    if (harderr_st.flag == 1)
		check_hard();
	    else
		wait_msg(0, 5, 16, "Error:remove failed !");
	    return (-1);
	}
    }
    while (_dos_findnext(&buffer) == 0);

    return (0);
}



/*
#################################################
#	function:	             		#
#	author	:ChenHong	90/01/11	#
#################################################
*/

int             find_cnt;
char            attr;
find_file(char *choose_dir)
{
    static struct datast data2[] =
    {
	{0, 0, 1, "File Name   :", 12, 'T', 14, "              "},
	{5, 1, 1, "Archive     :", 1, 'W', 14, "off on"},
	{2, 2, 1, "System      :", 1, 'W', 14, "off on"},
	{1, 3, 1, "Hidden file :", 1, 'W', 14, "off on"},
	{0, 4, 1, "Read only   :", 1, 'W', 14, "off on"},
    };
    int             i;
    WIN             nwin;
    WIN             win;
    char           *obj_fn = data2[0].dat.buf;

    nwin = *mkwindow(25, disp_end - 9, 55, disp_end - 5, " Find File ");
    if (-1 != choose_data(&nwin, data2, 0, 5, 0))
    {
	attr = 0;
	for (i = 1; i < 5; i++)
	    attr |= data2[i].w << data2[i].ret;

	win = *mkwindow(30, 5, 70, disp_end - 2, "FIND FILE ");
	find_cnt = 0;
	selwindow(&win, crt_YELLOW);
	allfile(choose_dir, obj_fn);
	allsub(choose_dir, obj_fn);
	selwindow(&win, crt_CYAN);
	wait_msg(0, 5, disp_end - 1, "Press a key to continue ... ");
	rmwindow(&win);
    }
    rmwindow(&nwin);
    return (0);
}

allfile(char *directry, char *wild)
{
    char            path[100];
    int             ans;
    struct find_t   buffer;

    sprintf(path, "%s\\%s\0", directry, wild);
    ans = _dos_findfirst(path, attr, &buffer);
    if (ans != 0)
	return (0);

    do
    {
/*    	if(a = buffer.attrib & attr)	*/
	{
	    if (++find_cnt > (disp_end-7))
	    {
		find_cnt = 0;
		if (0 != correct("Yes No", "Continue, Sure ? "))
		    return (-1);	/* Quit */
	    }
	    wprintf("%s\\%-12s %8ld\n", directry, buffer.name, buffer.size);
	}
    } while (_dos_findnext(&buffer) == 0);
    return (0);
}

allsub(char *directry, char *wild)
{
    char            path[100], newdir[100];
    int             ans;
    struct find_t   buffer;

    sprintf(path, "%s\\*.*\0", directry);
    ans = _dos_findfirst(path, _A_SUBDIR | _A_HIDDEN, &buffer);
    if (ans != 0)
	return (0);

    do
    {
	if ((buffer.attrib & _A_SUBDIR) && *(buffer.name) != '.')
	{
	    sprintf(newdir, "%s\\%s\0", directry, buffer.name);
	    if (-1 == allfile(newdir, wild))
		return (-1);
	    allsub(newdir, wild);
	}
    } while (_dos_findnext(&buffer) == 0);
    return (0);
}

/*
#################################################
#	function:	             		#
#	author	:ChenHong	90/01/11	#
#################################################
*/
ren_dir(struct DIR_ST * dir_buf)
{
    char            obj_dir[80];
    char            choose_dir[128];

    sprintf(choose_dir, "%Fs", dir_buf->dat[dir_buf->point].fpath);

    if (-1 == question(obj_dir, 51, choose_dir, "RENDIR %s ", choose_dir))
	return (-1);

    chdir("..");
    if (-1 == do_environ("Rendir", choose_dir, obj_dir))
	return (-1);
    chdir(obj_dir);
    getcwd(obj_dir, 78);

    sprintf(dir_buf->dat[dir_buf->point].packed_name, "%s",
	    strrchr(obj_dir, '\\') + 1);
    _ffree(dir_buf->dat[dir_buf->point].fpath);
    dir_buf->dat[dir_buf->point].fpath = _fstrdup(obj_dir);
    make_dir_tree(dir_buf);

    return (0);
}

/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
make_dir(struct DIR_ST * dir_buf)
{
    char            obj_dir[51];
    char            tmp_dir[81];

    if (dir_buf->total >= buffer_avail)
    {
	wait_msg(0, 5, 16, "Error:FE Dir Buffer Full !");
	return (-1);
    }
    sprintf(tmp_dir, "%Fs", dir_buf->dat[dir_buf->point].fpath);
    if (-1 == question(obj_dir, 50, "", "%s>>MKDIR ", tmp_dir))
	return (-1);
    if (mkdir(obj_dir) == -1)
    {
	if (harderr_st.flag == 1)
	    check_hard();
	else
	    wait_msg(0, 5, 16, "Error:mkdir failed !");
	return (-1);
    }

    /* memory move for inseart record after dir_buf->point */
    memmove(dir_buf->dat + dir_buf->point + 1, dir_buf->dat + dir_buf->point,
	    sizeof(struct DIR_BUF) * (dir_buf->total - dir_buf->point));

    sprintf(dir_buf->dat[dir_buf->point + 1].packed_name, "%s", obj_dir);
    sprintf(tmp_dir, "%Fs\\%s\0", dir_buf->dat[dir_buf->point].fpath, obj_dir);
    dir_buf->dat[dir_buf->point + 1].fpath = _fstrdup(tmp_dir);

    dir_buf->total++;
    make_dir_tree(dir_buf);
    return (0);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
remove_dir(struct DIR_ST * dir_buf)
{
    int             qes = 1;
    char 	    choose_dir[128];

    sprintf(choose_dir, "%Fs", dir_buf->dat[dir_buf->point].fpath);

    if (-1 == remove_file("*.*", &qes))
	return (-1);
    chdir("..");
    if (rmdir(choose_dir) == -1)
    {
	wait_msg(0, 5, 16, "Error:rmdir failed !");
	chdir(choose_dir);
	return (-1);
    }

    flush_disk();
    if (harderr_st.flag == 1)
    {
	check_hard();
	return (-1);
    }

    _ffree(dir_buf->dat[dir_buf->point].fpath);
    /* memory move for delete record at dir_buf->point */
    memmove(dir_buf->dat + dir_buf->point, dir_buf->dat + dir_buf->point + 1,
	    sizeof(struct DIR_BUF) * (dir_buf->total - dir_buf->point));

    dir_buf->total--;
    if (dir_buf->point == dir_buf->total)
	dir_buf->point--;
    make_dir_tree(dir_buf);
    return (0);
}


/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
do_setfattr(filename)
    char           *filename;
{
    static struct datast data2[] =
    {
	{5, 0, 1, "Archive     :", 0, 'W', 14, "off on"},
	{2, 1, 1, "System      :", 0, 'W', 14, "off on"},
	{1, 2, 1, "Hidden file :", 0, 'W', 14, "off on"},
	{0, 3, 1, "Read only   :", 0, 'W', 14, "off on"},
    };
    unsigned int    attr = 0;
    int             i;
    WIN             win;

    if (_dos_getfileattr(filename, &attr) != 0)
    {
	wait_msg(0, 5, 16, "Error: Get attributes failed");
	return (-1);
    }

    for (i = 0; i < 4; i++)
	data2[i].w = (attr >> data2[i].ret) & 1;

    win = *mkwindow(29, disp_end - 13, 51, disp_end - 10, " SET ATTRIBUTE ");
    if (-1 != choose_data(&win, data2, 0, 4, 0))
    {
	attr = 0;
	for (i = 0; i < 4; i++)
	    attr += data2[i].w << data2[i].ret;
	if (_dos_setfileattr(filename, attr) != 0)
	{
	    if (harderr_st.flag == 1)
		check_hard();
	    else
		wait_msg(0, 5, 16, "Error: Set attributes failed");
	    rmwindow(&win);
	    return (-1);
	}
    }
    rmwindow(&win);
    return (0);
}



/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
do_copy(struct DIR_ST * buf, int method)
{
    static struct datast cpy_menu[] =
    {
	{1, 0, 1, "Object Dir :", 30, 'P', 14, ""},
	{2, 1, 1, "Copy Method:", 0, 'W', 14, "Copy Update Move"},
    };
    int             i, current_drv, status = 1;
    WIN             win_menu;
    char            obj_dir_str[32];
    char            newpath[50];

    cpy_menu[0].dat.buf = getcwd(obj_dir_str, 78);
    current_drv = obj_dir_str[0];
    cpy_menu[1].w = method;
    win_menu = *mkwindow(15, disp_end - 7, 64, disp_end - 6, " FILE COPY ");
    if (-1 == ex_choose_data(&win_menu, cpy_menu, 0, 2, 1))
    {
	rmwindow(&win_menu);
	return (-1);
    }

    if (buf->change)
	for (i = 0; i < buffer_avail; i++)
	{
	    if (buf->dat[i].mapc == 1)
	    {
		sscanf(obj_dir_str, "%s", newpath);	/* space[' '] kill */
		if (3 != strlen(newpath))
		    strcat(newpath, "\\");
		strcat(newpath, buf->dat[i].packed_name);
		if (-1 == do_rename(buf->dat[i].packed_name,
				    newpath, current_drv, cpy_menu[1].w))
		    status = -1;	/* é∏îs */
	    }
	}
    else
    {
	i = buf->point;
	sscanf(obj_dir_str, "%s", newpath);	/* space[' '] kill */
	if (3 != strlen(newpath))
	    strcat(newpath, "\\");
	strcat(newpath, buf->dat[i].packed_name);
	if (-1 == do_rename(buf->dat[i].packed_name,
			    newpath, current_drv, cpy_menu[1].w))
	    status = -1;		/* é∏îs */
    }
    rmwindow(&win_menu);
    return (status);
}


do_rename(char *name, char *newpath, int current_drv, int mode)
{
    int             status;

    switch (mode)
    {
	case 2:
	    if ((newpath[1] == ':') && ((current_drv & 0xf) == (newpath[0] & 0xf)))
	    {
		if (-1 == rename(name, newpath))
		{
		    if (harderr_st.flag == 1)
			check_hard();
		    else
			wait_msg(0, 5, 16, "Error:rename failed (NO:%d)!", errno);
		    status = -1;	/* é∏îs */
		}
		break;
	    }
	    /* break;	 */
	case 0:
	case 1:
	    if (0 != (status = fcopy(name, newpath, mode)))
	    {
		if (harderr_st.flag == 1)
		    check_hard();
		else
		{
		    if (mode == 2)
			wait_msg(0, 5, 16, "Error:Up to Date  !");
		    else
			wait_msg(0, 5, 16, "Error:copy failed !");
		}
		status = -1;		/* é∏îs */
	    }
	    if (mode == 2)
		remove(name);
	    break;
    }
    return(0);
}



/*
#################################################
#	function:				#
#	author	:ChenHong	90/01/11	#
#################################################
*/
do_setftime(struct DIR_ST * buf)
{
    int             i;
    int             fh;
    union
    {
	struct DATEST   st;
	unsigned int    date;
    }               dateun;
    union
    {
	struct TIMEST   st;
	unsigned int    time;
    }               timeun;
    static struct datast data1[] =
    {
	{1, 0, 0, " years  :", 2, 'D', 10, "80"},
	{2, 1, 0, " months :", 2, 'D', 10, "00"},
	{1, 2, 0, " dat    :", 2, 'D', 10, "00"},
	{2, 3, 0, " hours  :", 2, 'D', 10, "00"},
	{1, 4, 0, " minutes:", 2, 'D', 10, "00"},
	{2, 5, 0, " seconds:", 2, 'D', 10, "00"},
    };
    WIN             win;
    long            etime;
    struct tm       tm0;

    time(&etime);
    tm0 = *localtime(&etime);

    sprintf(data1[0].dat.buf, "%2.2d", tm0.tm_year);
    sprintf(data1[1].dat.buf, "%2.2d", tm0.tm_mon + 1);
    sprintf(data1[2].dat.buf, "%2.2d", tm0.tm_mday);
    sprintf(data1[3].dat.buf, "%2.2d", tm0.tm_hour);
    sprintf(data1[4].dat.buf, "%2.2d", tm0.tm_min);
    sprintf(data1[5].dat.buf, "%2.2d", tm0.tm_sec);

    win = *mkwindow(30, disp_end - 11, 44, disp_end - 6, " TIMESTAMPE ");
    if (-1 != choose_data(&win, data1, 0, 6, 0))
    {
	dateun.st.tm_year = atoi(data1[0].dat.buf) - 80;
	dateun.st.tm_mon = atoi(data1[1].dat.buf);
	dateun.st.tm_mday = atoi(data1[2].dat.buf);
	timeun.st.tm_hour = atoi(data1[3].dat.buf);
	timeun.st.tm_min = atoi(data1[4].dat.buf);
	timeun.st.tm_sec = atoi(data1[5].dat.buf) / 2;

	if (buf->change)
	    for (i = 0; i < buffer_avail; i++)
	    {
		if (buf->dat[i].mapc == 1)
		{
		    _dos_open(buf->dat[i].packed_name, O_RDWR, &fh);
		    _dos_setftime(fh, dateun.date, timeun.time);
		    _dos_close(fh);
		}
	    }
	else
	{
	    i = buf->point;
	    _dos_open(buf->dat[i].packed_name, O_RDWR, &fh);
	    _dos_setftime(fh, dateun.date, timeun.time);
	    _dos_close(fh);
	}

    }
    rmwindow(&win);
    return (0);
}


do_removefile(struct DIR_ST * buf)
{
    int             status = 0, i, qes = 1;

    if (buf->change)
	for (i = 0; i < buffer_avail; i++)
	{
	    if (buf->dat[i].mapc == 1)
	    {
		if (-1 == remove_file(buf->dat[i].packed_name, &qes))
		    status = -1;	/* é∏îs */
	    }
	}
    else
    {
	i = buf->point;
	if (-1 == remove_file(buf->dat[i].packed_name, &qes))
	    status = -1;		/* é∏îs */
    }
    return (status);
}

do_packfile(struct DIR_ST * buf)
{
    int             i;
    char            pack_name[21];
    char            obj_file[251], *ptr;

    strcpy(obj_file, buf->dat[buf->point].packed_name);
    if (ptr = strrchr(obj_file, '.'))
	*ptr = 0;
    if (-1 == question(pack_name, 51, obj_file, "LHA pack name :"))
	return (-1);

    obj_file[0] = 0;
    if (buf->change)
	for (i = 0; i < buffer_avail; i++)
	{
	    if ((buf->dat[i].mapc == 1) && 
	    	(sizeof(obj_file) - strlen(obj_file) > 14))
	    {
		strcat(obj_file, buf->dat[i].packed_name);
		strcat(obj_file, " ");
	    }
	}
    else
    {
	i = buf->point;
	strcat(obj_file, buf->dat[i].packed_name);
    }

    if (-1 == do_environ("Lha", pack_name, obj_file))
	return (-1);

    return (0);
}
