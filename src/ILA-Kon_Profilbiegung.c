/*
 ============================================================================
 Name        : ILA-Kon_Profilbiegung.c
 Author      : Lukas Klass
 Version     : 1.0
 Copyright   :
 Description : Calculates points of a bended NACA-profile
 ============================================================================
 */

// String bis Semikolon auslesen: %*10[^;]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265
#define Rad(Grad) ((double) (Grad)/180.0*PI)

float OriginalProfil[100][2];
float NeuProfil[100][2];
float xSchwerpunkt0, ySchwerpunkt0;

//neue Punktewolke berechnen
void PunkteBestimmen(float DeltaBeta, float BetaEins, float BetaZwei, float SehnenlängeNabe, float SehnenlängeGehäuse, float Schaufelhöhe,
		float Radiusfaktor, char Speicherort[100]) {
	//NeuProfil leeren
	for(int j=0; j<=99; j++) {
		NeuProfil[j][0]=-1;
		NeuProfil[j][1]=-1;
	}

	//neue Wölbungslinie berechnen
	double Sehnenlänge = (double) (Radiusfaktor*(SehnenlängeGehäuse-SehnenlängeNabe)+SehnenlängeNabe);
	double Radius = Sehnenlänge/(cos(Rad(BetaZwei+90.0))-cos(Rad(BetaEins)));
	double xMitte = -cos(Rad(BetaEins))*Radius;
	double yMitte = -sin(Rad(BetaEins))*Radius;
	double Skelettlinie = PI*Radius*DeltaBeta/180.0;

	//Punkte berechnen
	int i=0;
	while((OriginalProfil[i][0] != -1.0) && i<99) {
		NeuProfil[i][0] = xMitte + cos(Rad(BetaEins-OriginalProfil[i][0] * DeltaBeta))*(Radius+OriginalProfil[i][1]*Skelettlinie);
		NeuProfil[i][1] = yMitte + sin(Rad(BetaEins-OriginalProfil[i][0] * DeltaBeta))*(Radius+OriginalProfil[i][1]*Skelettlinie);
		i++;
	}
	int ArrayOffset=i;

	//Fläche berechnen
	float Fläche=0.0;
	i=0;
	while((NeuProfil[i+1][0] != -1) && i<98) {
		Fläche -= 0.5 * (NeuProfil[i][0] * NeuProfil[i+1][1] - NeuProfil[i+1][0] * NeuProfil[i][1]);
		i++;
	}
	Fläche -= 0.5 * (NeuProfil[i][0] * NeuProfil[0][1] - NeuProfil[0][0] * NeuProfil[i][1]);

	//Schwerpunkt berechnen
	float xSchwerpunkt=0.0, ySchwerpunkt=0.0;
	i=0;
	while((NeuProfil[i+1][0] != -1) && i<98) {
		xSchwerpunkt -= 1 / (6 * Fläche) * (NeuProfil[i][0] + NeuProfil[i+1][0]) * (NeuProfil[i][0] * NeuProfil[i+1][1] - NeuProfil[i+1][0] * NeuProfil[i][1]);
		ySchwerpunkt -= 1 / (6 * Fläche) * (NeuProfil[i][1] + NeuProfil[i+1][1]) * (NeuProfil[i][0] * NeuProfil[i+1][1] - NeuProfil[i+1][0] * NeuProfil[i][1]);
		i++;
	}
	xSchwerpunkt -= 1 / (6 * Fläche) * (NeuProfil[i][0] + NeuProfil[0][0]) * (NeuProfil[i][0] * NeuProfil[0][1] - NeuProfil[0][0] * NeuProfil[i][1]);
	ySchwerpunkt -= 1 / (6 * Fläche) * (NeuProfil[i][1] + NeuProfil[0][1]) * (NeuProfil[i][0] * NeuProfil[0][1] - NeuProfil[0][0] * NeuProfil[i][1]);

	//Zusatzpunkte für Tangentenkonstruktion
	NeuProfil[ArrayOffset][0] = xMitte + cos(Rad(BetaEins))*(Radius+2);
	NeuProfil[ArrayOffset][1] = yMitte + sin(Rad(BetaEins))*(Radius+2);
	NeuProfil[ArrayOffset+1][0] = xMitte + cos(Rad(BetaEins-DeltaBeta))*(Radius+2);
	NeuProfil[ArrayOffset+1][1] = yMitte + sin(Rad(BetaEins-DeltaBeta))*(Radius+2);

	//Schwerpunkt zur Korrektur setzen
	if(Radiusfaktor == 0.0) {
		xSchwerpunkt0 = xSchwerpunkt;
		ySchwerpunkt0 = ySchwerpunkt;
	}

	//Schwerpunkt verschieben
	float xDelta = xSchwerpunkt0 - xSchwerpunkt;
	float yDelta = ySchwerpunkt0 - ySchwerpunkt;
	i=0;
	while((NeuProfil[i][0] != -1.0) && i<99) {
		NeuProfil[i][0] += xDelta;
		NeuProfil[i][1] += yDelta;
		i++;
	}

	//Punkte speichern
	FILE  *ZielDatei;
	ZielDatei = fopen(Speicherort,"w");
	if(ZielDatei == NULL) {
		printf("Fehler: Profildatei konnte nicht geöffnet werden!\n");
		exit(1);
	} else {
		i=0;
		//fprintf(ZielDatei, "PhiEins=%.4f\tPhiZwei=%.4f\r\n", BetaEins, BetaZwei+90);
		//fprintf(ZielDatei, "xMitte=%.4f\t\tyMitte=%.4f\r\n", xMitte, yMitte);
		//fprintf(ZielDatei, "Radius=%.4f\r\n", Radius);
		//fprintf(ZielDatei, "Fläche=%.6f\r\n", Fläche);
		//fprintf(ZielDatei, "%.6f;%.6f;0.0\r\n", xDelta, yDelta);
		//fprintf(ZielDatei, "%.6f;%.6f;0.0\r\n", xMitte + xDelta, yMitte + yDelta);
		fprintf(ZielDatei, "%.6f;%.6f;0.0\r\n", xSchwerpunkt + xDelta, ySchwerpunkt + yDelta);
		while((NeuProfil[i][0] != -1.0) && i<99) {
			fprintf(ZielDatei, "%.6f;%.6f;0.0\r\n", (float) NeuProfil[i][0], (float) NeuProfil[i][1]);
			i++;
		}
		fclose(ZielDatei);
	}
	return;
}


