#include <stdio.h>
#include <assert.h>
#include "drivers/temp/temp_hal.h"

void test_initialization(TemperatureSensor *temp) {
    
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
void test_threshold_operations(TemperatureSensor *temp) {
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
void test_comparasion_operations(TemperatureSensor *temp) {
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
int main(){
    TemperatureSensor temp;

    test_initialization(&temp);
    test_threshold_operations(&temp);
    test_comparasion_operations(&temp);

    return 0;
}
