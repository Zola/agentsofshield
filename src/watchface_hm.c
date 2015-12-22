/*
 * =====================================================================================
 * 
 *    Corporation:  zuola.com
 *       Filename:  demo_watchface.c
 *         Author:  Zola , zuola.com@gmail.com
 *        Created:  2015年11月10日 23时30分30秒
 * 
 *    Description:  显示时间，
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
static int32_t g_app_mwd_window_id = -1;


/*小时分钟图层ID，通过该图层ID获取图层句柄*/
static int32_t g_app_mwd_time_layer_id = -1;

/*温度高度图层ID，通过该图层ID获取图层句柄*/
static int32_t g_app_mwd_layer_altitude_id = -1;
static int32_t g_app_mwd_layer_temperature_id = -1;
static int32_t g_app_mwd_layer_sport_id = -1;
static int32_t g_app_mwd_layer_pressure_id = -1;
static int32_t g_app_mwd_battery_layer_id = -1;

static uint8_t app_timer_change_id = -1;

/*蓝牙图层ID，通过该图层ID获取图层句柄*/
static int32_t g_app_mwd_layer_ble_id = -1;

/*定时器状态static int accel_event = 0;*/
static	int accel_event = 0;
static	int second_timezone = 0;

/*蓝牙状态*/


/*显示小时分钟文本图层*/
#define MWD_HM_ORIGIN_X		0
#define MWD_HM_ORIGIN_Y		70
#define MWD_HM_SIZE_H		48		
#define MWD_HM_SIZE_W		128


void app_mwd_watch_time_change(enum SysEventType type, void *context)
{

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
			P_Layer layer_sport = app_window_get_layer_by_id(p_window, g_app_mwd_layer_sport_id);
		if (NULL == layer_sport)
		{
			return;
		}

		P_Layer layer_pressure = app_window_get_layer_by_id(p_window, g_app_mwd_layer_pressure_id);
		if (NULL == layer_pressure)
		{
			return;
		}

		P_Layer layer_battery = app_window_get_layer_by_id(p_window, g_app_mwd_battery_layer_id);
		if (NULL == layer_battery)
		{
			return;
		}
		
		struct date_time datetime;
		app_service_get_datetime(&datetime);
		char str[20] ="", buf2[]="温度：25.5C", buf[]="海拔：8888m";

	/*更新时间*/
		sprintf(str, "%d:%02d", datetime.hour, datetime.min);
		app_layer_set_text_text(p_hm_layer, str);	

	/*更新第二时区*/
	int hour=datetime.hour;
	if (hour == 7 || hour == 12 || hour == 19 )
	{}
	else
	{
	hour = (hour+24-7)%24;
 
		sprintf(str, "%02d:%02d", hour, datetime.min);
		app_layer_set_text_text(layer_battery, str);	
	}

	/*更新海拔*/
	float altitude, accuracy;
	int res = maibu_get_altitude(&altitude, &accuracy);
	sprintf(buf, "%.1fm", altitude, accuracy);
//	if (0 == res)
//	{
		app_layer_set_text_text(layer_altitude, buf);	
//	}
	/*更新温度*/
	float temp;
	if (0 == maibu_get_temperature(&temp))
	{sprintf(buf2, "%.1f C", temp);
	
		app_layer_set_text_text(layer_temperature, buf2);	
	}
	/*更新运动步数*/

	SportData data;
	if (0 == maibu_get_sport_data(&data, 0))
	{
		sprintf(str, "%d", data.step);	
		app_layer_set_text_text(layer_sport, str);	
	}

	/*更新气压*/
	float pressure;
	if (0 == maibu_get_pressure(&pressure))
	{
		sprintf(str, "%0.1f", pressure);
		app_layer_set_text_text(layer_pressure, str);	
	}


		app_window_update(p_window);
	}
}


