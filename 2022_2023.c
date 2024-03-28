#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX 512

 //-std=gnu11
typedef struct nodo{
	int km;
	int numEv;
	int* ev;
	struct nodo* left;
	struct nodo* right;
}stazione;

typedef struct appoggio{
	unsigned short length;
	int km;
	unsigned short* index;
}appoggio;

typedef struct adiacenza{
	unsigned short length;
	int km;
	int range;
	unsigned short* index;
}adiacenza;

int gIndex;

stazione* creaNodo(int key)
{
	stazione* head = (stazione*)malloc(sizeof(stazione));
	int temp;
	if(head == NULL)
		exit(EXIT_FAILURE);
	head->km = abs(key);
	if(fscanf(stdin,"%d",&head->numEv)) //inserisco num veicoli
		head->numEv = abs(head->numEv);
	else
	{
		free(head);
		return NULL;
	}
	if(head->numEv > MAX)
	{
		free(head);
		return NULL;
	}
	if(head->numEv > 0)
	{
		head->ev = (int*)calloc(head->numEv, sizeof(int));
		for(int i = 0; i < head->numEv; i++)
		{
			if(fscanf(stdin,"%d",&temp))
				head->ev[i] = abs(temp);
			
			else
			{
				free(head);
				return NULL;
			}
		}
	}
	else
		head->ev = NULL;
		
	head->left = NULL;
	head->right = NULL;
	return head;
}

stazione* aggiungiStazione(stazione* root, int key)
{
	if(root == NULL)
	{
		stazione* ns = creaNodo(key);
		fprintf(stdout,"aggiunta\n");
		return ns;
	}
	
	if(key < root->km)
		root->left = aggiungiStazione(root->left,key);
		
	else if(key > root->km)
		root->right = aggiungiStazione(root->right,key);
		
	else if(key == root->km)
	{
		fprintf(stdout,"non aggiunta\n");
	}
	
	return root;
}

stazione* leftmostStation(stazione* nodo)
{
	stazione* curr = nodo;
	while(curr && curr->left != NULL)
		curr = curr->left;
	return curr;
}

stazione* rimuoviStazione(stazione* root, int distanza) //con binary research
{
	if(root == NULL)
	{
		fprintf(stdout,"non demolita\n");
		return root;
	}
	if(distanza < root->km)
		root->left = rimuoviStazione(root->left,distanza);
	else if(distanza > root->km)
		root->right = rimuoviStazione(root->right,distanza);
	else
	{
		if(root->left == NULL)
		{
			stazione* temp = root->right;
			free(root->ev);
			free(root);
			fprintf(stdout,"demolita\n");
			return temp;
		}
		else if(root->right == NULL)
		{
			stazione* temp = root->left;
			free(root->ev);
			free(root);
			fprintf(stdout,"demolita\n");
			return temp;
		}

		stazione* temp = leftmostStation(root->right);
		root->km = temp->km;
		free(root->ev);
		root->ev = (int*)calloc(temp->numEv,sizeof(int));
		//memcpy(root->ev,temp->ev,temp->numEv);
		for(int i = 0; i<temp->numEv;i++)
			root->ev[i] = temp->ev[i];
		root->numEv = temp->numEv;
		root->right = rimuoviStazione(root->right,temp->km);
	}
	return root;
}

void printListInOrder(stazione* root)
{
	if(root != NULL)
	{
		printListInOrder(root->left);
		fprintf(stdout,"-> %d \nAUTO: ",root->km);
		if(root->numEv > 0)
			for(int i = 0; i < root->numEv; i++)
				fprintf(stdout,"[%d] ",root->ev[i]);
		fprintf(stdout,"\n");
		printListInOrder(root->right);
	}
	return;
}



void aggiungiAuto(stazione* root, int distanza, int autonomia)
{
	while(root != NULL)
	{
		if(distanza > root->km)
			root = root->right;
		else if(distanza < root->km)
			root = root->left;
		else
		{
			root->ev = (int*)realloc(root->ev,sizeof(int)*(root->numEv+1));
			root->ev[root->numEv] = autonomia;
			root->numEv = root->numEv + 1;
			fprintf(stdout,"aggiunta\n");
			return;
		}
	}
	fprintf(stdout,"non aggiunta\n");
	return;
}

