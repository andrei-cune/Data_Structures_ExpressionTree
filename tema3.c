#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

typedef char* TInfo;
typedef struct nod
{
	TInfo info;
	struct nod *st,*dr;
}TNod,*TArb,**AArb;

TArb ConstrFr(TInfo x)
{
	TArb a;
	a = (TArb) malloc (sizeof (TNod));
	if(!a)
		return NULL;
	a->info = malloc( 10 *sizeof(char));;
	if ( ! a->info )
	{
		free(a);
		return NULL;
	}
	strcpy(a->info , x);
	a->st = NULL;
	a->dr = NULL;
	return a;

}
void afis(TArb a)
{	
	if(!a)
		return;
	printf("%s\n" , a->info );
	afis(a->st);
	afis(a->dr);
}
void distruge(TArb r)    //functie auziliara
{
	if(!r)
		return ;
	distruge( r->st );
	distruge( r->dr );
	free( r->info );
	free( r );
}
void DistrugeA( TArb *r)
{
	if( !(*r) )
		return;
	distruge(*r);
	*r = NULL;
}
int verifFrunza(char* f)
{
	if ((strcmp(f,"+")!=0) && (strcmp(f,"-")!=0) &&     //verifcam daca e informatia (char*) se afla pe o poz frunza
		(strcmp(f,"*")!=0) && (strcmp(f,"/")!=0) && 
		(strcmp(f,"^")!=0) && (strcmp(f,"sqrt")!=0))

		return 1;
		else 
		return 0;
}


void inserare( TArb *r , TInfo q )
{	
	if(q == NULL)
		return;
	
	if( !(*r) )
		(*r) = ConstrFr( q);   //in caz ca nodul este null construim frunza
  
	
	if( verifFrunza( q ))   //verificare
		return;
	

	if( strcmp( q , "sqrt" ) == 0 )     //caz in care avem radical ,avem nevoie doar de un fiu
	{
		(*r)->dr = NULL;     // dreptul este deja pe null insa este mai intuitiv asa
		inserare( &(*r)->st ,strtok( NULL , " \n" ));   //inseram urmatoarea valoare citita pe fiul stang
	}
		
	if ((strcmp(q,"+")==0) || (strcmp(q,"-")==0) ||     
		(strcmp(q,"*")==0) || (strcmp(q,"/")==0) || 
		(strcmp(q,"^")==0) )

	{
		inserare( &(*r)->st ,strtok( NULL , " \n" ));
		inserare( &(*r)->dr ,strtok( NULL , " \n" ));   //inseram urmatoarele valori 
	}

		return;
 
}

int calcul(TArb a , char values[][10] , char int_values[][10] ,int N , int *okay1  ,FILE *out)
{	
	
	if(  a != NULL && *okay1 == 0 )
	{


	if(verifFrunza( a->info ) )
	{
		int i;
		for( i = 0; i < N ; i++ )
			if( strcmp( a->info , values[i] ) == 0 )   //cazul in care avem frunza
			{	
				*okay1 = 0;
				return atoi(int_values[i]);
			}
			
		if( *(a->info) >= 48 &&  *(a->info) <= 57 )
		{	
			
			return atoi(a->info);
		}
			*okay1 = 1;
		if(*okay1 == 1)
		{	
			fprintf(out , "Variable %s undeclared\n" , a->info );
			return 999999;
		}
	}
	
	int rez_st = calcul( a->st , values , int_values , N ,okay1  , out);   //recursiv retinem valorile din fiul stang si drept
	int rez_dr = calcul( a->dr , values , int_values , N ,okay1  , out);
	

	if( strcmp( a->info , "+" ) == 0 && *okay1 == 0 )
		return rez_st + rez_dr;

	if( strcmp( a->info , "-" ) == 0  && *okay1 == 0 )
	return rez_st - rez_dr;

	if( strcmp( a->info , "*" ) == 0  && *okay1 == 0 )   //in functie de semn returnam valorile corespunzatoare
	return rez_st * rez_dr;
	
	if( strcmp( a->info , "/" ) == 0  &&  *okay1 == 0 )
	{
		if(rez_dr == 0)
			{	
				*okay1 = 1;
				fprintf( out , "Invalid expression\n");
				return 999999;
			}
		return rez_st / rez_dr;
	} 
	

	if( strcmp( a->info , "^" ) == 0  && *okay1 == 0)
	return pow( rez_st , rez_dr );

	if( strcmp( a->info , "sqrt" ) == 0  && *okay1 == 0)
	{
		if( rez_st < 0 )
		{	
			*okay1 = 1;
			fprintf( out , "Invalid expression\n");
			return 999999;
		}
		return sqrt( rez_st);
	}


	}
	return 101;

}
int main(int argc , char const *argv[])
{	
    FILE *in,*out;
    
    in = fopen(argv[1] ,"rt");
    if(!in)
    {
        fprintf( stderr,"Unable to open %s" , argv[1]);  //deschidem fisierele
        return -1;
    }
    out = fopen(argv[2] ,"wt");
    if(!out)
    {
        fprintf( stderr,"Unable to open %s" , argv[2]);
        return -1;
    }

     char* buffer;
    size_t lungime = 32;
    buffer = malloc (lungime);
    if(!buffer)
        return 0;
    
    getline( &buffer , &lungime , in);
    unsigned int N = atoi( buffer );
    int i;

    const char* delims = " =\n";
    char values[N][10];
    char int_values[N][10];
    for( i = 0;i < N ; i++)
    {  
        getline( &buffer , &lungime ,in);
        char* val1 = strtok( buffer , delims );    //construim vectorii de valori efective si valori numerice
        if( !val1 )
        	return 0;
        char* val2 = strtok( NULL , delims );
        if( ! val2 )
        	return 0;
        
        

        strcpy( values[i] , val1);
        strcpy( int_values[i] , val2);
        
        
    }
  	getline( &buffer , &lungime , in);        
    unsigned int M = atoi( buffer );
    

    for( i =0; i < M ; i++)
    {
    	getline( &buffer , &lungime ,in);  //citim primul semn
    	TArb a = NULL;

    	inserare( &a , strtok(buffer , " \n"));  // construirea arborelui
    	int okay1=0;

    	int result = calcul(a,values,int_values,N, &okay1  , out);
    	//if( result != 999999  && okay1 == 0 )
    	if( okay1 == 0 )
    	fprintf( out , "%d\n" , result);  //calculul propriuzis

    	DistrugeA( &a );  
    }
    
    free (buffer);
    fclose(in);
    fclose(out);
	return 0;
}