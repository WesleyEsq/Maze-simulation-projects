#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "list.c"
#include <unistd.h>

int rows, columns, entranceCellI, entranceCellJ, exitCellI, exitCellJ; //rows and columns saves the number of rows and columns in the matrix
int **matrix; //pointer to the matrix where we will draw the maze
int *arrayI; // pointer to the arrays where the new borders will be inserted
int *arrayJ;
int hexagoon; 
long borderCount; // counter of how many borders cells are in the maze at a moment
char **raster; //Bitmap used by the GUI. behaves and is used like a raster file.
int interval = 500000;
int randomMouseAmount = 0, leftHandAmount = 0, rigthHandAmount =0, pledgeAmount = 0, tremauxAmount = 0, fatahAmount = 0;

void calcExits();
int commingDirection( int dir, int posI, int posJ);
int loga2(int x);
bool validDirectionNI(struct Node * node, int dir);
bool validDirectionOI(struct Node * node, int dir);

struct coord
{
 int x;
 int y;
};

void resetCounters(){
    randomMouseAmount = 
    leftHandAmount = 
    rigthHandAmount = 
    pledgeAmount = 
    tremauxAmount = 
    fatahAmount = 0;

}
void setInterval(int amount)
{
    interval = amount;
}

int ranCoord(int max){       //this function returns a pseudorandom number between 0 to max 
    if(max == 0)
        return 0;
    int r = random() % max;
    return r;
}

int power(int base, int power){
    int retvalue = 1;
    while (power > 0)
    {
        retvalue = retvalue * base;
        power--;
    }
    return retvalue;
}

int ** matrixInit(int r, int c){
    rows = r;
    columns = c;
    int **array = calloc(rows, sizeof(int *));
    for(int i = 0; i<rows; i++)
        array[i] = calloc(columns, sizeof(int));

    return array;
}

/*Return a raster   
It's job is to create a raster bitmap of the maze, that the GUI will use to quickly generate it graphically.

"Raster" Is a method used frequenly in map making aplications and GIS systems.
*/
char ** returnARaster(int **matrix)
{
    char ** cMatrix = calloc(rows*2+1, sizeof(char *));

    for(int i = 0; i<rows*2+1; i++)
        cMatrix[i] = calloc(columns*2+1, sizeof(char));

    for (int r = 0; r < rows*2+1; r++){
        for(int c=0; c < columns*2+1; c++){            
            cMatrix[r][c]= 'X';
            }
    }

    for(int x=0; x<rows; x++){
        for (int y=0; y<columns; y++){
            int value = matrix[x][y];
            cMatrix[x*2+1][y*2+1] = 0;

            //Manipulacion de bits (bitwise).
            bool rigth = 1 == (1 & value);
            bool down = 2 == (value & 2);
            //bool left = 4 == (value & 4);
            //bool up = 8 == (value & 8); Variables inecesarias

            //No tengo alternativa más que un monton de ifs.
            if(rigth)
                cMatrix[x*2+1][y*2+2] = 0;

            if(down)
                cMatrix[x*2+2][y*2+1] = 0;
        }
    }
    return cMatrix;
}

void sleepTime(){
    usleep(interval);
}

void makeFile(char dir[])
{
    //char letter[1]; Nunca se usa
    FILE *cleaner = fopen(dir, "w");// Crea el archivo, o si ya existe
    fclose(cleaner);                // lo limpia.

    FILE *append = fopen(dir, "a");
    fprintf(append, "%d", hexagoon);
    fprintf(append, "%s", " ");
    fprintf(append, "%d", rows);
    fprintf(append, "%s", " ");
    fprintf(append, "%d", columns);
    fprintf(append, "%s", " ");
    fprintf(append, "%d", entranceCellI);
    fprintf(append, "%s", " ");
    fprintf(append, "%d", entranceCellJ);
    fprintf(append, "%s", " ");
    fprintf(append, "%d", exitCellI);
    fprintf(append, "%s", " ");
    fprintf(append, "%d", exitCellJ);
    fprintf(append, "%s", "\n");


    for(int x=0; x<rows;x++){
        for (int y=0;y<columns;y++){
            fprintf(append, "%d",matrix[x][y]);
            fprintf(append, "%s"," ");
        }
        fprintf(append, "\n");
    }
    fclose(append);
}

int *initArray(int r, int c){

    long value = 2*r*c;
    value = value/3;
    int * retvalue = calloc(value, sizeof(int));
    return retvalue;
}

void readFile(char dir[])
{
    FILE *pf = fopen(dir, "r"); 
    char charBuffer[5];
    //int width,height;Unnecesary, aparently
    /********************************************
     * here we initializate the meta data variables
     ********************************************/
    
    charBuffer[0] =getc(pf);
    
    if(charBuffer[0]== '1')
        hexagoon = true; //we set if the maze has hexagonal cells

    charBuffer[0] = getc(pf);

    //now we start to initialize the columns and rows variables

    int i = 0;
    int value = 0;

    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }

 
 
    for (i = 0; charBuffer[i] != ' '; i++)  //here we 
    {
        charBuffer[i] = getc(pf);
        if (charBuffer[i] == ' ')
            break;
        

    }
    
    
    for (int j = 0 ; charBuffer[j] != ' '; j++)
    {   
        value =  value + (charBuffer[j] - '0') * power(10, i-1);
        i--;

    }

    rows = value;
    
    value = 0;
    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }
    //printMatrix();
     for (i = 0; (charBuffer[i] != ' ') ; i++)  //here we 
    {
        charBuffer[i] = getc(pf);
        if ((charBuffer[i] == ' ')| (charBuffer[i] == '\n') )
            break; 

    }
    
    
    for (int j = 0 ; (charBuffer[j] != ' ') | (charBuffer[j] != '\n') ; j++)
    {   
        if ((charBuffer[j] == ' ')| (charBuffer[j] == '\n') )
            break; 

        value =  value + (charBuffer[j] - '0') * power(10, i-1);
        i--;
        


    }

    columns = value;
    value = 0;

    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }
    //printMatrix();
     for (i = 0; (charBuffer[i] != ' ') ; i++)  //here we 
    {
        charBuffer[i] = getc(pf);
        if ((charBuffer[i] == ' ')| (charBuffer[i] == '\n') )
            break; 

    }
    
    
    for (int j = 0 ; (charBuffer[j] != ' ') | (charBuffer[j] != '\n') ; j++)
    {   
        if ((charBuffer[j] == ' ')| (charBuffer[j] == '\n') )
            break; 

        value =  value + (charBuffer[j] - '0') * power(10, i-1);
        i--;
        


    }

    entranceCellI = value;
    value = 0;
    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }
    //printMatrix();
     for (i = 0; (charBuffer[i] != ' ') ; i++)  //here we 
    {
        charBuffer[i] = getc(pf);
        if ((charBuffer[i] == ' ')| (charBuffer[i] == '\n') )
            break; 

    }
    
    
    for (int j = 0 ; (charBuffer[j] != ' ') | (charBuffer[j] != '\n') ; j++)
    {   
        if ((charBuffer[j] == ' ')| (charBuffer[j] == '\n') )
            break; 

        value =  value + (charBuffer[j] - '0') * power(10, i-1);
        i--;
        


    }

    entranceCellJ = value;
    value = 0;

    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }
    //printMatrix();
     for (i = 0; (charBuffer[i] != ' ') ; i++)  //here we 
    {
        charBuffer[i] = getc(pf);
        if ((charBuffer[i] == ' ')| (charBuffer[i] == '\n') )
            break; 

    }
    
    
    for (int j = 0 ; (charBuffer[j] != ' ') | (charBuffer[j] != '\n') ; j++)
    {   
        if ((charBuffer[j] == ' ')| (charBuffer[j] == '\n') )
            break; 

        value =  value + (charBuffer[j] - '0') * power(10, i-1);
        i--;
        


    }

    exitCellI = value;
    value = 0;

    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }
    //printMatrix();
     for (i = 0; (charBuffer[i] != ' ') ; i++)  //here we 
    {
        charBuffer[i] = getc(pf);
        if ((charBuffer[i] == ' ')| (charBuffer[i] == '\n') )
            break; 

    }
    
    
    for (int j = 0 ; (charBuffer[j] != ' ') | (charBuffer[j] != '\n') ; j++)
    {   
        if ((charBuffer[j] == ' ')| (charBuffer[j] == '\n') )
            break; 

        value =  value + (charBuffer[j] - '0') * power(10, i-1);
        i--;
        


    }

    exitCellJ = value;
    value = 0;

    matrix = matrixInit(rows, columns);
    arrayI = initArray(rows, columns);
    arrayJ = initArray(rows, columns);

    for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
    {
        charBuffer[i] = 0;
    }


    for(int x=0; x< rows;x++){
        
        for (int y=0;y < columns;y++){

            for (i = 0; charBuffer[i] != ' '; i++)  //here we 
            {
                charBuffer[i] = getc(pf);
                if (charBuffer[i] == ' ')
                    break; 

            }
            
            for (int j = 0 ; charBuffer[j] != ' '; j++)
            {   
                value =  value + (charBuffer[j] - '0') * power(10, i-1);
                i--;


            }
            matrix[x][y] = value;
            value = 0;

            for (int i = 0; i < 5; i++) // we initialize the charBuffer to zeros
            {
                charBuffer[i] = 0;
            }
            
        }
            
            charBuffer[0] = getc(pf);
           
    }
     if(charBuffer[0] == '\n'){
                charBuffer[0] = getc(pf);//Si es un salto de linea, pase al siguiente caracter otra vez
            fclose(pf);
                return; 
        }
    fclose(pf);
}

