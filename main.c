
#include "tests/unit_tests.h"


int main() {

    run_address_arithmetic_tests();
    run_init_tests();
    run_malloc_tests();
    run_write_tests();
    run_read_tests();
    run_free_tests();

    return 0;
}