void rimuoviAuto(stazione* root, int distanza, int autonomia)
{
	while(root != NULL)
	{
		if(distanza > root->km)
			root = root->right;
		else if(distanza < root->km)
			root = root->left;
		else
		{
			int i = 0;
			while(root->ev[i] != autonomia && i < root->numEv)
				i++;
			if(root->ev[i] == autonomia)
			{
				for(int j = i; j < root->numEv - 1; j++)
					root->ev[j] = root->ev[j+1];
				root->numEv = root->numEv - 1;
				//root->ev = (int*)realloc(root->ev,root->numEv);
				fprintf(stdout,"rottamata\n");
				return; 
			}
			else
			{
				fprintf(stdout,"non rottamata\n");
				return; 	
			}
		}
	}
	fprintf(stdout,"non rottamata\n");
	return;
}

void printAdjList(adiacenza* v, int dim)
{
	fprintf(stdout,"LISTA ADIACENZA \n{ ");
	for(int i = 0; i<dim; i++)
	{
		printf("\n %d: ",v[i].km);
		for(int j = 0; j < v[i].length; j++)
			printf("[ STAZ ADJA INDEX: %u  ] -> ",v[i].index[j]);
		printf("fin  \n");
	}
	fprintf(stdout,"}\n\n");
	return;
}

void listaAdiacenzaAndata(adiacenza* v, int dim, int start, int dest, int i) //start < dest; reminder controllare e sistemare eventuali memory leaks
{
	int reach;
	unsigned short dimVett = 0;

	reach = v[i].range + v[i].km;
	for(int j = i+1; j < dim && reach >= v[j].km; j++)
		dimVett++;
	v[i].length = dimVett;
	v[i].index = realloc(v[i].index,dimVett*sizeof(unsigned short));
	for(int j = i+1; j < dim && reach >= v[j].km; j++)
		v[i].index[j-i-1] = (unsigned short)j;
	
	return;
}

void listaAdiacenzaRitorno(adiacenza* v, int dim, int start, int dest) //start > dest; reminder controllare e sistemare eventuali memory leaks - porri di memoria
{
	appoggio app[dim]; //lista adiacenza intera NB non mi serve parametro range; dichiara struct apposita per risparmiare spazio!
	int reach;
	int skip = dim-1;
	int dimVett = 0;
	int z = 0;
	unsigned short alloc[dim];
	for(int i = 0; i<dim; i++)
	{
		v[i].index = (unsigned short*)calloc(1,sizeof(unsigned short));
		v[i].length = 0;
		alloc[i] = 1;
	}
	for(int i = 0; i < dim; i++)
	{
		reach = v[i].km - v[i].range;
		for(int j = dim-1; reach > v[j].km; j--)
			skip--;		
		for(int j = skip; j >= i+1; j--)
			dimVett++;
		
		app[i].km = v[i].km; //NB fai struttura apposita per app con solo short* index e int length
		app[i].length = (unsigned short)dimVett;
		app[i].index = NULL;
		app[i].index = (unsigned short*)realloc(app[i].index,dimVett*sizeof(unsigned short));
		

		for(int j = skip; j >= i+1; j--)
			app[i].index[skip-j] = (unsigned short)j; //criticità su indice J, ma dovrebbe essere corretto
		for(int j = 0; j < app[i].length; j++)
		{
			z = app[i].index[j];
			v[z].index[v[z].length] = (unsigned short)i;
			v[z].length++;
			if(v[z].length == alloc[z])
			{
				if(alloc[z]*2 <= dim)
					alloc[z] = alloc[z]*2;
				else
					alloc[z] = (unsigned short)dim;
				v[z].index = (unsigned short*)realloc(v[z].index,sizeof(unsigned short)*alloc[z]);
			}
		}
		free(app[i].index);
		skip = dim-1;
		dimVett = 0;
	}
	
	return;
}

