/*
 * libbutton.h
 *      Author: Administrator
 */

#ifndef LIBBUTTON_H_
#define LIBBUTTON_H_

#include "../include/af.h"

typedef void (*user_button_event_callback)(void *arg);



typedef enum
{
	USER_BUTTON  = 0,
    USER_BUTTON_MAX
} user_button_index_t;


#define UKEY_BUTTON_PRESSED_HIGH       1                             // 
#define UKEY_BUTTON_PRESSED_LOW        0                             //

#define SCAN_INTERVAL_MS           50                             //
#define DEBOUNCE_TICKS_CNT         3	                          //
#define SHORT_TICKS_CNT            (800 / SCAN_INTERVAL_MS)       // 1000ms
#define LONG_TICKS_CNT             (2400 / SCAN_INTERVAL_MS)      // 3s
#define LONG_HOLD_TICKS_CNT        (5000 / SCAN_INTERVAL_MS)      // 5s
#define MAX_COMBOS_CLICK_SOLT      (300 / SCAN_INTERVAL_MS)       // Combos slot, default 300ms */

/****************************user define  Region*******************************************/



typedef enum
{
    KEY_BTN_PRESS_DOWN = 0,
    KEY_BTN_PRESS_CLICK,
    KEY_BTN_PRESS_DOUBLE_CLICK,
    KEY_BTN_PRESS_TRIPLE_CLICK,
    KEY_BTN_PRESS_FOUR_CLICK,
    KEY_BTN_PRESS_FIVE_CLICK,
    KEY_BTN_PRESS_REPEAT_CLICK =6,    //事件
    KEY_BTN_PRESS_UP,
    KEY_BTN_PRESS_SHORT_START,
    KEY_BTN_PRESS_SHORT_UP,
    KEY_BTN_PRESS_LONG_START,
    KEY_BTN_PRESS_LONG_UP,
    KEY_BTN_PRESS_LONG_HOLD,
    KEY_BTN_PRESS_LONG_HOLD_UP,
    KEY_BTN_EVENT_MAX,
    KEY_BTN_EVENT_NONE,
} user_button_event_t;



/*****************************user define  Region**************************************/


enum KEY_BTN_STAGE
{
    KEY_BTN_STAGE_DEFAULT = 0,
    KEY_BTN_STAGE_DOWN    = 1,
    KEY_BTN_STAGE_COMBOS  = 2
};


typedef struct _user_button
{

    uint8_t  (*u_button_read)(void *);

    uint16_t  scan_cnt;
    uint16_t  click_cnt;
    uint16_t  max_combos_click_solt;

    uint16_t debounce_tick;
    uint16_t short_press_start_tick;
    uint16_t long_press_start_tick;
    uint16_t long_hold_start_tick;
    uint8_t  id;
    uint8_t  pressed_logic_level : 1;
    uint8_t  event               : 4;
    uint8_t  status              : 3;

    user_button_event_callback  CallBack_Function[KEY_BTN_EVENT_MAX];
    struct _user_button* next;
} user_button_t;



 void user_button_scan_handle(void);
 void user_button_creat(user_button_index_t button_idx,uint8_t(*get_pin_level)(),uint8_t active_level);
 void button_event_add(user_button_index_t button_idx,user_button_event_t btn_event,user_button_event_callback btn_callback);

 extern uint32_t g_btn_status_reg ;

 extern uint32_t g_logic_level;

#endif /* LIBBUTTON_H_ */
