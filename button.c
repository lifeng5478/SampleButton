/************************************************************
  * @brief   按键驱动
	* @param   NULL
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    button.c
  ***********************************************************/

#include "button.h"

struct button *Head_Button;


#define TRIGGER_CB(event)   \
        if(btn->CallBack_Function[event]) \
          btn->CallBack_Function[event]((Button_t*)btn)
		  
static void Add_Button(Button_t* btn)
{
  btn->Next = Head_Button;
  Head_Button = btn;
}

/************************************************************
  * @brief   按键创建
	* @param   name : 按键名称
	* @param   btn : 按键结构体
  * @param   read_btn_level : 按键电平读取函数，需要用户自己实现返回uint8_t类型的电平
  * @param   btn_trigger_level : 按键触发电平	高电平1，低电平0
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Create_Button(const char *name,Button_t *btn,uint8_t (*read_button_level)(void),uint8_t triggle)
{
      if( btn == NULL)
  {
    PRINT_ERR("struct button is null!");
  }
    StrnCopy(btn->Name, name, BTN_NAME_MAX); /* 创建按键名称 */
  btn->Button_State = NONE_TRIGGER;           //按键状态
  btn->Button_Trigger_Event = NONE_TRIGGER;   //按键触发事件
  btn->Read_Button_Level = read_button_level;    //按键读电平函数
  btn->Button_Trigger_Level = triggle;  //按键触发电平
  btn->Debounce_Time = 0;
  Add_Button(btn);          //创建的时候添加到单链表中
}


/************************************************************
  * @brief   按键附加功能
	* @param   btn : 按键结构体
  * @param   bothway : 双向按键功能使能
  * @param   combination : 联合按键使能
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Special_Func(Button_t *btn,BOOL_BTN bothway,BOOL_BTN combination)
{
    if(btn == NULL)
  {
    PRINT_ERR("struct button is null!");
  }
  btn->bothway_flag = bothway;
  btn->combination = combination;
}


/************************************************************
  * @brief   按键扫描
	* @param   btn : 按键结构体
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void BUTTON_SCAN(Button_t *btn)
{
    uint8_t current_level = (uint8_t)btn->Read_Button_Level();//获取当前按键电平
    if(btn->bothway_flag != TRUE&&btn->combination !=TRUE)
    {
    if(current_level == btn->Button_Trigger_Level)
    {
        btn->Btn_Lock = 0;
        btn->Timer_Count=0;
        if(btn->short_flag == 1)
        {
            btn->Button_State = BUTTON_DOWM;
        }
    }
    else if(btn->Btn_Lock==0)
    {
        btn->Timer_Count++;
        if(btn->Timer_Count>BTN_SHORT_TIME)
        {
            btn->short_flag =1;
        }
        if(btn->Timer_Count>BTN_LONG_TIME)
        {
            btn->Timer_Count=0;
            btn->short_flag = 0;
            btn->Btn_Lock = 1;
            btn->Button_State = BUTTON_LONG;
        }
    }
    }
    if(btn->bothway_flag==TRUE)
    {
        if(current_level==btn->Button_Trigger_Level)//双向抗抖程序
		{
			btn->inlock = 0;
			if (btn->outlock==0)
			{
					btn->Timer_Count++;
					if (btn->Timer_Count>BOTHWAY_TIME)
					{
						btn->Timer_Count=0;
						btn->outlock=1;
						btn->Button_State=BUTTON_BOTH_IN;
					}
			}
		}

		else if(btn->inlock==0)
		{
			btn->outlock=0;
			btn->Timer_Count=0;
			if(btn->Timer_Count>BOTHWAY_TIME)
			{
				btn->Timer_Count=0;
				btn->inlock=1;
				btn->Button_State=BUTTON_BOTH_OUT;
			}
		}
    }

}

/************************************************************
  * @brief   所有按键链表式扫描
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    需要在1ms定时器中周期性扫描（用户置入）
  ***********************************************************/
void Button_Scan_Process(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
      BUTTON_SCAN(pass_btn);
  }
}


/************************************************************
  * @brief   按键服务程序
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    需要在主程序调用
  ***********************************************************/
void Button_Process_Service(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
  uint8_t btn_event = Get_Button_Event(pass_btn);
      TRIGGER_CB(btn_event);
      pass_btn->Button_State = NONE_TRIGGER;
  }
}


/************************************************************
  * @brief   按键触发事件映射
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    初始化后调用
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if( btn == NULL)
  {
    PRINT_ERR("struct button is null!");
    //ASSERT(ASSERT_ERR);       //断言
  }
  
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
  }
}