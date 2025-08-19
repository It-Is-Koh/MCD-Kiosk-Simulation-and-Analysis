#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <cmath>
#include <ctime>
#include <cstring>
using namespace std;
#define CategorySize 12 // number of category-types in menu
#define TermSize 50 // number of term-types in menu
#define OrderSize 20 // max number of terms in a single order
//!!!!!!!!!!!!!!!!!!!! #define NumberOfItemsPerGroupWhenShowingMenu 8 // in function "showMenu()", define how many rows per group (adjust and run the program to check what will happen!)
#define TermNameMaxLength 56 // the longest term name-size in the menu
#define FileNameMaxLength 128 // used when creating files
#define ReceiptNoMaxDigit 4 // the upper limit of the receipt no.; ex. 4 for 9999
#define BreakfastHoursStartTime 5 // when the breakfast hours start
#define BreakfastHoursEndTime 11 // when the breakfast hours end
#define TaxRate 0.06 // tax rate; ex. 6% tax

struct menu { // menu files will be read in this form
	char termName[TermNameMaxLength];
	float price;
};

struct order { // each term in a single order
	int Ctgy;
	int Term;
	int Qty;
	char TermName[TermNameMaxLength];
	float Price;
};

void generateTodayReceiptFolder();
void generateStatisticsFile();
	void appendDateToFileName(char []); // nested
void generateReceipt(int);
	void printDateAndTimeInReceipt(FILE *); // nested
void readMenu(struct menu [][TermSize], int *);
	int breakfastOrRegular(); // nested
void showMenu(struct menu [][TermSize]);
void orderSingleTerm(int, int, int, int, struct menu [][TermSize], struct order []);
void countThePrice(struct order [], int, float *, float *, float *, float *);
void showOrder(struct order [], int);
void showCost(float *, float *, float *, float *, float *);
void completeReceipt(int, struct order [], int, float *, float *, float *, float *, float *);
void lastReceipt(int, float);
void appendStatisticsFile(struct order [], int);
void finishStatisticsFile(float);

// main function
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

int main() {
	
	struct menu sys[CategorySize][TermSize]; // system menu, sys[category][term]
	struct order csm[OrderSize]; // the consumer order list
	int i; // used to indicate how many terms have been in a order
	int isThereFileOrNot;
	isThereFileOrNot = 1; // check if there are menus and statistics file to be read
	
	int R; // receipt no.
	int C; // category no. when ordering
	int T; // term no. when ordering
	int Q; // quantity
	float p; // subtotal price for single order
	float pt; // price including tax
	float pr; // round-off price
	float ptt; // total daily earns including tax
	float k; // payment
	float kc; // amount changed
	
	R = 1;
	ptt = 0;
	generateTodayReceiptFolder();
	generateStatisticsFile();
	while (1) { // the whole day repeating order
		while (1) { // a single order
			// reset structure items to 0
			for (int m=0;m<CategorySize;m++) {
				for (int n=0;n<TermSize;n++) {
					sys[m][n].price = 0;
					for (int o=0;o<TermNameMaxLength;o++) {
						sys[m][n].termName[o] = ' ';
					}
				}
			}
			for (int x=0;x<OrderSize;x++) {
				csm[x].Ctgy = 0;
				csm[x].Term = 0;
				csm[x].Qty = 0;
				for (int y=0;y<TermNameMaxLength;y++) {
					csm[x].TermName[y] = ' ';
				}
				csm[x].Price = 0;
			}
			// reset variables about total price and count of terms in a single order to 0
			p = 0;
			pt = 0;
			pr = 0;
			k = 0;
			kc = 0;
			i = 0;
			generateReceipt(R);
			readMenu(sys, &isThereFileOrNot);
			if (isThereFileOrNot == 0) {
				break;
			}
			printf("Order No. %d\n",R); 
			printf("Please enter your order: (Category,Term,Quantity)\n");
			printf("Note: Enter (0,1,0) to complete the order, (0,2,0) to cancel the order, (0,3,0) to display the menu\n\n"); // (0,0,0) to end the daily operation
			printf("Term order:\n");
			while (1) { // ordering term one by one
				// reset the order term and quantity
				C = 0;
				T = 0;
				Q = 0;
				printf("%d. ",i+1); // no. of terms ordered
				scanf(" (%d,%d,%d)", &C, &T, &Q); // entering the term and quantity
				if (C==0) { // if system code (0,T,Q) is entered
					if ((T==0||T==1||T==2||T==3) && (Q==0)) { // (0,0/1/2/3,0), valid system code
						if (T==2) { // (0,2,0) order cancelled
							printf("\n[Your order has been cancelled.]\n\n");
							printf("----------------------------------------------------------------------------------------------------\n");
						}
						if (T==3) { // (0,3,0) menu shown
							showMenu(sys);
							printf("\n[Menu displayed.]\n\n");
							printf("----------------------------------------------------------------------------------------------------\n");
						}
						if (T==2 || T==1) {
							printf("\n");
						}
						break;
					} else { // invalid system code, ex. (0,T,Q) with T!=0/1/2 or/and Q!=0
						printf("[Please enter the valid system code.]\n");
					}
				} else if ((C<0) || (C>CategorySize) || (T<=0) || (T>TermSize) || (Q<1)) { // invalid menu/quantity
					if ((C<0) || (C>CategorySize) || (T<=0) || (T>TermSize)) {
						printf("[Please order according to the valid menu.]\n");
					}
					if (Q<1) {
						printf("[Please order with a valid quantity.]\n");
					}
				} else { // general order process
					orderSingleTerm(i, C, T, Q, sys, csm);
					i++;
				}
			}
			if ((C==0) && (T==0||T==1)) { // (0,1,0) complete order, (0,0,0) end operation
				break;
			}
		}
		if (isThereFileOrNot == 0) {
			break;
		}
		if (C==0 && T==0) { // (0,0,0) end operation
			lastReceipt(R, ptt);
			printf("[End operation code input.]\n\n");
			printf("----------------------------------------------------------------------------------------------------\n\n");
			break;
		}
		countThePrice(csm, i, &p, &pt, &pr, &ptt);
		if (pr>0) {
			showOrder(csm, i);
			showCost(&p, &pt, &pr, &k, &kc);
			completeReceipt(R, csm, i, &p, &pt, &pr, &k, &kc);
			printf("Thank you. Please Come Again.\n\n");
			printf("----------------------------------------------------------------------------------------------------\n\n");
			appendStatisticsFile(csm, i);
			R += 1;
		} else { // if the ordered terms are not available
			printf("[You have not done any order.]\n\n");
			printf("----------------------------------------------------------------------------------------------------\n\n");
		}
	}
	if (isThereFileOrNot == 0) {
		printf("[Please check your menu files location.]\n\n");
	} else {
		printf("Today operation has ended.\n");
		printf("The number of today orders\t\t: %d order\n",R-1);
		printf("Today Total Earns (including tax)\t: %.2f\n\n",ptt);
		finishStatisticsFile(ptt);
	}
	
	system("pause");

	return 0;
}

