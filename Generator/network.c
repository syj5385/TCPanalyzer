
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "network.h"


boolean change_congestion_algorithm(){
	FILE* fp;
	char available[BUFSIZ];
	char cong[100];
	bzero(cong,sizeof(cong));


	if((fp = fopen("/proc/sys/net/ipv4/tcp_available_congestion_control","r")) == NULL){
		return false;
	}
	if(fgets(available,BUFSIZ,fp) == NULL){
		return false;
	}
	if(strstr(available,cong) == NULL){
		return false;
	}
	printf("\n\n<Congestion control algorithm>\n");
	printf("Available algorithm\n");
	printf("%s",available);
	printf(">> ");
	fgets(cong,100,stdin);

	bzero(available,sizeof(available));

	sprintf(available,"sudo sysctl -w net.ipv4.tcp_congestion_control=%s",cong);
	system(available);

	waitpid(-1,NULL,0);

	return true;
}
