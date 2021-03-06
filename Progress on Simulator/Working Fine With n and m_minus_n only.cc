#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <math.h>
#include "sim_bp.h"
int c=0; int c1=0;
bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int GetIndex (unsigned long int addr, unsigned long int size_of_pc, int minus)
{
	unsigned long int n = addr; int binary[32]={0};	unsigned long int sum=0;			// here index and tags are basically the index.size() & tag.size()
	int q=31; 																			// respectively
    while (n!=0 && q>0)
     {
        binary[q]=n%2;                                                                  // 31 is the least signifcant bit
        n=n/2; q--;
     }
    
	 if (minus==0)
     	for (int i=(30-size_of_pc); i<(30); i++)
     	{
        	sum=sum+pow(2,(29-i))*binary[i];
        	//printf("%d", binary[i]);
     	}
     
    else if(minus==1)																// to extract the bits from reverse side
    	for (int i=(32-size_of_pc); i<(32); i++)
     	{
        	sum=sum+pow(2,(31-i))*binary[i];
        	printf("%d\n", binary[i]);
     	}
    else if (minus==2)																// to extract the bits from front side
    	for (int i=(32-params.M1); i<( 32-(params.M1-size_of_pc) ); i++)
    	{
    		sum=sum+pow(2,(31-i-params.M1+params.N) )*binary[i];
    		printf("%d", binary[i]);
		}
     	return sum;
}

void G_history_counter(char outcome, int G[])
{	
	for (int i=0; i<params.N; i++)
	{
		G[params.N-1-i]=0;
		G[params.N-1-i]=G[params.N-2-i];
	}
	G[0] = (outcome=='t') ? 1 : 0;	
}

int Global_History_Value(int G[])
{
	unsigned long int sum=0;
	for (int i=0; i<params.N; i++)
	{
		//sum=sum+pow(2,params.N-i-1)*G[i];
		printf("%d",G[i]);	
	}
	return sum;
}

int ConcatenateValues(unsigned long int new_n, unsigned long int m_minus_n)
{
	unsigned long int x = m_minus_n; int binary1[32]={0};	int combine[params.M1];
	unsigned long int n = new_n; 	 int binary0[32]={0};	unsigned long int sum=0;		
	int q=31; 		
																		
    while (n!=0 && q>0)
     {
        binary0[q]=n%2;                                                                  // 31 is the least signifcant bit
        n=n/2; q--;
     }
	
	int q1=31; 																			
    while (x!=0 && q1>0)
     {
        binary1[q1]=x%2;                                                                 // 31 is the least signifcant bit
        x=x/2; q1--;
     }
     
    for (int i=(32-params.M1+params.N); i<32; i++)
    {
		printf("%d", binary0[i]);
		combine[34-i]=binary0[i];
	}
	printf("\t");
     
	for (int i=(32-params.N); i<32; i++)
	{
		printf("%d", binary1[i]);
		combine[31-i]=binary1[i];
	}
	printf("\t");
	for (int i=params.M1-1; i>=0; i--)
	{
		printf("%d",combine[i]);	
		sum=sum+pow(2,i)*combine[i];
	}
	printf("\t");
	
	return sum;
}

void G_Share(unsigned long int B[], unsigned long int index, unsigned long int sets, char outcome, int G[])
{
	printf(" Index: %lu\t",index);
	unsigned long int n_for_g_share = GetIndex(index,params.N,2);				// extract m minus n only first
	printf("\t 'n' value is %lu ", n_for_g_share);

	//unsigned long int n_for_g_share = GetIndex(index,(params.M1-params.N),2);	// extract n bits only first 
	//printf("'n' value %lu:\t", n_for_g_share);
	//printf(" 'n'value %lu\n", n_for_g_share);
	
	unsigned long int m_minus_n_for_g_share = GetIndex(index,(params.M1-params.N),1);				// extract m minus n only first
	printf("\t 'm-n' value is %lu\n", m_minus_n_for_g_share);
	//printf(" 'm-n' value is %lu\n", m_minus_n_for_g_share);
	/*
	unsigned long int m_minus_n_for_g_share = GetIndex(index,(params.M1-params.N),2);	// extract n bits only first 
	//printf("'m-n' value %lu\t", m_minus_n_for_g_share);
	printf("'m-n' value %lu:\t", m_minus_n_for_g_share);
		
	unsigned long int g = Global_History_Value(G);										// extract the contents of g variable only first
	printf("\t");

	G_history_counter(outcome,G);														// update the gloabl history only after you are done with the operations
		
	unsigned long int new_n = (g^n_for_g_share);										// finding out new value for (n previous_n ^ g_share contents)
	//printf("\t%lu ",new_n);
		
	int new_index=ConcatenateValues(new_n,m_minus_n_for_g_share);						// extract m bits by concatenating new_n with m_minus_n
	printf("\t newindex %lu\n", new_index);

	//printf("\n The updated values for history counter are with outcome is %c: ", outcome);*/
}

void Bimodal(unsigned long int B[], unsigned long int index, unsigned long int sets, char outcome)
{

 for (int i=0; i<sets; i++)
     {
    if ( (i==index && B[i]>=2 && outcome=='n') || (i==index && B[i]<=1 && outcome=='t') )
     		c1++;
     		
        if (i==index && outcome=='t')
        {
            B[i] = ( (B[i]++)>=3) ? 3 : B[i]++ ;
            goto OUT;
        }
        else if (i==index && outcome=='n')
        {
           B[i] = ( (B[i]--)<=0) ? 0 : B[i]-- ;
            goto OUT;
        }
    }
    OUT:c++;
}


int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;

    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    unsigned long int index;// Find Out the index value
    unsigned long int sets; // Total number of entries in the table
    

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    params.bp_name  = argv[1];

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    int G[params.N];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
		 sets = pow(2,params.M2);
	else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
	{
		 sets = pow(2,params.M1);
		for (int i=0; i<params.N; i++)
				G[i]=0;	
		//printf("The inital contents of G_history are: ");
		//for (int i=0; i<params.N; i++)
				//printf("%d",G[i]);
	}
    unsigned long int B[sets];
     for (int i=0; i<sets; i++)
        B[i]=2;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    char str[2];
while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        outcome = str[0];
/********************************************************************************************************************************************************************************************
                                                            Add branch predictor code here
******************************************************************************************************************************************************************************************************/
     if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
		{
	 		index=GetIndex(addr, params.M2,0);
	 		Bimodal(B,index,sets,outcome);
	 	}
	
	else if(strcmp(params.bp_name, "gshare") == 0)           // Gshare
		{
			printf("%lx ", addr);
			index=GetIndex(addr, params.M1,0);
			G_Share(B,index,sets,outcome,G);
		}
     
	}
	/*
	float a = c; float b = c1;
	printf("OUTPUT\n");
	printf("number of predictions: %d\n", c);
    printf("number of mispredictions: %d\n", c1);
    printf("misprediction rate: %0.2f%\n", b*100/a);
    if(strcmp(params.bp_name, "bimodal") == 0)              		  // Bimodal
    	printf("FINAL BIMODAL CONTENTS\n");
    else if(strcmp(params.bp_name, "gshare") == 0)           	// Gshare
    	printf("FINAL GSHARE CONTENTS\n");
    for (int i=0; i<sets; i++)
        {
            printf("%lu\t", i);
            printf("%lu\n", B[i]);
        }*/
    return 0;
}
