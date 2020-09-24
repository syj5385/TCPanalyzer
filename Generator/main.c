#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

#include "package.h"
#include "network.h"
#include "result.h"
#include "define.h"

void printError(const char* message);
static pid_t *ifstat_pid,*tcpprobe_pid;
static sigset_t pset;
static boolean processed_result = false;
static pthread_t result_thread;

static void exitProcess(){
	kill(*ifstat_pid, SIGINT);
	kill(*tcpprobe_pid,SIGINT);
}

/* signal Handler */
static void sigHandler(int signo){
	if(signo == SIGINT){
		exitProcess();
		exit(1);
	}
	if(signo == SIGTSTP){
		exitProcess();
		exit(1);
	}
	if(signo == SIGCHLD){
		sleep(1);

		printf("\n\n<Making a result file...>\n");
		/* process output file*/
		printf("Make output\n");
		pthread_create(&result_thread,NULL,makeOutputFile,NULL);
		pthread_join(result_thread,NULL);

		exitProcess();

		////////////////////////
		processed_result=true;
		exit(1);
	}
	if(signo == SIGTERM){
		exitProcess();
		exit(1);
	}
}
static void initializeSignalHandler(){
	sigemptyset(&pset);
	sigaddset(&pset,SIGQUIT);
	sigprocmask(SIG_BLOCK, &pset, NULL);

	if(signal(SIGINT,sigHandler) == SIG_ERR){
		exit(1);
	}
	if(signal(SIGCHLD,sigHandler) == SIG_ERR){
		exit(1);
	}
	if(signal(SIGTSTP,sigHandler) == SIG_ERR){
		exit(1);
	}
	if(signal(SIGTERM,sigHandler) == SIG_ERR){
		exit(1);
	}
}


int main(int argc, char** argv){
	int i;
	int dialog_result;
	char *interface;
	char client[30];
	int* port;
	int* time;
	pthread_t ifstat_pthread,probe_pthread, iperf_pthread;


	printf("Start TENET Packet Generator\n");
	processed_result = false;


	/* check if essential package is installed */
	if(!isPackageInstalled("ifstat")){
		printf("Error : You cannot start process\n");
		return -1;
	}



	if(!isPackageInstalled("iperf3")){
		printf("Error : You cannot start process\n");
		return -1;
	}


	/* set congestion control algorithm */
	if(change_congestion_algorithm() == false){
		printf("Error : Failed to set congestion control algorithm\n");
	}

	interface = (char*)malloc(sizeof(char) * 100);
	bzero(interface,sizeof(interface));
	printf("\n<Network Interface>\n");
	printf("Interface >> ");
	fgets(interface, sizeof(interface), stdin);

	printf("\n<Client Information>\n");
	printf("client address >> ");
	bzero(client,sizeof(client));
	fgets(client, sizeof(client),stdin);

	port = (int*)malloc(sizeof(int));
	client[strlen(client)-1] = 0;
	printf("client port >> ");
	scanf( "%d", port);

	time = (int*)malloc(sizeof(int));
	printf("Time >> ");
	scanf( "%d", time);

	initialzieModProbe(port);

	initializeSignalHandler();

	/* start ifstat process */

	ifstat_pid = (pid_t*)malloc(sizeof(pid_t));
	if(ifstatProcess(interface, ifstat_pid) == false){
		exitProcess();
		return -1;
	}

	/* start tcpprobe module  */
	tcpprobe_pid = (pid_t*)malloc(sizeof(pid_t));

	if(tcpprobeModule(port, ifstat_pid) == false){
		exitProcess();
		return -1;
	}
	/* iperf */
	iperfProcess(client, *port, *time);



	return 0;
}

