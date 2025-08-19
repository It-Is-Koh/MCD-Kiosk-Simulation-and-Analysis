#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <cstring>
using namespace std;
#define CategorySize 12 // number of category-types in menu
#define TermSize 50 // number of term-types in menu
#define TermNameMaxLength 56 // the longest term name-size in the menu
#define FileNameMaxLength 128 // used when creating files

struct statistics {
	char termName[TermNameMaxLength];
	float unitPrice;
	int quantity;
	float dailyEarns;
	int rankQ;
	int rankDE;
};

// reading menus and statistics file
void readBothMenu(struct statistics [][TermSize], int *);
	void loadSingleMenu(FILE *, struct statistics [][TermSize]); // nested
void readStatisticsFile(struct statistics [][TermSize], int *, float *);
	void appendDateToFileName(char []); // nested

// counting earns
void countDailyEarns(struct statistics [][TermSize]);
void countTotalEarnsExcludingTax(struct statistics [][TermSize], float *); 

// ranking and generating files
void rankWithQuantity(struct statistics [][TermSize]);
void fileWithQuantity(struct statistics [][TermSize]);
	void printDateInFile(FILE *myFile); // nested
void rankWithDailyEarns(struct statistics [][TermSize]);
void fileWithDailyEarns(struct statistics [][TermSize]);
void overallFile(struct statistics [][TermSize], float *, float *);

// showing results on screen
void showOverall(struct statistics [][TermSize], float *, float *);
void showWithQuantity(struct statistics [][TermSize]);
void showWithDailyEarns(struct statistics [][TermSize]);

// main function
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

int main() {
	
	struct statistics stt[CategorySize][TermSize]; // statistics[category][term]
	float totalEarnsIncludingTax = 0; // total earns of the day (including tax)
	float totalEarnsExcludingTax = 0; // total earns of the day (excluding tax)
	int isThereFileOrNot = 1; // check if there are menus and statistics file to be read
	
	// initialize the variables in the struct statistics
	for (int i=0;i<CategorySize;i++) {
		for (int j=0;j<TermSize;j++) {
			for (int k=0;k<TermNameMaxLength;k++) {
				stt[i][j].termName[k] = ' ';
			}
			stt[i][j].unitPrice = 0;
			stt[i][j].quantity = 0;
			stt[i][j].dailyEarns = 0;
			stt[i][j].rankQ = 1;
			stt[i][j].rankDE = 1;
		}
	}
	
	// reading menus and statistics files
	readBothMenu(stt, &isThereFileOrNot);
	readStatisticsFile(stt, &isThereFileOrNot, &totalEarnsIncludingTax);
	
	if (isThereFileOrNot == 1) {
		
		// counting earns
		countDailyEarns(stt);
		countTotalEarnsExcludingTax(stt, &totalEarnsExcludingTax);
		
		// ranking and generating files
		rankWithQuantity(stt);
		fileWithQuantity(stt);
		rankWithDailyEarns(stt);
		fileWithDailyEarns(stt);
		overallFile(stt, &totalEarnsExcludingTax, &totalEarnsIncludingTax);
	
		// showing results on screen
		printf("Press any key to show an overall view for today sales.\n");
		system("pause");
		printf("\n");
		showOverall(stt, &totalEarnsExcludingTax, &totalEarnsIncludingTax);
		printf("\n----------------------------------------------------------------------------------------------------\n\n");
		printf("Press any key to show ranking according to items' daily sales.\n");
		system("pause");
		printf("\n");
		showWithQuantity(stt);
		printf("\n----------------------------------------------------------------------------------------------------\n\n");
		printf("Press any key to show ranking according to profits each item creates.\n");
		system("pause");
		printf("\n");
		showWithDailyEarns(stt);
	}
	
	printf("\n");
	system("pause");
	
	return 0;
}

// function list
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// reading menus and statistics files
// ----------------------------------------------------------------------------------------------------

