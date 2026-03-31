#include "AF_TestFramework.h"
#include "AF_MeshLoad.h"

void Test_MeshLoad_HeaderAvailable(void){
    AF_Assets assets = AF_Assets_ZERO();
    AF_TEST_ASSERT(assets.nextAvailableMesh == 0, "AF_Assets_ZERO should set nextAvailableMesh to 0");
}
