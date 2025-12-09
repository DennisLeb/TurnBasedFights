// File characterStats.h
// Date 05/12/2025
//

/* Hier werden die structs für Spielercharacter und Monstertypen definiert. */

typedef struct 
{
	int health;
	int attackDmg;
	int armor;
} player;

typedef struct 
{
	int idNum;
	char name[64];
	char intro[512];
	char death[512];
	int health;
	int attackDmg;
	int armor;
} monster;

/*
--player

health = 10;
attackDmg = 2;
armor = 1;


--goblin

idNum = 0;
health = 3;
attackDmg = 1;
armor = 0;


--crazed magician

idNum = 1;
health = 4;
attackDmg = 4;
armor = 0;
*/