/*定时器，更新第二时区或剩余电量*/
void app_timer_change(date_time_t tick_time, uint32_t millis, void* context )
{
	P_Window p_window = app_window_stack_get_window_by_id(g_app_mwd_window_id);	
		if (NULL == p_window)
		{
			return ;
		}

	P_Layer layer_battery = app_window_get_layer_by_id(p_window, g_app_mwd_battery_layer_id);
		if (NULL == layer_battery)
		{
			return;
		}

	/*更新第二时区*/
	int8_t percent;
	int16_t x, y, z;
	char str[20] = "";

 	maibu_get_accel_data(&x, &y, &z);
	int accelx=x;
	int accely=y;
	int accelz=z;
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	int hour=datetime.hour;
	maibu_get_battery_percent(&percent);
	
	if (accelx < 2100 && accelx > 1960 && accely > 1950 && accely < 2080 && accelz > 2200 && accelz < 2350  && accel_event == 0)
		{
				

	if(second_timezone == 0)
		{
		/* 手腕向下  */
		hour = (hour+24-7)%24;
		sprintf(str, "%02d:%02d", hour, datetime.min);
		app_layer_set_text_text(layer_battery, str);		 
 		/*定时器状态，乒乓开关     */
		accel_event = 1;
		second_timezone = 1;	
		}
	else{
		/* 手腕向下  */
		sprintf(str, "电量:%d%%", percent);
		app_layer_set_text_text(layer_battery, str);
 		/*定时器状态，乒乓开关     */
		accel_event = 1;
		second_timezone = 0;	
		} 

		}


	if (accelx < 1900 && accely > 2000 && accely < 2080 && accelz > 1800 && accelz < 2250)
	{
		accel_event = 0; 
	} 

		if (accelx > 2200  && accely > 1900 && accely < 2080 && accelz > 1600 && accelz < 2250 )
	{
		accel_event = 0; 
	}

/*如果系统事件是蓝牙断开 
enum BleStatus ble_status = maibu_get_ble_status();
	if (ble_status == BLE_STATUS_CONNECTED)
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

	res_get_user_bitmap(BLE_BITMAP_DISCONNECTED, &bitmap_ble2);

	LayerBitmap layer_ble_bitmap = {bitmap_ble2, frame_ble, GAlignLeft};

	app_layer_set_bitmap_bitmap(p_ble_layer, &layer_ble_bitmap);
	 
	app_layer_set_bg_color(p_ble_layer, GColorBlack);
		
	}

 */

	app_window_update(p_window);
}