bool bfsAndata(adiacenza* v, int dim, int start, int dest, int* pred) //v sarebbe la coda; in posizione zero ho il nodo di start e in posizione dim-1 quello di destinazione
{
	int queue[dim]; //ci salvo l'indice di V
	int front = 0; //quando faccio la pop: incremento front
	int rear = 0; //quando faccio la push, metto elemento in coda e incremento. Nota: FRONT <= REAR SEMPRE!! se front == rear, coda vuota
	int u;
	bool visited[dim];
	//adiacenza* curr; //NB posizione zero in quanto la prima cella del vettore è quella di start
	
	for (int i = 0; i < dim; i++) 
	{
		visited[i] = false;
		pred[i] = -1;
	}
    
	visited[rear] = true; //per struttura non ho stazioni prima di start, quindi l'indice di source è zero
	queue[rear] = 0;
	rear++;
	
	while(front < rear /*&& rear < dim*/)
	{
		u = queue[front];
		listaAdiacenzaAndata(v,dim,start,dest,u);
		front++;
		//sistema indicizzazione basato sulla lista COMPLETA e non sugli indici delle liste di adiacenza dei nodi!! quindi va cambiata indicizzazione lista adj		
		for(int j = 0; j < v[u].length; j++)
		{
			if(visited[v[u].index[j]] == false)
			{
				
				visited[v[u].index[j]] = true;
				pred[v[u].index[j]] = (unsigned short)u;
				queue[rear] = v[u].index[j];
				rear++;
				if(v[u].index[j] == dim-1)
					return true;
		
			}
		}
	}
	return false;
}

bool bfsRitorno(adiacenza* v, int dim, int* pred) //v sarebbe la coda; in posizione zero ho il nodo di start e in posizione dim-1 quello di destinazione
{
	int queue[dim]; //ci salvo l'indice di V
	int front = dim - 1; //quando faccio la pop: incremento front
	int rear = dim - 1; //quando faccio la push, metto elemento in coda e incremento. Nota: FRONT <= REAR SEMPRE!! se front == rear, coda vuota
	int u;
	bool visited[dim];
	//adiacenza* curr; //NB posizione zero in quanto la prima cella del vettore è quella di start
	
	for (int i = 0; i < dim; i++) 
	{
		visited[i] = false;
		pred[i] = -1;
	}
    
	visited[dim-1] = true; //per struttura non ho stazioni prima di start, quindi l'indice di source è zero
	queue[dim-1] = dim - 1;
	front--;
	
	while(front < rear /*&& rear < dim*/)
	{
		u = queue[rear];
		rear--;
		//sistema indicizzazione basato sulla lista COMPLETA e non sugli indici delle liste di adiacenza dei nodi!! quindi va cambiata indicizzazione lista adj		
		for(int j = v[u].length-1; j >= 0; j--)
		{
			if(visited[v[u].index[j]] == false)
			{
				visited[v[u].index[j]] = true;
				pred[v[u].index[j]] = (unsigned short)u;
				queue[front] = v[u].index[j];
				front--;
				if(v[u].index[j] == 0)				
					return true;
			
			}
		}
	}
	return false;
}

void deallocateTree(stazione* root)
{
	if(root == NULL)
		return;
	deallocateTree(root->left);
	deallocateTree(root->right);
	free(root->ev);
	free(root);
}

void getDim(stazione* root, int* dim, int start, int dest)
{
	if(root != NULL)
	{
		getDim(root->left,dim,start,dest);
		if((root->km >= start && root->km <= dest) || (root->km >= dest && root->km <= start))
			*dim+=1;
		getDim(root->right,dim,start,dest);
	}
	return;
}

void riempiInOrder(stazione* root, int start, int dest, adiacenza* v, int dim)
{
	//printf("\n\nROOT->%d\n\n",root->km);
	if(start < dest)
	{
		if(root != NULL)
		{
			
			riempiInOrder(root->left, start, dest, v, dim);
			if(root->km >= start && root->km <= dest)
			{
				int max = -1;
				v[gIndex].km = root->km;
				v[gIndex].index = NULL;
				v[gIndex].length = 0;
				for(int j = 0; j < root->numEv; j++)
				{
					if(max <= root->ev[j])
						max = root->ev[j];

				}
				if(max < 0)
					max = 0;
				v[gIndex].range = max;
				gIndex++; 
			}
			riempiInOrder(root->right, start, dest, v, dim);
		}
		//fprintf(stdout,"\n\nINDEX: %d\n\n",i);
	}
	else
	{
		//fprintf(stdout,"\n\nINDEX: %d\n\n",gIndex);
		if(root != NULL)
		{
			riempiInOrder(root->left, start, dest, v, dim);
			if(root->km >= dest && root->km <= start)
			{
				int max = -1;
				v[gIndex].km = root->km;
				v[gIndex].index = NULL;
				v[gIndex].length = 0;
				for(int j = 0; j < root->numEv; j++)
					if(max <= root->ev[j])
						max = root->ev[j];
				if(max < 0)
					max = 0;
				v[gIndex].range = max;
				gIndex--;
			}
			riempiInOrder(root->right, start, dest, v, dim);
		}
	}
}