void RastaGotSoul(){
    raster = returnARaster(matrix);
}

void expandBorders(int i, int j){ // this function creates new borders for a new branch in the spanning tree

    int right = i+1;
    int down = j+1;
    int left = i-1;
    int up  = j-1;

    if(right <= rows-1){                  // here we evaluate if the cells next to the new cell exists. and if it exists and is uninicializated, convert it to a border
        if (matrix[right][j] == 0){      // then, if it is a border add its coordenates to the coordenates arrays
                matrix[right][j] = -1;
                arrayI[borderCount] = right;
                arrayJ[borderCount] = j;
                borderCount++;
            }
            
    }
    if(down <= columns-1){
        if(matrix[i][down] == 0){
                matrix[i][down] = -1;
                arrayI[borderCount] = i;
                arrayJ[borderCount] = down;
                borderCount++;
            }
            
    }
    if(left >= 0){
        if(matrix[left][j] == 0){
                matrix[left][j] = -1;
                arrayI[borderCount] = left;
                arrayJ[borderCount] = j;
                borderCount++;
            }
    }
    if(up >= 0){
        if (matrix [i][up] == 0){
                matrix [i][up] = -1;
                arrayI[borderCount] = i;
                arrayJ[borderCount] = up;
                borderCount++;
            }
    }
    
}

void expandTree(int i, int j){
    
    int branchI[4];         //arrays to store the coordinates of the branch joint candidates
    int branchJ[4];
    int branchCount = 0;    // counter for the brand joint candidates
    int right = i+1;        //coordinates for the surrounding positions of the new cell to add to the tree
    int down = j+1;
    int left = i-1;
    int up  = j-1;

    matrix[i][j] = 0;      // we put in 0 the current cell to later add the corresponding value

    if(right <= rows -1){                          // in this section we evaluate if the surrounding cells are joint candidates for the new cell in the tree
        if (matrix[right][j] > 0){
                branchI[branchCount] = right;
                branchJ[branchCount] = j;
                branchCount++;
            }
            
    }
    if(down <= columns -1){
        if(matrix[i][down] > 0){
                branchI[branchCount] = i;
                branchJ[branchCount] = down;
                branchCount++;
            }
            
    }
    if(left >= 0){
        if(matrix[left][j] > 0){
                branchI[branchCount] = left;
                branchJ[branchCount] = j;
                branchCount++;
            }
    }
    if(up >= 0){
        if (matrix [i][up] > 0){
                branchI[branchCount] = i;
                branchJ[branchCount] = up;
                branchCount++;
            }
    }

 /*when we are done evaluating the surrounding positions we randomly select one
 and get it coordinates*/
    int candidate =  ranCoord(branchCount);
    int candidateI = branchI[candidate];
    int candidateJ = branchJ[candidate];
    
    /*then we update the cell values for the current cell, and the branch where
    we are opening a path to.
    We also evaluate only if the result is 1 or -1 because if it is 0 then the candidate
    is on the vertical axis*/
    if (matrix[candidateI][candidateJ] == 666)  // here we check if we are evaluating the first cell in the tree, so we reinitializate the value to 0
        matrix[candidateI][candidateJ] = 0;
    
    if(i - branchI[candidate] == -1){
        matrix[i][j] = matrix[i][j] + 2; 
        matrix[candidateI][j] = matrix[candidateI][j] + 8;
    }
    if(i - branchI[candidate] == 1){
        matrix[i][j] = matrix[i][j] + 8; 
        matrix[candidateI][j] = matrix[candidateI][j] + 2;
    }

    /*then we check the  vertical axis*/

    if(j - branchJ[candidate] == -1){
        matrix[i][j] = matrix[i][j] + 1; 
        matrix[i][candidateJ] = matrix[i][candidateJ] + 4;
    }
    if(j - branchJ[candidate] == 1){
        matrix[i][j] = matrix[i][j] + 4; 
        matrix[i][candidateJ] = matrix[i][candidateJ] + 1;
    }

}

