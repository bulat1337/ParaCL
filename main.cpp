#include "driver.hh"
#include "log.h"

#include <iostream>

int main(int argc, char **argv)
{
    int res = 0;

	MSG("MACROSES:\n");
	LOG("YYDEBUG: {}\n", YYDEBUG);
	LOG("YY_FLEX_DEBUG: {}\n", YY_FLEX_DEBUG);

    Driver drv;

    for (int i = 1; i < argc; ++i)
    	drv.parse(argv[i]);

    return res;
}
