
#include "Unity-master/src/unity.h"
#include "assign1.h"

void test_byte_sort(void)
{
    TEST_ASSERT_EQUAL_HEX64(0xab80706050403020, byte_sort(0xab20304050607080));
    TEST_ASSERT_EQUAL_HEX64(0xffeeddccbbaa9988, byte_sort(0x8899aabbccddeeff));
    TEST_ASSERT_EQUAL_HEX64(0xefdebead04030201, byte_sort(0x0403deadbeef0201));

}

void test_nibble_sort(void)
{
    TEST_ASSERT_EQUAL_HEX64(0xfeeeddba43210000, nibble_sort(0x0403deadbeef0201));
    TEST_ASSERT_EQUAL_HEX64(0xfedcba9876543210, nibble_sort(0x0123456789abcdef));
    TEST_ASSERT_EQUAL_HEX64(0x1000000000000000, nibble_sort(0x0000000000000001));
    TEST_ASSERT_EQUAL_HEX64(0xfffffffffffffff1, nibble_sort(0xfffff1ffffffffff));

}

void test_name_list(void){
    struct elt *ans = name_list();
    TEST_ASSERT_EQUAL('R', ans->val);
    TEST_ASSERT_EQUAL('y', ans->link->val);
    TEST_ASSERT_EQUAL('a', ans->link->link->val);
    TEST_ASSERT_EQUAL('n', ans->link->link->link->val);
    TEST_ASSERT_EQUAL(NULL, ans->link->link->link->link);
    TEST_ASSERT_NOT_EQUAL('r', ans->val);
}

void test_convert(void){
    convert(HEX, 0x1234567891234567);
    convert(HEX, 0x1234567abc);
    convert(HEX, 0xdeadbeef);
    convert(BIN, 0x1234567abc);
    convert(BIN, 0xdeadbeef);
    convert(OCT, 0x1234567abc);
    convert(OCT, 0xdeadbeef);

}

void test_draw_me(void){
    draw_me();
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_byte_sort);
    RUN_TEST(test_nibble_sort);
    RUN_TEST(test_name_list);
    RUN_TEST(test_convert);
    RUN_TEST(test_draw_me);
    return UNITY_END();
}
