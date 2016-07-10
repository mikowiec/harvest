#include "os/compat.h"

#include <iostream>
#include <stdio.h>

#include "net/sockstream.h"

#include "misc/test_main.h"

using reaper::net::sock_stream;

int test_main()
{
	sock_stream ss("licia.dtek.chalmers.se", 25);
	char buf[100];
	printf("%d %d %d %d\n", ss.good(), ss.bad(), ss.eof(), ss.fail());
	ss.getline(buf, 100);
	printf("%d %d %d %d\n", ss.good(), ss.bad(), ss.eof(), ss.fail());
	if (strncmp(buf, "220", 3))
		return 2;
	ss.write("HELO dummy\r\n", 12);
	printf("%d %d %d %d\n", ss.good(), ss.bad(), ss.eof(), ss.fail());
	ss << std::flush;
//	ss << "HELO dummy\r\n" << std::flush;
	printf("%d %d %d %d\n", ss.good(), ss.bad(), ss.eof(), ss.fail());
	ss.getline(buf, 100);
	printf("%d %d %d %d\n", ss.good(), ss.bad(), ss.eof(), ss.fail());
	if (strncmp(buf, "250", 3))
		return 3;
	return 0;
}

