#include "air_qual_test.h"

#include <stdio.h>
#include <assert.h>
#include "environment_systems/air_quality.h"

void air_test_threshold_operations() {
    air_set_threshold(150);
    assert(air_get_threshold() == 150);

    air_set_threshold(3000);
    assert(air_get_threshold() == 3000);

    air_set_threshold(0);
    assert(air_get_threshold() == 0);

    air_set_threshold(100);
    assert(air_get_threshold() == 100);
}

void air_test_level_operations(){

    air_set_level(3000);
    assert(air_get_level() == 3000);

    air_set_level(0);
    assert(air_get_level() == 0);

    air_set_level(150);
    assert(air_get_level() == 150);
}
void air_test_comparasion_operations(){

    air_set_level(3000);
    assert(exceeding_threshold() == true);

    air_set_level(0);
    assert(exceeding_threshold() == false);

    air_set_level(100);
    printf("Is exceeding: %d\n", exceeding_threshold());
    // assert(is_exceeding_threshold(co2) == false);

}
void air_test_initialization(){
    // do not air_init since it onlt inits the GPIO pin (entirely hardware)
    
    assert(air_get_threshold() == 450);
    assert(air_get_level() == 0);

}
int air_test_main(){

    air_test_initialization();
    air_test_threshold_operations();
    air_test_level_operations();
    air_test_comparasion_operations();

    return 0;
}
