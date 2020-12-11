/************************************************************
  * @brief   ��������
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
  * @brief   ��������
	* @param   name : ��������
	* @param   btn : �����ṹ��
  * @param   read_btn_level : ������ƽ��ȡ��������Ҫ�û��Լ�ʵ�ַ���uint8_t���͵ĵ�ƽ
  * @param   btn_trigger_level : ����������ƽ	�ߵ�ƽ1���͵�ƽ0
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
    StrnCopy(btn->Name, name, BTN_NAME_MAX); /* ������������ */
  btn->Button_State = NONE_TRIGGER;           //����״̬
  btn->Button_Trigger_Event = NONE_TRIGGER;   //���������¼�
  btn->Read_Button_Level = read_button_level;    //��������ƽ����
  btn->Button_Trigger_Level = triggle;  //����������ƽ
  btn->Debounce_Time = 0;
  Add_Button(btn);          //������ʱ�����ӵ���������
}


/************************************************************
  * @brief   �������ӹ���
	* @param   btn : �����ṹ��
  * @param   bothway : ˫�򰴼�����ʹ��
  * @param   combination : ���ϰ���ʹ��
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
  * @brief   ����ɨ��
	* @param   btn : �����ṹ��
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void BUTTON_SCAN(Button_t *btn)
{
    uint8_t current_level = (uint8_t)btn->Read_Button_Level();//��ȡ��ǰ������ƽ
    if(btn->bothway_flag != TRUE&&btn->combination !=TRUE)
    {
    if(current_level == btn->Button_Trigger_Level)
    {
        btn->Btn_Lock = 0;
        btn->Timer_Count=0;
        btn->short_lock = 0;

        if(btn->Button_Click_Time>0)
        {
            btn->Button_Double_Timer_Counter++;
            if(btn->Button_Double_Timer_Counter>BTN_DOUBOU_INTERVAL)
            {
                btn->Button_State = BUTTON_DOWM;
                btn->Timer_Count = 0;
            	btn->Button_Double_Timer_Counter = 0;
            	btn->Button_Click_Time = 0;
            }
        }
    }
    else if(btn->Btn_Lock==0)
    {
        btn->Timer_Count++;
        if(btn->Timer_Count>BTN_SHORT_TIME)
        {
//            btn->short_flag =1;
        	if(btn->short_lock==0)
        	{
            btn->short_lock = 1;
            btn->Button_Click_Time++;
        	}
            if(btn->Button_Click_Time>1)
            {
            	btn->Button_Click_Time = 0;
            	btn->Timer_Count = 0;
                btn->Btn_Lock = 1;
            	btn->Button_State = BUTTON_DOUBLE;
            }
        }
        if(btn->Timer_Count>BTN_LONG_TIME)
        {
            btn->Timer_Count=0;
            btn->short_lock = 0;
            btn->Btn_Lock = 1;
            btn->Button_Click_Time =0;
            btn->Button_State = BUTTON_LONG;
        }
    }
    }
    if(btn->bothway_flag==TRUE)
    {
        if(current_level==btn->Button_Trigger_Level)//˫�򿹶�����
		{
			btn->inlock = 0;
			if (btn->outlock==0)
			{
					btn->Timer_Count++;
					if (btn->Timer_Count>BOTHWAY_TIME)
					{
						btn->Timer_Count=0;
						btn->outlock=1;
						btn->Button_State=BUTTON_BOTH_OUT;
					}
			}
		}

		else if(btn->inlock==0)
		{
			btn->outlock=0;
			btn->Timer_Count++;
			if(btn->Timer_Count>BOTHWAY_TIME)
			{
				btn->Timer_Count=0;
				btn->inlock=1;
				btn->Button_State=BUTTON_BOTH_IN;
			}
		}
    }

}

/************************************************************
  * @brief   ���а�������ʽɨ��
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    ��Ҫ��1ms��ʱ����������ɨ�裨�û����룩
  ***********************************************************/
void Button_Scan_Process(void)
{
  struct button* btn;
  for(btn = Head_Button; btn != NULL; btn = btn->Next)
  {
      BUTTON_SCAN(btn);
  }
}


/************************************************************
  * @brief   �����������
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    ��Ҫ�����������
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
  * @brief   ���������¼�ӳ��
  * @return  NULL
  * @author  lifeng
  * @github  https://github.com/lifeng5478
  * @date    2020-11-23
  * @version v1.0
  * @note    ��ʼ�������
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if( btn == NULL)
  {
    PRINT_ERR("struct button is null!");
    //ASSERT(ASSERT_ERR);       //����
  }
  
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback; //�����¼������Ļص����������ڴ��������¼�
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //�����¼������Ļص����������ڴ��������¼�
  }
}