void readBothMenu(struct statistics stt[][TermSize], int *isThereFileOrNot) {
	
	FILE *myFile;
	
	for (int t=0;t<2;t++) { // both of the menu lists should be loaded
		if (t==0) {
			myFile = fopen("../data/menus/MCD_Breakfast_Menu.txt","r");
			if (myFile == NULL) {
				*isThereFileOrNot = 0;
				printf("[!! Breakfast Menu Not Found !!]\n");
			}
		} else {
			myFile = fopen("../data/menus/MCD_Regular_Menu.txt","r");
			if (myFile == NULL) {
				*isThereFileOrNot = 0;
				printf("[!! Regular Menu Not Found !!]\n");
			}
		}
		if (*isThereFileOrNot == 1) {
			loadSingleMenu(myFile, stt);
		}
	}
	
	fclose(myFile);
}

void loadSingleMenu(FILE *myFile, struct statistics stt[][TermSize]) { // nested
	
	int C=0; // category
	int T=0; // term
	float P=0; // price
	char TN[TermNameMaxLength]; // termname
	
	while (!feof(myFile)) {
		
		// read data from txt file
		fscanf(myFile,"%d",&C);
		fscanf(myFile,"%d",&T);
		fscanf(myFile,"%f",&P);
		fgetc(myFile); // omit "\t"
		fgets(TN,TermNameMaxLength,myFile);
		TN[strcspn(TN, "\n")] = 0; // clearing "\n"
		
		// pass the data read (price & term name) to stt[][]
		stt[C-1][T-1].unitPrice = P;
		for (int m=0;m<TermNameMaxLength;m++) {
			stt[C-1][T-1].termName[m] = TN[m];
		}
	}
}

void readStatisticsFile(struct statistics stt[][TermSize], int *isThereFileOrNot, float *totalEarnsIncludingTax) {
	
	// char valuable for file name
	char statisticsFileName[FileNameMaxLength]="../data/statistics/";
	appendDateToFileName(statisticsFileName); // append date to it 
	strcat(statisticsFileName, "_Statistics.txt"); // remember to add ".txt" suffix about file type to generate a txt file
	// statisticsFileName now become "Statistics_YYYYMMDD.txt"
	
	// read the statistics file
	FILE *statisticsFile = fopen(statisticsFileName, "r");
	
	if (statisticsFile == NULL) {
		*isThereFileOrNot = 0;
		printf("[!! Statistics File Not Found !!]\n");
	} else {
		int C=0; // category
		int T=0; // term
		int Q=0; // quantity
		char date[20]; // used to omit the first line of the txt file
		
		fgets(date,20,statisticsFile); // omit first line about date
		while (!feof(statisticsFile)) {
			// read data from txt file
			fscanf(statisticsFile,"%d",&C);
			fscanf(statisticsFile,"%d",&T);
			if (C==0 && T==0) {
				break;
			} 
			fscanf(statisticsFile,"%d",&Q);
			stt[C-1][T-1].quantity += Q; // to accumulate the count of daily sales line by line
		}
		fscanf(statisticsFile,"%f",totalEarnsIncludingTax);
	}
	
	fclose(statisticsFile);
}

void appendDateToFileName(char fileName[]) { // nested
	
	// date and time for now (we'll only use date then)
	time_t now = time(NULL);
	tm *ltm = localtime(&now);
	
	// char[] for date
	char fileNameYear[4]="";
	char fileNameMonth[2]="";
	char fileNameDay[2]="";
	
	// change type of today date from int to string
	itoa(1900+ltm->tm_year,fileNameYear,10);
	itoa(1+ltm->tm_mon,fileNameMonth,10);
	itoa(ltm->tm_mday,fileNameDay,10);
	
	//// append date to file name
	// year
	strcat(fileName, fileNameYear);
	// month
	if (1+ltm->tm_mon < 10) { // note that there will be only "X" shown if this month is less than 10
		strcat(fileName, "0"); // thus we have to add a "0" 
	}
	strcat(fileName, fileNameMonth);
	// day
	if (ltm->tm_mday < 10) { // about day, it's same as month above
		strcat(fileName, "0");
	}
	strcat(fileName, fileNameDay);
}