void calcExits(){

    int entranceCell=ranCoord(rows);
        
    int exitCell = ranCoord(rows);
    
    int roulette = random() % 4;

    if(roulette == 0){          // in this case the entrance will be at the left side of the maze
        entranceCellI = entranceCell;
        entranceCellJ = 0;
       // matrix [entranceCellI][entranceCellJ]= matrix [entranceCellI][entranceCellJ] + 4;
    
        }
    else if(roulette == 1)     // in this case the entrance cell will be on the top of the maze
    {   entranceCellI = 0;
        entranceCellJ = entranceCell;
       // matrix [entranceCellI][entranceCellJ]= matrix [entranceCellI][entranceCellJ] + 8;
    }

    else if(roulette == 2)     // in this case the entrance cell will be at the right side of the maze
    {   entranceCellI = entranceCell;
        entranceCellJ = columns - 1;
       // matrix [entranceCellI][entranceCellJ]= matrix [entranceCellI][entranceCellJ] + 1;
    }

    else                                // in this case the entrance cell will be at the bottom of the maze
    {   entranceCellI = rows - 1;
        entranceCellJ = entranceCell;
       // matrix [entranceCellI][entranceCellJ]= matrix [entranceCellI][entranceCellJ] + 2;
    }

    roulette = random() % 4;


    if(roulette == 0)        // in this case the exit will be at the left side of the maze
    {   
        exitCellI = exitCell;
        exitCellJ = 0;

       // if ((entranceCellI != exitCellI) || (entranceCellJ != exitCellJ))
           // matrix [exitCellI][exitCellJ]= matrix [exitCellI][exitCellJ] + 4;
    }

    else if(roulette == 1)     // in this case the exit cell will be on the top of the maze
    {   
        exitCellI = 0;
        exitCellJ = exitCell;
        //if ((entranceCellI != exitCellI) || (entranceCellJ != exitCellJ))
          //  matrix [exitCellI][exitCellJ]= matrix [exitCellI][exitCellJ] + 8;
        
    }

    else if(roulette == 2)      // in this case the exit cell will be at the right side of the maze
    {      
        exitCellI = exitCell;
        exitCellJ = columns - 1;
       // if ((entranceCellI != exitCellI) || (entranceCellJ != exitCellJ))
           // matrix [exitCellI][exitCellJ]= matrix [exitCellI][exitCellJ] + 1;
    }

    else                                // in this case the exit cell will be at the bottom of the maze
    {   
        exitCellI = rows - 1;
        exitCellJ = exitCell;
       // if ((entranceCellI != exitCellI) || (entranceCellJ != exitCellJ))
           // matrix [exitCellI][exitCellJ]= matrix [exitCellI][exitCellJ] + 2;
    }

    if((entranceCell == exitCellI) || (entranceCellJ == exitCellJ) || entranceCellI > rows || entranceCellJ >columns)
        calcExits();
}

void createMaze(){
    /*first we select a random cell within the 2d array to convert it to the seed of the maze*/
    int firstI = ranCoord(rows);
    int firstJ = ranCoord(columns);
    
    int nextIndex;
    int nextI;
    int nextJ;


    matrix[firstI][firstJ] = 666;
    expandBorders(firstI, firstJ);

    while(borderCount > 0){
        /*we randomly select a border to expand the tree*/
        //printArray();
        nextIndex = ranCoord(borderCount);
        nextI = arrayI[nextIndex];
        nextJ = arrayJ[nextIndex];

        expandTree(nextI, nextJ); // here we choose a border to be added to the spantree
        
        /*then se update the values of the coordinates of the new cell to the values of the last in the borders arrays, and
        decrease the value of borderCount, indicating we have a new cell*/
        
        arrayI[nextIndex] = arrayI[borderCount-1];
        arrayJ[nextIndex] = arrayJ[borderCount-1];
        borderCount--;
        
        /*now we add the new cell border cells and update the border count with the new borders*/
        expandBorders(nextI, nextJ);
    }
    calcExits();
        
}

void cleanUp(){
for(int i = 0; i < rows; i++)
    free(matrix[i]);
free(matrix);

for(int i = 0; i < rows*2+1; i++)
    free(raster[i]);
free(raster);

}

//Creates a new maze with a specific
void redefine(int x, int y)
{
    srandom(time(NULL));
    matrix = matrixInit(x,y);
    arrayI = initArray(x, y);
    arrayJ = initArray(x, y);
    borderCount = 0;
    createMaze();
    raster = returnARaster(matrix);
}

void iWasThere(int I, int J, int algoritmo)
{
    int potenciaDeDos;
    switch (algoritmo)
    {
        case 1: potenciaDeDos = 1; break;
        case 2: potenciaDeDos = 2; break;
        case 3: potenciaDeDos = 4; break;
        case 4: potenciaDeDos = 8; break;
        case 5: potenciaDeDos = 16; break;
        case 6: potenciaDeDos = 32; break;
    }
    char juanito = raster[(I*2)+1][(J*2)+1];
    if (potenciaDeDos != (potenciaDeDos & juanito))
    {
        sleepTime();
        raster[(I*2)+1][(J*2)+1] += potenciaDeDos;
        switch (algoritmo)
        {
            case 1: potenciaDeDos = 1; 
                    leftHandAmount++; break;
            case 2: potenciaDeDos = 2; 
                    randomMouseAmount++; break;
            case 3: potenciaDeDos = 4; 
                    pledgeAmount++; break;
            case 4: potenciaDeDos = 8; 
                    tremauxAmount++; break;
            case 5: potenciaDeDos = 16; 
                    fatahAmount++; break;
            case 6: potenciaDeDos = 32; 
                    rigthHandAmount++; break;
        }
    }
}

void printArray(){
    for(int i = 0; i< rows; i++){
        for(int j = 0; j < columns; j++)
        printf("%d ", matrix[i][j]);
        printf("\n");

    }
     printf("\n");

}

void printSquareLabyrinth(int **matrix)
{
    char cMatrix[rows*2+1][columns*2+1];

    for (int r = 0; r < rows*2+1; r++){
        for(int c=0; c < columns*2+1; c++){            
            cMatrix[r][c]= 'X';
            }
    }

    for(int x=0; x<rows; x++){
        for (int y=0; y<columns; y++){
            int value = matrix[x][y];
            cMatrix[x*2+1][y*2+1] = ' ';

            //Manipulacion de bits (bitwise).
            bool rigth = 1 == (1 & value);
            bool down = 2 == (value & 2);
            bool left = 4 == (value & 4);
            bool up = 8 == (value & 8);

            //No tengo alternativa más que un monton de ifs
            if(rigth)
                cMatrix[x*2+1][y*2+2] = ' ';

            if(down)
                cMatrix[x*2+2][y*2+1] = ' ';
            
            if(left)
                cMatrix[x*2+1][y*2] = ' ';
            
            if(up)
                cMatrix[x*2][y*2+1] = ' ';
            
        }
    }

    if(entranceCellJ == 0)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';
    else if(entranceCellI  == 0)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';
    else if(entranceCellJ  == columns - 1)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';
    else if (entranceCellI  == rows - 1)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';

    
    if(exitCellJ == 0)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';
    else if(exitCellI  == 0)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';
    else if(exitCellJ  == columns - 1)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';
    else if (exitCellI  == rows - 1)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';

    if ((entranceCellI == exitCellI) && (entranceCellJ == exitCellJ))
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = '@';

    for (int r = 0; r< rows*2+1; r++){
        for(int c=0; c < columns*2+1; c++){
            printf("%c",cMatrix[r][c]);
        }
        printf("\n");
    }   
}

// solving algorithms

bool isJunction( int number){
    if (number == 10)
        return false;
    else if (number == 5)
        return false;
    else return true;
}

