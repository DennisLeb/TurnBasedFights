// File main.c
// Date 05/12/2025
//

/* Dieses Programm soll mein Wissen über structs und das Lesen einer
	Speicherdatei festigen. Dazu schreibe ich ein kleines turn-based
	fighting game mit verschiedenen Monstertypen und einem simplen Spielercharacter (PC). */

/* Features:  -typedef struct player und monster
			  -Zugriff auf structs mithilfe von pointer und double pointer
			  -Spielerinput für Kampfaktion (Attack, Defend, Flee)
			  -Rechnung mithilfe structs
			  -Zufällige Gegnerwahl
			  -Personalisierung durch voice lines
			  -Speicherdatei für Kills (09/12/2025)

   Fehlt:	  -Dynamische Speicherzuweisung mit malloc() und free()
			  -Ausgabe des Ranges, bestimmt durch Kills
			  -Level-System, um PC stats zu verbessern */

// Erstes Build fertiggestellt am 07/12/2025

#include <stdio.h>
#include <io.h>		// für _access()
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "characterStats.h"

#define TYPES_OF_ENEMIES 2
#define FILE_PATH "C:\\Users\\infod\\Documents\\Software Dev\\Lernmaterial\\Eigene Solutions\\TurnBasedFights\\Speicherdateien\\Turn Based Fights.txt"

FILE* fptr;

void getEnemy(monster *pEnemyStats);
int combatRoutine(player *pPlayerStats, monster enemyStats);							// playerStats wird hier global verändert, daher pointer. enemyStats nicht, daher kein pointer. Entweder enemy stirbt oder Game Over.
int playerAction(int combatDecision, player **dpPlayerStats, monster *pEnemyStats);		// enemyStats wird in Subfunktion verändert, dann zurück in combatRoutine() geprüft, daher hier pointer.
void enemyAction(player **dpPlayerStats, monster *pEnemyStats);
void saveKills(int enemyTypeSlain[]);

int main(void) {

	int enemyTypeSlain[TYPES_OF_ENEMIES] = { 0 };
	char ans;

	player playerStats = { 0 };
	player *pPlayerStats = &playerStats;

	playerStats.health = 10;
	playerStats.attackDmg = 2;
	playerStats.armor = 1;

	puts("*** Get ready to fight for your life... ***");

	do {
		srand((unsigned int)time(NULL));

		monster enemyStats = { 0 };				// initialisiert leere struct enemy
		monster *pEnemyStats = &enemyStats;
		
		getEnemy(pEnemyStats);					// befüllt enemy mit Monster stats

		puts("\nYour next foe approaches...\n");

		if (combatRoutine(pPlayerStats, enemyStats)) {
			enemyTypeSlain[enemyStats.idNum]++;
		}

		printf("That fight is behind you, do you wish to step up once more? (Y/N) ");
		scanf_s(" %c", &ans, (unsigned int)sizeof(ans));
		
		if (toupper(ans) != 'Y') {
			puts("\nYou put your weapon down, as you ponder spilled blood. The only thing that matters is your survival, you tell yourself.");
			break;
		}

	} while (1);

	saveKills(enemyTypeSlain);

	return (0);
}