// counting earns
// ----------------------------------------------------------------------------------------------------

void countDailyEarns(struct statistics stt[][TermSize]) {
	for (int i=0;i<CategorySize;i++) {
		for (int j=0;j<TermSize;j++) {
			stt[i][j].dailyEarns = stt[i][j].unitPrice*stt[i][j].quantity; // the daily earns of an item is just how much it cost multiplied with its daily sales
		}
	}
}

void countTotalEarnsExcludingTax(struct statistics stt[][TermSize], float *totalEarnsExcludingTax) {
	for (int i=0;i<CategorySize;i++) {
		for (int j=0;j<TermSize;j++) {
			*totalEarnsExcludingTax += stt[i][j].dailyEarns; // adding daily earns of each item into the total one variable
		}
	}
}

// ranking and generating files
// ----------------------------------------------------------------------------------------------------

void rankWithQuantity(struct statistics stt[][TermSize]) {
	for (int m=0;m<CategorySize;m++) {
		for (int n=0;n<TermSize;n++) {
			for (int x=0;x<CategorySize;x++) {
				for (int y=0;y<TermSize;y++) {
					if (stt[m][n].quantity < stt[x][y].quantity) {
						stt[m][n].rankQ++; // its rank gets dropping when there's other with a higher sales
					}
				}
			}
		}
	}
}

void fileWithQuantity(struct statistics stt[][TermSize]) {
	
	char RQFileName[FileNameMaxLength]="../data/statistics/";
	appendDateToFileName(RQFileName);
	strcat(RQFileName, "_Rank_With_Quantity.txt");
	
	FILE *RQFile = fopen(RQFileName, "w");
	
	printDateInFile(RQFile);
	fprintf(RQFile,"This ranking is based on the number of daily sales about the items.\n");
	fprintf(RQFile,"Note: The items without any sales will not be printed out.\n\n");
	fprintf(RQFile,"Rank\tQty\tUnit Price\tDaily Earns\tCtgy\tTerm\tItems\n\n");
	
	// checking one by one, from higher rank to lower rank
	for (int rank=1;rank<(CategorySize*TermSize)+1;rank++) {
		for (int x=0;x<CategorySize;x++) {
			for (int y=0;y<TermSize;y++) {
				if (stt[x][y].rankQ == rank && stt[x][y].quantity != 0) { // the term without any sales on that day will not be printed out
					fprintf(RQFile, "%d\t%d\t%.2f\t\t%.2f\t\t%d\t%d\t%s\n", stt[x][y].rankQ, stt[x][y].quantity, stt[x][y].unitPrice, stt[x][y].dailyEarns, x+1, y+1, stt[x][y].termName);
				}
			}
		}
	}
	
	fclose(RQFile);
}

void printDateInFile(FILE *myFile) { // nested
	// current date/time based on current system
	time_t now = time(NULL);	
	tm *ltm = localtime(&now);

	//// print date
	fprintf(myFile,"Date: ");
	// print year
	fprintf(myFile,"%d/", 1900+ltm->tm_year);
	// print month
	if (1+ltm->tm_mon < 10) {
		fprintf(myFile,"%c", '0');
	}
	fprintf(myFile,"%d/", 1+ltm->tm_mon);
	// print day
	if (ltm->tm_mday < 10) {
		fprintf(myFile,"%c", '0');
	}
	fprintf(myFile,"%d\n\n", ltm->tm_mday);
}

