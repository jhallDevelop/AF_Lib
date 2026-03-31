#include "AF_TestFramework.h"
#include "AF_Input.h"

void Test_Input_ZeroInit(void){
    AF_Input input = AF_Input_ZERO();
    AF_TEST_ASSERT(input.firstMouse == AF_TRUE, "AF_Input_ZERO should set firstMouse true");
    AF_Key* key = AF_Input_GetKey(AF_KEY_A, &input);
    AF_TEST_ASSERT(key != NULL, "AF_Input_GetKey should not return NULL");
}
