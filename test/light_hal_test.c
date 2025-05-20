#include <stdio.h>
#include <assert.h>
#include "drivers/light/led_hal.h"

void test_initialization(GrowLight *light) {
    
    light = GrowLight_init();

    if(light == NULL) {
        printf("Light is NULL\n");
        return;
    }

    assert(light->get_brightness(light) == 0);
    assert(light->get_threshold(light) == 100);
    assert(light->on == false);
    assert(light->manual_mode == false);

    assert(light->init != NULL);
    assert(light->set_brightness != NULL);
    assert(light->get_brightness != NULL);
    assert(light->set_threshold != NULL);
    assert(light->get_threshold != NULL);
    assert(light->set_manual_mode != NULL);
    assert(light->get_manual_mode != NULL);
    assert(light->turn_on != NULL);
    assert(light->is_on != NULL);
}

void test_brightness_operations(GrowLight *light) {
    if(light == NULL) {
        printf("Light is NULL\n");
        return;
    }
    light->set_brightness(light, 50);
    assert(light->get_brightness(light) == 50);

    light->set_brightness(light, 300);
    assert(light->get_brightness(light) == 255);

    light->set_brightness(light, -50);
    assert(light->get_brightness(light) == 0);

    light->set_brightness(light, 0);
    assert(light->get_brightness(light) == 0);

    
}

void test_threshold_operations(GrowLight *light) {
    if(light == NULL) {
        printf("Light is NULL\n");
        return;
    }

    light->set_threshold(light, 150);
    assert(light->get_threshold(light) == 150);
    light->set_threshold(light, 3000);
    assert(light->get_threshold(light) == 3000);
    light->set_threshold(light, 0);
    assert(light->get_threshold(light) == 0);
}

void test_mode_operations(GrowLight *light) {
    if(light == NULL) {
        printf("Light is NULL\n");
        return;
    }
    light->set_manual_mode(light, true);
    assert(light->get_manual_mode(light) == true);

    light->set_manual_mode(light, false);
    assert(light->get_manual_mode(light) == false);

}
void test_power_operations(GrowLight *light) {
    if(light == NULL) {
        printf("Light is NULL\n");
        return;
    }
    light->turn_on(light, true);
    assert(light->is_on(light) == true);

    light->turn_on(light, false);
    assert(light->is_on(light) == false);
}
int main(){
    GrowLight light;

    test_initialization(&light);
    test_brightness_operations(&light);
    test_threshold_operations(&light);
    test_mode_operations(&light);
    test_power_operations(&light);

    return 0;
}
