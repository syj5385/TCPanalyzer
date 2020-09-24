#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "result.h"

static FILE* file;
static FILE* new;

void processIFSTAT(){
	char buf[BUFSIZ];
	int count = 0;
	double timing = 0.0;
	char *ptr;
	printf("Make throughput file\n");
	if((file = fopen("./ifstat.tmp","r")) == NULL){
		printf("Error : Failed to process ifstat\n");
		return;
	}
	if((new = fopen("./output/throughput.xls","w+")) == NULL){
		printf("Error : Failed to process ifstat\n");
		fclose(file);
		return;
	}


	while(fgets(buf,BUFSIZ,file) != NULL){
		if(count++ < 2)
			continue;
		int inner_count =0;
		char file_buf[100];
		ptr = strtok(buf, " ");
		while(ptr != NULL){
//			printf("%d : %s\n",inner_count, ptr);
			if(inner_count == 1){
				bzero(file_buf,sizeof(file_buf));
//				sprintf(file_buf,"%.1lf\t%s\n",timing,ptr);
				fprintf(new,"%.1lf\t%s",timing,ptr);
				timing += 0.1;
			}
			inner_count ++;
			ptr = strtok(NULL," ");
		}

	}
	fclose(new);
	fclose(file);
}

void processTCPPROBE(const int position){

	char buf[BUFSIZ];
	char timing[BUFSIZ];
	int count = 0;
	char *ptr;
	char newname[BUFSIZ];
	bzero(newname,sizeof(newname));
	switch(position){
	case 6 :
		strcpy(newname, "./output/cwnd.xls");
		printf("make Congestion window file\n");
		break;
	case 9 :
		strcpy(newname, "./output/rtt.xls");
		printf("make rtt file\n");
		break;
	}

	if((file = fopen("./probe.tmp","r")) == NULL){
		printf("Error : Failed to process ifstat\n");
		return;
	}

	if((new = fopen(newname,"w+")) == NULL){
		printf("Error : Failed to process ifstat\n");
		fclose(file);
		return;
	}

	while(fgets(buf,BUFSIZ,file) != NULL){
		int inner_count = 0;
		char file_buf[BUFSIZ];
		ptr = strtok(buf," ");
		/* Timimg */

//		strcpy(timing, ptr);
		while(ptr != NULL){
			if(inner_count == 0){
				strcpy(timing,ptr);
			}
			if(inner_count == position){
//				sprintf(file_buf, "%s\t%s",timing, ptr);
//				puts(file_buf);
				fprintf(new,"%s\t%s\n",timing, ptr);
//				printf("%d : %s\n",inner_count, ptr);
				break;
			}
			inner_count++;
			ptr = strtok(NULL," ");
		}


	}

	fclose(new);
	fclose(file);
}

void* makeOutputFile(void* arg){
	processIFSTAT();
	processTCPPROBE(6);
	processTCPPROBE(9);

}