// Funktion um einen zufälligen Gegner zu wählen und enemy auszufüllen
void getEnemy(monster *pEnemyStats) {

	int enemyID = rand() % 2;

	switch (enemyID) {
		case (0): {
			pEnemyStats->idNum = enemyID;

			// strcpy_s() ist nötig um string zu ersetzen
			strcpy_s(pEnemyStats->name, sizeof(pEnemyStats->name), "Goblin");
			strcpy_s(pEnemyStats->intro, sizeof(pEnemyStats->intro), "Betwixt hanging branches and dense shrubbery, you barely make out the shape of a familiar critter.\nThe greedy thing approaches...\n\nYou've encountered a Goblin!\n\nGoblin:\tGold... golden trinkets, I smell on you. You hide them in your guts? I cuts you up nice and good...\n");
			strcpy_s(pEnemyStats->death, sizeof(pEnemyStats->death), "As the skittish creature succumbs to its own weight, it mutters a last plea for your gold purse through gurgling blood. You clutch it tightly and move on.");
			pEnemyStats->health = 3;
			pEnemyStats->attackDmg = 1;
			pEnemyStats->armor = 0;
			break;
		}
		case (1): {
			pEnemyStats->idNum = enemyID;

			// strcpy_s() ist nötig um string zu ersetzen
			strcpy_s(pEnemyStats->name, sizeof(pEnemyStats->name), "Crazed Magician");
			strcpy_s(pEnemyStats->intro, sizeof(pEnemyStats->intro), "As you wander by a misty cave entrace, you detect the faint mumblings of a tall and hooded figure.\nIt has detected you also. As the thing steps closer, a trace of madness pollutes your senses.\n\nYou've encountered a Crazed Magician!\n\nCrazed Magician:\tUnstable, delusional... they claim superiority... over... over me?! I'll end them, END those\n\t\t\tfools yes... but first I shall end YOU!\n");
			strcpy_s(pEnemyStats->death, sizeof(pEnemyStats->death), "The bearded man falls and with him, his delusions of grandeur: \"T'wasn't meant to be after all, forgive me...\",\nhe mumbles as his hand reaches out to you, then hits the ground with a sudden thump. You move on.");
			pEnemyStats->health = 4;
			pEnemyStats->attackDmg = 4;
			pEnemyStats->armor = 0;
			break;
		}
		default: {
			puts("\n\nERROR--getEnemy()");
			exit(1);
		}
	}
}

// Funktion für die Kampfphase
int combatRoutine(player *pPlayerStats, monster enemyStats) {
	
	int playerDecision, playerFlee = 0;
	int playerDead = 0, enemyDead = 0;
	int playerBaseArmor = pPlayerStats->armor, enemyBaseArmor = enemyStats.armor;

	player **dpPlayerStats = &pPlayerStats;		// double pointer um die Adresse an Subfunktionen weiterzugeben
	monster *pEnemyStats = &enemyStats;

	printf("%s", enemyStats.intro);
	
	do {
		puts("\n1. Attack");
		puts("2. Defend");
		puts("3. Flee");
		printf("Your decision? ");
		int ans = scanf_s(" %d", &playerDecision);

		if (ans != 1) {	// falscher Datentyp
			// buffer leeren
			while (getchar() != '\n') {}

			puts("\nInvalid data input! Try again!\n");
			continue;
		}

		if (playerDecision >= 1 && playerDecision <= 3) {
			playerFlee = playerAction(playerDecision, dpPlayerStats, pEnemyStats);
		}
		else {	// falscher int input
			puts("\nInvalid input! Try again!\n");
			continue;
		}

		if (playerFlee) {
			break;
		}

		if (enemyStats.health <= 0) {
			enemyDead = 1;
			break;
		}

		if (enemyStats.armor > enemyBaseArmor) {	// stellt armor Basiswert wieder her, nachdem Defense abläuft
			enemyStats.armor = enemyBaseArmor;
		}

		enemyAction(dpPlayerStats, pEnemyStats);

		if (pPlayerStats->health <= 0) {
			playerDead = 1;
			break;
		}

		if (pPlayerStats->armor > playerBaseArmor) {	// stellt armor Basiswert wieder her, nachdem Defense abläuft
			pPlayerStats->armor = playerBaseArmor;
		}
		
	} while (1);

	if (enemyDead) {
		printf("\n%s\n\n", enemyStats.death);
		return (1);
	}
	else if (playerDead) {
		puts("\nStreaming blood engulfs your vision. Between strained huffs, you feel your weapon slip your grasp. It's over now.");
		exit(1);
	}
	else if (playerFlee) {
		puts("\nYour foe curses your cowardice, though their voice sinks into the distance the further you run.\n");
		return (0);
	}

	puts("\n\nERROR--combatRoutine()");
	exit(1);
}

// Funktion für die Rechnung, abhängig von player's Entscheidung
int playerAction(int combatDecision, player **dpPlayerStats, monster *pEnemyStats) {

	switch (combatDecision) {
		case (1): {				// Attack
			int effectiveDamage = (((*dpPlayerStats)->attackDmg - pEnemyStats->armor) > 0) ? (((*dpPlayerStats)->attackDmg - pEnemyStats->armor)) : (0);	// prüft ob effectiveDamage <=0 und gibt in diesem Fall 0 zurück, statt negativer Zahl

			pEnemyStats->health -= effectiveDamage;
			printf("\n%s hit for %d damage!\n", pEnemyStats->name, effectiveDamage);
			
			return (0);
		}
		case (2): {				// Defend
			(*dpPlayerStats)->armor += 2;
			puts("\nArmor +2 for this turn!");
			
			return (0);
		}
		case (3): {				// Flee
			puts("\nYou flee successfully!");

			return (1);
		}
		default: {
			puts("\n\nERROR--playerAction()");
			exit(1);
		}
	}
}

