#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"


TLista AlocareCelula(void *x)
{
	TLista aux;
	aux = (TLista)malloc(sizeof(Celula));
	if (!aux) return NULL;
	aux->info = x;
	aux->urm = NULL;
	return aux;
}

int AdaugareMasa(ALista L, void* x)
/*intoarce 1-succes, 0-eroare*/
{
	TLista ultim = NULL, p = *L, aux;
	aux = AlocareCelula(x);
	if(!aux) return 0;
	for(; p != NULL; ultim = p, p = p->urm);
	if(ultim == NULL) { 
		*L = aux;
	} else {	
		ultim->urm = aux;
	}
	return 1;
}

TLista InitL()
/*initializarea listei circulare cu SANTINELA*/
{
	TLista aux;
	aux = (TLista)malloc(sizeof(Celula));
	if (!aux) return NULL;
	aux->info = NULL;
	aux->urm = aux;
	return aux;
}

int AdaugareJucator(TLista L, void* x)
/*intoarce 1-succes, 0-eroare*/
{
	TLista ultim = L, aux;
	while(ultim->urm != L) ultim = ultim->urm;
	aux = AlocareCelula(x);
	if(!aux) return 0;
	aux->urm = L;
	ultim->urm = aux;
	return 1;
}

void AfisareListaMese(TLista L, void (*fafi)(void*, FILE*), FILE * f)
{
	if(L == NULL) {
		fprintf(f, "Sala este inchisa!");
	} else {
		TLista aux = L;
		for(; aux != NULL; aux = aux->urm)
		{
			fafi(aux->info, f);
		}
	}
}

void AfisareListaJucatori(TLista L, void (*fafis)(void*,char, FILE*), FILE* f)
{
	TLista aux = L->urm;
	for(; aux != L; aux = aux->urm)
	{
		if(aux->urm != L) {
			fafis(aux->info, ';', f);
		} else {
			fafis(aux->info, '.', f);
		}
	}
}

void AfiJucator(void * x, char c, FILE* f)
{
	fprintf(f, " %s - %d%c", ((Jucator)x)->nume, ((Jucator)x)->nrMaini, c);
}

void AfiMasa(void *y, FILE* f)
{
	fprintf(f, "%s:", ((Masa)y)->numeMasa);
	AfisareListaJucatori(((Masa)y)->jucatori, AfiJucator, f);
	fprintf(f, "\n");	
}

void DistrugeListaMese(ALista L, void(*felib1)(void*))
{
	TLista aux;
	while(*L != NULL)
	{
		aux = *L;
		*L = aux->urm;
		felib1(aux->info);
		free(aux);
	}
	
}

void DistrugeListaJucatori(ALista L, void(*felib1)(void*))
{
	TLista aux, inc = (*L)->urm;
	while(inc != *L)
	{
		aux = inc;
		inc = inc->urm;
		felib1(aux->info);
		free(aux);
	}
	free(inc);
}

void EliberareJucator(void *x)
{
	free(((Jucator)(x))->nume);
	free((Jucator)(x));
}

void EliberareMasa(void *y)
{
	free(((Masa)(y))->numeMasa);
	DistrugeListaJucatori(&(((Masa)(y))->jucatori),EliberareJucator);
	free((Masa)(y));
}

void DistrugeSala(Sala S)
{
	DistrugeListaMese(&(S->masa), EliberareMasa);
	free(S);
}

void EliminareMasa(Sala Sala, TLista* LM, TLista MasaC, void(*felib1)(void*))
{
	TLista ant = NULL; 
	TLista aux = *LM;
	for(; aux != NULL;)
		if(aux == MasaC) {
			Sala->nrMese--;
			Sala->nrLocMax -= ((Masa)(MasaC->info))->nrMaxJucatori;
			if(ant == NULL) {
				*LM = aux->urm;
			} else {
				ant->urm = aux->urm;
			}
			felib1(MasaC->info);
			free(MasaC);
			break;
		} else {
			ant = aux; 
			aux = aux->urm;
		}
}


int EliminareJucator(Sala Sala, TLista MasaC, TLista Jucator, 
					void(*felib1)(void*))