bool isDeadEnd (int number){
    return ((number == 1) || (number == 2) || (number == 4) || (number == 8));
}

int frontWallReRoute(int dir, int cell){
    if (dir == 1)
        {
            int newdir = random() % 2;
            if (newdir)
                return 8;
            else
                return 2;
        }
    else if (dir == 4)
        {
            int newdir = random() % 2;
            if (newdir)
                return 8;
            else
                return 2;
        }
    else if (dir == 2)
        {
            int newdir = random() % 2;
            if (newdir)
                return 4;
            else
                return 1;
        }
    else if (dir == 8)
        {
            int newdir = random() % 2;
            if (newdir)
                return 4;
            else
                return 1;
        }
}

bool isFrontWall(int dir, int cell){
    
    int direction = cell;
    int calcDir = dir;
    int exponente = -1;
    int wall = 0;
    
    while (calcDir != 0){
        calcDir = calcDir / 2;
        exponente++;
    }

    while(exponente >= 0){
        exponente = exponente - 1;
        wall= direction % 2;
        direction = direction / 2;
        }

    return !wall;

}

void randomMouse(){//Random movements Mouse
    int direction;
    int curI= entranceCellI;
    int curJ = entranceCellJ;
    int currentCell = matrix[curI][curJ];
    iWasThere(curI, curJ, 2);
    bool firstRun = true;


        if(entranceCellJ == 0){
        direction = 1;}
    else if(entranceCellI  == 0)
        direction = 2;
    else if(entranceCellJ  == columns - 1)
        direction = 4;
    else if (entranceCellI  == rows - 1)
        direction  = 8;

    //printWithMouse(matrix, curI, curJ);
    iWasThere(curI, curJ, 2);
    while((curI != exitCellI) || (curJ != exitCellJ)){
    //printWithMouse(matrix, curI, curJ);
    iWasThere(curI, curJ, 2);
        if(firstRun && isFrontWall(direction, currentCell))
            direction = frontWallReRoute(direction, currentCell);


         if(isJunction(currentCell)){
            direction = power(2, (random() % 4));
                if (!isFrontWall(direction, currentCell) ){
                    if (direction == 8) // the mouse goes upward                
                        curI = curI - 1;
                    else if (direction == 1) // the mouse goes to the right
                        curJ = curJ + 1;
                    else if (direction ==2) // the mouse goes downward
                        curI =curI + 1;
                    else if (direction == 4) // the mouse goes to the left
                        curJ = curJ - 1;
                    currentCell = matrix[curI][curJ];
                }
            } 

        else if (!isFrontWall(direction, currentCell) ){
            if (direction == 8) // the mouse goes upward                
                curI = curI - 1;
            else if (direction == 1) // the mouse goes to the right
                curJ = curJ + 1;
            else if (direction ==2) // the mouse goes downward
                curI =curI + 1;
            else if (direction == 4) // the mouse goes to the left
                curJ = curJ - 1;
            currentCell = matrix[curI][curJ];
        }
        
        

    }
    //printWithMouse(matrix, curI, curJ);
    iWasThere(curI, curJ, 2);
}

int getFrontI(int angle, int currentI)
{
        int infront;
        switch (angle)
        {
        case 0: //East
            infront = currentI;
            break;
        
        case 90: //North
            infront = currentI-1;
            break;

        case 180: //West
            infront = currentI;
            break;
        
        case 270: //South
            infront = currentI+1;
            break;
        }
        return infront;
}
int getFrontJ(int angle, int currentJ)
{
        int infront;
        switch (angle)
        {
        case 0: //East
            infront = currentJ+1;
            break;
        
        case 90: //North
            infront = currentJ;
            break;

        case 180: //West
            infront = currentJ-1;
            break;
        
        case 270: //South
            infront = currentJ;
            break;
        }
        return infront;
}

void printWithMouse(int **matrix, int mouseI, int mouseJ)
{
    char cMatrix[rows*2+1][columns*2+1];

    for (int r = 0; r < rows*2+1; r++){
        for(int c=0; c < columns*2+1; c++){            
            cMatrix[r][c]= 'X';
            }
    }

    for(int x=0; x<rows; x++){
        for (int y=0; y<columns; y++){
            int value = matrix[x][y];
            cMatrix[x*2+1][y*2+1] = ' ';

            //Manipulacion de bits (bitwise).
            bool rigth = 1 == (1 & value);
            bool down = 2 == (value & 2);
            bool left = 4 == (value & 4);
            bool up = 8 == (value & 8);

            //No tengo alternativa más que un monton de ifs.
            if(rigth)
                cMatrix[x*2+1][y*2+2] = ' ';

            if(down)
                cMatrix[x*2+2][y*2+1] = ' ';
            
            if(left)
                cMatrix[x*2+1][y*2] = ' ';
            
            if(up)
                cMatrix[x*2][y*2+1] = ' ';
            
        }
    }

    if(entranceCellJ == 0)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';
    else if(entranceCellI  == 0)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';
    else if(entranceCellJ  == columns - 1)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';
    else if (entranceCellI  == rows - 1)
        cMatrix[entranceCellI * 2 + 1][entranceCellJ * 2 + 1] = 'i';

    
    if(exitCellJ == 0)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';
    else if(exitCellI  == 0)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';
    else if(exitCellJ  == columns - 1)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';
    else if (exitCellI  == rows - 1)
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = 'o';

    if ((entranceCellI == exitCellI) && (entranceCellJ == exitCellJ))
        cMatrix[exitCellI * 2 + 1][exitCellJ * 2 + 1] = '@';

    cMatrix[mouseI * 2 + 1][mouseJ * 2 + 1] = 'r';

    for (int r = 0; r< rows*2+1; r++){
        for(int c=0; c < columns*2+1; c++){
            printf("%c",cMatrix[r][c]);
        }
        printf("\n");
    }   
}

void rigthHand(){
    int currentPos[2] = {entranceCellI, entranceCellJ};
    int infrontI, infrontJ, compass = 0;
    bool handOnWall, isFrontFree;
    char value;
    //Valores de brujula: 0 = Este, 90 = norte, 180 = Oeste, 270 = Sur.
    while(currentPos[0] !=exitCellI || currentPos[1] != exitCellJ)
    {
        iWasThere(currentPos[0], currentPos[1], 6);
        value = matrix[currentPos[0]][currentPos[1]];
        bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
        bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
        isFrontFree = true;
        handOnWall = true;

        infrontI = getFrontI(compass, currentPos[0]);
        infrontJ = getFrontJ(compass, currentPos[1]);
        switch (compass)
        {
        case 0: //East
            if(!rigth) isFrontFree = false;
            if(down) handOnWall = false; 
            break;
        
        case 90: //North
            if(!up) isFrontFree = false;
            if(rigth) handOnWall = false; 
            break;

        case 180: //West
            if(!left) isFrontFree = false;
            if(up) handOnWall = false; 
            break;
        
        case 270: //South
            if(!down) isFrontFree = false;
            if(left) handOnWall = false;
            break;
        }

        if(!handOnWall)
        {
            compass -=90;
            if (compass < 0) 
                compass = 270;
            infrontI = getFrontI(compass, currentPos[0]);
            infrontJ = getFrontJ(compass, currentPos[1]);
            currentPos[0]= infrontI;
            currentPos[1]= infrontJ;
        }
        else if(!isFrontFree)
        {
            compass +=90;
            if (compass > 270) compass = 0;
        }

        else
        {
            currentPos[0]= infrontI;
            currentPos[1]= infrontJ;
        }
    }
    iWasThere(currentPos[0], currentPos[1], 6);
}

