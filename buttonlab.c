/*
 * buttonlib.c
 */


#include "user_button.h"

#ifndef NULL
#define NULL 0
#endif



#define EVENT_SET_AND_EXEC_CB(btn, evt)                                        \
    do                                                                         \
    {                                                                          \
        btn->event = evt;                                                      \
        if(btn->CallBack_Function[evt])                                                            \
              btn->CallBack_Function[evt]((user_button_t*)btn);                                      \
    } while(0);




/*******************************************************************
 *                          变量声明
 *******************************************************************/

static user_button_t *btn_head = NULL;


static user_button_t user_button[USER_BUTTON_MAX];

/**
 * BTN_IS_PRESSED
 *
 * 1: is pressed
 * 0: is not pressed
*/
#define BTN_IS_PRESSED(i) (g_btn_status_reg & (1 << i))

/**
 * g_logic_level
 *
 * The logic level of the button pressed,
 * Each bit represents a button.
 *
 * First registered button, the logic level of the button pressed is
 * at the low bit of g_logic_level.
*/
uint32_t g_logic_level = (uint32_t)0;

/**
 * g_btn_status_reg
 *
 * The status register of all button, each bit records the pressing state of a button.
 *
 * First registered button, the pressing state of the button is
 * at the low bit of g_btn_status_reg.
*/
uint32_t g_btn_status_reg = (uint32_t)0;


/*******************************************************************
 *                         函数声明
 *******************************************************************/






/**
 * @brief Register a user button
 *
 * @param button: button structure instance
 * @return Number of keys that have been registered
*/
static uint8_t Add_Button(user_button_t *button)
{
    user_button_t *curr = btn_head;

    if (!button || (USER_BUTTON_MAX > sizeof(uint32_t) * 8))
    {
        return -1;
    }

    while (curr)
    {
        if(curr == button)
        {
            return -1;  //already exist.
        }
        curr = curr->next;
    }

    /**
     * First registered button is at the end of the 'linked list'.
     * btn_head points to the head of the 'linked list'.
    */
    button->next = btn_head;
    button->status = KEY_BTN_STAGE_DEFAULT;
    button->event = KEY_BTN_EVENT_NONE;
    button->scan_cnt = 0;
    button->click_cnt = 0;
    button->max_combos_click_solt = MAX_COMBOS_CLICK_SOLT;
    btn_head = button;

    /**
     * First registered button, the logic level of the button pressed is
     * at the low bit of g_logic_level.
    */
    g_logic_level |= (button->pressed_logic_level << (button->id));

    return 1;
}

/**
 * @brief Read all key values in one scan cycle
 *
 * @param void
 * @return none
*/
static void my_user_button_read(void)
{
    uint8_t button_index;
    user_button_t* target;

    /* The button that was registered first, the button value is in the low position of raw_data */
    uint32_t raw_data = 0;

    for(target = btn_head, button_index = USER_BUTTON_MAX - 1;
        (target != NULL) && (target->u_button_read != NULL);
        target = target->next, button_index--)
    {
        raw_data = raw_data | ((target->u_button_read)(target) << (target->id));
    }
    g_btn_status_reg = (~raw_data) ^ g_logic_level;

    //my_printf_uart("g_btn_status_reg = %02x g_logic_level = %02x\n" ,g_btn_status_reg,g_logic_level);
}


/**
 * @brief Handle all key events in one scan cycle.
 *        Must be used after 'user_button_read' API
 *
 * @param void
 * @return none
*/

