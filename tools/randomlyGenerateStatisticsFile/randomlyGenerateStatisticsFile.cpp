#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstring> 

void generateStatisticsFile(int);
void randomCTQ(int *, int *, int *);

int main() {
	
	srand(time(NULL));
	
	int line = 0;
	printf("How many lines do you want in your file?\n");
	scanf("%d",&line);
	
	generateStatisticsFile(line);
	
	return 0;
}

void generateStatisticsFile(int line) {
	
	time_t now = time(NULL);	
	tm *ltm = localtime(&now);
	
	char statisticsFileName[30]="../../data/test_statistics/";
	char statisticsYear[4]="";
	char statisticsMonth[2]="";
	char statisticsDay[2]="";
	
	itoa(1900+ltm->tm_year,statisticsYear,10);
	itoa(1+ltm->tm_mon,statisticsMonth,10);
	itoa(ltm->tm_mday,statisticsDay,10);
	
	strcat(statisticsFileName, statisticsYear);
	if (1+ltm->tm_mon < 10) {
		strcat(statisticsFileName, "0");
	}
	strcat(statisticsFileName, statisticsMonth);
	if (ltm->tm_mday < 10) {
		strcat(statisticsFileName, "0");
	}
	strcat(statisticsFileName, statisticsDay);
	strcat(statisticsFileName, "_Statistics.txt");
	
	FILE *statisticsFile = fopen(statisticsFileName, "w");
	//// print date
	fprintf(statisticsFile,"Date: ");
	// print year
	fprintf(statisticsFile,"%d/", 1900+ltm->tm_year);
	// print month
	if (1+ltm->tm_mon < 10) {
		fprintf(statisticsFile,"%c", '0');
	}
	fprintf(statisticsFile,"%d/", 1+ltm->tm_mon);
	// print day
	if (ltm->tm_mday < 10) {
		fprintf(statisticsFile,"%c", '0');
	}
	fprintf(statisticsFile,"%d\n\n", ltm->tm_mday);
	
	int C,T,Q;
	for(int count=0;count<line;count++) {
		randomCTQ(&C,&T,&Q);
		fprintf(statisticsFile,"%d %d %d\n",C,T,Q);
	}
	
	int A = rand()%100000;
	int B = rand()%100;
	fprintf(statisticsFile,"\n0 0 %d.%d\n",A,B); 
	
	fclose(statisticsFile);
	
}

void randomCTQ(int *C, int *T, int *Q) {
	*C = rand()%12+1;
	*T = rand()%50+1;
	*Q = rand()%20+1;
}