P_Window init_mwd_window()
{

	/*创建窗口，窗口中可以添加唯一的基本元素图层*/
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return 0;	
	}



	char str[20] = "";

	/*创建神盾局背景图*/
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

 
 




	/*创建海拔图层1.1API*/
	GRect frame_altitude = {{80, 0}, {15, 48}};

	float altitude, accuracy;
	int res = maibu_get_altitude(&altitude, &accuracy);



	sprintf(str, "%0.1fm", altitude, accuracy);

	/*生成文本结构体, 依次为文本内容、文本显示框架、对齐方式、字体字号*/
	LayerText text_altitude = {str, frame_altitude, GAlignCenter, U_ASCII_ARIALBD_12, 0};

	/*创建文本图层*/
	P_Layer layer_altitude = app_layer_create_text(&text_altitude);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_altitude, GColorBlack);
	/*添加文本图层到窗口中*/

	if(layer_altitude != NULL)
	{
		g_app_mwd_layer_altitude_id = app_window_add_layer(p_window, layer_altitude);
	}

	


	/*添加气压数据图层*/
	GRect frame_pressure = {{80, 15}, {14, 48}};
	float pressure;
	if (0 == maibu_get_pressure(&pressure))
	{
		sprintf(str, "%0.1f ", pressure);
		LayerText lt_pressure = {str, frame_pressure, GAlignCenter, U_ASCII_ARIAL_12, 0};
		P_Layer layer_pressure = app_layer_create_text(&lt_pressure);
		app_layer_set_bg_color(layer_pressure, GColorBlack);
		if(layer_pressure != NULL)
	{
		g_app_mwd_layer_pressure_id = app_window_add_layer(p_window, layer_pressure);
	}
	}




	/*创建温度图层1.1API*/
	GRect frame_temperature = {{80, 30}, {15, 48}};

	float temp;
	if (0 == maibu_get_temperature(&temp))
	{
	sprintf(str, "%0.1f C", temp);
	/*生成文本结构体, 依次为文本内容、文本显示框架、对齐方式、字体字号*/
	LayerText text_temp = {str, frame_temperature, GAlignCenter, U_ASCII_ARIALBD_12, 0};

	/*创建文本图层*/
	P_Layer layer_temperature = app_layer_create_text(&text_temp);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_temperature, GColorBlack);
	/*添加文本图层到窗口中*/
	if(layer_temperature != NULL)
	{
		g_app_mwd_layer_temperature_id = app_window_add_layer(p_window, layer_temperature);
	}
	}	


 
	/*添加运动数据图层*/
	GRect frame_step = {{80, 45}, {15, 48}};
	SportData data;
	if (0 == maibu_get_sport_data(&data, 0))
	{
		sprintf(str, "%d", data.step);
		LayerText lt_step = {str, frame_step, GAlignCenter, U_GBK_SIMSUNBD_12, 0};
		P_Layer layer_sport = app_layer_create_text(&lt_step);
		app_layer_set_bg_color(layer_sport, GColorBlack);
	if(layer_sport != NULL)
	{
		g_app_mwd_layer_sport_id = app_window_add_layer(p_window, layer_sport);
	}
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


	/*添加电量数据图层，仅在7点，12点和晚上19点这三个小时显示电量百分比*/
	GRect frame_battery ={{0, 62}, {14, 60}};
	int8_t percent;
	maibu_get_battery_percent(&percent);
	int hour=d.hour;
	if (hour == 7 || hour == 12 || hour == 19 )
	{
		sprintf(str, "电量:%d%%", percent);
		LayerText lt_battery = {str, frame_battery, GAlignCenter, U_GBK_SIMSUN_12, 0};
		P_Layer layer_battery = app_layer_create_text(&lt_battery);
		app_layer_set_bg_color(layer_battery, GColorBlack);
		if(layer_battery != NULL)
		{
			g_app_mwd_battery_layer_id = app_window_add_layer(p_window, layer_battery);
		}
	}
	/*显示第二时区,减7小时*/
else{
	
	hour = (hour+24-7)%24;
	char time_str[20] = "";
	sprintf(time_str, "%02d:%02d", hour, d.min);
	LayerText lt_battery = {time_str, frame_battery, GAlignCenter, U_GBK_SIMSUN_12, 0};
	P_Layer layer_battery = app_layer_create_text(&lt_battery);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(layer_battery, GColorBlack);

	if(layer_battery != NULL)
	{
		g_app_mwd_battery_layer_id = app_window_add_layer(p_window, layer_battery);
	}

	}

	/*创建状态栏*/
//	app_plug_status_bar_create(p_window, NULL, NULL, NULL);
//	app_plug_status_bar_add_time(p_window);		/*状态栏中添加时间*/
//	app_plug_status_bar_add_ble(p_window);		/*状态栏中添加蓝牙状态*/
//	app_plug_status_bar_add_battery(p_window);	/*状态栏中添加电量状态*/


	/*添加日期图层*/
	GRect frame_date = {{0, 110}, {20, 128}};
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
	GRect frame_week = {{0, 62}, {14, 110}};
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

	LayerText lt_week = {week_str, frame_week, GAlignRight, U_GBK_SIMSUN_12, 0};
	P_Layer p_week_layer = app_layer_create_text(&lt_week);
	/*反色图层，弄为黑色*/
	app_layer_set_bg_color(p_week_layer, GColorBlack);
	
	app_window_add_layer(p_window, p_week_layer);
	

/*添加蓝牙状态图层
	GRect frame_ble = {{0, 0}, {14, 18}};
	GBitmap bitmap_ble;	

 
	res_get_user_bitmap(DOT_BITMAP_TEST, &bitmap_ble);

 
	LayerBitmap layer_ble_bitmap = {bitmap_ble, frame_ble, GAlignLeft};

 	P_Layer p_ble_layer = app_layer_create_bitmap(&layer_ble_bitmap);

 
	app_layer_set_bg_color(p_ble_layer, GColorBlack);
	
 
	if(p_ble_layer != NULL)
	{
		g_app_mwd_layer_ble_id = app_window_add_layer(p_window, p_ble_layer);
	}
 */

 	/*创建位图图层DOT,作用是代替显示ºC，*/
	GRect frame_dot = {{110, 32}, {2, 2}};
	GBitmap bitmap_dot;	

	/*获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置*/
	res_get_user_bitmap(DOT_BITMAP_TEST, &bitmap_dot);

	/*生成位图结构体, 依次为位图资源、显示位置、对齐方式*/
	LayerBitmap layer_bitmap_dot = {bitmap_dot, frame_dot, GAlignRight};

	/*创建图层*/
	P_Layer layer_dot = app_layer_create_bitmap(&layer_bitmap_dot);

	/*反色图层，弄为黑色*/
	//app_layer_set_bg_color(layer_dot, GColorBlack);

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
/*每300毫米刷新*/
	app_timer_change_id = app_service_timer_subscribe(100, app_timer_change, NULL);

}



