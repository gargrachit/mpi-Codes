#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;


int totalSize,subSize,worldRank;
double start,end;
vector<int> array;

int findPartner(int turn){
    if( turn%2 == 0){
        if(worldRank%2 == 0)
            return worldRank + 1;
        else
            return worldRank - 1;
    }
    else{
        if(worldRank%2 == 0)
            return worldRank - 1;
        else
            return worldRank + 1;
    }
}

void getMax(int pair){
    vector<int> tempReceive;

    tempReceive.resize(subSize);

    MPI_Send(&array[0], subSize,MPI_INT, pair,0,MPI_COMM_WORLD);
    MPI_Recv(&tempReceive[0],subSize,MPI_INT, pair,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    int i = 0;
    int j = 0;

    vector<int> merge;


    while(i < subSize && j < subSize){
        if(array[i] < tempReceive[j]){
            merge.push_back(array[i]);
            i++;
        }
        else{
            merge.push_back(tempReceive[j]);
            j++;
        }
    }

    while(i < subSize){
        merge.push_back(array[i]);
        i++;
    }

    while(j < subSize){
        merge.push_back(tempReceive[j]);
        j++;
    }

    for(int i = 0;i<subSize;i++){
        array[i] = merge[i + subSize];
    }
    merge.clear();
    tempReceive.clear();
    return;
}

void getMin(int pair){
    vector<int> tempReceive;

    tempReceive.resize(subSize);

    MPI_Recv(&tempReceive[0],subSize,MPI_INT, pair,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Send(&array[0], subSize,MPI_INT, pair ,0,MPI_COMM_WORLD);


    int i = 0;
    int j = 0;
    vector<int> merge;
    while(i < subSize && j < subSize){
        if(array[i] < tempReceive[j]){
            merge.push_back(array[i]);
            i++;
        }
        else{
            merge.push_back(tempReceive[j]);
            j++;
        }
    }
    while(i < subSize){
        merge.push_back(array[i]);
        i++;
    }
    while(j < subSize){
        merge.push_back(tempReceive[j]);
        j++;
    }
    for(int i = 0;i<subSize;i++){
        array[i] = merge[i];
    }
    merge.clear();
    tempReceive.clear();
    return;
}

int main(int argc, char * argv[]) {

    srand(time(NULL));

    MPI_Status status;

    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    totalSize = atoi(argv[1]);
    subSize  = totalSize / worldSize;

    if(worldRank == 0){
        start = MPI_Wtime();
    }

    array.clear();
    array.resize(subSize);


    for(int i = 0;i<subSize;i++){
        array[i] = rand()%100;
    }


    sort(array.begin(),array.end());

    MPI_Barrier(MPI_COMM_WORLD);


    for(int i = 0; i < worldSize; i++){
        int pair = findPartner(i);
        if(pair < 0 || pair >= worldSize)
            continue;
        if(i%2 == 0){
            if(worldRank%2 == 0)
                getMin(pair);
            else
                getMax(pair);
        }
        else{
            if(worldRank%2 == 0)
                getMax(pair);
            else
                getMin(pair);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(worldRank == 0){
        vector<int> finalArray;
        finalArray.resize(totalSize);
        for(int i = 0;i<subSize;i++){
            finalArray[i] = array[i];
        }
        for(int i= 1;i<worldSize;i++){
            MPI_Recv(&finalArray[i*subSize],subSize,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        for(int i = 0;i<totalSize;i++){
            cout << finalArray[i] << " ";
        }
        //cout << endl;
        end = MPI_Wtime();
        cout << "TIME TAKEN : " << end - start << " Seconds" << endl; 
    }
    else{
        MPI_Send(&array[0], subSize,MPI_INT, 0 ,0,MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