void leftHand(){
    int currentPos[2] = {entranceCellI, entranceCellJ};
    int infrontI, infrontJ, compass = 0;
    bool handOnWall, isFrontFree;
    char value;
    //Valores de brujula: 0 = Este, 90 = norte, 180 = Oeste, 270 = Sur.
    while(currentPos[0] !=exitCellI || currentPos[1] != exitCellJ)
    {
        iWasThere(currentPos[0], currentPos[1], 1);
        //printWithMouse(matrix, currentPos[0],currentPos[1]);
        value = matrix[currentPos[0]][currentPos[1]];
        bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
        bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
        isFrontFree = true;
        handOnWall = true;

        infrontI = getFrontI(compass, currentPos[0]);
        infrontJ = getFrontJ(compass, currentPos[1]);
        switch (compass)
        {
        case 0: //East
            if(!rigth) isFrontFree = false;
            if(up) handOnWall = false; 
            break;
        
        case 90: //North
            if(!up) isFrontFree = false;
            if(left) handOnWall = false; 
            break;

        case 180: //West
            if(!left) isFrontFree = false;
            if(down) handOnWall = false; 
            break;
        
        case 270: //South
            if(!down) isFrontFree = false;
            if(rigth) handOnWall = false;
            break;
        }

        if(!handOnWall)
        {
            compass +=90;
            if (compass >270) 
                compass = 0;
            infrontI = getFrontI(compass, currentPos[0]);
            infrontJ = getFrontJ(compass, currentPos[1]);
            currentPos[0]= infrontI;
            currentPos[1]= infrontJ;
        }
        else if(!isFrontFree)
        {
            compass -=90;
            if (compass < 0) compass = 270;
        }

        else
        {
            currentPos[0]= infrontI;
            currentPos[1]= infrontJ;
        }
    }
    //printWithMouse(matrix, currentPos[0],currentPos[1]);
    iWasThere(currentPos[0], currentPos[1], 1);
}

void pledge(){
    int currentPos[2] = {entranceCellI, entranceCellJ};
    int infrontI, infrontJ, compass = 0, turns, previousAngle;
    bool handOnWall, isFrontFree;
    char value;
    //Valores de brujula: 0 = Este, 90 = norte, 180 = Oeste, 270 = Sur.
    while(currentPos[0] !=exitCellI || currentPos[1] != exitCellJ)
    {
        turns = 0;
        //printWithMouse(matrix, currentPos[0], currentPos[1]);
        iWasThere(currentPos[0], currentPos[1], 3);
        value = matrix[currentPos[0]][currentPos[1]];
        bool rigth = 1 == (1 & value); 
        bool down = 2 == (value & 2);
        bool left = 4 == (value & 4);  
        bool up = 8 == (value & 8); 
        isFrontFree = true; 
        handOnWall = true;
        infrontI = getFrontI(compass, currentPos[0]); infrontJ = getFrontJ(compass, currentPos[1]);
        switch (compass)
        {
            case 0: //East
                if(!rigth) isFrontFree = false;
                break;
            case 90: //North
                if(!up) isFrontFree = false;
                break;
            case 180: //West
                if(!left) isFrontFree = false;
                break;
            case 270: //South
                if(!down) isFrontFree = false;
                break;
        }
        if(isFrontFree){//move forward
            currentPos[0]=infrontI;
            currentPos[1]=infrontJ;
        }
        else{
            previousAngle = compass;
            compass +=90;
            turns ++;
            if(compass > 270) compass = 0;
            while((currentPos[0] !=exitCellI || currentPos[1] != exitCellJ) && (previousAngle != compass || turns !=0)){
                isFrontFree = true;
                handOnWall = true;
                infrontI = getFrontI(compass, currentPos[0]);
                infrontJ = getFrontJ(compass, currentPos[1]);
                value = matrix[currentPos[0]][currentPos[1]];
                bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
                bool left = 4 == (value & 4);  bool up = 8 == (value & 8); isFrontFree = true; handOnWall = true;
                //printWithMouse(matrix, currentPos[0], currentPos[1]);
                iWasThere(currentPos[0], currentPos[1], 3);
                switch (compass)
                {
                case 0: //East
                    if(!rigth) isFrontFree = false;
                    if(down) handOnWall = false; 
                    break;
                
                case 90: //North
                    if(!up) isFrontFree = false;
                    if(rigth) handOnWall = false; 
                    break;

                case 180: //West
                    if(!left) isFrontFree = false;
                    if(up) handOnWall = false; 
                    break;
                
                case 270: //South
                    if(!down) isFrontFree = false;
                    if(left) handOnWall = false;
                    break;
                }
                        if(!handOnWall)
                        {
                            compass -=90;
                            turns--;
                            if (compass < 0) 
                                compass = 270;
                            infrontI = getFrontI(compass, currentPos[0]);
                            infrontJ = getFrontJ(compass, currentPos[1]);
                            currentPos[0]= infrontI;
                            currentPos[1]= infrontJ;
                        }
                        else if(!isFrontFree)
                        {
                            compass +=90;
                            turns++;
                            if (compass > 270) compass = 0;
                        }

                        else{
                            currentPos[0]= infrontI;
                            currentPos[1]= infrontJ;
                        }
            }
        }
    }
    iWasThere(currentPos[0], currentPos[1], 3);
}


/*⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⣀⣀⡀⠈⡖⡤⠄⠀
⠀⠀⢀⡀⠀⠀⠀⡐⠁⠀⠀⠠⠐⠂⠀⠁⠀⠀⠀⠀
⠀⠰⡁⠐⢉⣉⣭⡍⠁⠂⠉⠘⡀⠀⠀⠀⠀⠂⠡⠀
⢀⣊⠀⡄⠻⠿⠋⠀⠀⠀⠀⠀⢃⠀⠀⠀⠀⠀⠀⢀
⡎⣾⠀⠁⣴⡆⠀⠡⢺⣿⣆⠀⢠⢱⣄⠀⠀⠀⠀⠈
⡑⠟⠀⠀⠀⠀⠀⢀⣸⡿⠟⠀⠀⠈⢿⣿⡦⡀⠀⡰
⠙⠔⠦⣤⣥⣤⣤⣤⡤⠆⠀⠀⠀⠀⢀⢀⠀⠈⠎⠀
⠀⠀⠈⣰⡋⢉⠉⠁⠒⠂⢇⢠⡆⠀⠸⢴⣿⠀⠘⠀
⠀⠀⠘⡿⠃⠀⠨⠒⢆⣸⣿⠁⠀⡠⡇⠈⠋⠀⠰⠀
⠀⠀⠀⠛⠒⠒⠁⠀⠈⠷⡤⠤⠐⠀⠘⠒⠒⠖⠁⠀

  ********************************
 * helping functions for tremaux * Esta bonito el bulbasur jaja
 *********************************/
