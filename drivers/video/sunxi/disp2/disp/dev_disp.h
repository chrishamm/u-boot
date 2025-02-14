/*
 * drivers/video/sunxi/disp2/disp/dev_disp.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __DEV_DISP_H__
#define __DEV_DISP_H__

#include "de/bsp_display.h"

#if defined(SUPPORT_TV) && defined(CONFIG_HDMI_DISP2_SUNXI)
#include "../tv/drv_tv.h"
#endif

#if defined(SUPPORT_EDP) && defined(CONFIG_EDP_DISP2_SUNXI)
#include "../edp/drv_edp.h"
#endif

#if defined(SUPPORT_EDP) && defined(CONFIG_EDP2_DISP2_SUNXI)
#include "../edp2/drv_edp2.h"
#endif

#ifdef CONFIG_ION_SUNXI
#define FB_RESERVED_MEM
#endif

typedef enum
{
	DISPLAY_NORMAL = 0,
	DISPLAY_LIGHT_SLEEP = 1,
	DISPLAY_DEEP_SLEEP = 2,
	DISPLAY_BLANK = 4,
}disp_standby_flags;

enum disp_de_to_tcon_lcd_index {
	DE_TO_TCON_NOT_DEFINE = -1,
	TO_TCON0 = 0,
	TO_TCON1 = 1,
	TO_TCON2 = 2,
	TO_TCON3 = 3,
	TO_TCON4 = 4,
	TO_TCON5 = 5,
};

struct info_mm
{
	void *info_base;	/* Virtual address */
	uintptr_t mem_start;	/* Start of frame buffer mem */
				/* (physical address) */
	u32 mem_len;			/* Length of frame buffer mem */
};

struct proc_list
{
	void (*proc)(u32 screen_id);
	struct list_head list;
};

struct ioctl_list
{
	unsigned int cmd;
	int (*func)(unsigned int cmd, unsigned long arg);
	struct list_head list;
};

struct standby_cb_list
{
	int (*suspend)(void);
	int (*resume)(void);
	struct list_head list;
};

typedef struct disp_init_para
{
	bool                  b_init;
	enum disp_init_mode        disp_mode;//0:single screen0(fb0); 1:single screen1(fb0); 2:single screen2(fb0)

	//for screen0/1/2
	enum disp_output_type      output_type[DISP_SCREEN_NUM];
	unsigned int          output_mode[DISP_SCREEN_NUM];
	enum disp_de_to_tcon_lcd_index	to_lcd_index[8];
	//for fb0/1/2
	unsigned int          buffer_num[DISP_SCREEN_NUM];
	enum disp_pixel_format     format[DISP_SCREEN_NUM];
	unsigned int          fb_width[DISP_SCREEN_NUM];
	unsigned int          fb_height[DISP_SCREEN_NUM];

	unsigned int chn_cfg_mode;
}disp_init_para;

typedef struct disp_drv_info
{
	struct device           *dev;
	uintptr_t               reg_base[DISP_MOD_NUM];
	u32                     irq_no[DISP_MOD_NUM];
	struct clk              *mclk[DISP_MOD_NUM];

	disp_init_para          disp_init;
	struct disp_manager     *mgr[DISP_SCREEN_NUM];
	struct disp_eink_manager *eink_manager[1];

	struct proc_list        sync_proc_list;
	struct proc_list        sync_finish_proc_list;
	struct ioctl_list       ioctl_extend_list;
	struct standby_cb_list  stb_cb_list;
	struct mutex            mlock;
	struct work_struct      resume_work[DISP_SCREEN_NUM];
	struct work_struct      start_work;

	u32    		              exit_mode;//0:clean all  1:disable interrupt
	bool			              b_lcd_enabled[DISP_SCREEN_NUM];
	bool                    inited;//indicate driver if init
	disp_bsp_init_para      para;
#if defined(CONFIG_ION_SUNXI)
	struct ion_client *client;
	struct ion_handle *handle;
#endif
}disp_drv_info;

struct sunxi_disp_mod {
	disp_mod_id id;
	char name[32];
};

