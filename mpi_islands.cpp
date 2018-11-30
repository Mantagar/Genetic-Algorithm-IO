#include<iostream>
#include<mpi.h>
#include<vector>
#include<unistd.h>
int rank;
int size;
MPI_Comm comm = MPI_COMM_WORLD;
using namespace std;

int main(){
MPI_Init(NULL, NULL);
MPI_Comm_rank(comm, &rank);
MPI_Comm_size(comm, &size);
//maybe split the communicators?

vector<int> links;
int node = rank+1;
if(node==size) node = 0;
links.push_back(node);
node = rank-1;
if(node==-1) node = size-1;
links.push_back(node);


int* log = new int[size*2];
while(true) {
int received[2];
received[0]=-1;
received[1]=-1;

MPI_Request send_req[2];
MPI_Request recv_req[2];
for(int i=0; i<links.size(); i++)
  MPI_Isend(&rank, 1, MPI_INT, links[i], 0, comm, &send_req[i]);

for(int i=0; i<links.size(); i++)
  MPI_Irecv(&received[i], 1, MPI_INT, links[i], 0, comm, &recv_req[i]);

for(int i=0; i<links.size(); i++)
  MPI_Wait(&recv_req[i], MPI_STATUS_IGNORE);

MPI_Gather(&received, 2, MPI_INT, log, 2, MPI_INT, 0, comm);
if(rank==0) {
  cout << "NeXt ItErAtIoN" << endl;
  for(int i=0; i<size; i++) {
    cout << i << " <-- " << log[2*i] << endl;
    cout << i << " <-- " << log[2*i+1] << endl;
  }
}
sleep(5);
}
MPI_Finalize();
}