int commingDirection( int dir, int posI, int posJ){ // returns the direction where we are comming
    
    if(dir == 1)
        return 4;
    else if (dir == 4)
        return 1;
    else if (dir == 2)
        return 8;
    else if (dir == 8)
        return 2;
    else
        return -1;
}


int nextStep( int cell, int dir, int posI, int posJ){ // this function returns the direction of the next step we can take
    
    if((cell == 5) || (cell == 10))     // this is a straight path case, so you can continue in your direction
        return dir;
                                        // the fallowing are elbow cases, so we have only comming path and a going path but are not straight
    else if((cell == 3) && (dir == 8))
        return 1;
    else if ((cell == 3) && (dir == 4))
        return 2;
    else if ((cell == 6) && (dir == 1))
        return 2;
    else if ((cell == 6) && (dir == 8))
        return 4;
    else if ((cell == 12) && (dir == 1))
        return 8;
    else if ((cell == 12) && (dir == 2))
        return 4;
    else if ((cell == 9) && (dir == 2))
        return 1;
    else if ((cell == 9) && (dir == 4))
        return 8;
    else if (isDeadEnd(cell)) // if it is dead end we return the opposite direction;
        return cell;
    else                                // if is none of this cases we have an intersection
        return -1;

}

int returnArray[5];

void possibleDirections(int num)        // this function set returnArray with the quantity of possible directions in returnArray[0]
{                                       // and toggle the possible directions the mouse can go in returnArray[1] for right    
    returnArray[0]= 0;                  // returnArray[2] for down returnArray[3] for left and returnArray[4] for up
    returnArray[1]= 0;
    returnArray[2]= 0;
    returnArray[3]= 0;
    returnArray[4]= 0;

    int temp = num;
    for(int i = 1; i < 5; i++){
        if(temp % 2)
        {
            returnArray[i] = 1;
            returnArray[0] = returnArray[0] + 1;
        }
        temp = temp / 2;
    }
 }

 void setValidDirections(struct Node * n)
 {
    for(int i = 0; i < 4; i++)
    {
        if(returnArray[i+1] == 0)
            n->neigbor[i] = -1;
    }
 }

 int newRandDirection()                 // this function selects one of the possible directions randomly and returns it
 {
    int dirPointer = random() % returnArray[0];
    int next = 1;
    for(dirPointer; dirPointer >= 0; dirPointer--)
    {
        if(returnArray[next] == 1)
            next++;
        else{
            while(returnArray[next] < 1)
                next++;
        }
        
    }

    return power(2, next -1);
 }

 int bestDirection(struct Node * n, int nodeVal, int cDirection){
    int i = 1;
    int newDir = n->neigbor[0];
    int posNewDir = 0;
    while(i < 4){
        if(((n->neigbor[i] < newDir) && (n->neigbor[i] != -1)) || newDir == -1 )
            {newDir = n->neigbor[i];
             posNewDir = i;
            }
            i++;
    }

    if(newDir != 0)
    {
        newDir = power(2, posNewDir);
        possibleDirections(nodeVal);
        while(!validDirectionOI(n,newDir)) //not valid direction
            newDir = newRandDirection();
        return newDir;
    }

    else
    return power(2, posNewDir);

 }

bool validDirectionOI(struct Node * node, int dir){     // this validate the possible directions for a known intersection
    int directionInArray = loga2(dir);
    if (node->neigbor[directionInArray] >= 0)
        return true;
    else 
        return false;
}

int loga2(int x){
    if (x == 1)
        return 0;
    if (x == 2)
        return 1;
    if (x == 4)
        return 2;
    if (x == 8)
        return 3;
    else
        return -1;
}

bool validDirectionNI(struct Node * node, int dir){     // this validate the possible directions for a new intersection
    int directionInArray = loga2(dir);
    if (node->neigbor[directionInArray] == 0)
        return true;
    else
        return false;
}  

