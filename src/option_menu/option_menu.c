    /*
 * option_menu.c
 *
 *  Created on: May 13, 2025
 *      Author: riginel
 */

#include "option_menu/option_menu.h"
#include "option_menu/option_menu_input.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "scheduling/scheduler.h"
#include "uart_communication/uart_comm.h"
Graphics_Context * gc;

struct {
   task_list_index handle_input;
   task_list_index display_on_screen;
} option_menu_tasks;

int32_t option_menu_push_option(Option option) {
    if (option_list.len == MAX_OPTIONS) {
        return -1;
    }
    option_list.arr[option_list.len] = option;
    if (current_setting == -1) {
        current_setting = 0;
    }
    return option_list.len++;
}
int32_t option_menu_pop_option() {
    if (current_setting == option_list.len) {
        current_setting--;
    }
    if (option_list.len == 0) {
        return -1;
    }
    option_list.len--;
    return 1;
}

void option_menu_init_option_list() {
    option_list.len = 0;
    current_setting = 0;
    option_menu_is_enabled = true;
}

int option_menu_nav_next_option(){
    if ( current_setting == option_list.len-1){
        return -1;
    }
    return ++current_setting;

}
int option_menu_nav_prev_option(){
    if(current_setting == -1 || current_setting == 0){
        return -1;
    }

    return --current_setting;

}

void option_menu_decrement_current(){
    if(current_setting == -1){return;}
    option_decrement(&option_list.arr[current_setting]);
}
void option_menu_increment_current(){
    if(current_setting == -1){return;}
    option_increment(&option_list.arr[current_setting]);
}


void option_change_confirm(){
    int i;
    for (i =0;i<option_list.len;i++){
        if(option_list.arr[i].changed){
            option_list.arr[i].on_change_action(option_get_value(&option_list.arr[i]));
            option_list.arr[i].changed = false;
        }
    }
    return;
}


void option_menu_draw_current_option(){
    #ifdef SCREEN_OPTION_MENU_WORKS
    Graphics_clearDisplay(gc);

    if(current_setting == -1) {
        Graphics_drawStringCentered(gc,(int8_t *)"no option selected", 18, 64,64, OPAQUE_TEXT);
       // printf("no option selected");
        return;
    }
//    Graphics_Rectangle fill_rect = {
//                                    0,
//                                    50,
//                                    128,
//                                    128,
//
//    };
    Option curr = option_list.arr[current_setting];
    //Graphics_fillRectangle(gc,&fill_rect);
    Graphics_drawStringCentered(gc,
                        (int8_t *) curr.name,
                                OPTION_NAME_MAX_LENGTH,
                                64,
                                48,
                                OPAQUE_TEXT);

   // printf("%s\n",curr.name);
    char buf[20];
    curr.to_string(buf,option_get_value(&curr),20);
    //printf("%d: ",option_get_value(&curr));
    Graphics_drawStringCentered(gc,"<",1,32,64, OPAQUE_TEXT);
    Graphics_drawStringCentered(gc,(int8_t *) buf,20,64,64, OPAQUE_TEXT);
    Graphics_drawStringCentered(gc,">",1,96,64, OPAQUE_TEXT);

    //printf("%s\n",buf);
    #else
    if(current_setting == -1) {
            UART_write("no option selected", 18, NULL );
           // printf("no option selected");

            return;
    }
    Option curr = option_list.arr[current_setting];
        //Graphics_fillRectangle(gc,&fill_rect);

       // printf("%s\n",curr.name);

    char val_buf[20];
    curr.to_string(val_buf,option_get_value(&curr),20);
    char buf[80];
    snprintf(buf,80, "%s < %s >",curr.name,val_buf );

    UART_write(buf,80,NULL);

    #endif
}

void option_menu_toggle(){
    if(option_menu_is_enabled){
        option_menu_is_enabled = false;
        disable_task_at(option_menu_tasks.handle_input);
        disable_task_at(option_menu_tasks.display_on_screen);
    }else {
        option_menu_is_enabled = true;
        Graphics_drawStringCentered(gc, "Option Menu",20,64,20,OPAQUE_TEXT);
        enable_task_at(option_menu_tasks.handle_input);
        enable_task_at(option_menu_tasks.display_on_screen);
    }
}
void option_menu_handle_input(){
    ControllerInputOption input = input_buffer_dequeue();
    switch(input){
        case UP: {
            option_menu_nav_prev_option();
            //Graphics_drawStringCentered(gc,"UP PRESSED",11,100,64, OPAQUE_TEXT);
            //printf("UP PRESSED\n");
            break;
        }
        case DOWN: {
            option_menu_nav_next_option();
            //Graphics_drawStringCentered(gc,"DOWN PRESSED",13,100,64, OPAQUE_TEXT);
            //printf("DOWN PRESSED\n");
            break;
        }
        case LEFT: {
            option_menu_decrement_current();
            //Graphics_drawStringCentered(gc,"LEFT PRESSED",13,100,64, OPAQUE_TEXT);
            //printf("LEFT PRESSED\n");
            break;
        }
        case RIGHT: {
            option_menu_increment_current();
            //Graphics_drawStringCentered(gc,"RIGHT PRESSED",14,100,64, OPAQUE_TEXT);
            //printf("RIGHT PRESSED\n");
            break;
        }
        case BUTTON_A:{
            option_change_confirm();
            //Graphics_drawStringCentered(gc,"A PRESSED",10,100,64, OPAQUE_TEXT);
            //printf("A PRESSED\n");
            break;
        }
        default:{
            break;
        }
    }
}
void option_menu_init(Graphics_Context * graphics_context){
    gc = graphics_context;
    option_menu_init_option_list();
    init_option_menu_input();
    STask handle_input_task = {option_menu_handle_input,10,10,true};
    STask draw_current= {option_menu_draw_current_option,500,500,true};
    option_menu_tasks.handle_input = push_task(handle_input_task);
    option_menu_tasks.display_on_screen = push_task(draw_current);


}