// Funktion um die Aktion des Gegners zufällig zu bestimmen
void enemyAction(player **dpPlayerStats, monster *pEnemyStats) {

	int enemyDecision = (rand() % 3);

	switch (enemyDecision) {
		case (0): {		// Attack
			if ((rand() % 3) != 2) {
				int effectiveDamage = ((pEnemyStats->attackDmg - (*dpPlayerStats)->armor) > 0) ? (pEnemyStats->attackDmg - (*dpPlayerStats)->armor) : (0);	// prüft ob effectiveDamage <=0 und gibt in diesem Fall 0 zurück, statt negativer Zahl

				(*dpPlayerStats)->health -= effectiveDamage;
				printf("\n%s hits you for %d damage!\n", pEnemyStats->name, effectiveDamage);
			}
			else {
				printf("\n%s slips on a banana peel and misses their attack!\n", pEnemyStats->name);
			}
			break;
		}
		case (1): {		// Defend
			if ((rand() % 3) != 2) {
				pEnemyStats->armor++;
				printf("\n%s takes on a defensive stance!\n", pEnemyStats->name);
			}
			else {
				printf("\n%s tries to defend themselves, but stumbles over pathetically!\n", pEnemyStats->name);
			}
			break;
		}
		case (2): {		// Does a flip
			printf("\n%s does a sick-ass flip! You are very impressed.\n", pEnemyStats->name);
			break;
		}
		default: {
			puts("\n\nERROR--enemyAction()");
			exit(1);
		}
	}
}

// Funktion um Kills in .txt Datei zu speichern
void saveKills(int enemyTypeSlain[]) {

	int newGobSlain = enemyTypeSlain[0], newMagSlain = enemyTypeSlain[1];
	int newTotalSlain = newGobSlain + newMagSlain;
	
	if (_access(FILE_PATH, 0) == 0) {	// if (Datei existiert)
		int err = fopen_s(&fptr, FILE_PATH, "r+");

		if (err != 0) {
			printf("\n\nERROR--SAVE FILE COULD NOT BE READ\n");
			exit(1);
		}

		int prevGobSlain = 0, prevMagSlain = 0, prevTotalSlain = 0;
		
		// Werte einlesen
		fscanf_s(fptr, "***\t\tMonster Kills\t\t***\n\n");
		fscanf_s(fptr, "Goblin Kills:\t\t%d\n", &prevGobSlain);
		fscanf_s(fptr, "Crazed Magician Kills:\t%d\n", &prevMagSlain);
		fscanf_s(fptr, "-------------------------------------------\n");
		fscanf_s(fptr, "Total Kills:\t\t%d", &prevTotalSlain);

		// neu berechnen
		newGobSlain += prevGobSlain;
		newMagSlain += prevMagSlain;
		newTotalSlain += prevTotalSlain;

		// Werte aktualisieren
		fseek(fptr, 0, SEEK_SET);
		fprintf(fptr, "***\t\tMonster Kills\t\t***\n\n");
		fprintf(fptr, "Goblin Kills:\t\t%d\n", newGobSlain);
		fprintf(fptr, "Crazed Magician Kills:\t%d\n", newMagSlain);
		fprintf(fptr, "-------------------------------------------\n");
		fprintf(fptr, "Total Kills:\t\t%d", newTotalSlain);
	}
	else {								// else {Datei neu anlegen}
		int err = fopen_s(&fptr, FILE_PATH, "w");

		if (err != 0) {
			printf("\n\nERROR--SAVE FILE COULD NOT BE CREATED\n");
			exit(1);
		}

		fprintf(fptr, "***\t\tMonster Kills\t\t***\n\n");
		fprintf(fptr, "Goblin Kills:\t\t%d\n", newGobSlain);
		fprintf(fptr, "Crazed Magician Kills:\t%d\n", newMagSlain);
		fprintf(fptr, "-------------------------------------------\n");
		fprintf(fptr, "Total Kills:\t\t%d", newTotalSlain);
	}

	fclose(fptr);
}