void tremaux(){
    int direction;
    int curI= entranceCellI;
    int curJ = entranceCellJ;
    int currentCell = matrix[curI][curJ];
    List *oldInter = createList(); // list of known intersections
    bool commingFromNew= true;
    bool firstRun =true;
 

    if(entranceCellJ == 0){
        direction = 1;}
    else if(entranceCellI  == 0)
        direction = 2;
    else if(entranceCellJ  == columns - 1)
        direction = 4;
    else if (entranceCellI  == rows - 1)
        direction  = 8;
    iWasThere(curI, curJ, 4);
        //printWithMouse(matrix, curI, curJ);

    if(isFrontWall(direction, currentCell))                             // when we enter the maze and get into a frontal wall
            direction = frontWallReRoute(direction, currentCell);       // then we change the initial route to a possible one


    while((curI != exitCellI) || (curJ != exitCellJ)){
        //printWithMouse(matrix, curI, curJ);
        iWasThere(curI, curJ, 4);

        int newDir = nextStep(currentCell, direction,curJ,curI);    // we calculate the new direction where we can walk

        /************************************************************************************************
         * when we got a deadEnd we go back where we came from and mark the comming from new path false *
         ************************************************************************************************/
        if(isDeadEnd(currentCell))
            {
                direction = currentCell;
                if(firstRun)
                    firstRun = false;                
                else                             // we set the flag that tell us we are comming from a new path
                    commingFromNew = false; 

                if (direction == 8) // the mouse goes upward                
                    curI = curI - 1;
                else if (direction == 1) // the mouse goes to the right
                    curJ = curJ + 1;
                else if (direction ==2) // the mouse goes downward
                    curI =curI + 1;
                else if (direction == 4) // the mouse goes to the left
                    curJ = curJ - 1;
                currentCell = matrix[curI][curJ];                       // when we already took the step, we update currentCell
                newDir = nextStep(currentCell, direction, curI, curJ);// and update the newdir variable
                
            }
        /******************************************************************************************************
         * if we step on an intersection, we have various cases: first we evaluate if the intersection is new *
         ******************************************************************************************************/
        else if(newDir == -1)    //when we have an intersection
        {
            if(!isMember(oldInter, curI, curJ))                         // if is a new intersection
            {   int comefrom = commingDirection(direction, curI, curJ); // we get the direction of the path we are comming from
                
                if(comefrom == 1)                                       // and we add the new node marked with the path we're comming from
                    addNode(oldInter, curI, curJ,1, 0, 0, 0);
                else if(comefrom == 2)
                    addNode(oldInter, curI, curJ,0, 1, 0, 0);
                else if(comefrom == 4)
                    addNode(oldInter, curI, curJ,0, 0, 1, 0);
                else if(comefrom == 8)
                    addNode(oldInter, curI, curJ,0, 0, 0, 1);
                
                struct Node * currentNode = getNode(oldInter, curI, curJ);  // then we get the current intersection in the list to evaluate it
                
                possibleDirections(currentCell);                            // we calculate the theorical valid directions
                setValidDirections(currentNode);                            // now we set the information in the node of where are walls 
                newDir = newRandDirection(currentCell);                     // we get a random direction in the current cell
                while(!validDirectionNI(currentNode, newDir))               // if this direction isn't valid for the current node
                    newDir = newRandDirection(currentCell);                 // we recalculate this direction until we get one valid

                if(newDir == 1)                                             // and we update the current node marking the direction we're going now
                    updateNode(oldInter, curI, curJ,1, 0, 0, 0);
                else if(newDir == 2)
                    updateNode(oldInter, curI, curJ,0, 1, 0, 0);
                else if(newDir == 4)
                    updateNode(oldInter, curI, curJ,0, 0, 1, 0);
                else if(newDir == 8)
                    updateNode(oldInter, curI, curJ,0, 0, 0, 1);
                direction = newDir;                                         // we set the direction to the calculated newdir
                commingFromNew = true;                                      // we set the flag that tell us we are comming from a new path
            }
        /******************************************************************************
         * we evaluate if the intersection we step on is while comming from a new path*
         ******************************************************************************/   

            else if(commingFromNew)                                         // this is the case when we visited an repeated intersection from
            {                                                               // a new intersection, then we have to trace back where we came from           
                int comefrom  = commingDirection(newDir, curI, curJ);
                
                if(newDir == 1)                                             // we update the current node marking the direction we're going now
                    updateNode(oldInter, curI, curJ,1, 0, 0, 0);
                else if(newDir == 2)
                    updateNode(oldInter, curI, curJ,0, 1, 0, 0);
                else if(newDir == 4)
                    updateNode(oldInter, curI, curJ,0, 0, 1, 0);
                else if(newDir == 8)
                    updateNode(oldInter, curI, curJ,0, 0, 0, 1);
                direction = newDir;                                        // we update the direction to newDir(the opposite where we came from)
                commingFromNew = false;                                    // and set off the comming from new path flag
            }
         /*****************************************************************************************
         * at last, we evaluate if the intersection we step on is while comming from a known path *
         ******************************************************************************************/

            else                                                            // when we get to a known intersection comming from a known path
            {
                struct Node * currentNode = getNode(oldInter, curI, curJ);
                int comefrom = commingDirection(direction, curI, curJ);  // we get the direction of the path we are comming from
                if(comefrom == 1)                                       // and we add the new node marked with the path we're comming from
                    updateNode(oldInter, curI, curJ,1, 0, 0, 0);
                else if(comefrom == 2)
                    updateNode(oldInter, curI, curJ,0, 1, 0, 0);
                else if(comefrom == 4)
                    updateNode(oldInter, curI, curJ,0, 0, 1, 0);
                else if(comefrom == 8)
                    updateNode(oldInter, curI, curJ,0, 0, 0, 1);
                
                
                    direction = bestDirection(currentNode, currentCell, direction);    //we take the best direction we can, if isnt any, we take any 
                                                                                   // but the direction we came from
                if(currentNode->neigbor[(int)loga2(direction)] == 0)                 // if we are going to a new path
                    commingFromNew = true;                                          // we set on the comming from a new path

                if(direction == 1)                                                  // and we add the new node marked with the path we're comming from
                    updateNode(oldInter, curI, curJ,1, 0, 0, 0);
                else if(direction == 2)
                    updateNode(oldInter, curI, curJ,0, 1, 0, 0);
                else if(direction == 4)
                    updateNode(oldInter, curI, curJ,0, 0, 1, 0);
                else if(direction == 8)
                    updateNode(oldInter, curI, curJ,0, 0, 0, 1);
            }
                /**************************************************************************************************
                 * when we have finished the intersection evaluation, we take a step forward to the new direction *
                 **************************************************************************************************/

            if (direction == 8) // the mouse goes upward                
                    curI = curI - 1;
                else if (direction == 1) // the mouse goes to the right
                    curJ = curJ + 1;
                else if (direction ==2) // the mouse goes downward
                    curI =curI + 1;
                else if (direction == 4) // the mouse goes to the left
                    curJ = curJ - 1;
                currentCell = matrix[curI][curJ];                       // when we already took the step, we update currentCell
                newDir = nextStep(currentCell, direction, curI, curJ);// and update the newdir variable
                
        }
        /********************************************************************************
         * if we are not in a intersection, we are in a path, so we take a step forward *
         ********************************************************************************/
        else
        {
            while(newDir != -1)      // now we go to the previous intersection
            {   if ((curI == exitCellI) && (curJ == exitCellJ))
                    break;
                direction = newDir;
                if (direction == 8) // the mouse goes upward                
                    curI = curI - 1;
                else if (direction == 1) // the mouse goes to the right
                    curJ = curJ + 1;
                else if (direction ==2) // the mouse goes downward
                    curI =curI + 1;
                else if (direction == 4) // the mouse goes to the left
                    curJ = curJ - 1;
                currentCell = matrix[curI][curJ];                       // when we already took the step, we update currentCell
                if(isDeadEnd(currentCell))
                    commingFromNew = false;
                newDir = nextStep(currentCell, direction, curI, curJ);// and update the newdir variable
                //printWithMouse(matrix, curI, curJ);
                iWasThere(curI, curJ, 4);
            }       
        }
    }
}


int ManhatanDistance(int x, int y){
    //struct coord exit = {exitCellI, exitCellJ};
    int mdX = x-exitCellI;
    int mdY = y-exitCellJ;
    if(mdX < 0)
        mdX *=-1;
    if(mdY<0)
        mdY *=-1;
    return(mdX + mdY);
}

int ManhatanDistanceCord(struct coord cordenada){
    int mdX = cordenada.x-exitCellI;
    int mdY = cordenada.y-exitCellJ;
    if(mdX < 0)
        mdX *=-1;
    if(mdY<0)
        mdY *=-1;
    return(mdX + mdY);
}

bool polarLine(struct coord oldPosition, struct coord NewPosition)
{
    //First, we convert the coordinates of both to be centered around the exit.
    int x1 = oldPosition.x -exitCellI;
    int y1 = oldPosition.y -exitCellJ;
    int x2 = NewPosition.x -exitCellI;
    int y2 = NewPosition.y -exitCellJ;
    if(x1 ==0)
    {
        x1 ++; x2++;
    }
    if(x2 == 0)
    {
        x1++; x2++;
    }
    //Then, we calculate the polar angle from the exit. r is not needed.
    int Theta1 = atan(y1/x1);
    int Theta2 = atan(y2/x2);
    if(Theta2 > Theta1)//If it increased in angle, then it moved to the rigth.
        return true;
    else return false;
}

bool isThereAProductivePath(struct coord currentPos, int currentMD){
    int value = matrix[currentPos.x][currentPos.y];
    bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
    bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
    if(rigth && ManhatanDistance(currentPos.x, currentPos.y+1)<currentMD)
    {
        //if(!isDeadEnd(matrix[currentPos.x][currentPos.y+1]))
        return true;
    }
    else if(left && ManhatanDistance(currentPos.x, currentPos.y-1)<currentMD)
    {
        return true;
    }
    else if(down && ManhatanDistance(currentPos.x+1, currentPos.y)<currentMD)
    {
        return true;
    }
    else if(up && ManhatanDistance(currentPos.x-1, currentPos.y)<currentMD)
    {
        return true;
    }
    return false;
}

