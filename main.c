// main.c
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include <stdlib.h>

#define VERSION "0.0.1"
#define MAX_POINTS 100

double rand_from_0_to_1(){
	return (sin(rand()) + 1 ) / 2;
}

void multicore(int n){
}

void singlecore(){
	srand(time(NULL));
	printf("%d\n", 10);
	printf("%d\n", 10);
	printf("%f\n", rand_from_0_to_1());
}

time_t calculation(int number_of_cores){
	time_t middle;

	multicore(number_of_cores);

	time(&middle);

	singlecore();

	return middle;
}

void calculate_points(double *array, int number_of_cores){
	time_t start, mid, end;
	double first, second;

	time(&start);

	mid = calculation(number_of_cores);

	time(&end);

	first = difftime(mid, start);
	second = difftime(end, mid);

	if(first > 100)
		first = 100;
	if(second > 100)
		second = 100;

	array[0] = MAX_POINTS - first;
	array[1] = MAX_POINTS - second;
}

void print_points(int number_of_cores){
	double data[2];
	int i;

	calculate_points(&data[0], number_of_cores);

	printf("Multicore:\t\e[31m%.6f\e[0m\n", data[0]);
	printf("Singlecore:\t\e[31m%.6f\e[0m\n", data[1]);
	printf("\n");
	printf(".............0____5____10___15___20___25___30___35___40___45");
	printf("___50___55___60___65___70___75___80___85___90___95___100\n");
	printf(".............|----:----|----:----|----:----|----:----|----:");
	printf("----|----:----|----:----|----:----|----:----|----:----|\n");
	
	printf("\e[32mMulticore:\e[0m...\e[36m");
	for(i = 0; i < data[0]; i++){
		printf("X");
	}

	printf("!\e[0m\n\e[32mSinglecore:\e[0m..\e[36m");
	for(i = 0; i < data[1]; i++){
		printf("X");
	}
	printf("!\e[0m\n");
}

int string_to_num(char *str){
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
	if(argc != 3){
		printf("please run:\n");
		printf("\t\e[32m%s -j\e[0m \e[34m<number of cores>\e[0m\n", argv[0]);
		return 1;
	}

	int number_of_cores = string_to_num(argv[2]);
	printf("Runnig with \e[34m%d\e[0m CPU-Cores.\n\n", number_of_cores);

	print_points(number_of_cores);
	return 0;
}
