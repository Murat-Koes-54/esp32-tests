#include <limits.h>
#include "unity.h"
#include "arithmetics.h"

TEST_CASE("Arithmetic tests", "[arithmetics]")
{
    TEST_ASSERT_EQUAL(10, add(5, 5));
    TEST_ASSERT_EQUAL(12, add(10, 2));
}
