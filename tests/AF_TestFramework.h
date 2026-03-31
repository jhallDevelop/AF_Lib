#ifndef AF_TEST_FRAMEWORK_H
#define AF_TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>

extern int g_af_tests_passed;
extern int g_af_tests_failed;

#define AF_TEST_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "[FAILED] %s:%d %s - %s\n", __FILE__, __LINE__, __func__, (msg)); \
        g_af_tests_failed++; \
        return; \
    } \
} while(0)

#define AF_TEST_EQUAL_INT(a, b) do { \
    int _a = (a); int _b = (b); \
    if (_a != _b) { \
        fprintf(stderr, "[FAILED] %s:%d %s - expected %d, got %d\n", __FILE__, __LINE__, __func__, _b, _a); \
        g_af_tests_failed++; \
        return; \
    } \
} while(0)

#define AF_RUN_TEST(fn) do { \
    int before = g_af_tests_failed; \
    fprintf(stdout, "[RUN] %s\n", #fn); \
    fn(); \
    if (g_af_tests_failed == before) { \
        g_af_tests_passed++; \
    } \
} while (0)

#define AF_PRINT_SUMMARY() do { \
    fprintf(stdout, "\n=== TEST SUMMARY ===\n"); \
    fprintf(stdout, "Passed: %d\n", g_af_tests_passed); \
    fprintf(stdout, "Failed: %d\n", g_af_tests_failed); \
} while(0)

#endif // AF_TEST_FRAMEWORK_H
