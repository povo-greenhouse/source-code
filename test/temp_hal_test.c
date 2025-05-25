#include <stdio.h>
#include <assert.h>
#include "include/environment_systems/temp_hal.h"
#include "test/temp_hal_test.h"
void temp_test_initialization(TemperatureSensor *temp) {
    
    temp = Temperature_init();

    if(temp == NULL) {
        printf("TemperatureSensor is NULL\n");
        return;
    }

    assert((temp->get_lower_threshold(temp) - 10.0) < 0.0001);
    assert((temp->get_higher_threshold(temp) - 40.0) < 0.0001);
    assert((temp->get_current_temperature(temp) - 21.0) < 0.0001);

    assert(temp->init != NULL);
    assert(temp->set_lower_threshold != NULL);
    assert(temp->get_lower_threshold != NULL);
    assert(temp->get_higher_threshold != NULL);
    assert(temp->set_higher_threshold != NULL);
    assert(temp->get_current_temperature != NULL);
    assert(temp->set_current_temperature != NULL);
    assert(temp->would_goldilocks_like_this != NULL);
}
void temp_test_threshold_operations(TemperatureSensor *temp) {
    if(temp == NULL) {
        printf("TemperatureSensor is NULL\n");
        return;
    }
    temp->set_lower_threshold(temp, 15.0);
    assert((temp->get_lower_threshold(temp) - 15.0) < 0.0001);

    temp->set_higher_threshold(temp, 35.0);
    assert((temp->get_higher_threshold(temp) - 35.0) < 0.0001);

    temp->set_lower_threshold(temp, 45.0); // should not change
    assert((temp->get_lower_threshold(temp) - 15.0) < 0.0001);

    temp->set_higher_threshold(temp, 5.0); // should not change
    assert((temp->get_higher_threshold(temp) - 35.0) < 0.0001);
}
void temp_test_comparasion_operations(TemperatureSensor *temp) {
    if(temp == NULL) {
        printf("TemperatureSensor is NULL\n");
        return;
    }
    temp->set_current_temperature(temp, 20.0);
    assert(temp->would_goldilocks_like_this(temp) == 0);

    temp->set_current_temperature(temp, 10.0);
    assert(temp->would_goldilocks_like_this(temp) == -1);

    temp->set_current_temperature(temp, 50.0);
    assert(temp->would_goldilocks_like_this(temp) == 1);
}
int temp_test_main(){
    TemperatureSensor temp;

    temp_test_initialization(&temp);
    temp_test_threshold_operations(&temp);
    temp_test_comparasion_operations(&temp);

    return 0;
}
