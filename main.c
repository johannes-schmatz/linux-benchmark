// main.c

// <config>
#define PROG_NAME "linux benchmark utility"
#define PROG_AUTH "Johannes Schmatz <johannes_schmatz@gmx.de>"
#define PROG_LICENCE "GPLv3"
#define PROG_VERS "v0.0.1"

#define MAX_POINTS 100		// Maximale Anzahl an Punkten

#define PI_MATCH_MIN 3.141	// minimal akzeptierter PI-Wert
#define PI_RIGHT_VAL 3.1415	// PI
#define PI_MATCH_MAX 3.142	// maximal akzeptierter PI-Wert

#define LOOP_SHORT_NUM 10000000	// Alle x getesteten Werte Anzeigen

#define COMATA_POS 7		// Pi wird bis zur x-ten Nachkommastelle berrechnet
#define PI_ARRAY_CHECK 15	// Länge des Überprüften Arrays

#define RUNS 3			// So oft wird Ausgeführt, dann Mittelwert genommen
// </config>

// includes
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// code
#define TRUE 1 == 1
#define FALSE 1 != 1

double rand_from_0_to_1(){
	double ret = (double) rand() / RAND_MAX;
	return ret;
}

int match_range(double pi){
	// return TRUE if ok
	int ret;
	if(PI_MATCH_MIN < pi && pi < PI_MATCH_MAX){
		ret = TRUE;
	}else{
		ret = FALSE;
	}
	return ret;
}

int does_diff_at(double pi, double *pi_array){
	// returns TRUE if diffs
	int ret = FALSE;
	int i;
	double i_val;
	
	int ten = pow(10, COMATA_POS);
	pi *= ten;

	for(i = 0; i < PI_ARRAY_CHECK; i++){
		i_val = pi_array[i];
		i_val *= ten;

		if((int)i_val != (int)pi && !ret){
			ret = TRUE;
		}
	}
	return ret;
}

int check(double pi, double *pi_array){
	// returns TRUE if ok
	int ret;
	if(!does_diff_at(pi, pi_array) && match_range(pi)){
		ret = TRUE;
	}else{
		ret = FALSE;
	}
	return ret;
}

void multicore(int run_nr, int n){
	// TODO just call n times singlecore!
}

void singlecore(int run_nr){
	// init random
	srand(time(NULL));

	int i, now;
	int inner = 0, all = 0;
	double x, y, x2_y2;
	double pi = 0;
	int start = (int) time(NULL);


	// create check array
	double pi_array[PI_ARRAY_CHECK];
	for(i = 0; i < PI_ARRAY_CHECK; i++)
		pi_array[i] = 0;

	pi_array[0] = 0;

	do{
		all++;

		// get random
		x = rand_from_0_to_1();
		y = rand_from_0_to_1();
		
		// calculate x^2 + y^2
		x2_y2 = x * x + y * y;

		// check if x,y is in circle
		if(x2_y2 < 1.0)
			inner++;

		// calculate pi
		pi = 4 / ((double) all / (double) inner);

		// save pi in array
		pi_array[all % PI_ARRAY_CHECK] = pi;

		now = (int) time(NULL);

		// some verbose output
//		printf("In %ds wurden %d Positionen getestet.", (now-start), all);
//		printf(" ~%6lf%% liegen Innen. ", ((double) inner/all) * 100);
//		printf("PI ist derzeit ~%.12lf.\n", pi);

		// print some stuff every LOOP_SHORT_NUM
		if(all % LOOP_SHORT_NUM == 0){
			printf("Run %d: In %3ds sind %d Positionen getestet.\n", run_nr, \
					(now - start), all);
		}

	// do this while pi is wrong
	}while(!check(pi, &pi_array[0]));
}

void calculate_points(int run_nr, double *array, int number_of_cores){
	time_t start, mid, end;

	// start measuring
	time(&start);

	// make the multicore test
	multicore(run_nr, number_of_cores);

	// stop measuring the multicore, start singlecore
	time(&mid);

	// make the singlecore test
	singlecore(run_nr);

	// stop measuring
	time(&end);

	// calculate how long it take
	double multi = difftime(mid, start);
	double single = difftime(end, mid);

	// if the tests took to long set to maximum
	if(multi > MAX_POINTS)
		multi = MAX_POINTS;
	if(single > MAX_POINTS)
		single = MAX_POINTS;

	// save in array
	array[0] = MAX_POINTS - multi;
	array[1] = MAX_POINTS - single;
}

int string_to_num(char *str){
	// convert one string to numbers
	int num = 0;
	int ten = 1;
	int i;
	for(i = (int) strlen(str) -1; i >= 0; i--){
		num += (str[i] - 48) * ten;
		ten *= 10;
	}
		
	return num;
}

void info(){
	// print some info test
	printf("%s %s\nLizenz: %s, Autor: %s\n", PROG_NAME, PROG_VERS, \
			PROG_LICENCE, PROG_AUTH);
}


int main(int argc, char **argv){
	int i;

	// help
	if(argc != 3){
		printf("Aufruf:\n");
		printf("\t\e[32m%s -j\e[0m \e[34m<Anzahl der Kerne>\e[0m\n", argv[0]);
		return 1;
	}

	// get the number of cores form string
	int number_of_cores = string_to_num(argv[2]);

	// print the info
	info();

	// show the cores
	printf("Programm läuft mit \e[34m%d\e[0m CPU-Kernen.\n\n", number_of_cores);

	// create the data array for measuring
	double data[RUNS][2];

	// run the tests
	for(i = 0; i<RUNS; i++){
		calculate_points(i, &data[i][0], number_of_cores);
	}

	// read the data from the tests, calculate median
	double multi = 0, single = 0;
	for(i = 0; i<RUNS; i++){
		multi += data[i][0];
		single += data[i][1];
	}
	multi /= (double) RUNS;
	single /= (double) RUNS;

	// print out all data!
	printf("\n\nMulticore:\t\e[31m%.6f\e[0m\n", multi);
	printf("Singlecore:\t\e[31m%.6f\e[0m\n", single);
	printf("\n");
	printf(".............80___81___82___83___84___85___86___87___88___89___90___91___92___93___94___95___96___97___98___99___100\n");
	printf(".............|----:----|----:----|----:----|----:----|----:----|----:----|----:----|----:----|----:----|----:----|\n");

	// we don't want to go out of the bar
	single *= 5;
	single -= 400;

	// make some bars
	printf("\e[32mMulticore:\e[0m...\e[36m");
	for(i = 0; i < multi; i++){
		printf("X");
	}
	printf("!\e[0m\n\e[32mSinglecore:\e[0m..\e[36m");
	for(i = 0; i < single; i++){
		printf("X");
	}
	printf("!\e[0m\n\n");

	// print the info
	info();

	// DONE!
	return 0;
}
