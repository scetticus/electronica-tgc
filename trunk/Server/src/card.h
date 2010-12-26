#ifndef CARD_H
#define CARD_H

#include "common.h"
#include <fstream>
#include <vector>
#include <sstream>

struct Card{
	int img;
	int id;
	string name;
	int life;
	int originalLife;
	int energy;
	int action;
	int amount;
	int attackOffset;
	string attName[3];
	int attDmg[3], attWait[3];
	int attackAmount;
	int cardindex;
	string imgStr;
	
	Card(int cardind, int idd){
		//cout << "Cargando carta " << filename << endl;
		energy = 0;
		amount = 0;
		attackOffset = 0;
		cardindex = cardind;
		stringstream cardir; cardir << "./res/cards/carta" << cardindex << ".cdf";
		//cout << cardir.str() << endl;
		ifstream in;
		in.open(cardir.str().c_str());
		attackAmount=0;
		img=0;
		id = idd;
		
		if(!in.is_open()) return ;
		
		while(!in.eof())
		{
			string line;
			getline(in, line);
			
			unsigned int equal = line.find("=");
			
			if(equal != string::npos){
				string ident = line.substr(0, equal);
				string valor = line.substr(equal+1);
				
				if(ident == "img")
					imgStr = valor;
				
				if(ident == "name")
					name = valor;
				if(ident == "life"){
					sscanf(valor.c_str(), "%i", &life);
					originalLife = life;
				}
				if(ident == "att")
				{
					stringstream ss(valor);
					
					for(int i=0;i<3;i++){
						char temp[128];
						ss.get(temp, 128, ',');
						ss.get();
						
						if(i==0) attName[attackAmount] = string(temp);
						if(i==1) attDmg[attackAmount] = atoi(temp);
						if(i==2) attWait[attackAmount] = atoi(temp);
					}
					attackAmount++;
				}
				if(ident == "action")
					sscanf(valor.c_str(), "%i", &action);
				if(ident == "amount")
					sscanf(valor.c_str(), "%i", &amount);
				
				//cout << "Indice: -" << ident << "-" << endl;
				//cout << "Valor: -" << valor << "-" << endl;
				//cout << "----" << endl;
			}else{
				//cerr << "Linea sin igual" << endl;
			}			
		}
		
		in.close();
		//cout << "todo bien, salud " << life;
		//cout << "Fin carga carta" << endl;
	}
};

struct User{

	string name, passw;
	int deck[40]; //40 nomas
	vector<int> allCards; //todas las cartas q tiene
	
	User(const string& filename)
	{
		ifstream in;
		in.open(filename.c_str());
		
		if(!in.is_open()) return ;
		
		while(!in.eof())
		{
			string line;
			getline(in, line);
			
			unsigned int equal = line.find("=");
			
			if(equal != string::npos){
				string ident = line.substr(0, equal);
				string valor = line.substr(equal+1);
				//
				if(ident == "name")
					name = valor;
				if(ident == "passw")
					passw = valor;
	
				if(ident == "deck"){
					stringstream ss(valor);
					for(int i=0;i<40;i++)
						ss >> deck[i];
				}
				if(ident == "allCards"){
					stringstream ss(valor);
					while(!ss.eof()){
						int temp;
						ss >> temp;
						allCards.push_back(temp);
					}
				}
				
				//cout << "Indice: -" << ident << "-" << endl;
				//cout << "Valor: -" << valor << "-" << endl;
				//cout << "----" << endl;
			}else{
				//cerr << "Linea sin igual" << endl;
			}			
		}
		
		in.close();
	}
};
#endif
