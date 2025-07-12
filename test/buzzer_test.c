#include "buzzer_test.h"
#include "environment_systems/buzzer.h"
#include <assert.h>

void buzzer_test_initialization() {
    init_buzzer();

    assert(is_buzzer_on() == false);
}

void buzzer_test_activation() {
    turn_on_buzzer();

    assert(is_buzzer_on() == true);
}
void buzzer_test_deactivation() {

    turn_off_buzzer(0, true);
    assert(is_buzzer_on() == true);

    turn_off_buzzer(1, true);
    assert(is_buzzer_on() == true);

    turn_off_buzzer(-1, true);
    assert(is_buzzer_on() == true);

    turn_off_buzzer(1, false);
    assert(is_buzzer_on() == true);

    turn_off_buzzer(-1, false);
    assert(is_buzzer_on() == true);

    turn_off_buzzer(0, false);
    assert(is_buzzer_on() == false);
}

void buzzer_test_main() {
    buzzer_test_initialization();
    buzzer_test_activation();
    buzzer_test_deactivation();

    return;
}