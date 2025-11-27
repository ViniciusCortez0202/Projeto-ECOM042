#include <zephyr/ztest.h>
#include "plate.h"

ZTEST(app_tests, test_plate_br_valid)
{
    zassert_true(plate_is_br("ABC1D23"), NULL);
}

ZTEST(app_tests, test_plate_br_invalid)
{
    zassert_false(plate_is_br("ABC 1123"), NULL);
}

ZTEST(app_tests, test_plate_ar_valid)
{
    zassert_true(plate_is_ar("AB 142 LH"), NULL);
}

ZTEST(app_tests, test_plate_ar_invalid)
{
    zassert_false(plate_is_ar("BC 1A2 LH"), NULL);
}

ZTEST(app_tests, test_plate_pr_valid)
{
    zassert_true(plate_is_pr("ABCD 234"), NULL);
}

ZTEST(app_tests, test_plate_pr_invalid)
{
    zassert_false(plate_is_pr("A1C 1234"), NULL);
}

ZTEST(app_tests, test_plate_pr_motorcycle_valid)
{
    zassert_true(plate_is_pr("123 ADFC"), NULL);
}

ZTEST(app_tests, test_plate_pr_motorcycle_invalid)
{
    zassert_false(plate_is_pr("A23 WWWW"), NULL);
}

ZTEST(app_tests, test_plate_ur_valid)
{
    zassert_true(plate_is_ur("ABC 1234"), NULL);
}

ZTEST(app_tests, test_plate_ur_invalid)
{
    zassert_false(plate_is_ur("AC4 0001"), NULL);
}

ZTEST(app_tests, test_plate_bo_valid)
{
    zassert_true(plate_is_bo("BB 12341"), NULL);
}

ZTEST(app_tests, test_plate_bo_invalid)
{
    zassert_false(plate_is_bo("11 AAAAA"), NULL);
}

ZTEST(app_tests, test_mercosul_plate_valid)
{
    zassert_true(plate_is_valid_mercosul("590 VBEA"), NULL);
}

ZTEST(app_tests, test_mercosul_plate_invalid)
{
    zassert_false(plate_is_valid_mercosul("KHI7Y3E"), NULL);
}

