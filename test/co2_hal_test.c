#include <stdio.h>
#include <assert.h>
#include "drivers/air/co2_hal.h"

void test_initialization(CO2 *co2) {
    
    co2 = GrowLight_init();

    if(co2 == NULL) {
        printf("CO2 is NULL\n");
        return;
    }
    assert(co2->get_current_level(co2) == 50);
    assert(co2->get_threshold(co2) == 500);
    assert(co2->is_exceeding_threshold(co2) == false);
    assert(co2->init != NULL);
    assert(co2->set_threshold != NULL);
    assert(co2->get_threshold != NULL);
    assert(co2->set_current_level != NULL);
    assert(co2->get_current_level != NULL);
    assert(co2->is_exceeding_threshold != NULL);

}

void test_threshold_operations(CO2 *co2) {
    if(co2 == NULL) {
        printf("CO2 is NULL\n");
        return;
    }
    co2->set_threshold(co2, 150);
    assert(co2->get_threshold(co2) == 150);

    co2->set_threshold(co2, 3000);
    assert(co2->get_threshold(co2) == 3000);

    co2->set_threshold(co2, 0);
    assert(co2->get_threshold(co2) == 0);

    co2->set_threshold(co2, 100);
    assert(co2->get_threshold(co2) == 100);
}

void test_level_operations(CO2 *co2) {
    if(co2 == NULL) {
        printf("CO2 is NULL\n");
        return;
    }

    co2->set_current_level(co2, 3000);
    assert(co2->get_current_level(co2) == 3000);

    co2->set_current_level(co2, 0);
    assert(co2->get_current_level(co2) == 0);

    co2->set_current_level(co2, 150);
    assert(co2->get_current_level(co2) == 150);
}
void test_comparasion_operations(CO2 *co2) {
    if(co2 == NULL) {
        printf("CO2 is NULL\n");
        return;
    }

    co2->set_current_level(co2, 3000);
    assert(co2->is_exceeding_threshold(co2) == true);

    co2->set_current_level(co2, 0);
    assert(co2->is_exceeding_threshold(co2) == false);

    co2->set_current_level(co2, 100);
    printf("Is exceeding: %d\n", co2->is_exceeding_threshold(co2));
    // assert(co2->is_exceeding_threshold(co2) == false);
    
}
int main(){
    CO2 co2;

    test_initialization(&co2);
    test_threshold_operations(&co2);
    test_level_operations(&co2);
    test_comparasion_operations(&co2);

    return 0;
}
