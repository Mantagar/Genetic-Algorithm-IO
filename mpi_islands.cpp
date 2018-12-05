#include<iostream>
#include<mpi.h>
#include<vector>
#include<unistd.h>
#include<stdlib.h>
int mpi_rank;
int mpi_size;
MPI_Comm comm = MPI_COMM_WORLD;
using namespace std;

int main(int argc, char* argv[]){
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(comm, &mpi_rank);
  MPI_Comm_size(comm, &mpi_size);

  vector<int> links;

  string topology(argv[1]);

  if(topology=="ring") {
    int node = mpi_rank+1;
    if(node==mpi_size) node = 0;
    links.push_back(node);
    node = mpi_rank-1;
    if(node==-1) node = mpi_size-1;
    links.push_back(node);
  }
  else if(topology=="torus") {
    int width = atoi(argv[2]);
    if(mpi_size%width){
      if(mpi_rank==0) cerr << "Incompatible mpi_size" << endl;
      MPI_Finalize();
      return 0;
    }
    int height = mpi_size/width;
    int x = mpi_rank%width;
    int y = mpi_rank/width;
    int nodeX = x+1;
    if(nodeX==width) nodeX = 0;
    links.push_back(y*width+nodeX);
    if(mpi_rank==0) cout << links[0] << endl;
    nodeX = x-1;
    if(nodeX==-1) nodeX = width-1;
    links.push_back(y*width+nodeX);
    if(mpi_rank==0) cout << links[1] << endl;
    int nodeY = y+1;
    if(nodeY==height) nodeY = 0;
    links.push_back(nodeY*width+x);
    if(mpi_rank==0) cout << links[2] << endl;
    nodeY = y-1;
    if(nodeY==-1) nodeY = height-1;
    links.push_back(nodeY*width+x);
    if(mpi_rank==0) cout << links[3] << endl;
  }
  else {
    if(mpi_rank==0) cerr << "Unknown topology setup" << endl;
    MPI_Finalize();
    return 0;
  }


  int* received = new int[links.size()];
  int* log = new int[mpi_size*links.size()];

  MPI_Request send_req[links.size()];
  MPI_Request recv_req[links.size()];

  while(true) {
    for(int i=0; i<links.size(); i++)
      MPI_Isend(&mpi_rank, 1, MPI_INT, links[i], 0, comm, &send_req[i]);

    for(int i=0; i<links.size(); i++)
      MPI_Irecv(&received[i], 1, MPI_INT, links[i], 0, comm, &recv_req[i]);

    for(int i=0; i<links.size(); i++)
      MPI_Wait(&recv_req[i], MPI_STATUS_IGNORE);

    MPI_Gather(received, links.size(), MPI_INT, log, links.size(), MPI_INT, 0, comm);
    if(mpi_rank==0) {
      cout << "NeXt ItErAtIoN" << endl;
      for(int i=0; i<mpi_size; i++)
        for(int j=0; j<links.size(); j++)
          cout << i << " <-- " << log[links.size()*i+j] << endl;
    }
    sleep(5);
  }
  MPI_Finalize();
}
