#ifndef CARD_H
#define CARD_H

#include "common.h"
#include "graphics.h"
#include <fstream>
#include <vector>
#include <sstream>

typedef struct Card{
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
		stringstream cardir; cardir << "res/cards/carta" << cardindex << ".cdf";
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
		
		//cout << "Fin carga carta" << endl;
	}
	int getImg(){
		if(img == 0){
			img = loadImg("res/cards/"+imgStr);
			if(img == 0){
				img = loadImg("res/cards/cartaerror.png");
			}
		}
					
		return img;
	}
};

typedef struct Conf{
	//int amount;
	string name;
	string serverHostName;
	int w, h, fs;
	
	Point cardCoord[14]; 
	Conf()
	{
	}
	Conf(const string& filename)
	{
		w=1024; h=768; fs=0;
		
		ifstream in;
		in.open(filename.c_str());
		int j=0;
		
		if(!in.is_open()) return ;
		
		while(!in.eof())
		{
			string line;
			getline(in, line);
			
			unsigned int equal = line.find("=");
			
			if(equal != string::npos){
				string ident = line.substr(0, equal);
				string valor = line.substr(equal+1);
				
				/*if(ident == "amount")
				{
					stringstream ss(valor);
					ss >> amount;
				}*/
				if(ident == "name")
					name = valor;
				
				if(ident == "server");
					serverHostName = valor;
				
				if(ident == "c"){
					stringstream ss(valor);
					
					for(int i=0;i<2;i++){
						char temp[128];
						ss.get(temp, 128, ',');
						ss.get();
						
						if(i==0) cardCoord[j].x = atoi(temp);
						if(i==1) cardCoord[j].y = atoi(temp);
					}
					j++;
				}
				if(ident == "width"){
					stringstream ss(valor);
					ss >> w;
				}
				if(ident == "height"){
					stringstream ss(valor);
					ss >> h;
				}
				if(ident == "fs"){
					stringstream ss(valor);
					ss >> fs;
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

typedef struct User{

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
