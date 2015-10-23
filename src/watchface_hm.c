/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  demo_watchface.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2015年08月19日 15时30分40秒
 * 
 *    Description:  
 *
 * =====================================================================================
 *
 * =====================================================================================
 * 
 *   MODIFICATION HISTORY :
 *    
 *		     DATE :
 *		     DESC :
 * =====================================================================================
 */	
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"



/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int8_t g_app_mwd_window_id = -1;


/*小时分钟图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_mwd_time_layer_id = -1;

/*温度高度图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_mwd_layer_altitude_id = -1;
static int8_t g_app_mwd_layer_temperature_id = -1;

/*蓝牙图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_mwd_layer_ble_id = -1;

/*显示小时分钟文本图层*/
#define MWD_HM_ORIGIN_X		0
#define MWD_HM_ORIGIN_Y		65
#define MWD_HM_SIZE_H		48		
#define MWD_HM_SIZE_W		128



void app_mwd_watch_time_change(enum SysEventType type, void *context)
{
	/*如果系统事件是蓝牙断开 ，但由于抓不到SysEventTypeBleDisconnected的SysEventType，目前以下蓝牙状态抓不到*/
	if (type == SysEventTypeBleDisconnected)
	{
		P_Window p_window = app_window_stack_get_window_by_id(g_app_mwd_window_id);	
		if (NULL == p_window)
		{
			return ;
		}

		P_Layer p_ble_layer = app_window_get_layer_by_id(p_window, g_app_mwd_layer_ble_id);
		if (NULL == p_ble_layer)
		{
			return;
		}
	 GRect frame_ble = {{0, 0}, {14, 18}};
	GBitmap bitmap_ble2;	

	/*获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置*/
	res_get_user_bitmap(RES_BITMAP_SETTING_BLE_2, &bitmap_ble2);

	/*生成位图结构体, 依次为位图资源、显示位置、对齐方式*/
	LayerBitmap layer_ble_bitmap = {bitmap_ble2, frame_ble, GAlignLeft};

	/*创建图层*/
	app_layer_set_bitmap_bitmap(p_ble_layer, &layer_ble_bitmap);

	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(p_ble_layer, GColorBlack);
	

			app_window_update(p_window);
	}
 

	/*如果系统事件是时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_app_mwd_window_id);	
		if (NULL == p_window)
		{
			return ;
		}

		P_Layer p_hm_layer = app_window_get_layer_by_id(p_window, g_app_mwd_time_layer_id);
		if (NULL == p_hm_layer)
		{
			return;
		}

		P_Layer layer_altitude = app_window_get_layer_by_id(p_window, g_app_mwd_layer_altitude_id);
		if (NULL == layer_altitude)
		{
			return;
		}


		P_Layer layer_temperature = app_window_get_layer_by_id(p_window, g_app_mwd_layer_temperature_id);
		if (NULL == layer_temperature)
		{
			return;
		}
		
		struct date_time datetime;
		app_service_get_datetime(&datetime);
		char str[20] ="", buf2[]="温度：25.5C", buf[]="海拔：8888m";

	/*更新时间*/
		sprintf(str, "%d:%02d", datetime.hour, datetime.min);
		app_layer_set_text_text(p_hm_layer, str);	

	/*更新海拔*/
		sprintf(buf, "%.1fm", maibu_bsp_pressure_get_height());
		app_layer_set_text_text(layer_altitude, buf);	

	/*更新温度*/
		sprintf(buf2, "%.1f C", maibu_bsp_pressure_get_temperature());
		app_layer_set_text_text(layer_temperature, buf2);	

		app_window_update(p_window);
	}
}



P_Window init_mwd_window()
{

	/*创建窗口，窗口中可以添加唯一的基本元素图层*/
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return 0;	
	}




	/*创建位图图层显示框架范围，起始点{x=32,y=32},屏幕左上角为{0,0},高度宽度{h=64,w=64},位图大小必须小于等于框架*/
	GRect frame = {{0, 0}, {128, 128}};
	GBitmap bitmap;	

	/*获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置*/
	res_get_user_bitmap(RES_BITMAP_TEST, &bitmap);

	/*生成位图结构体, 依次为位图资源、显示位置、对齐方式*/
	LayerBitmap layer_bitmap = {bitmap, frame, GAlignCenter};



	/*创建图层*/
	P_Layer layer = app_layer_create_bitmap(&layer_bitmap);

	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer, GColorBlack);

	/*添加图层到窗口中*/
	app_window_add_layer(p_window, layer);

 


/*创建文本图层框架范围*/
	GRect frame_altitude = {{80, 27}, {22, 48}};

	unsigned char buf[]="8888m";
	sprintf(buf, "%.1fm", maibu_bsp_pressure_get_height());


	/*生成文本结构体, 依次为文本内容、文本显示框架、对齐方式、字体字号*/
	LayerText text = {buf, frame_altitude, GAlignLeft, U_GBK_SIMSUN_12};

	/*创建文本图层*/
	P_Layer layer_altitude = app_layer_create_text(&text);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_altitude, GColorBlack);
	/*添加文本图层到窗口中*/

	if(layer_altitude != NULL)
	{
		g_app_mwd_layer_altitude_id = app_window_add_layer(p_window, layer_altitude);
	}

	

 