/*returneaza 1 daca se elimina si masa si 0 daca nu e nevoie de eliberarea 
mesei */
{
	TLista ant = ((Masa)(MasaC->info))->jucatori; 
	TLista aux = (((Masa)(MasaC->info))->jucatori)->urm;
	for(; aux != ((Masa)(MasaC->info))->jucatori;)
	{
		if(aux == Jucator) {
			ant->urm = aux->urm;
			felib1(Jucator->info);
			free(Jucator);
			Sala->nrLocCrt--;
			((Masa)(MasaC->info))->nrCrtJucatori--;
			break;
		} else {
			ant = aux; 
			aux = aux->urm;
		}
	}
	if(((Masa)(MasaC->info))->nrCrtJucatori <= 0) {
		EliminareMasa(Sala, &(Sala->masa),MasaC, EliberareMasa);
		return 1;
	}
	return 0;	
}

TLista CautareMasa(TLista Mese, char* nume_masa)
/*intoarce adresa mesei cautate*/
{
	TLista aux = Mese;
	for(;aux != NULL; aux = aux->urm)
		if(strcmp(((Masa)(aux->info))->numeMasa,nume_masa) == 0) return aux;
	return NULL;
}

TLista CautareJucator(TLista Jucatori, char* nume_jucator)
/*intoarce adresa jucatorului cautat*/
{
	TLista aux = Jucatori->urm;
	for(;aux != Jucatori; aux = aux->urm)
		if(strcmp(((Jucator)(aux->info))->nume,nume_jucator) == 0) return aux;
	return NULL;
}

void Noroc(TLista Mese, char* masa, char* jucator, int grad_noroc, FILE * f)
{
	TLista masa_cautata = CautareMasa(Mese, masa);
	if(!masa_cautata) {
		fprintf(f,"Masa %s nu exista!\n", masa);
	} else {
		TLista jucator_cautat = CautareJucator(((Masa)(masa_cautata->info))->
												jucatori, jucator);
		if(!jucator_cautat) {
			fprintf(f,"Jucatorul %s nu exista la masa %s!\n", jucator, masa);
		} else {
			((Jucator)(jucator_cautat->info))->nrMaini += grad_noroc;	
		}
	}
}

void Ghinion(Sala Sala, char* masa, char* jucator, int grad_noroc, FILE * f)
{
	TLista masa_cautata = CautareMasa(Sala->masa, masa);
	if(!masa_cautata) {
		fprintf(f, "Masa %s nu exista!\n", masa);
	} else {
		TLista jucator_cautat = CautareJucator(((Masa)(masa_cautata->info))->jucatori, jucator);
		if(!jucator_cautat) {
			fprintf(f, "Jucatorul %s nu exista la masa %s!\n", jucator, masa);
		} else {
			((Jucator)(jucator_cautat->info))->nrMaini -= grad_noroc;
			if(((Jucator)(jucator_cautat->info))->nrMaini <= 0) 
				EliminareJucator(Sala, masa_cautata, jucator_cautat,
					EliberareJucator);	
		}
	}
}

void Tura(Sala Sala, char * masa, FILE * f)
{
	int ok;
	TLista masa_cautata = CautareMasa(Sala->masa, masa);
	TLista aux, juc, ant;
	if(!masa_cautata) {
		fprintf(f, "Masa %s nu exista!\n", masa);
	} else {
		ant = ((Masa)(masa_cautata->info))->jucatori;
		aux = (((Masa)(masa_cautata->info))->jucatori)->urm;
		(((Masa)(masa_cautata->info))->jucatori)->urm = aux->urm;
		juc = aux->urm;
		for(;juc != ((Masa)(masa_cautata->info))->jucatori; ant = juc, 
			juc = juc->urm);
		ant->urm = aux;
		aux->urm = ((Masa)(masa_cautata->info))->jucatori;
		for(juc = (((Masa)(masa_cautata->info))->jucatori)->urm ;
			juc != ((Masa)(masa_cautata->info))->jucatori;)
		{
			((Jucator)(juc->info))->nrMaini--;
			if(((Jucator)(juc->info))->nrMaini <= 0) {
				aux = juc->urm;
				ok = EliminareJucator(Sala, masa_cautata, juc,EliberareJucator);
/*verific daca masa la care se afla jucatorul a fost stearsa*/
				if (ok == 1) {
					break;
/*daca masa nu a fost stearsa trec la urmatorul jucator de la masa*/
				} else {
					juc = aux;
				}			
			} else {
				juc = juc->urm;
			}	
		}
	}
	
}

