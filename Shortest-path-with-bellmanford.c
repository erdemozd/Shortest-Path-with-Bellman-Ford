#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SIZE 50
typedef struct edge {
    char fromnode[100];
    int fromid;
    char tonode[100];
    int toid;
    int weight;
} Edge;

typedef struct distance {
    char node[100];
    int weight;
} Distance;

typedef struct prev {
    char fromnode[100];
    char backtonode[100] ;
} Prev;

const int MAXI = INT_MAX;

static void show_usage(char* name)
{
    fprintf(stderr, "Usage: %s <option(s)> SOURCES\n\
\tOptions:\n\
\t\t-h,--help\t\tShow this help message\n\
\t\t-i,--infile INFILE\tSpecify the input file path\n\
\t\t-o,--outfile OUTFILE\tSpecify the output file path\n", name);
}

int getID(char *wanted, Distance* arr, int size) //this function is for getting the index from Distance type structure.It recieves the wanted string, Distance typed array and the size of the array.
{
    for(int i = 0; i<size ; i++) //looping the entire array for wanted element
    {
        if((strcmp(arr[i].node, wanted)) == 0) //if the wanted string and the element of current index is the same, we return the current index.
            return i;
    }
    return -1; //otherwise, return -1.
}


int main(int argc, char* argv[])
{
    if(argc<5){
        show_usage(argv[0]);
        return 2;
    }
    char infile[100], outfile[100];
    for (int i = 1; i < argc; ++i) {
        char arg[15];
        strcpy(arg, argv[i]);
        if (!strcmp(arg,"-h") || !strcmp(arg,"--help")) {
            show_usage(argv[0]);
            return 0;
        }
        else if (!strcmp(arg,"-i") || !strcmp(arg,"--infile")) {
            if (i + 1 < argc) {
                strcpy(infile,argv[++i]);
            } else {
                fprintf(stderr, "--infile option requires one argument.");
                return 1;
            }
        }
        else if (!strcmp(arg,"-o") || !strcmp(arg,"--outfile")) {
            if (i + 1 < argc) {
                strcpy(outfile,argv[++i]);
            } else {
                fprintf(stderr, "--outfile option requires one argument.");
                return 1;
            }
        } else {
            show_usage(argv[0]);
            return 1;
        }
    }
    int w; //The edge distance
    char astr[100], bstr[100]; //Source(a) and destination(b) for each edge
    char fromnode[100], tonode[100]; //Initial and final nodes

    Edge edges[15]; //Vector holds edge list
    char tokenHolder[100];         			//Just temporary variable to hold tokens which are in the same line.
    FILE* fp = fopen(infile, "r");			//Input file handler

    fgets(tokenHolder, 100, fp);
    printf("%s", tokenHolder);

    int numberofedges, numberofnodes;
    sscanf(tokenHolder, "%d %d", &numberofnodes, &numberofedges);

    Distance *distance = calloc(numberofnodes, sizeof(Distance));
    Prev *prevs = calloc(numberofnodes, sizeof(Prev)); //Holds previous node when coming from the initial node.

    //Input file reading.
    //Containers are initialized
    int edgeCount=0;
    for(int i=0; i<numberofedges; i++){
        fgets(tokenHolder,100,fp);
        sscanf(tokenHolder, "%s %s %d", astr, bstr, &w );
        strcpy(edges[edgeCount].fromnode, astr);
        strcpy(edges[edgeCount].tonode, bstr);
        edges[edgeCount].weight = w;
        edgeCount++;
    }

    //Last line in input file is read.
    fgets(tokenHolder,100,fp);
    sscanf( tokenHolder, "%s %s", fromnode, tonode);

    fclose(fp);

    //Control if input is read correctly

    for(int i=0; i<numberofedges; i++){
        printf("%s -> %s: %d\n", edges[i].fromnode, edges[i].tonode, edges[i].weight);
    }

    //Start algorithm

    Distance uniquenodes[numberofnodes]; // i created a array of unique nodes from edge list

    int uniquenodeselements = 0; //looking for unique elements in edge list's first column
    for(int i = 0; i < numberofedges; i++) //searching all the edges
    {
        int elementFound = 0; //initilazing a boolean
        for(int k = 0; k<uniquenodeselements; k++)
        {
            if(strcmp(edges[i].fromnode,uniquenodes[k].node) == 0) //if a repetitive element is found ,break from the loop since we dont want any repetitions.
            {
                elementFound = 1; //indication of element found
                break;
            }
        }
        if(elementFound == 0) { //if there is no repetiton
            strcpy(uniquenodes[uniquenodeselements].node, edges[i].fromnode); //add the element to our unique array.
            uniquenodeselements++; //increment the element size to move to other index
        }
    }
    for(int i = 0; i<numberofedges; i++) //looking for the unique elements in the edges list for SECOND COLUMN(there could be undetected elements in the first column so we need to trace other column as well.)
    {
        int elementFound1 = 0; //initializing a boolean
        for(int k = 0; k<uniquenodeselements; k++) //unique nodes elements could be incremeneted on the upper lines, it means we could be on other indexis
        {
            if(strcmp(edges[i].tonode,uniquenodes[k].node) == 0) //if we find a repetitive element,
            {
                elementFound1 = 1; //we mark the boolean to True and break from loop.
                break;
            }
        }
        if(elementFound1 == 0){ //if a unique element is found
            strcpy(uniquenodes[uniquenodeselements].node, edges[i].tonode); //we are adding it to our unique array.
            uniquenodeselements++; //and incrementing the index.
        }
    }

    for (int i = 0; i < numberofedges; i++) { //by using the getID function that defined on the top, we are getting every index of out unique array.
        edges[i].fromid = getID(edges[i].fromnode, uniquenodes, uniquenodeselements);
        edges[i].toid = getID(edges[i].tonode, uniquenodes, uniquenodeselements);

    }

    // Bellman Ford
    int formerpath[numberofnodes]; //this is a predecessor array for Bellman Ford algorithm, i used it to trace back the elements for finding the path.
    int startID = getID(fromnode,uniquenodes,uniquenodeselements); //getting the index of source.
    int finishID = getID(tonode,uniquenodes,uniquenodeselements); //getting the index of destintation.

    for(int i = 0; i<uniquenodeselements; i++) //for all elements of the unique array,
    {
        uniquenodes[i].weight = MAXI; //we set their weights to be infinity
        formerpath[i]= -1; //and setting the predecessor's elements to be -1(i will explain why i did -1 in later lines)
    }
    uniquenodes[startID].weight = 0; //setting the source nodes weight to 0.

    for(int i = 1; i <= numberofnodes-1; i++){ //according the Bellman Ford algorithm, we need the update/relax each node "edge" times,first by going through 1 node, then going through 2 nodes etc.
        for(int j = 0; j < numberofedges; j++)
        {
            int a,b,wei;
            a = edges[j].fromid; //we update the elements every time.
            b = edges[j].toid;
            wei = edges[j].weight;

            if(uniquenodes[a].weight != MAXI && uniquenodes[b].weight > uniquenodes[a].weight + wei) //if there is a shorter path avaliable
            {
                uniquenodes[b].weight = uniquenodes[a].weight + wei; //we choose the shorter path
                formerpath[b] = a; //and also getting the index of the former node to trace it back(we are marking the way we got through)
            }
        }
    }
    //when the algorithm is finished we have a list of distances from the souce to all other destinations, for a desired destination; all we have to do is use the index of the desired location



    //Output results to output file.
    //////////////////YOUR CODE HERE//////////////////////
    Distance shortestPath[SIZE]; //for the trace-back action, I created a stack to hold the path we went through.
    int top = -1; //setting it top to -1 for indicating that it is empty
    FILE *fs = fopen(outfile,"w"); //opening the output file in writing mode
    int current = finishID; //setting the current index to be destinations ID,
    while(current != -1 && uniquenodes[current].weight != MAXI) //since the no index could be -1 every element can go to this loop and can be processed, thats the reason why i didnt made formerpath arrays elements to be since the index=0 can not go through the loop.
    {
        strcpy(shortestPath[++top].node,uniquenodes[current].node); //copying the element to our stack and incrementing its top
        current = formerpath[current]; //backing to the former city we went through.
        //but in the end we traversed from destination to source. we need to print the path backwards.
    }

    fprintf(fs,"Path: ");
    for(int i = top; i >= 0 ; i--) //starting from top(it is the source node) and working our way back.
    {
        if(i == 0)
        {
            fprintf(fs,"%s",shortestPath[i].node); //for the last element we print it without the arrow.
            break;
        }
        fprintf(fs,"%s -> ",shortestPath[i].node); //printing each location we went through

    }
    fprintf(fs,"\n");

    fprintf(fs,"Distance: %d km",uniquenodes[finishID].weight); //this is the shortest distance from source to target.
    fclose(fs); //closing the file.
    return 0;

}