// function list
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

void generateTodayReceiptFolder() {
	
    // receipts folder path
    char dateString[9] = "";
    appendDateToFileName(dateString);
    char folderPath[256] = "../data/receipts/";
    strcat(folderPath, dateString);

    // Try to create the directory
    int result = _mkdir(folderPath);

    // Check if the directory was created successfully
    if (result == 0) {
    }
    // If _mkdir returns -1, check the specific error
	else {
        // errno == EEXIST means the directory already exists
        if (errno == EEXIST) {
            // Do nothing, folder is already there
        } 
        // A different error occurred (e.g., permission denied)
        else {
            // Terminate the program on critical failure
            exit(1); 
        }
    }
    
}

void generateStatisticsFile() {
	
	time_t now = time(NULL);	
	tm *ltm = localtime(&now);
	
	char statisticsFileName[FileNameMaxLength]="../data/statistics/";
	appendDateToFileName(statisticsFileName);
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

void generateReceipt(int R) {
	
	// receipt no. changed to string
	char Rs[ReceiptNoMaxDigit]="";
	itoa(R,Rs,10);
	
	// txt file name
	char receiptFileName[FileNameMaxLength]="../data/receipts/";
	appendDateToFileName(receiptFileName);
	strcat(receiptFileName, "/");
	appendDateToFileName(receiptFileName);
	strcat(receiptFileName, "_Receipt_No._");
	strcat(receiptFileName, Rs);
	strcat(receiptFileName, ".txt");
	
	// text in txt file
	char receiptNo[FileNameMaxLength]="Receipt No. ";
	strcat(receiptNo, Rs);
	
	// generate receipt 
	FILE *rec = fopen(receiptFileName, "w");
	fprintf(rec,"\t\t\t\t\t\tMcDonald's\n"); // title
	fprintf(rec,"\t\t\t\t\t\t%s\n\n",receiptNo); // receipt no.
	printDateAndTimeInReceipt(rec); // date and time
	fprintf(rec,"\n--------------------------------------------------------------------------------\n\n");
	fclose(rec);
}

void printDateAndTimeInReceipt(FILE *rec) {
	
	// current date/time based on current system
	time_t now = time(NULL);	
	tm *ltm = localtime(&now);

	//// print date
	fprintf(rec,"\t\t\t\t\t\tDate: ");
	// print year
	fprintf(rec,"%d/", 1900+ltm->tm_year);
	// print month
	if (1+ltm->tm_mon < 10) {
		fprintf(rec,"%c", '0');
	}
	fprintf(rec,"%d/", 1+ltm->tm_mon);
	// print day
	if (ltm->tm_mday < 10) {
		fprintf(rec,"%c", '0');
	}
	fprintf(rec,"%d\n", ltm->tm_mday);
	
	//// print time
	fprintf(rec,"\t\t\t\t\t\tTime: ");
	// print hour
	if (ltm->tm_hour < 10) {
		fprintf(rec,"%c", '0');
	}
	fprintf(rec,"%d:", ltm->tm_hour);
	// print minute
	if (ltm->tm_min < 10) {
		fprintf(rec,"%c", '0');
	}
	fprintf(rec,"%d:", ltm->tm_min);
	// print second
	if (ltm->tm_sec < 10) {
		fprintf(rec,"%c", '0');
	}
	fprintf(rec,"%d\n", ltm->tm_sec);
}

void readMenu(struct menu sys[][TermSize], int *isThereFileOrNot) {
	
	FILE *myFile;
	
	for (int findMenuTimes=0;findMenuTimes<2;findMenuTimes++) { // check if both menus exist
		if (findMenuTimes==0) {
			myFile = fopen("../data/menus/MCD_Breakfast_Menu.txt","r");
		} else {
			myFile = fopen("../data/menus/MCD_Regular_Menu.txt","r");
		}
		if (myFile == NULL) { // file not found
			*isThereFileOrNot = 0;
		}
	}
	
	if (*isThereFileOrNot == 0) { // file not found
		printf("[!! Menu Not Found !!]\n");
	} else { // both menus exist
		if (breakfastOrRegular()) { // identify which menu hours
			myFile = fopen("../data/menus/MCD_Breakfast_Menu.txt","r");
		} else {
			myFile = fopen("../data/menus/MCD_Regular_Menu.txt","r");
		}
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
			
			// pass the data read to sys[][]
			sys[C-1][T-1].price = P;
			for (int m=0;m<TermNameMaxLength;m++) {
				sys[C-1][T-1].termName[m] = TN[m];
			}
		
		}
	}
	
	fclose(myFile);
}