void rankWithDailyEarns(struct statistics stt[][TermSize]) {
	for (int m=0;m<CategorySize;m++) {
		for (int n=0;n<TermSize;n++) {
			for (int x=0;x<CategorySize;x++) {
				for (int y=0;y<TermSize;y++) {
					if (stt[m][n].dailyEarns < stt[x][y].dailyEarns) {
						stt[m][n].rankDE++; // its rank gets dropping when there's other with a higher profits
					}
				}
			}
		}
	}
}

void fileWithDailyEarns(struct statistics stt[][TermSize]) {
	
	char RDEFileName[FileNameMaxLength]="../data/statistics/";
	appendDateToFileName(RDEFileName);
	strcat(RDEFileName, "_Rank_With_Daily_Earns.txt");
	
	FILE *RDEFile = fopen(RDEFileName, "w");
	
	printDateInFile(RDEFile);
	fprintf(RDEFile,"This ranking is based on the income of daily sales about the items.\n");
	fprintf(RDEFile,"Note: The items without any sales will not be printed out.\n\n");
	fprintf(RDEFile,"Rank\tDaily Earns\tUnit Price\tQty\tCtgy\tTerm\tItems\n\n");
	
	// checking one by one, from higher rank to lower rank
	for (int rank=1;rank<(CategorySize*TermSize)+1;rank++) {
		for (int x=0;x<CategorySize;x++) {
			for (int y=0;y<TermSize;y++) {
				if (stt[x][y].rankDE == rank && stt[x][y].quantity != 0) { // the term without any sales on that day will not be printed out
					fprintf(RDEFile, "%d\t%.2f\t\t%.2f\t\t%d\t%d\t%d\t%s\n", stt[x][y].rankDE, stt[x][y].dailyEarns, stt[x][y].unitPrice, stt[x][y].quantity, x+1, y+1, stt[x][y].termName);
				}
			}
		}
	}
	
	fclose(RDEFile);
}

void overallFile(struct statistics stt[][TermSize], float *totalEarnsExcludingTax, float *totalEarnsIncludingTax) {
	char overallFileName[FileNameMaxLength]="../data/statistics/";
	appendDateToFileName(overallFileName);
	strcat(overallFileName, "_Overall_Results.txt");
	
	FILE *overallFile = fopen(overallFileName, "w");
	printDateInFile(overallFile);
	fprintf(overallFile,"This is a overall view about today sales.\n");
	fprintf(overallFile,"Items will be shown below, one by one, according to their category no. and term no.\n\n");
	fprintf(overallFile,"Today Total Earns (including tax)\t\t: %.2f\n", *totalEarnsIncludingTax);
	fprintf(overallFile,"Today Total Earns (excluding tax)\t\t: %.2f\n", *totalEarnsExcludingTax);
	fprintf(overallFile,"\t- 6%% tax\t\t\t\t: %.2f\n", (*totalEarnsExcludingTax)*0.06);
	fprintf(overallFile,"\t- total amount\t\t\t\t: %.2f\n", (*totalEarnsExcludingTax)*1.06);
	fprintf(overallFile,"Total Rounding\t\t\t\t\t: %.2f\n\n", (*totalEarnsIncludingTax)-((*totalEarnsExcludingTax)*1.06));
	fprintf(overallFile,"Category\tTerm\t\tUnit Price\tQuantity\tRank/Q\t\tDaily Earns\tRank/DE\t\tItems\n\n");
	
	for (int x=0;x<CategorySize;x++) {
		for (int y=0;y<TermSize;y++) {
			if (stt[x][y].unitPrice != 0) { // only to print out the items shown in menu
//			if (stt[x][y].unitPrice != 0 || stt[x][y].quantity != 0) { // this is for the random generating statistics file
				fprintf(overallFile, "%d\t\t%d\t\t%.2f\t\t%d\t\t%d\t\t%.2f\t\t%d\t\t%s\n", x+1, y+1, stt[x][y].unitPrice, stt[x][y].quantity, stt[x][y].rankQ, stt[x][y].dailyEarns, stt[x][y].rankDE, stt[x][y].termName);
			}
		}
	}
	
	fclose(overallFile);
}