struct coord moveproductivily(struct coord currentPos, int currentMD){
    int value = matrix[currentPos.x][currentPos.y];
    bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
    bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
    if(rigth && ManhatanDistance(currentPos.x, currentPos.y+1)<currentMD)
    {
        //if(!isDeadEnd(matrix[currentPos.x][currentPos.y+1]))
        currentPos.y++;
    }
    else if(left && ManhatanDistance(currentPos.x, currentPos.y-1)<currentMD)
    {
        currentPos.y--;
    }
    else if(down && ManhatanDistance(currentPos.x+1, currentPos.y)<currentMD)
    {
        currentPos.x++;
    }
    else if(up && ManhatanDistance(currentPos.x-1, currentPos.y)<currentMD)
    {
        currentPos.x--;
    }
    
    return currentPos;
}

void fatah(){
    struct coord currentPos;
    int bestMD;
    currentPos.x = entranceCellI; currentPos.y = entranceCellJ;
    bool derecha; //Para esa cosa de la linea...
    int value;
    int compass = 0;
    bestMD = ManhatanDistanceCord(currentPos);
    while(currentPos.x !=exitCellI || currentPos.y !=exitCellJ)
    {
        value = matrix[currentPos.x][currentPos.y];
        bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
        bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
        int currentMD = ManhatanDistanceCord(currentPos);
        iWasThere(currentPos.x, currentPos.y, 5);
        if(isThereAProductivePath(currentPos, currentMD))
        {
            currentPos = moveproductivily(currentPos,currentMD);
            iWasThere(currentPos.x, currentPos.y, 5);
        }

        else{//Vayase a cualquier otra direccion disponible, evalue si se fue a la derecha o izquierda de la "linea".
            bestMD = ManhatanDistanceCord(currentPos);
            struct coord nextPos;
            derecha = false;
            if(left){
                nextPos.x = currentPos.x; nextPos.y = currentPos.y-1;
                derecha = polarLine(currentPos, nextPos);
                currentPos = nextPos;
            }
            else if(rigth){
                nextPos.x = currentPos.x; nextPos.y = currentPos.y+1;
                derecha = polarLine(currentPos, nextPos);
                currentPos = nextPos;
            }
            else if(up){
                nextPos.x = currentPos.x-1; nextPos.y = currentPos.y;
                derecha = polarLine(currentPos, nextPos);
                currentPos = nextPos;
            }
            else if(down){
                nextPos.x = currentPos.x+1; nextPos.y = currentPos.y;
                derecha = polarLine(currentPos, nextPos);
                currentPos = nextPos;
            }
            compass +=90;
            if (compass >270) compass = 0;
            currentMD = ManhatanDistanceCord(currentPos);
            //Then, if you went to the rigth of the line->, use left hand, otherwise use rigthHand
            if(!derecha)
                while(ManhatanDistanceCord(currentPos)!= bestMD || !isThereAProductivePath(currentPos, currentMD))
                {
                    value = matrix[currentPos.x][currentPos.y];
                    bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
                    bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
                    bool isFrontFree = true;
                    bool handOnWall = true;
                    int infrontI = getFrontI(compass, currentPos.x);
                    int infrontJ = getFrontJ(compass, currentPos.y);
                    //value = matrix[currentPos.x][currentPos.y];
                    isFrontFree = true; handOnWall = true;
                    //printWithMouse(matrix, currentPos.x, currentPos.y);
                    iWasThere(currentPos.x, currentPos.y, 5);
                    switch (compass)
                    {
                    case 0: //East
                        if(!rigth) isFrontFree = false;
                        if(down) handOnWall = false; 
                        break;
                    
                    case 90: //North
                        if(!up) isFrontFree = false;
                        if(rigth) handOnWall = false; 
                        break;

                    case 180: //West
                        if(!left) isFrontFree = false;
                        if(up) handOnWall = false; 
                        break;
                    
                    case 270: //South
                        if(!down) isFrontFree = false;
                        if(left) handOnWall = false;
                        break;
                    }
                            if(!handOnWall)
                            {
                                compass -=90;
                                if (compass < 0) 
                                    compass = 270;
                                infrontI = getFrontI(compass, currentPos.x);
                                infrontJ = getFrontJ(compass, currentPos.y);
                                currentPos.x= infrontI;
                                currentPos.y= infrontJ;
                            }
                            else if(!isFrontFree)
                            {
                                compass +=90;
                                if (compass > 270) compass = 0;
                            }

                            else{
                                currentPos.x= infrontI;
                                currentPos.y= infrontJ;
                            }
                            currentMD = ManhatanDistanceCord(currentPos);
                            iWasThere(currentPos.x, currentPos.y, 5);
                }
            else
                while(ManhatanDistanceCord(currentPos)!= bestMD || !isThereAProductivePath(currentPos, currentMD)){
                        bool isFrontFree = true;
                        bool handOnWall = true;
                        int infrontI = getFrontI(compass, currentPos.x);
                        int infrontJ = getFrontJ(compass, currentPos.y);
                        value = matrix[currentPos.x][currentPos.y];
                        bool rigth = 1 == (1 & value); bool down = 2 == (value & 2);
                        bool left = 4 == (value & 4);  bool up = 8 == (value & 8);
                        isFrontFree = true; handOnWall = true;
                        
                        //printWithMouse(matrix, currentPos.x, currentPos.y);
                        iWasThere(currentPos.x, currentPos.y, 5);
                        switch (compass)
                        {
                        case 0: //East
                            if(!rigth) isFrontFree = false;
                            if(up) handOnWall = false; 
                            break;
                        
                        case 90: //North
                            if(!up) isFrontFree = false;
                            if(left) handOnWall = false; 
                            break;

                        case 180: //West
                            if(!left) isFrontFree = false;
                            if(down) handOnWall = false; 
                            break;
                        
                        case 270: //South
                            if(!down) isFrontFree = false;
                            if(rigth) handOnWall = false;
                            break;
                        }
                                if(!handOnWall)
                                {
                                    compass +=90;
                                    if (compass > 270)
                                        compass = 0;
                                    infrontI = getFrontI(compass, currentPos.x);
                                    infrontJ = getFrontJ(compass, currentPos.y);
                                    currentPos.x= infrontI;
                                    currentPos.y= infrontJ;
                                }
                                else if(!isFrontFree)
                                {
                                    compass -=90;
                                    if (compass < 0) compass = 270;
                                }

                                else{
                                    currentPos.x= infrontI;
                                    currentPos.y= infrontJ;
                                }
                    currentMD = ManhatanDistanceCord(currentPos);
                    iWasThere(currentPos.x, currentPos.y, 5);
                }
        }
    }
}
/*
int main()
{srandom(time(NULL));  
    int x = 2;
    int y = 2;
    matrix = matrixInit(x,y);
    arrayI = initArray(x, y);
    arrayJ = initArray(x, y);
    hexagoon = 0;
    borderCount = 0;
    createMaze();
    RastaGotSoul();
    leftHand();
    return 0;
}
*/
//"rigthHand" = 1, random = 2, pledge = 3, tremaux=4, fatah=5, leftHand = 6