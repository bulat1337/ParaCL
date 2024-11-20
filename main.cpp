#include <string>     // for basic_string

#include "ast.hh"     // for AST
#include "driver.hh"  // for Driver
#include "log.h"      // for LOG, MSG

int main(int argc, char **argv)
{

    MSG("MACROSES:\n");
    LOG("YYDEBUG: {}\n", YYDEBUG);
    LOG("YY_FLEX_DEBUG: {}\n", YY_FLEX_DEBUG);

    int status = 0;

    Driver drv;

	if (argc <= 1)
    {
        LOG("Reading from standard input.\n");
        status = drv.parse("");
    }
    else status = drv.parse(argv[1]);

    LOG("global statements amount: {}\n", drv.ast.globalScope->nstms());

    drv.ast.eval();

    return status;
}
