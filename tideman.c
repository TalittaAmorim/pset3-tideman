#include <cs50.h>
#include <stdio.h>
#include <string.h>
// Max número de candidatos
#define MAX 9

// preferences[voter][ranking] número de eleitores que preferem i em vez de j
int preferences[MAX][MAX];

//  locked[i][j] significa que i está fechado em cima de j
bool locked[MAX][MAX];

// Cada par tem um vencedor e um perdedor
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array de candidatos
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Gráfico claro de locked em pares
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // consulta eleitor
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] é a preferência dos eleitores
        int ranks[candidate_count];

        // Consulta para cada posição 
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Atualização dos rank/postos dada uma nova votação
bool vote(int rank, string name, int ranks[])
{
    for(int i =0; i< candidate_count; i++)
    {
       if(strcmp(candidates[i] ,name ) == 0) 
       {
          ranks[rank] = i; // autalizar a matriz de classificação de cada eleitor com as preferências de rank de cada eleitor. EXMPL: (eleitor 1)[ 0,1,2]
          return true; 
       }
       
    }
    return false;
}

// atualizar o array de preferências , que é um array 2D que indica o número de eleitores que preferem o candidato i ao candidato j .
void record_preferences(int ranks[])
{ 
    for( int i =0; i <candidate_count; i++)
    {   //colunas
        for(int j = i+1; j< candidate_count; j++) //  começar de i + 1 , pois só queremos adicionar ao array de preferências quando j vier depois de i no array de ranks .
        { 
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Registrar pares de candidatos onde um é preferido em relação ao outro ao arrays pairs
void add_pairs(void)
{
    for(int i =0; i < candidate_count; i++)
    {
        for( int j = i +1; j < candidate_count; j++)  
        /*. Isso é feito para evitar verificações repetidas da matriz de preferências .Exemplo de três candidatos, o candidato [0] será verificado contra [1] e [2], então o candidato [1] será verificado 
        apenas contra o candidato [2]. Isso preenche cada combinação exatamente
        uma vez, pois os candidatos não devem ser comparados a si mesmos.*/
        {
            if(preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if(preferences[i][j] < preferences[j][i])
            {
                  pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    
    return;
}

int pair_weight(i)
{
    int pair_winner = pairs[i].winner; 
    int pair_losers = pairs[i].loser;
    return preferences[pair_winner][pair_losers];
} // obtemos o vencedor e o perdedor desse par
void sort_pairs(void)
{      // selection sort em ordem descrescente
    for( int i = pair_count -1; i >=0; i--) 
    {
        // força minima da vitoria
        int min_weight = pair_weight(i); // vai retornar o número mínimo para ganhar. Exemol: if alice vencee bob por 7 a 3, entao o o numero minimo é 7
        int min_idx = i; // quardar o índice do par comparado
        
        // j tem esse valor, pois queremos incrementar cada valor que vem antes do peso mínimo(MAIOR NUM)
        for( int j= i -1; j>=0; j--)
        {
            if(pair_weight(j) < min_weight) //se algum par for menor do valor já conhecido, ele vai autalizar 
            {
                min_weight = pair_weight(j);
                min_idx = j; //atualizar onde esta localizado o valor mínimo 
            }
        }
    
    // troca 
    pair temp = pairs[min_idx]; 
    pairs[min_idx] = pairs[i];
    pairs[i] = temp;
    }
    return;
}

bool tem_ciclo( int winner,int loser)
{
    if(locked[loser][winner] == true) // se a gente já tem o ciclo
    {
        return true;
    }
    for(int i =0; i < candidate_count; i++)
    {
        if(locked[loser][i] == true && tem_ciclo(winner,i)) // verificar se trocar o loser com outro candidato que ja tiver bloqueado e verificar se 
        {
            return true;
        }
    }
    return false;
}
// Bloquear pares no gráfico do candidato em ordem, sem criar ciclos.
void lock_pairs(void)
{
    for(int i =0; i < pair_count; i++)
    {
        int winner = pairs[i].winner; 
        int loser = pairs[i].loser;
        
        if(!tem_ciclo(winner,loser))
        {
            locked[winner][loser] = true; 
        }
    }
    return;
}

// Imprimir o venceor da eleição 
void print_winner(void)
{
    for(int i =0; i < candidate_count; i++)
    {
        int count_false = 0;
        
        for(int j =0; j < candidate_count; j++)
        {
            if(locked[j][i] == false)
            {
                count_false++;
                if(candidate_count == count_false)
                {
                    printf(" %s\n", candidates[i]);
                }
            }
        }
    }
    return;
}