struct __fb_addr_para
{
	uintptr_t fb_paddr;
	int fb_size;
};

#define DISP_RESOURCE(res_name, res_start, res_end, res_flags) \
{\
	.start = (int __force)res_start, \
	.end = (int __force)res_end, \
	.flags = res_flags, \
	.name = #res_name \
},

typedef struct bmp_color_table_entry {
	u8	blue;
	u8	green;
	u8	red;
	u8	reserved;
} __attribute__ ((packed)) bmp_color_table_entry_t;

typedef struct bmp_header {
	/* Header */
	char signature[2];
	u32	file_size;
	u32	reserved;
	u32	data_offset;
	/* InfoHeader */
	u32	size;
	u32	width;
	u32	height;
	u16	planes;
	u16	bit_count;
	u32	compression;
	u32	image_size;
	u32	x_pixels_per_m;
	u32	y_pixels_per_m;
	u32	colors_used;
	u32	colors_important;
	/* ColorTable */

} __attribute__ ((packed)) bmp_header_t;

typedef struct bmp_image {
	bmp_header_t header;
	/* We use a zero sized array just as a placeholder for variable
	   sized array */
	bmp_color_table_entry_t color_table[0];
} bmp_image_t;

typedef struct
{
	int x;
	int y;
	int bit;
	void *buffer;
}
sunxi_bmp_store_t;

//int disp_open(struct inode *inode, struct file *file);
//int disp_release(struct inode *inode, struct file *file);
//ssize_t disp_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
//ssize_t disp_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
//int disp_mmap(struct file *file, struct vm_area_struct * vma);
//long disp_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

s32 disp_create_heap(u32 pHeapHead, u32 pHeapHeadPhy, u32 nHeapSize);
void *disp_malloc(u32 num_bytes, void *phy_addr);
void  disp_free(void *virt_addr, void* phy_addr, u32 num_bytes);

extern s32 disp_register_sync_proc(void (*proc)(u32));
extern s32 disp_unregister_sync_proc(void (*proc)(u32));
extern s32 disp_register_sync_finish_proc(void (*proc)(u32));
extern s32 disp_unregister_sync_finish_proc(void (*proc)(u32));
extern s32 disp_register_ioctl_func(unsigned int cmd, int (*proc)(unsigned int cmd, unsigned long arg));
extern s32 disp_unregister_ioctl_func(unsigned int cmd);
extern s32 disp_register_standby_func(int (*suspend)(void), int (*resume)(void));
extern s32 disp_unregister_standby_func(int (*suspend)(void), int (*resume)(void));
extern s32 composer_init(disp_drv_info *psg_disp_drv);
extern unsigned int composer_dump(char* buf);
extern s32 disp_tv_register(struct disp_tv_func * func);


extern disp_drv_info    g_disp_drv;

extern int sunxi_disp_get_source_ops(struct sunxi_disp_source_ops *src_ops);
extern s32 disp_lcd_open(u32 sel);
extern s32 disp_lcd_close(u32 sel);
#if defined(__LINUX_PLAT__)
extern s32 fb_init(struct platform_device *pdev);
extern s32 fb_exit(void);
#endif
extern int lcd_init(void);
extern s32 bsp_disp_get_fps(u32 disp);

#if defined(__LINUX_PLAT__)
s32 sunxi_get_fb_addr_para(struct __fb_addr_para *fb_addr_para);
s32 fb_draw_colorbar(char* base, u32 width, u32 height, struct fb_var_screeninfo *var);
s32 fb_draw_gray_pictures(char* base, u32 width, u32 height, struct fb_var_screeninfo *var);
s32 drv_disp_vsync_event(u32 sel);
#endif
void DRV_disp_int_process(u32 sel);
s32 Display_set_fb_timming(u32 sel);
unsigned int disp_boot_para_parse(void);
int disp_get_parameter_for_cmdlind(char *cmdline, char *name, char *value);
#if defined(CONFIG_AIOT_DISP_PARAM_UPDATE) && defined(CONFIG_FAT_WRITE)
int disp_updata_param(void);
#endif
//int flash_update_lcd_param(void);
#endif
