/*
    main.c -- linux benchmark for cpu
    Copyright (C) 2020  Johannes Schmatz <johannes_schmatz@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

double last_pi = 3.1;//....

void singlecore(int run_nr, int single, int mulit_thread_nr){
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
			if(single){
				printf("Run %d S:\tIn %3ds sind %d Positionen getestet. PI=%lf\n", \
					run_nr, (now - start), all, pi);
			}else{
				printf("Run %d M %d:\tIn %3ds sind %d Positionen getestet. PI=%lf\n", \
					run_nr, mulit_thread_nr, (now - start), all, pi);
			}
		}

	// do this while pi is wrong
	}while(!check(pi, &pi_array[0]));

	// save pi to global var
	last_pi = pi;
}

void multicore(int run_nr, int n){
	// TODO just call n times singlecore!
	singlecore(run_nr, FALSE, 0);
}

void calculate_points(int run_nr, double *array, int number_of_cores){
	time_t start, mid, end;

	// inform the user
//	printf("\tRun %d: Start Multicore\n", run_nr);

	// start measuring
	time(&start);

	// make the multicore test
	multicore(run_nr, number_of_cores);

	// stop measuring the multicore, start singlecore
	time(&mid);

	// inform the user
//	printf("\tRun %d: Start Singlecore\n", run_nr);

	// make the singlecore test
	singlecore(run_nr, TRUE, 0);

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

	// create the data array for measuring
	double data[RUNS][2];

	// run the tests
	for(i = 0; i<RUNS; i++){
//		printf("Start Run %d\n", i);
		calculate_points(i, &data[i][0], number_of_cores);
//		printf("Ende Run %d\n", i);
	}

	// read the data from the tests, calculate median
	double multi = 0, single = 0;
	for(i = 0; i<RUNS; i++){
		multi += data[i][0];
		single += data[i][1];
	}
	multi /= (double) RUNS;
	single /= (double) RUNS;

	// show the result
	printf("\nDas Programm hat nebenbei PI berechnet:\n\t\e[32m%.10lf\e[0m\n\n", last_pi);

	// show the cores
	printf("Programm lief mit \e[34m%d\e[0m CPU-Kernen.\n\n", number_of_cores);

	// print out all data!
	printf("\n\nMulticore:\t\e[31m%.6f\e[0m\n", multi);
	printf("Singlecore:\t\e[31m%.6f\e[0m\n", single);
	printf("\n");
	printf(".............80___81___82___83___84___85___86___87___88___89___90");
	printf("___91___92___93___94___95___96___97___98___99___100\n");
	printf(".............|----:----|----:----|----:----|----:----|----:----|");
	printf("----:----|----:----|----:----|----:----|----:----|\n");

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

	// print NAME...
	printf("%s %s\nLizenz: %s, Autor: %s\n", PROG_NAME, PROG_VERS, \
			PROG_LICENCE, PROG_AUTH);

	// DONE!
	return 0;
}

// syntax
// vim: ts=8 sts=8 sw=8 noet si syntax=c
