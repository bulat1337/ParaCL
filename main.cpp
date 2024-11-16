#include <string> // for basic_string

#include "driver.hh" // for Driver
#include "log.h"     // for LOG, MSG

int main(int argc, char **argv)
{

    MSG("MACROSES:\n");
    LOG("YYDEBUG: {}\n", YYDEBUG);
    LOG("YY_FLEX_DEBUG: {}\n", YY_FLEX_DEBUG);

    int status = 0;

    Driver drv;

    for (int i = 1; i < argc; ++i)
        status = drv.parse(argv[i]);

	LOG("global statements amount: {}\n", drv.ast.globalScope->nstms());

	drv.ast.eval();

    return status;
}
