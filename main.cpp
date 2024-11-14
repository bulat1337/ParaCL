#include <string>     // for basic_string

#include "driver.hh"  // for Driver
#include "log.h"      // for LOG, MSG

int main(int argc, char **argv)
{

	MSG("MACROSES:\n");
	LOG("YYDEBUG: {}\n", YYDEBUG);
	LOG("YY_FLEX_DEBUG: {}\n", YY_FLEX_DEBUG);

    int res = 0;

    Driver drv;

    for (int i = 1; i < argc; ++i)
    	res = drv.parse(argv[i]);

    return res;
}