/*创建文本图层框架范围*/
	GRect frame_temperature = {{80, 10}, {22, 48}};

	unsigned char buf2[]="25.5'C";
	sprintf(buf2, "%.1f C", maibu_bsp_pressure_get_temperature());

	/*生成文本结构体, 依次为文本内容、文本显示框架、对齐方式、字体字号*/
	LayerText text2 = {buf2, frame_temperature, GAlignLeft, U_GBK_SIMSUN_12};

	/*创建文本图层*/
	P_Layer layer_temperature = app_layer_create_text(&text2);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_temperature, GColorBlack);
	/*添加文本图层到窗口中*/
	if(layer_temperature != NULL)
	{
		g_app_mwd_layer_temperature_id = app_window_add_layer(p_window, layer_temperature);
	}


	

 

	/*添加小时分钟图层*/
	GRect frame_hm = {{MWD_HM_ORIGIN_X, MWD_HM_ORIGIN_Y}, {MWD_HM_SIZE_H, MWD_HM_SIZE_W}};
	struct date_time d;
	app_service_get_datetime(&d);
	char time_str[20] = "";
	sprintf(time_str, "%d:%02d", d.hour, d.min);
	LayerText lt_hm = {time_str, frame_hm, GAlignCenter, U_ASCII_ARIALBD_42, 0};
	P_Layer layer_text_hm = app_layer_create_text(&lt_hm);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_text_hm, GColorBlack);
	if(layer_text_hm != NULL)
	{
		g_app_mwd_time_layer_id = app_window_add_layer(p_window, layer_text_hm);
	}


	/*添加日期图层*/
	GRect frame_date = {{0, 110}, {24, 128}};
	struct date_time t;
	app_service_get_datetime(&t);
	char date_str[20] = "";
	sprintf(date_str, "%d年%d月%02d日", t.year, t.mon, t.mday);
	LayerText lt_date = {date_str, frame_date, GAlignCenter, U_GBK_SIMSUN_14, 0};
	P_Layer layer_text_date = app_layer_create_text(&lt_date);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_text_date, GColorBlack);
	app_window_add_layer(p_window, layer_text_date);
 

	/*添加星期状态图层*/
	GRect frame_week = {{0, 53}, {14, 110}};
	struct date_time w;
	app_service_get_datetime(&w);
	char week_str[20] = "";
	int num=w.wday;
	if (num== 0){
	sprintf(week_str, "星期天");
			}
	if (num== 1){
	sprintf(week_str, "星期一");
			}
	if (num== 2){
	sprintf(week_str, "星期二");
			}	 
	if (num== 3){
	sprintf(week_str, "星期三");
			}
	if (num== 4){
	sprintf(week_str, "星期四");
			}
	if (num== 5){
	sprintf(week_str, "星期五");
			}
	if (num== 6){
	sprintf(week_str, "星期六");
			}

	LayerText lt_week = {week_str, frame_week, GAlignRight, U_GBK_SIMSUN_14, 0};
	P_Layer p_week_layer = app_layer_create_text(&lt_week);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(p_week_layer, GColorBlack);
	
	app_window_add_layer(p_window, p_week_layer);
	

	/*添加蓝牙状态图层*/
	GRect frame_ble = {{0, 0}, {14, 18}};
	GBitmap bitmap_ble;	

	/*获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置*/
	res_get_user_bitmap(RES_BITMAP_SETTING_BLE_1, &bitmap_ble);

	/*生成位图结构体, 依次为位图资源、显示位置、对齐方式*/
	LayerBitmap layer_ble_bitmap = {bitmap_ble, frame_ble, GAlignLeft};

	/*创建图层*/
	P_Layer p_ble_layer = app_layer_create_bitmap(&layer_ble_bitmap);

	/*反色图层，弄为黑色*/
	//app_layer_set_bg_color(p_ble_layer, GColorBlack);
	
	/*添加图层到窗口中*/
	if(p_ble_layer != NULL)
	{
		g_app_mwd_layer_ble_id = app_window_add_layer(p_window, p_ble_layer);
	}
 

 	/*创建位图图层DOT,作用是代替显示ºC，起始点{x=105,y=16},屏幕左上角为{横,竖},高度宽度{h=1,w=1},位图大小必须小于等于框架*/
	GRect frame_dot = {{105, 16}, {1, 1}};
	GBitmap bitmap_dot;	

	/*获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置*/
	res_get_user_bitmap(DOT_BITMAP_TEST, &bitmap_dot);

	/*生成位图结构体, 依次为位图资源、显示位置、对齐方式*/
	LayerBitmap layer_bitmap_dot = {bitmap_dot, frame_dot, GAlignRight};



	/*创建图层*/
	P_Layer layer_dot = app_layer_create_bitmap(&layer_bitmap_dot);

	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_dot, GColorBlack);

	/*添加图层到窗口中*/
	app_window_add_layer(p_window, layer_dot);

 
	


	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
	maibu_service_sys_event_subscribe(app_mwd_watch_time_change);





	return p_window;

}



int main()
{

	/*创建显示时间窗口*/
	P_Window p_window = init_mwd_window();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_app_mwd_window_id = app_window_stack_push(p_window);
	}	


}



