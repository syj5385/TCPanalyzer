#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include "define.h"
#include <stdlib.h>

static int running =0;

boolean initialzieModProbe(int* port);
boolean isPackageInstalled(char* package_name);
boolean ifstatProcess(char* interface, pid_t* ifstat);
boolean tcpprobeModule(int* port, pid_t* probe_pid);
void iperfProcess(char* client, int port, int time);




#endif /* PACKAGE_H_ */