// showing results on screen
// ---------------------------------------------------------------------------------------------------- 

void showOverall(struct statistics stt[][TermSize], float *totalEarnsExcludingTax, float *totalEarnsIncludingTax) {
	// as per the generating file function
	printf("This is a overall view about today sales.\n");
	printf("Items will be shown below, one by one, according to their category no. and term no.\n\n");
	printf("Today Total Earns (including tax)\t\t: %.2f\n", *totalEarnsIncludingTax);
	printf("Today Total Earns (excluding tax)\t\t: %.2f\n", *totalEarnsExcludingTax);
	printf("\t- 6%% tax\t\t\t\t: %.2f\n", (*totalEarnsExcludingTax)*0.06);
	printf("\t- total amount\t\t\t\t: %.2f\n", (*totalEarnsExcludingTax)*1.06);
	printf("Total Rounding\t\t\t\t\t: %.2f\n\n", (*totalEarnsIncludingTax)-((*totalEarnsExcludingTax)*1.06));
	printf("Category\tTerm\t\tUnit Price\tQuantity\tRank/Q\t\tDaily Earns\tRank/DE\t\tItems\n\n");
	
	for (int x=0;x<CategorySize;x++) {
		for (int y=0;y<TermSize;y++) {
			if (stt[x][y].unitPrice != 0) { // only to print out the items shown in menu
//			if (stt[x][y].unitPrice != 0 || stt[x][y].quantity != 0) { // this is for the random generating statistics file
				printf("%d\t\t%d\t\t%.2f\t\t%d\t\t%d\t\t%.2f\t\t%d\t\t%s\n", x+1, y+1, stt[x][y].unitPrice, stt[x][y].quantity, stt[x][y].rankQ, stt[x][y].dailyEarns, stt[x][y].rankDE, stt[x][y].termName);
			}
		}
	}
}

void showWithQuantity(struct statistics stt[][TermSize]) {
	// as per the generating file function
	printf("This ranking is based on the number of daily sales about the items.\n");
	printf("Note: The items without any sales will not be printed out.\n\n");
	printf("Rank\tQty\tUnit Price\tDaily Earns\tCtgy\tTerm\tItems\n\n");
	
	for (int rank=1;rank<(CategorySize*TermSize)+1;rank++) {
		for (int x=0;x<CategorySize;x++) {
			for (int y=0;y<TermSize;y++) {
				if (stt[x][y].rankQ == rank && stt[x][y].quantity != 0) { // the term without any sales on that day will not be printed out
					printf("%d\t%d\t%.2f\t\t%.2f\t\t%d\t%d\t%s\n", stt[x][y].rankQ, stt[x][y].quantity, stt[x][y].unitPrice, stt[x][y].dailyEarns, x+1, y+1, stt[x][y].termName);
				}
			}
		}
	}
}

void showWithDailyEarns(struct statistics stt[][TermSize]) {
	// as per the generating file function
	printf("This ranking is based on the income of daily sales about the items.\n");
	printf("Note: The items without any sales will not be printed out.\n\n");
	printf("Rank\tDaily Earns\tUnit Price\tQty\tCtgy\tTerm\tItems\n\n");
	
	for (int rank=1;rank<(CategorySize*TermSize)+1;rank++) {
		for (int x=0;x<CategorySize;x++) {
			for (int y=0;y<TermSize;y++) {
				if (stt[x][y].rankDE == rank && stt[x][y].quantity != 0) { // the term without any sales on that day will not be printed out
					printf("%d\t%.2f\t\t%.2f\t\t%d\t%d\t%d\t%s\n", stt[x][y].rankDE, stt[x][y].dailyEarns, stt[x][y].unitPrice, stt[x][y].quantity, x+1, y+1, stt[x][y].termName);
				}
			}
		}
	}
}