int breakfastOrRegular() {
	
	// current date/time based on current system
	time_t now = time(NULL);
	tm *ltm = localtime(&now);
		
	// return 1 for breakfast hours and 0 for regular time
	if (ltm->tm_hour >= BreakfastHoursStartTime && ltm->tm_hour < BreakfastHoursEndTime) {
		return 1;
	} else {
		return 0;
	}
	
}

void showMenu(struct menu sys[][TermSize]) {
	int countPerGroup = 0;
	printf("\nCtgy\tTerm\tPrice\t\tItem\n\n");
	for (int v=0;v<CategorySize;v++) {
		for (int w=0;w<TermSize;w++) {
			if (sys[v][w].price != 0) { // only available items (that is, with a price) will be shown
				printf("%d\t%d\t%.2f\t\t%s\n", v+1, w+1, sys[v][w].price, sys[v][w].termName);
				countPerGroup++;
//				if (countPerGroup%NumberOfItemsPerGroupWhenShowingMenu==0) {
//					printf("\n");
//				}
			}
		}
	}
	printf("\n");
}

void orderSingleTerm(int i, int C, int T, int Q, struct menu sys[][TermSize], struct order csm[]) {
	// assign the corresponding value from sys[][] to csm[]
	csm[i].Ctgy = C;
	csm[i].Term = T;
	csm[i].Qty = Q;
	for (int k=0;k<TermNameMaxLength;k++) {
		csm[i].TermName[k] = sys[C-1][T-1].termName[k];
	}
	csm[i].Price = sys[C-1][T-1].price;
}

void countThePrice(struct order csm[], int i, float *p, float *pt, float *pr, float *ptt) {
	for (int j=0;j<i;j++) {
		*p += csm[j].Qty*csm[j].Price;
	}
	*pt = (*p)*(1+TaxRate); // price including tax
	int temp = round((*pt)*10); // round off the second decimal place
	*pr = (float)temp/10; // to one decimal place
	*ptt += *pr; // total daily earns, increasing for each order
}

void showOrder(struct order csm[], int i) {
	printf("No.\tCtgy\tTerm\tQty\tPrice\t\tItems\n");
	int count = 1;
	for (int j=0;j<i;j++) {
		if (csm[j].Price != 0) { // only available items (that is, with a price) will be shown
			printf("%d\t%d\t%d\t%d\t%.2f\t\t%s\n", count, csm[j].Ctgy, csm[j].Term, csm[j].Qty, csm[j].Price*csm[j].Qty, csm[j].TermName);
			count++;
		}
	}
	printf("\n");
}

