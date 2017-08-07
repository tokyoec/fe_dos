/*
 $Header: FE.HV  2.3  95/09/13 16:26:04  Hong.Chen  Exp $
 $Log: RCS/FE.HV $
 * Revision 2.3  95/09/13 16:26:04  Hong.Chen
 * same thing change
 * 
 * Revision 2.1  95/04/27 22:43:02  Hong.Chen
 * for released
 * 
 * Revision 1.6  95/04/26 01:31:58  Hong.Chen
 * for MSVC
 * 
 * Revision 1.5  95/01/10 01:05:00  Hong.Chen
 * For Rel
 * 
 * Revision 1.1  94/11/23 01:00:00  Hong.Chen
 * Initial revision
 * 
*/

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<conio.h>
#include 	<ctype.h>
#include 	<jctype.h>
#include	<string.h>
#include	<jstring.h>
#include	<time.h>
#include 	<fcntl.h>
#include 	<direct.h>
#include	<search.h>
#include	<signal.h>
#include	<malloc.h>
#include	<dos.h>

#include	<choose.h>


#ifdef LSI_C
#define _MAX_PATH      144      /* max. length of full pathname */
#define	_memavl() 	(0)
#endif

#ifndef LSI_C
#pragma pack(1)
#endif
struct FAT_ID
{
    unsigned char   driver;		/* driver No */
    unsigned char   driver_e;		/* unit No. */
    unsigned short  bytes_per_sector;
    unsigned char   sectors_per_cluster;/* n = sectors_per_cluster+1 */
    unsigned char   driver_type;	/* Shift count */
    unsigned short  fat_start;		/* Reserved Sector */
    unsigned char   fat_cnt;
    unsigned short  max_root_entry;
    unsigned short  data_start;
    unsigned short  total_data_cluster;	/* n = total_data_cluster-1 */
    unsigned short  sectors_per_fat;	/* dosv4: char-->short */
    unsigned short  root_dir_start;
    unsigned char far *DeviceHeader;
    unsigned char   MediaDiscriptor;
    unsigned char   ChangeDisk;
    struct FAT_ID far *NextDpb;
    unsigned short  CurrentDirFat;
};
#ifndef LSI_C
#pragma pack()
#endif

/* user define area */

#define		ASCII_CODE		0
#define		SJIS_CODE		1
#define		GB_CODE			2

#define		buffer_avail	400
#define		disp_start	3
#define		ishanzi(c)		((0x00ff & (char)c) > 0x00a0)

struct DIR_BUF
{
    char        packed_name[13];
    char 	attrib;
    unsigned 	wr_time;
    unsigned 	wr_date;
/*  long 	size;		/* comm to fpath */
    int         mape;		/* map end	 */
    int         mapc;		/* map cross */
    char  far  *fpath;		/* use in Dir. only */
};

struct DIR_ST
{
    int             start;		/* dir_table start ptr r_total-1) */
    int             point;		/* dir_table current ptr 0..(dir_total-1) */
    int             total;		/* data count ..dir_total	 */
    int             change;		/* dir has change 0..1)	 */
    int             mode;		/* dir / file mode 0..1)	 */
    long            size;		/* file total size ..dir_total	 */
    int             attr;		/* file attrbute			 */
    struct DIR_BUF  dat[buffer_avail];
};

struct PPC_ST
{
    FILE           *fp0;
    FILE           *fp1;
    char            title_str[90];
    int             line_perpage;
    int             word_perline;
    int             tab_length;
    int             left_margin;
    char            line_number;
};

struct TIMEST
{
    unsigned        tm_sec:5;		/* seconds after the minute - [0,59] */
    unsigned        tm_min:6;		/* minutes after the hour - [0,59] */
    unsigned        tm_hour:5;		/* hours since midnight - [0,23] */
};
struct DATEST
{
    unsigned        tm_mday:5;		/* day of the month - [1,31] */
    unsigned        tm_mon:4;		/* months since January - [0,11] */
    unsigned        tm_year:7;		/* years since 1980 */
};


struct HARDERR_ST
{
    int             flag;
    unsigned int    deverror;
    unsigned int    errcode;
    unsigned far   *devhdr;
};

extern struct HARDERR_ST harderr_st;
extern int      disp_end;
extern char     group[10];

extern int      kjcode;			/* file side, 0:ASCII ,   1:SJ, 2:GB */
extern int      idx;			/* 0:OFF, 1:ON */
extern int      tab_length;

/*	cl /Zg /c *.c	*/