void pianificaPercorso(stazione* root, int start, int dest)
{
	int dim = 0;
	int j;
	if(start == dest) //caso base
	{
		fprintf(stdout,"%d\n",start);
		return;
	}
	getDim(root,&dim,start,dest);
	adiacenza v[dim]; 
	int pred[dim];
	if(start < dest)
	{
		gIndex = 0;
		riempiInOrder(root,start,dest,v,dim);
		gIndex = 0;
		if(bfsAndata(v,dim,start,dest,pred) == false) //pred tiene memorizzati gli indici
		{
			fprintf(stdout,"nessun percorso\n");
			for(int i = 0; i<dim; i++)
				free(v[i].index);
			return;
		}
		for(int i = 0; i<dim; i++)
			free(v[i].index);
	
		int rear = 0;
		int path[dim];
		int crawl = dim-1; 
		path[rear] = crawl;
		rear++;
	
		while(pred[crawl] != -1)
		{
			path[rear]= pred[crawl];
			crawl = pred[crawl];
			rear++;
		}
		j = rear - 1;
		for(int i = 0; i < dim; i++)
		{
			if(path[j] == i)
			{
				if(i != dim-1)
				{
					fprintf(stdout,"%d ",v[i].km);
					j--;
				}
				else
					fprintf(stdout,"%d\n",v[i].km);	
			}
		}
		return;
	}
	else
	{
		gIndex = dim-1;
		riempiInOrder(root,start,dest,v,dim);
		listaAdiacenzaRitorno(v, dim, start, dest);
		gIndex = dim-1;
		//printAdjList(v,dim);
		if(bfsRitorno(v,dim,pred) == false) //pred tiene memorizzati gli indici
		{
			fprintf(stdout,"nessun percorso\n");
			for(int i = 0; i<dim; i++)
				free(v[i].index);
			return;
		}
		for(int i = 0; i<dim; i++)
			free(v[i].index);
		int rear = dim-1;
		int path[dim];
		int crawl = 0; 
		path[rear] = crawl;
		rear--;
		while(pred[crawl] != -1)
		{
			path[rear]= pred[crawl];
			crawl = pred[crawl];
			rear--;
		}
		j = dim - 1;
		for(int i = 0; i < dim; i++)
		{
			//fprintf(stdout,"[%d][%d]->",path[j],i);
			if(path[j] == i)
			{
				if(i != dim-1)
				{
					fprintf(stdout,"%d ",v[i].km);
					j--;
				}
				else
					fprintf(stdout,"%d\n",v[i].km);	
			}
		}
		return;
	}
}

int main()
{
	stazione* root = NULL;
	char comando[20];
	int distanza;
	int autonomia;
	int start,dest;
	int num = 0;
	while(fscanf(stdin,"%s",comando) == 1)
	{
		if(strcmp(comando,"aggiungi-stazione") == 0)
		{
			if(fscanf(stdin,"%d",&distanza))
				root = aggiungiStazione(root,distanza);
			num++;
		}
		else if(strcmp(comando,"demolisci-stazione") == 0)
		{
			if(fscanf(stdin,"%d",&distanza))
				root = rimuoviStazione(root,distanza);
			num++;
		}
		else if(strcmp(comando,"aggiungi-auto") == 0)
		{
			if(fscanf(stdin,"%d",&distanza) && fscanf(stdin,"%d",&autonomia))
				aggiungiAuto(root,distanza,autonomia);
			num++;
		}
		else if(strcmp(comando,"rottama-auto") == 0)
		{
			if(fscanf(stdin,"%d",&distanza) && fscanf(stdin,"%d",&autonomia))
				rimuoviAuto(root,distanza,autonomia);
			num++;
		}
		else if(strcmp(comando,"pianifica-percorso") == 0)
		{
			if(fscanf(stdin,"%d",&start) && fscanf(stdin,"%d",&dest))
				pianificaPercorso(root,start,dest);
			num++;
		}
		strcpy(comando,"");
	}
	deallocateTree(root);
	return 0;
}