void showCost(float *p, float *pt, float *pr, float *k, float *kc) {
	// showing details about price
	printf("Sub-Total\t\t%.2f\n",*p);
	printf("6%% Serving Tax\t\t%.2f\n",(*pt-*p));
	printf("Rounding\t\t%.2f\n",(*pr-*pt));
	printf("\n");
	printf("Total Amount\t\t%.2f\n",*pr);
	// accept payment
	while (1) {
		cout << "Payment\t\t\t";
		cin >> *k;
		if ((*k)>=(*pr)) {
			break;
		}
		printf("[The payment is invalid.]\n");
	}
	*kc = *k - *pr;
	// showing change
	printf("Change\t\t\t%.2f\n\n", *kc);
}

void completeReceipt(int R, struct order csm[], int i, float *p, float *pt, float *pr, float *k, float *kc) {
	
	// receipt no. changed to string
	char Rs[ReceiptNoMaxDigit]="";
	itoa(R,Rs,10);
	
	// find the txt file to append details
	char receiptFileName[FileNameMaxLength]="../data/receipts/";
	appendDateToFileName(receiptFileName);
	strcat(receiptFileName, "/");
	appendDateToFileName(receiptFileName);
	strcat(receiptFileName, "_Receipt_No._");
	strcat(receiptFileName, Rs);
	strcat(receiptFileName, ".txt");
	
	// appending details
	FILE *rec = fopen(receiptFileName, "a");
	
	// printing terms ordered
	fprintf(rec,"No.\tCtgy\tTerm\tQty\tPrice\t\tItems\n\n");
	int c = 1;
	for (int j=0;j<i;j++) {
		if (csm[j].Price != 0) { // only available items (that is, with a price) will be printed
			fprintf(rec,"%d\t%d\t%d\t%d\t%.2f\t\t%s\n", c, csm[j].Ctgy, csm[j].Term, csm[j].Qty, csm[j].Price*csm[j].Qty, csm[j].TermName);
			c++;
		}
	}
	fprintf(rec,"\n--------------------------------------------------------------------------------\n\n");
	
	// printing details about money
	fprintf(rec,"Sub-Total\t\t\t\t\tRM\t%.2f\n",*p);
	fprintf(rec,"6%% Serving Tax\t\t\t\t\tRM\t%.2f\n",(*pt-*p));
	fprintf(rec,"Rounding\t\t\t\t\tRM\t%.2f\n",(*pr-*pt));
	fprintf(rec,"\n");
	fprintf(rec,"Total Amount\t\t\t\t\tRM\t%.2f\n",*pr);
	fprintf(rec,"Payment\t\t\t\t\t\tRM\t%.2f\n",*k);
	fprintf(rec,"Change\t\t\t\t\t\tRM\t%.2f\n",*kc);
	
	fclose(rec);
}

void lastReceipt(int R, float ptt) {
	
	// receipt no. changed to string
	char Rs[ReceiptNoMaxDigit]="";
	itoa(R,Rs,10);
	
	// find the txt file to complete the last receipt
	char receiptFileName[FileNameMaxLength]="../data/receipts/";
	appendDateToFileName(receiptFileName);
	strcat(receiptFileName, "/");
	appendDateToFileName(receiptFileName);
	strcat(receiptFileName, "_Receipt_No._");
	strcat(receiptFileName, Rs);
	strcat(receiptFileName, ".txt");
	
	// appending details
	FILE *rec = fopen(receiptFileName, "a");
	
	// details about price
	fprintf(rec,"Today opearion has ended.\n\n");
	fprintf(rec,"The number of today orders\t\t\t%d orders\n",R-1);
	fprintf(rec,"Today Total Earns (including tax)\t\t%.2f\n",ptt);
	
	fclose(rec);
}

void appendStatisticsFile(struct order csm[], int i) {
	
	char statisticsFileName[FileNameMaxLength]="../data/statistics/";
	appendDateToFileName(statisticsFileName);
	strcat(statisticsFileName, "_Statistics.txt");
	
	FILE *statisticsFile = fopen(statisticsFileName, "a");
	for (int j=0;j<i;j++) {
		if (csm[j].Price != 0) { // append the available items (that is, with a price) into the statistics file
			fprintf(statisticsFile, "%d %d %d\n", csm[j].Ctgy, csm[j].Term, csm[j].Qty);
		}
	}
	fclose(statisticsFile);
	
}

void finishStatisticsFile(float ptt) {
	
	char statisticsFileName[FileNameMaxLength]="../data/statictics/";
	appendDateToFileName(statisticsFileName);
	strcat(statisticsFileName, "_Statistics.txt");
	
	FILE *statisticsFile = fopen(statisticsFileName, "a");
	fprintf(statisticsFile, "\n0 0 %.2f\n", ptt);
	fclose(statisticsFile);
	
}