static void user_button_process(void)
{
    uint8_t button_idx_num;
    user_button_t* target = NULL;

    for (target = btn_head, button_idx_num = USER_BUTTON_MAX - 1; target != NULL; target = target->next, button_idx_num --)
    {
          if (target->status > KEY_BTN_STAGE_DEFAULT)
          {
              target->scan_cnt ++;
          }

        switch (target->status)
        {
        case KEY_BTN_STAGE_DEFAULT: // stage: default(button up)

            if (BTN_IS_PRESSED(button_idx_num)) // is pressed
            {
            	//my_printf_uart("button_idx_num = %d, press\n",button_idx_num );
				target->scan_cnt = 0;
				target->click_cnt = 0;
				EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_DOWN);

				/* swtich to button down stage */
				target->status = KEY_BTN_STAGE_DOWN;


           }else
           {
        	    //my_printf_uart("button_idx_num = %d, no press and  scan end \n",button_idx_num );
                target->scan_cnt = 0;
           }
            break;

        case KEY_BTN_STAGE_DOWN: // stage: button down


            if (BTN_IS_PRESSED(button_idx_num)) // is pressed
            {

                if (target->click_cnt > 0) // combos
                {

                    if (target->scan_cnt > target->max_combos_click_solt)  //
                    {
                    	//my_printf_uart(" combos_click handle   end .... \n" );

                        EVENT_SET_AND_EXEC_CB(target,
                            target->click_cnt < KEY_BTN_PRESS_REPEAT_CLICK ?
                                target->click_cnt :
                                KEY_BTN_PRESS_REPEAT_CLICK);

                        /* swtich to button down stage */
                        target->status = KEY_BTN_STAGE_DOWN;
                        target->scan_cnt = 0;
                        target->click_cnt = 0;

                    }
                }
                else if (target->scan_cnt >= target->long_hold_start_tick)
                {
                    if (target->event != KEY_BTN_PRESS_LONG_HOLD)
                    {
                        EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_LONG_HOLD);
                    }
                }
                else if (target->scan_cnt >= target->long_press_start_tick)
                {
                    if (target->event != KEY_BTN_PRESS_LONG_START)
                    {
                        EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_LONG_START);
                    }
                }
                else if (target->scan_cnt >= target->short_press_start_tick)
                {
                    if (target->event != KEY_BTN_PRESS_SHORT_START)
                    {
                        EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_SHORT_START);
                    }
                }
            }
            else // is up
            {

                if (target->scan_cnt >= target->long_hold_start_tick)
                {
                    EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_LONG_HOLD_UP);
                    target->status = KEY_BTN_STAGE_DEFAULT;

                }
                else if (target->scan_cnt >= target->long_press_start_tick)
                {
                    EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_LONG_UP);
                    target->status = KEY_BTN_STAGE_DEFAULT;

                }
                else if (target->scan_cnt >= target->short_press_start_tick)
                {
                    EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_SHORT_UP);
                    target->status = KEY_BTN_STAGE_DEFAULT;


                }
                else
                {

                	//my_printf_uart("combos_click handle start...  \n" );
                    /* swtich to combos stage */
                    target->status = KEY_BTN_STAGE_COMBOS;
                    target->click_cnt ++;
                    EVENT_SET_AND_EXEC_CB(target, KEY_BTN_PRESS_UP);


                }
            }
            break;

        case KEY_BTN_STAGE_COMBOS: // stage: combos

            if (BTN_IS_PRESSED(button_idx_num)) // is pressed
            {
                /* swtich to button down stage */
                target->status = KEY_BTN_STAGE_DOWN;
                target->scan_cnt = 0;
            }
            else
            {

                if (target->scan_cnt > target->max_combos_click_solt)
                {
                	//my_printf_uart(" combos_click handle   end .... \n" );
                    EVENT_SET_AND_EXEC_CB(target,
                        target->click_cnt < KEY_BTN_PRESS_REPEAT_CLICK ?
                            target->click_cnt :
                            KEY_BTN_PRESS_REPEAT_CLICK);

                    /* swtich to default stage */
                    target->status = KEY_BTN_STAGE_DEFAULT;
                }
            }
            break;
        }

   }

}
/**
 * user_button_event_read
 *
 * @brief Get the button event of the specified button.
 *
 * @param button: button structure instance
 * @return button event
*/
user_button_event_t user_button_event_read(user_button_t* button)
{
    return (user_button_event_t)(button->event);
}

/**
 * user_button_scan
 *
 * @brief Start key scan.
 *        Need to be called cyclically within the specified period.
 *        Sample cycle: 5 - 20ms
 *
 * @param void
 * @return none
*/
void user_button_scan_handle(void)
{
	my_user_button_read();
    user_button_process();

}


void button_event_add(user_button_index_t button_idx,user_button_event_t btn_event,user_button_event_callback btn_callback)
{

  user_button[button_idx].CallBack_Function[btn_event] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
}


void user_button_creat(user_button_index_t button_idx,uint8_t(*get_pin_level)(),uint8_t active_level)
{

	//  memset(btn, 0, sizeof(user_button_t));  //清除结构体信息，建议用户在之前清除

    user_button[button_idx].id = button_idx;
    user_button[button_idx].u_button_read = get_pin_level;
    user_button[button_idx].pressed_logic_level = active_level;

    user_button[button_idx].debounce_tick = DEBOUNCE_TICKS_CNT;
    user_button[button_idx].short_press_start_tick = SHORT_TICKS_CNT;
    user_button[button_idx].long_press_start_tick = LONG_TICKS_CNT;
    user_button[button_idx].long_hold_start_tick = LONG_HOLD_TICKS_CNT;

    Add_Button(&user_button[button_idx]);

}


