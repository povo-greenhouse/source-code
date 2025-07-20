#include "light_test.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "light_system/growing_light.h"

void light_test_initialization(){
    grow_light_init();

    assert(grow_light_get_brightness() == 0);
    assert(grow_light_get_threshold() == 1500);
    assert(is_grow_light_on() == 0);
    assert(grow_light_get_mode() == 0);

}

void light_test_brightness_operations(){
    grow_light_set_brightness(550);
    assert(grow_light_get_brightness() == 550);

    grow_light_set_brightness(3000);
    assert(grow_light_get_brightness() == 3000);

    grow_light_set_brightness(0);
    assert(grow_light_get_brightness() == 0);

    grow_light_set_brightness(1290);
    assert(grow_light_get_brightness() == 1290);
    
}

void light_test_threshold_operations(){

    assert(grow_light_get_mode() == false);

    grow_light_set_threshold(50);
    assert(grow_light_get_threshold() == MIN_BRIGHTNESS);

    grow_light_set_threshold(10000);
    assert(grow_light_get_threshold() == MAX_BRIGHTNESS);

    grow_light_set_mode(true);
    assert(grow_light_get_mode() == true);

    grow_light_set_threshold(550);
    assert(grow_light_get_threshold() == 550);

    grow_light_set_threshold(6000);
    assert(grow_light_get_threshold() == MAX_BRIGHTNESS);

    grow_light_set_threshold(0);
    assert(grow_light_get_threshold() == MIN_BRIGHTNESS);

    grow_light_set_threshold(1290);
    assert(grow_light_get_threshold() == 1290);
    grow_light_set_threshold(500);
    assert(grow_light_get_threshold() == 500);

    grow_light_set_mode(false);
}

void light_test_mode_operations(){

    grow_light_set_mode(true);
    assert(grow_light_get_mode() == true);

    grow_light_set_mode(false);
    assert(grow_light_get_mode() == false);

}
void light_test_power_operations() {

    power_on_or_off(1);
    assert(is_grow_light_on() == false);

    grow_light_set_mode(true);
    power_on_or_off(1);
    assert(is_grow_light_on() == true);

    power_on_or_off(0);
    assert(is_grow_light_on() == false);
}

void light_test_update(){

    grow_light_set_mode(true);
    update_light_hal(0x0000); // 0
    assert(is_grow_light_on() == false);
    assert(grow_light_get_mode() == true);


    grow_light_set_mode(false);
    update_light_hal(0x6000); // 0
    assert(is_grow_light_on() == true);
    assert(grow_light_get_brightness() == 3000);

    update_light_hal(0x80F0); // 960
    assert(is_grow_light_on() == false);
    assert(grow_light_get_brightness() == 0);

    update_light_hal(0x60F0); // 240
    assert(is_grow_light_on() == 1);
    assert(grow_light_get_brightness() == 1560);
    update_light_hal(0x70F0); // 480
    assert(is_grow_light_on() == 1);
    assert(grow_light_get_brightness() == 120);


    update_light_hal(0x90F0); // 1920
    assert(is_grow_light_on() == 0);
    update_light_hal(0xA0F0); // 3840
    assert(is_grow_light_on() == 0);

}
int light_test_main(){

    light_test_initialization();
    light_test_brightness_operations();
    light_test_threshold_operations();
    light_test_mode_operations();
    light_test_power_operations();
    light_test_update();

    return 0;
}

