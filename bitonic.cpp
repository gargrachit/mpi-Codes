#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;

/*ll calculateHash(string str){

  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  ll hash = 5381;
  int l = str.size();

  for(int i = 0 ; i<l ; i++)
  hash = ((hash << 5) + hash) + (str[i]-'a');

  reverseHash[hash] = str;
  return hash;
  }*/

int totalSize,subSize,worldRank;
double start,end;
vector<int> array;

int findBit(int index){
 //   cout << worldRank << endl;
    if(  ((worldRank >> index)&1) == 0)
        return 0;
    else
        return 1;
}

void getMax(int index){
    vector<int> tempReceive;

    tempReceive.resize(subSize);
    
    MPI_Send(&array[0], subSize,MPI_INT, (worldRank ^ (1 << index)) ,0,MPI_COMM_WORLD);
    MPI_Recv(&tempReceive[0],subSize,MPI_INT, (worldRank ^ (1 << index)),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

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

void getMin(int index){
    vector<int> tempReceive;

    tempReceive.resize(subSize);

    MPI_Recv(&tempReceive[0],subSize,MPI_INT, (worldRank ^ (1<<index)),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Send(&array[0], subSize,MPI_INT, (worldRank ^ (1<<index)) ,0,MPI_COMM_WORLD);

   /* for(int i = 0;i<subSize;i++)
        cout << tempReceive[i] << " " ;
    cout << endl;*/
    
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

    //int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    totalSize = atoi(argv[1]);
    subSize  = totalSize / worldSize;
    if(worldRank == 0){
        start = MPI_Wtime();
    }
   // cout << totalSize << " " << subSize << endl;
    array.clear();
    array.resize(subSize);


    for(int i = 0;i<subSize;i++){
        array[i] = rand()%100;
    }

/*    cout << worldRank << endl;*/
  //  if(worldRank == 0){
    /*    for(int i = 0;i<subSize;i++){
            cout << array[i] << " ";
        }
        cout << endl;*/
    //}

    sort(array.begin(),array.end());

    MPI_Barrier(MPI_COMM_WORLD);

    int bits = log2(worldSize);

   // cout << bits << endl;
    for(int i = 0; i < bits; i++){
        for(int j = i; j>=0; j--){
            if(findBit(i+1) != findBit(j)){
                getMax(j);
            }
            else{
                getMin(j);
            }
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
        end = MPI_Wtime();
        cout << "TIME TAKEN : " << end - start << " Seconds" << endl; 
        for(int i = 0;i<totalSize;i++){
            cout << finalArray[i] << " ";
        }
        cout << endl;
    }
    else{
        MPI_Send(&array[0], subSize,MPI_INT, 0 ,0,MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