void Tura_completa(Sala Sala, FILE * f)
{
	TLista masaC, aux;
	for(masaC = Sala->masa; masaC != NULL;)
	{
		aux = masaC->urm;
		Tura(Sala, ((Masa)(masaC->info))->numeMasa, f);
		masaC = aux;	
	}
	
}

void Inchide(Sala Sala, char * masa, FILE * f)
{
	TLista masa_cautata = CautareMasa(Sala->masa, masa);
	int loc_disp;
	TLista aux, juc, ant, masaC;
	if(!masa_cautata) {
		fprintf(f, "Masa %s nu exista!\n", masa);
	} else {
		loc_disp = (Sala->nrLocMax - ((Masa)(masa_cautata->info))->nrMaxJucatori) -
			(Sala->nrLocCrt - ((Masa)(masa_cautata->info))->nrCrtJucatori);
		if(((Masa)(masa_cautata->info))->nrCrtJucatori > loc_disp) {
			fprintf(f, "Nu exista suficiente locuri in sala!\n");
		} else {
			masaC = Sala->masa;
			for(;((Masa)(masa_cautata->info))->nrCrtJucatori > 0 && 
				masaC != NULL; masaC = masaC->urm)
			{
				if(masaC != masa_cautata) {
					while(((Masa)(masaC->info))->nrCrtJucatori < 
						((Masa)(masaC->info))->nrMaxJucatori &&
						((Masa)(masa_cautata->info))->nrCrtJucatori > 0)
					{
						for(juc = (((Masa)(masaC->info))->jucatori)->urm;
							juc != ((Masa)(masaC->info))->jucatori; ant = juc, 
							juc = juc->urm);
						aux = (((Masa)(masa_cautata->info))->jucatori)->urm;
						ant->urm = aux;
						(((Masa)(masa_cautata->info))->jucatori)->urm =aux->urm;
						aux->urm = ((Masa)(masaC->info))->jucatori;
						((Masa)(masaC->info))->nrCrtJucatori++;
						((Masa)(masa_cautata->info))->nrCrtJucatori--;
					}
				}
			}
			EliminareMasa(Sala, &(Sala->masa), masa_cautata, EliberareMasa);		
		}
	}	
}

void EliminareCopie(ALista L)
/*eliberarea listei construite pentru ordonarea necesara clasamentului*/
{
	TLista aux, ant;
	for(aux = *L; aux != NULL;) {
		ant = aux->urm;
		free(aux);
		aux = ant;
	}
}

