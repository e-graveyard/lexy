#include "../ptest.h"
#include "../../core/fmt.h"


static void
test_strequ(void)
{
    char* value = "Acme Inc.";

    PT_ASSERT(strequ(value, "Acme Inc."));
    PT_ASSERT(!strequ(value, "Acme Corp."));
}

static void
test_isfint(void)
{
    float w = 1.0,
          x = 0.5,
          y = 0.999999,
          z = 0.9999991;

    PT_ASSERT(isvint(w));
    PT_ASSERT(!isvint(x));
    PT_ASSERT(!isvint(y));
    PT_ASSERT(isvint(z));
}

void
suite_fmt(void)
{
    char* suite_name = "Suite 'fmt'";

    pt_add_test(test_strequ, "Test 'strequ'", suite_name);
    pt_add_test(test_isfint, "Test 'isvint'", suite_name);
}


int
main(int argc, char** argv)
{
    pt_add_suite(suite_fmt);
    return pt_run();
}
