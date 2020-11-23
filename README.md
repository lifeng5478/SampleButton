# SampleButton
简易按键驱动

## 使用方法
1. 在main函数初始化时	
   Create_Button("btn1", &btn1,Read_Button1_level, FAULSE);
	Button_Attach(&btn1, BUTTON_DOWM, Btn1_Dowm_CallBack);
	Button_Attach(&btn1, BUTTON_LONG, Btn1_Long_CallBack);

2. 在1ms定时器中 		Button_Scan_Process();
3. 在main函数大循环中执行
       	Button_Process_Service();

4. 加入回调函数
   void Btn1_Dowm_CallBack(void *btn)
    {
        ChangePWMDuty(0,500);

    }

    void Btn1_Long_CallBack(void *btn)
    {
        ChangePWMDuty(0,0);

    }

V1.0
目前支持长按，短按，双向，其余正在开发。。。