

#include "package.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio_ext.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

boolean initialzieModProbe(int* port){
	char buf[100];
	bzero(buf,sizeof(buf));
	system("sudo modprobe -r tcp_probe");
	sprintf(buf,"sudo modprobe tcp_probe port=%d full=1",*port);
	system(buf);
	system("sudo chmod 444 /proc/net/tcpprobe");

	waitpid(-1,NULL,0);

}
boolean isPackageInstalled(char* package_name){

	pid_t child;
	char* arg_buf[3] = {"/usr/bin/dpkg","-l",};
	char system_buf[100];
	int pfd[2];
	int n=0;
	int res_count=0;
	char response;

	char buf[BUFSIZ];

	printf("Check if %s package is installed\n",package_name);
	if(pipe(pfd) <0){
		perror("pipe()");
		printf("Failed to open initialize to check package\n");
		return false;
	}

	if((child = vfork()) == 0){
		close(pfd[0]);
		dup2(pfd[1],1);
		arg_buf[2] = package_name;
		arg_buf[3] = 0;
		execv("/usr/bin/dpkg",arg_buf);
		close(pfd[1]);
		exit(127);
	}

	close(pfd[1]);
	if((n = read(pfd[0], buf, BUFSIZ)) <0){
		perror("read()");
		printf("Failed to read pipe data from dpkg\n");
		return false;
	}
	else if(n==0){
		/* Suggestion install package */
		printf("Do you want to install %s package?\n",package_name);
		printf("(y : install / n : will not install) : ");

		__fpurge(stdin);
		response = getchar();
		sleep(1);
		if(response == 'y'){
			printf("%s package will be installed\n",package_name);
			bzero(system_buf,sizeof(system_buf));
			sprintf(system_buf,"sudo apt-get install %s -y",package_name);
			printf("\n");
			system(system_buf);
			waitpid(-1,NULL,0);
			return true;
		}
		else if(response == 'n'){
			printf("This process will be exit process\n");
			return false;
		}
		else{
			printf("Wrong response type. Retry!!!\n");
			res_count++;
			response = 0;
			return false;
		}
	}
	else{
		/* this package is installed */
		printf("%s package is already installed\n",package_name);
		return true;
	}


	waitpid(child,NULL,0);
	return true;
}

boolean ifstatProcess(char* interface, pid_t* ifstat){
	char buf[100];
	int ifstat_fd;
	char* arg_buf[7] = {"/usr/bin/ifstat/","-n",};
	interface[strlen(interface)-1] = 0;

	bzero(buf,sizeof(buf));
	sprintf(buf,"ifstat -n -i %s -l 0.1",interface);

	if((ifstat_fd = open("./ifstat.tmp",O_RDWR|O_CREAT|O_TRUNC)) == -1){
		return false;
	}
	if(chmod("./ifstat.tmp",0777) == -1){
		return false;
	}

	if((*ifstat = vfork()) == 0){
		dup2(ifstat_fd,1);
		arg_buf[2] = "-i";
		arg_buf[3] = interface;
		arg_buf[4] = "-l";
		arg_buf[5] = "0.1";
		arg_buf[6] = 0;
//		printf("%s %s %s %s %s %s\n", arg_buf[0], arg_buf[1], arg_buf[2], arg_buf[3], arg_buf[4], arg_buf[5]);
		execv("/usr/bin/ifstat",arg_buf);
	}

}

boolean tcpprobeModule(int* port, pid_t* probe_pid){
	int probe_fd;

	printf("Start to modprobe\n");
	char* arg_buf[3] = {"/bin/cat","/proc/net/tcpprobe",0};
	if((probe_fd = open("./probe.tmp",O_RDWR|O_CREAT|O_TRUNC)) == -1){
		return false;
	}
	if(chmod("./probe.tmp",0777) == -1){
		return false;
	}
	if((*probe_pid = vfork()) == 0){
		dup2(probe_fd,1);
		execv("/bin/cat",arg_buf);
	}
}

void iperfProcess( char* client, int port, int time){
	char buf[100];
	sprintf(buf,"iperf3 -c %s -p %d -t %d",client, port, time);
	printf("%s\n",buf);
	system(buf);
}

