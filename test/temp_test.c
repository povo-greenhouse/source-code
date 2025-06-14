#include "temp_test.h"

#include <stdio.h>
#include <assert.h>
#include "environment_systems/temperature.h"
void temp_test_initialization(){

    assert(temp_get_lower_threshold() == 20);
    assert(temp_get_higher_threshold() == 40);
    assert(temp_get_current_temperature() == 21);

}
void temp_test_threshold_operations(){
    temp_set_lower_threshold(15);
    assert(temp_get_lower_threshold() == 15);

    temp_set_higher_threshold(35);
    assert(temp_get_higher_threshold() == 35);

    temp_set_lower_threshold(45); // should not change
    assert(temp_get_lower_threshold() == 15);

    temp_set_higher_threshold(5); // should not change
    assert(temp_get_higher_threshold() == 35);
}
void temp_test_comparasion_operations() {
    temp_set_current_temperature(20);
    assert(would_goldilocks_like_this() == 0);

    temp_set_current_temperature(10);
    assert(would_goldilocks_like_this() == -1);

    temp_set_current_temperature(50);
    assert(would_goldilocks_like_this() == 1);
}
void temp_update_test(){
    uint8_t i=0;
    for(;i<10;i++){
        update_temperature();
    }
}
int temp_test_main(){

    temp_test_initialization();
    temp_test_threshold_operations();
    temp_test_comparasion_operations();

    return 0;
}