void Clasament(Sala Sala, char * masa, FILE * f)
{
	TLista masa_cautata = CautareMasa(Sala->masa, masa);
	TLista juc, aux, lista_noua = NULL, ant, jucC, antC;
	int ok;	
	if(!masa_cautata) {
		fprintf(f, "Masa %s nu exista!\n", masa);
	} else {
		for(juc = (((Masa)(masa_cautata->info))->jucatori)->urm; 
			juc != ((Masa)(masa_cautata->info))->jucatori; juc = juc->urm)
		{
			aux = AlocareCelula((void *)juc->info);
			if (!aux) {
/*daca nu se poate face alocarea se elimina toata lista construita pana atunci*/
				EliminareCopie(&lista_noua);
				return;
			}
			if(lista_noua == NULL) {
				lista_noua = aux;
				lista_noua->urm = NULL;
			} else {
				ant = NULL;
				ok = 0;
				jucC = lista_noua;
				while(ok == 0 && jucC != NULL)
				{
/*verific daca pot face inserarea inainte unui jucator cu un numar de maini
mai mic decat cel al jucatorului pe care vreau sa il inserez*/
					if(((Jucator)(jucC->info))->nrMaini <
						 ((Jucator)(aux->info))->nrMaini) {
						if(ant == NULL) {
							aux->urm = lista_noua;
							lista_noua = aux;
						} else {
							aux->urm = jucC;
							ant->urm = aux;
						}
						ok = 1;
					}else if(((Jucator)(jucC->info))->nrMaini ==
						 ((Jucator)(aux->info))->nrMaini) {
						antC = ant;
/*parcurg lista creata pana atunci pentru a gasi locul potrivit, comparand
numele jucatorilor*/
/*cand gasesc locul potrivit opresc parcurgerea in continuare a listei*/
						while(jucC != NULL && ((Jucator)(jucC->info))->nrMaini 
								== ((Jucator)(aux->info))->nrMaini && ok == 0)
						{
/*verific daca il pot aseza in fata jucatorului curent*/
							if(strcmp(((Jucator)(jucC->info))->nume,
								((Jucator)(aux->info))->nume) < 0) {
								if(antC == NULL) {
									aux->urm = lista_noua;
									lista_noua = aux;
								} else {
									aux->urm = jucC;
									antC->urm = aux;
								}
/*dovada ca am facut inserarea inaintea unui jucator cu acelasi numar de maini*/
								ok = 1;
							} else {
								antC = jucC;
								jucC = jucC->urm;
							}
						}
/*daca nu il pot aseza in fata nici unui jucator cu acelasi numar de maini, 
atunci il asez dupa*/
						if(ok == 0) {
							aux->urm = antC->urm;
							antC->urm = aux;
							ok = 1;
						}
/*daca am ajuns la sfarsitul listei nou construite si nu am reusit sa fac 
inserarea inserez la sfarsitul acesteea*/
					}else if(((Jucator)(jucC->info))->nrMaini >
						 ((Jucator)(aux->info))->nrMaini && jucC->urm == NULL) {
						jucC->urm = aux;
						aux->urm = NULL;
						ok = 1;
					}
/*daca elementul curent din lista are un numaar de maini mai mare decat al 
elementului pe care vreau sa il inserez si daca nu am ajuns la sfarsitul 
listei nou create, atunci trec la urmatorul jucator din lista noua*/
					if (ok == 0) {
						ant = jucC; 
						jucC = jucC->urm;
					}	 
				}			
			}						
		}
		fprintf(f, "Clasament %s:\n", masa);
		for(jucC = lista_noua; jucC != NULL; jucC = jucC->urm)
			fprintf(f, "%s %d\n", ((Jucator)(jucC->info))->nume,
				((Jucator)(jucC->info))->nrMaini);
		EliminareCopie(&lista_noua);
	}
}

TLista InitializareListaJucatori(int nr, FILE * f)
{
	char c; 
	int i, j = 1;
	TLista L = InitL();
	for(;j <= nr; j++) 
	{	
		i = 1;
		Jucator juc = (Jucator)malloc(sizeof(struct jucator));
		if(!juc) {
			DistrugeListaJucatori(&L, EliberareJucator); 
			return NULL;
		}
/*initial aloc memorie pentru numele jucatorului doar pentru un char*/
		juc->nume = (char*)malloc(sizeof(char));
		if(!juc->nume) {
			free(juc);
			DistrugeListaJucatori(&L, EliberareJucator); 
			return NULL;
		}
		while((c = fgetc(f)) != ' ')
		{	
			if(c != '\n')
			{
				i = i + 1;
/*pe masura ce citesc cate o litera noua fac realocare pentru numele 
jucatorului*/
				juc->nume = (char*)realloc(juc->nume, i * sizeof(char));
/*daca realocarea esueaza sterg toata lista de jucatori creata pana la acest
moment*/
				if(!juc->nume) {
					free(juc->nume);
					free(juc);
					DistrugeListaJucatori(&L, EliberareJucator); 
					return NULL;
				}
/*dupa realocare pun litera citita sirul de caractere corespunzator numelui
jucatorului*/
				juc->nume[i-2] = c;
			} 
		}
		juc->nume[i-1] = '\0';
		fscanf(f, "%d", &(juc->nrMaini));	
		AdaugareJucator(L, (void*)juc);
	}
	return L;	
}

