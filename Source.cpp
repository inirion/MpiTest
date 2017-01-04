#include <mpi.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <algorithm>
#include <vector>
using namespace std;
#define MAX 20 //ilosc procesów minus 1
int IleUczestnikow(int *uczestnicy) {
	int ile = 0;
	for (int i = 0; i < MAX; i++) {
		if (uczestnicy[i] != 0) ile++;
	}
	return ile;
}
void dyskwalifikacja(int *glosowanie, int *uczestnicy, int nrTury) {
	int value = 0;
	if (IleUczestnikow(uczestnicy) == 4 || IleUczestnikow(uczestnicy) == 3)
		value = 1;
	for (int i = 0; i < MAX; i++) {
		
		if (glosowanie[i] == 0 && value < 2) {
			++value;
			cout << "Minimalny " << *std::min_element(glosowanie, glosowanie + MAX) << " value  = " << value << " user  = " << i +1 << endl;
			uczestnicy[i] = 0;
			glosowanie[i] = 100;
			
		}
		if (glosowanie[i] == *std::min_element(glosowanie, glosowanie + MAX) && value < 2) {
			++value;
			cout << "Minimalny " << *std::min_element(glosowanie, glosowanie + MAX) <<" value  = " << value << " user  = " << i +1 << endl;
			uczestnicy[i] = 0;
			glosowanie[i] = 100;
			
		}
		if (i == MAX-1 && value == 1) {
			i = 0;
			cout << "Reset" << endl;
		}
	}
}

void werdykt(int *glosowanie, int *uczestnicy) {
	int uczestnik1 = 0, uczestnik2 = 0;
	for (int i = 0; i < MAX; i++) {
		if (uczestnicy[i] != 0 && uczestnik1 == 0) {
			uczestnik1 = i;
		}
		else if (uczestnicy[i] != 0 && uczestnik1 != 0) {
			uczestnik2 = i;
		}
	}
	if (glosowanie[uczestnik1] != glosowanie[uczestnik2])
		glosowanie[uczestnik1] > glosowanie[uczestnik2] 
		? cout << "Wygral Kandydant nr : " << uczestnik1 + 1 << endl 
		: cout << "Wygral Kandydant nr : " << uczestnik2 + 1 << endl;
	if (glosowanie[uczestnik1] == glosowanie[uczestnik2]) 
		cout << "Remis " << endl;
}





int main() {
	int numtasks, rank, value = 0, glosowanie[MAX] = { 0 }, uczestnicy[MAX], numerTury = 0;
	for (int i = 1; i < MAX; i++) {
		uczestnicy[i] = i;
	}
	int i = 0;
	MPI_Status status;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program. Terminationg. /n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	srand((unsigned)time(NULL) + rank*numtasks);
	MPI_Barrier(MPI_COMM_WORLD);
	int go = 1;
	while (go) {
		if (rank == 0) {
			for (int i = 1; i < MAX + 1; i++) {
				if (uczestnicy[i - 1] != 0)
					MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			for (int i = 1; i < MAX + 1; i++) {
				if (uczestnicy[i-1] != 0) {
					MPI_Recv(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
					glosowanie[value] += 1;
				}
					if (i == MAX) {
						++numerTury;
						if (IleUczestnikow(uczestnicy) == 2) {
							for (int i = 0; i < MAX; i++) {
								if (uczestnicy[i] != 0)
									cout << "Kandydant nr : " << i + 1 << " Otrzymal " << glosowanie[i] << "glosow" << endl;
							}
							werdykt(glosowanie, uczestnicy);
							go = 0;
							break;
						}
						else {
							
							cout << "Tura numer " << numerTury << endl;

							for (int i = 0; i < MAX; i++) {
								if (uczestnicy[i] != 0)
									cout << "Kandydant nr : " << i + 1 << " Otrzymal " << glosowanie[i] << "glosow" << endl;
							}
							dyskwalifikacja(glosowanie, uczestnicy, numerTury);
						}
				}
			}
		}
		else {
			
			if (MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status) == MPI_SUCCESS) {
				value = rand() % MAX;
				
				MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

			}
			cout << "error";
		}
	}

	MPI_Finalize();

	return 0;
}