//Punktewolke des zu verbiegenden Profils laden
void ProfilDateiLaden(char Pfad[]) {
	FILE  *ProfilDatei;
	ProfilDatei = fopen(Pfad,"r");
	if(ProfilDatei == NULL) {
			printf("Fehler: Profildatei konnte nicht geöffnet werden!\n");
			exit(1);
	} else {
		//OriginalProfil leeren
		for(int j=0; j<=99; j++) {
			OriginalProfil[j][0]=-1.0;
			OriginalProfil[j][1]=-1.0;
		}
		int i=0;
		while(((fscanf(ProfilDatei,"%f;%f;%*f\n",&OriginalProfil[i][0], &OriginalProfil[i][1])) != EOF ) && i<99) {
			i++;
		}
		fclose(ProfilDatei);
	}
	printf("Profildatei erfolgreich geladen\n\n");
	return;
}


int main(int argc, char **argv) {
	void ProfilDateiLaden(char Pfad[]);
	void PunkteBestimmen(float DeltaBeta, float BetaEins, float BetaZwei, float SehnenlängeNabe, float SehnenlängeGehäuse, float Schaufelhöhe,
			float Radiusfaktor, char Dateiname[100]);

	FILE *ParameterDatei;

	float SehnenlängeNabe, SehnenlängeGehäuse, Schaufelzahl, OffsetGaskanal,
			NasenradiusNabe, NasenradiusGehäuse, Schaufelhöhe, DeltaBeta[5], BetaEins[5], BetaZwei[5];

	int Stufe;
	char Ausführung[10], Speicherort[100];

	printf("%c[2J",27); //Konsole löschen
	printf("%c[%d;%dH",27,1,1); //Cursor auf Anfang setzen
	printf("**************Berechnung der Punktewolke für gebogene NACA-Profile**************\n");
	printf("****************************Lukas Klass - 20.11.2015****************************\n\n");

	ProfilDateiLaden(argv[1]);

	//Biegeparameter laden
	ParameterDatei = fopen(argv[2],"r");
	if(ParameterDatei == NULL) {
		printf("Fehler: Parameterdatei konnte nicht geöffnet werden!\n");
		exit(1);
	} else {
		printf("Parameterdatei erfolgreich geladen\n\n");
		while((fscanf(ParameterDatei,"%i;%10[^;];%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n",
				&Stufe, &Ausführung[0], &SehnenlängeNabe, &SehnenlängeGehäuse, &Schaufelzahl, &OffsetGaskanal,
				&NasenradiusNabe, &NasenradiusGehäuse, &Schaufelhöhe, &DeltaBeta[0], &DeltaBeta[1],
				&DeltaBeta[2], &DeltaBeta[3], &DeltaBeta[4], &BetaEins[0], &BetaEins[1], &BetaEins[2],
				&BetaEins[3], &BetaEins[4], &BetaZwei[0], &BetaZwei[1], &BetaZwei[2], &BetaZwei[3], &BetaZwei[4])) != EOF ) {

			sprintf(Speicherort, "%s/Punktewolke_%i_%s_%s.dat", argv[3], Stufe, Ausführung, "000");
			printf("Berechne Punkte von %s Stufe %i mit Faktor 000\n", Ausführung, Stufe);
			PunkteBestimmen(DeltaBeta[0], BetaEins[0], BetaZwei[0], SehnenlängeNabe, SehnenlängeGehäuse, Schaufelhöhe, 0.0, Speicherort);
			sprintf(Speicherort, "%s/Punktewolke_%i_%s_%s.dat", argv[3], Stufe, Ausführung, "025");
			printf("Berechne Punkte von %s Stufe %i mit Faktor 025\n", Ausführung, Stufe);
			PunkteBestimmen(DeltaBeta[1], BetaEins[1], BetaZwei[1], SehnenlängeNabe, SehnenlängeGehäuse, Schaufelhöhe, 0.25, Speicherort);
			sprintf(Speicherort, "%s/Punktewolke_%i_%s_%s.dat", argv[3], Stufe, Ausführung, "050");
			printf("Berechne Punkte von %s Stufe %i mit Faktor 050\n", Ausführung, Stufe);
			PunkteBestimmen(DeltaBeta[2], BetaEins[2], BetaZwei[2], SehnenlängeNabe, SehnenlängeGehäuse, Schaufelhöhe, 0.5, Speicherort);
			sprintf(Speicherort, "%s/Punktewolke_%i_%s_%s.dat", argv[3], Stufe, Ausführung, "075");
			printf("Berechne Punkte von %s Stufe %i mit Faktor 075\n", Ausführung, Stufe);
			PunkteBestimmen(DeltaBeta[3], BetaEins[3], BetaZwei[3], SehnenlängeNabe, SehnenlängeGehäuse, Schaufelhöhe, 0.75, Speicherort);
			sprintf(Speicherort, "%s/Punktewolke_%i_%s_%s.dat", argv[3], Stufe, Ausführung, "100");
			printf("Berechne Punkte von %s Stufe %i mit Faktor 100\n", Ausführung, Stufe);
			PunkteBestimmen(DeltaBeta[4], BetaEins[4], BetaZwei[4], SehnenlängeNabe, SehnenlängeGehäuse, Schaufelhöhe, 1.0, Speicherort);
		}
		fclose(ParameterDatei);
		printf("\n\n\n");
	}
	return 0;
}