TLista InitializareListaMese(int nr, int* crt, int* max, FILE * f)
{
	char c; 
	int i, j = 1;
	TLista L = NULL;
	for(;j <= nr; j++) 
	{	
		i = 1;
		Masa masaCrt = (Masa)malloc(sizeof(struct masa));
/*daca alocarea nu reuseste sterg lista de mese*/
		if(!masaCrt)  {
			DistrugeListaMese(&L, EliberareMasa); 
			return NULL;
		}
		masaCrt->numeMasa = (char*)malloc(sizeof(char));
		if(!masaCrt->numeMasa) {
			free(masaCrt);
			DistrugeListaMese(&L, EliberareMasa); 
			return NULL;
		}
		while((c = fgetc(f)) != ' ')
		{	
			if(c != '\n')
			{
				i = i + 1;
/*realoc memorie pentru numele mesei pe masura ce citesc cate o litera din
numele acesteea*/
				masaCrt->numeMasa = (char*)realloc(masaCrt->numeMasa, 
									i * sizeof(char));
				if(!masaCrt->numeMasa) {
					free(masaCrt->numeMasa);
					free(masaCrt);
					DistrugeListaMese(&L, EliberareMasa); 
					return NULL;
				}
				masaCrt->numeMasa[i-2] = c;
			} 
		}
		masaCrt->numeMasa[i-1] = '\0';
		fscanf(f, "%d", &(masaCrt->nrCrtJucatori));
		*crt = *crt + masaCrt->nrCrtJucatori;
		fscanf(f, "%d", &(masaCrt->nrMaxJucatori));
		*max = *max + masaCrt->nrMaxJucatori;
		masaCrt->jucatori = InitializareListaJucatori(masaCrt->nrCrtJucatori,f);
		if(!masaCrt->jucatori) return NULL;		
		AdaugareMasa(&L, (void*)masaCrt);
	}
	return L;	
}


int main(int argc, char *argv[])
{
	char m[50], j[50],c[50];
	int n;
	FILE *in = fopen(argv[1], "rt");
	if (in == NULL) {
		fprintf(stderr, "ERROR: Can`t open file %s", argv[1]);
		return -1;
	}
	FILE *in1 = fopen(argv[2], "rt");
	if (in == NULL) {
		fprintf(stderr, "ERROR: Can`t open file %s", argv[2]);
		return -1;
	}
	FILE *in2 = fopen(argv[3], "wt");
	if (in == NULL) {
		fprintf(stderr, "ERROR: Can`t open file %s", argv[3]);
		return -1;
	}
	Sala salaJoc = (Sala)malloc(sizeof(struct sala));
	fscanf(in, "%d", &(salaJoc->nrMese));
	salaJoc->nrLocCrt = 0;
	salaJoc->nrLocMax = 0;
	salaJoc->masa = InitializareListaMese(salaJoc->nrMese, &(salaJoc->nrLocCrt),
			 &(salaJoc->nrLocMax), in);
	if(!salaJoc->masa) {
		printf("Eroare initializare sala de joc\n");
		return -1;
	}
	while (fscanf(in1, "%s", c) != EOF) {
		if(strcmp(c,"print") == 0) {
			AfisareListaMese(salaJoc->masa, AfiMasa, in2);
		} else if(strcmp(c,"noroc") == 0) {
			fscanf(in1, "%s %s %d", m, j, &n);
			Noroc(salaJoc->masa, m, j, n, in2);
		} else if(strcmp(c,"ghinion") == 0) {
			fscanf(in1, "%s %s %d", m, j, &n);
			Ghinion(salaJoc, m, j, n, in2);	
		} else if(strcmp(c,"tura") == 0) {
			fscanf(in1, "%s", m);
			Tura(salaJoc, m, in2);
		} else if(strcmp(c,"tura_completa") == 0) {
			Tura_completa(salaJoc, in2);
		} else if(strcmp(c,"clasament") == 0) {
			fscanf(in1, "%s", m);
			Clasament(salaJoc, m, in2);
		} else if(strcmp(c,"inchide") == 0) {
			fscanf(in1, "%s", m);
			Inchide(salaJoc, m, in2);
		}
	}	
	DistrugeSala(salaJoc);
	fclose(in);
	fclose(in1);
	fclose(in2);
	return 0;
}
