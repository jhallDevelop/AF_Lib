#include "AF_TestFramework.h"

int g_af_tests_passed = 0;
int g_af_tests_failed = 0;

// declare test functions
void Test_AddShader_Null(void);
void Test_AddShader_One(void);
void Test_AddMesh_ReturnsRightSlot(void);
void Test_AddTexture_AddThenGet(void);
void Test_GetTexture_NotFound(void);
void Test_AddFont_Duplicates(void);
void Test_Boundaries(void);
void Test_Input_ZeroInit(void);
void Test_MeshLoad_HeaderAvailable(void);

int main(void) {
    AF_RUN_TEST(Test_AddShader_Null);
    AF_RUN_TEST(Test_AddShader_One);
    AF_RUN_TEST(Test_AddMesh_ReturnsRightSlot);
    AF_RUN_TEST(Test_AddTexture_AddThenGet);
    AF_RUN_TEST(Test_GetTexture_NotFound);
    AF_RUN_TEST(Test_AddFont_Duplicates);
    AF_RUN_TEST(Test_Boundaries);
    AF_RUN_TEST(Test_Input_ZeroInit);
    AF_RUN_TEST(Test_MeshLoad_HeaderAvailable);

    AF_PRINT_SUMMARY();
    return g_af_tests_failed ? 1 : 0;
}
