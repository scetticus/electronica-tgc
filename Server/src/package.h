#ifndef PACKAGE_H
#define PACKAGE_H

namespace Package
{
	enum PackageHeader
	{
		//Client->Server
		Login=1, 
		Logout,
		UseCard,
		Attack,	
		NextTurn,			
		CreateAccount,
		
		//Server->Client
		CardFighting, 		//No podes mandar a alguien a pelear teniendo a alguien
		NoCard,				//No hay una carta en esa posicion
		NoEnergy,			//No tenes suficiente energia
		ErrServer,			//Error general
		InvalidObjectUse,	//No se puede usar la carta sin alguien peleando
		NoTurn,				//No es tu turno todavia
		NoFighter,			//No podes atacar si no tenes bicho
		OkAtt,				//Se ataco bien
		CardUpdate,			//Cambió una carta
		EmptyUpdate,		//Cambió los flags de deckEmpty y/o discardEmpty
		TurnNoFighter,		//No podes pasar de turno sin tener atacante!
		ErrorLogin,			//Error logueandose
		OkLogin,			//Login correcto
		ErrorRegister,		//Error registrandose
		OkRegister,			//Registro correcto
		FoundEnemy,			//Se encontro un oponente
		OponentLeft,		//El oponente se fue
		OkLogout,			//Logout correcto
		YourTurn,			//Empieza tu turno
		OponentTurn,		//Empieza el turno del oponente
		
		//No enviados
		CommError,			//Llegó un paquete erroneo
		ConnLost,			//Se perdio la conexion con el server
		Nothing				//No paso nada
	};
}

namespace objActions
{
	enum actions{
		viewOpCards=0, 
		
		incAttack, //Incrementa el dańo del proximo ataque (Amount: cantidad de dańo incrementado)
		
		heal,	//Cura al luchador (Amount: cantidad de HP curado)
		
		incEnergy, //Agrega una estrella al luchador (Amount: cantidad de estrellas agregadas)
		
		attackEnemy, //Baja salud a luchador del enemigo
		
		enemyChange, //Fuerza al enemigo a cambiar su luchador por otro
		
		healTarget, //Cura a una carta especifica (Amount: cantidad de HP curado)
		
		paralyzeEnemyAttack, //Hace que el oponente no pueda atacar por un turno
		
		decEnergy, //Incrementa los turnos necesarios para el prox ataque e incrementa el dańo de ese ataque 
				   //(Amount: cant turnos agregados)
						   
		searchCard,	//Busca una carta cualquiera en mi mazo
		
		incAttackTarget, //Incrementa el ataque del proximo ataque de una carta especifico
						//(Amount: cantidad de ataque, Target: dańo incrementado)
						
		shieldTurn, //Hace de escudo del luchador por Amount turnos
			
		stealEnemyCard, //Roba Amount carta(s) de las del oponente 
		
		doubleAttack, //Podes atacar Amount veces más en el turno
		
		healAll //Cura todos los bichos en mano
		
	};
}

#endif