extern  void far handler(unsigned int deverror,unsigned int errcode,unsigned int far *devhdr);
extern  int main(int argc,char * *argv);
extern  int file_selccted(struct DIR_ST *buf,int para);
extern  int choose_select(struct DIR_ST *buf,int left,int right);
extern  int show_current(struct DIR_ST *buf,int filel,int filer);
extern  int ex_choose_data(struct win *win,struct datast *buf,int ptr,int cnt,int mode);
extern  char *change_dir(char *p,int len,int a);
extern  int show_file_index(struct DIR_ST *buf,int filel,int filer);
extern  int show_file_title(struct DIR_ST *buf,int dirl,int dirr);
extern  int show_a_line(struct DIR_ST *buf,int rev,int dirl,int dirr);
extern  int show_waku(struct DIR_ST *buf);
extern  int open_select(struct DIR_ST *buf,int left,int right);
extern  int make_sub(char *directry,char *wild,struct DIR_ST *dir_buf);
extern  int make_dir_tab(struct DIR_ST *dir_buf);
extern  int make_dir_tree(struct DIR_ST *dir_buf);
extern  int do_file_menu(int k1,struct DIR_ST *buf);
extern  int do_dir_menu(int k1,struct DIR_ST *buf);
extern  int do_ref_menu(int k1,struct DIR_ST *buf);
extern  int dir_copy(char *choose_dir);
extern  int cpygrpfile(char *src_dir,char *wild,char *obj_dir,int tf);
extern  int cpygrpsub(char *src_dir,char *wild,char *obj_dir,int tf);
extern  int remove_file(char *group,int *qes);
extern  int find_file(char *choose_dir);
extern  int allfile(char *directry,char *wild);
extern  int allsub(char *directry,char *wild);
extern  int ren_dir(struct DIR_ST *dir_buf);
extern  int make_dir(struct DIR_ST *dir_buf);
extern  int remove_dir(struct DIR_ST *dir_buf);
extern  int do_setfattr(char *filename);
extern  int do_copy(struct DIR_ST *buf,int method);
extern  int do_rename(char *name,char *newpath,int current_drv,int mode);
extern  int do_setftime(struct DIR_ST *buf);
extern  int do_removefile(struct DIR_ST *buf);
extern  int do_packfile(struct DIR_ST *buf);
extern  int set_env_buff(char *tmp,struct SYS_CMD *cmd);
extern  char *get_env_buff(char *tmp,struct SYS_CMD *cmd);
extern  int read_env(void );
extern  int gcur_set(int x0,int y0,int sy,int ey);
extern  int gmovetext(int left,int top,int right,int bottom,int destleft,int desttop);
extern  void bputxy(int x,int y,char *str);
extern  int do_edit(char *choose_name);
extern  int comp_file(const struct DIR_BUF *buf1,const struct DIR_BUF *buf2);
extern  int make_file_buff(struct DIR_ST *buf);
extern  int sys_set(void );
extern  int read_para(int argc,char * *argv);
extern  int do_environ(char *cmd,char *fn1,char *fn2);
extern  int do_execute(char *fn1);
extern  int init_screen(void );
extern  void c_exit(int sig);
extern  int exit_screen(void );
extern  int do_hist(int idx,char *choose_name);
extern  int run_system(int wait,char *frm,...);
extern  int do_ppc(char *filename);
extern  int ppc(char *filename,struct PPC_ST *s);
extern  int disk_msg(int cur_sec,int step_sec,int all_sec,int mode);
extern  int disk_duplicator(void );
extern  int week_day(int year,int month,int day);
extern  int show_disk_info(void );
extern  int check_hard(void );
extern  int get_disk_data(int drive,struct FAT_ID *fat_id);
extern  int log_disk(char *msg);
extern  int drv_status(int drv);
extern  int choose_disk(char *msg);
extern  int abs_read(void far *buf,int dn,unsigned short sec,unsigned short cnt);
extern  int abs_write(void far *buf,int dn,unsigned short sec,unsigned short cnt);
extern  int flush_disk(void );
extern  int fcopy(char *sf,char *df,int tf);
extern  int comp_ent(const struct DIR_ENT *buf1,const struct DIR_ENT *buf2);
extern  unsigned short read_root(struct SEC_NO *sec,struct FAT_ID *fat_id);
extern  unsigned short read_subd(struct SEC_NO *sec,struct FAT_ID *fat_id,int cluster);
extern  unsigned short read_subd16(struct SEC_NO *sec,struct FAT_ID *fat_id,int cluster);
extern  int dsort(char *path);
extern  int draw_cur(int x,int y);
extern  int do_cg_f(struct EDT_ST *buf);
extern  int do_myedit(char *filename);
extern  int insert_menber(struct EDT_ST *buf,int posi,char *ptr);
extern  int delete_menber(struct EDT_ST *buf,int posi);
extern  int mk_cur_posi(struct EDT_ST *buf);
extern  int show_aline(struct EDT_ST *buf);
extern  int ins_rpl_mode(void );
extern  int jump_fun(struct EDT_ST *buf);
extern  int edit_proc(struct EDT_ST *buf,char *fname);
extern  char *convert_string(char *c,int cnt,int *tab);
