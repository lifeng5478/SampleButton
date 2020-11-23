/************************************************************
  * @brief   按键驱动
	* @param   NULL
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    button.h
  ***********************************************************/

#ifndef __BUTTON_H_
#define __BUTTON_H_

#include "sdk_project_config.h"
#include "string.h"
#define BTN_NAME_MAX  32	//按键名称占用长度
#define BTN_SHORT_TIME 30	//短按时间，单位ms
#define BTN_LONG_TIME 1000	//长按时间，单位ms
#define BOTHWAY_TIME 300	//双向时间，单位ms

#define TRIGGER_CB(event)   \
        if(btn->CallBack_Function[event]) \
          btn->CallBack_Function[event]((Button_t*)btn)

typedef enum{
    FAULSE=0,
    TRUE,
}BOOL_BTN;

typedef enum {
  BUTTON_DOWM = 0,	//短按
  BUTTON_UP,		
  BUTTON_DOUBLE,	//双击
  BUTTON_LONG,		//长按
  BUTTON_LONG_FREE,	
  BUTTON_CONTINUOS,	//连续按
  BUTTON_CONTINUOS_FREE,
  BUTTON_BOTH_IN,	//双向按下
  BUTTON_BOTH_OUT,	//双向释放
  BUTTON_ALL_RIGGER,
  number_of_event,
  NONE_TRIGGER
}Button_Event;




typedef void (*Button_CallBack)(void*);   /* 按键触发回调函数，需要用户实现 */


typedef struct button
{
    uint8_t (*Read_Button_Level)(void);					//获取按键电平
    char Name[BTN_NAME_MAX];							//按键名称
    uint8_t Button_State              :   4;			//按键当前状态
    uint8_t Button_Trigger_Level      :   2;   			//按键触发电平
    uint8_t Button_Trigger_Event;   					//按键触发事件
    Button_CallBack CallBack_Function[number_of_event];	//按键回调函数
    uint8_t Button_Cycle;								
    uint16_t Timer_Count;								//按键计时
    uint8_t Debounce_Time;								
    uint8_t Long_Time;									//长按时间
    uint8_t Btn_Lock;									//按键锁定
    uint8_t short_flag;									//短按标记
    struct button *Next;								//按键链表指针
    uint8_t bothway_flag;								//双向标记
    uint8_t combination;								//联合按键标记
    uint8_t outlock;									//双向释放锁定
    uint8_t inlock;										//双向按下锁定
}Button_t;

void Create_Button(const char *name,Button_t *btn,uint8_t (*read_button_level)(void),uint8_t triggle);
void Button_Special_Func(Button_t *btn,BOOL_BTN bothway,BOOL_BTN combination);
void Button_Scan_Process(void);
void Button_Process_Service(void);
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback);
#endif
