

#include "user_button.h"


//定式扫描定义
EmberEventControl buttonScanEventControl;


void buttonScanEventHandler()
{

	emberEventControlSetInactive(buttonScanEventControl);

	user_button_scan_handle();   //处理按键扫描 以及事件回调 回调事件在初始化按键数组中
	// emberAfDebugPrintln("g_btn_status_reg = %02x g_logic_level = %02x\n" ,g_btn_status_reg,g_logic_level);
    emberEventControlSetDelayMS(buttonScanEventControl,SCAN_INTERVAL_MS);


}


//引脚电平读取

uint8_t get_button_pin_level(void *arg)
{

	return GPIO_PinInGet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN);

}




//自定义事件回调

void Btn_click_CallBack(void *btn)
{
	emberAfDebugPrintln("ButtonHandler_event1 [KEY_BTN_PRESS_CLICK]\n");

	uint8_t status[2]={0x01,0x02};
	user_sendDataTo_mcu_handle(APP_UART_CMD_REPORT_WITH_ACK ,APP_PROID_REPORT_STATUS_WITH_ACK ,status ,2);
}


void Btn_Dobuleclick_CallBack(void *btn)
{
	emberAfDebugPrintln("ButtonHandler_event1 [KEY_BTN_PRESS_DOUBLE_CLICK]\n");
	 user_sendDataTo_mcu_handle(APP_UART_CMD_COMMAND ,APP_PROID_COMMAND_JOINNET ,NULL ,0);
}



void Btn_down_CallBack(void *btn)
{
	emberAfDebugPrintln("ButtonHandler_event1 [KEY_BTN_PRESS_Down]\n");

}


void Btn_up_CallBack(void *btn)

{
	emberAfDebugPrintln("ButtonHandler_event1 [KEY_BTN_PRESS_UP]\n");

}




main()
{

...
  user_button_creat(USER_BUTTON,get_button_pin_level,UKEY_BUTTON_PRESSED_HIGH);
   button_event_add(USER_BUTTON,KEY_BTN_PRESS_DOWN,Btn_down_CallBack);
   button_event_add(USER_BUTTON,KEY_BTN_PRESS_UP,Btn_up_CallBack);

   button_event_add(USER_BUTTON,KEY_BTN_PRESS_CLICK,Btn_click_CallBack);
   button_event_add(USER_BUTTON,KEY_BTN_PRESS_DOUBLE_CLICK,Btn_Dobuleclick_CallBack);


...














}
