#include <stdio.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <stdlib.h>
#include <helper_timer.h>
#include <random>
#include <math.h>
#define PI 3.1415926535
__host__ __device__ double rastrigin(int DIMENSION, double *point, int offset){
  double result = DIMENSION*10;
  for(int i=0; i<DIMENSION; i++){
    result+=point[offset+i]*point[offset+i]-10*cos(2*PI*point[offset+i]);
  }
  return result;
}

__global__ void curand_init_kernel(curandState *state){
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  curand_init(0, tid, 0, &state[tid]);
}

__global__ void sa_kernel(int INSTANCES, int DIMENSION, curandState *state, double *decay, double *point, double *neighbor, double *temperature) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if(tid < INSTANCES) {
    int axis = trunc(curand_uniform_double(&state[tid])*DIMENSION);
    neighbor[tid*DIMENSION+axis] = (curand_uniform_double(&state[tid])-0.5)*10.24;
    double point_fitness=rastrigin(DIMENSION, point, DIMENSION*tid);
    double neighbor_fitness=rastrigin(DIMENSION, neighbor, DIMENSION*tid);
    double change_prob = 2; //ensure change
    if(neighbor_fitness>point_fitness) change_prob = exp((point_fitness-neighbor_fitness)/temperature[tid]);
    if(change_prob > curand_uniform_double(&state[tid]))
      point[tid*DIMENSION+axis] = neighbor[tid*DIMENSION+axis];
    else
      neighbor[tid*DIMENSION+axis] = point[tid*DIMENSION+axis];
    temperature[tid]*=decay[tid];
  }
}

int main(int argc, char **argv) {
  if(argc<4) {
    printf("Required arguments:\nINSTANCES - number of CUDA threads\nDIMENSION - size of a problem\nITERATIONS - number of iterations");
    return 0;
  }
  const int INSTANCES = atoi(argv[1]);
  const int DIMENSION = atoi(argv[2]);
  const int ITERATIONS = atoi(argv[3]);


  curandState* dev_state;
  cudaMalloc((void**) &dev_state, INSTANCES * sizeof(curandState));
  curand_init_kernel<<<1,INSTANCES>>>(dev_state);

  std::default_random_engine generator(static_cast<long unsigned int>(time(0)));
  std::uniform_real_distribution<double> decay_distribution(0.9,1.0);
  std::uniform_real_distribution<double> point_distribution(-5.12,5.12);

  double *decay = new double[INSTANCES];
  for(int i=0; i<INSTANCES; i++){
    decay_distribution.reset();
    decay[i] = decay_distribution(generator);
    printf("DECAY FACTOR %d: %f\n",i,decay[i]);
  }
  double *dev_decay;
  cudaMalloc((void**)&dev_decay, INSTANCES * sizeof(double));
  cudaMemcpy(dev_decay, decay, INSTANCES * sizeof(double), cudaMemcpyHostToDevice);

  double *point = new double[INSTANCES*DIMENSION];
  double *neighbor = new double[INSTANCES*DIMENSION];
  for(int i=0; i<INSTANCES; i++){
    for(int d=0; d<DIMENSION; d++){
      point_distribution.reset();
      point[i*DIMENSION+d]=point_distribution(generator);
      neighbor[i*DIMENSION+d]=point[i*DIMENSION+d];
    }
  }
  double *dev_point, *dev_neighbor;
  cudaMalloc((void**)&dev_point, INSTANCES * DIMENSION * sizeof(double));
  cudaMemcpy(dev_point, point, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyHostToDevice);
  cudaMalloc((void**)&dev_neighbor, INSTANCES * DIMENSION * sizeof(double));
  cudaMemcpy(dev_neighbor, neighbor, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyHostToDevice);
  for(int i=0; i<INSTANCES; i++){
    //for(int d=0; d<DIMENSION; d++) printf("%f\n",point[i*DIMENSION+d]);
    printf("RASTRIGIN %d: %f\n",i,rastrigin(DIMENSION,point,DIMENSION*i));
  }

  double *temperature = new double[INSTANCES];
  for(int i=0; i<INSTANCES; i++){
    temperature[i] = 100.0;
  }
  double *dev_temperature;
  cudaMalloc((void**)&dev_temperature, INSTANCES * sizeof(double));
  cudaMemcpy(dev_temperature, temperature, INSTANCES * sizeof(double), cudaMemcpyHostToDevice);

  StopWatchInterface *timer=NULL;
  sdkCreateTimer(&timer);
  sdkResetTimer(&timer);
  sdkStartTimer(&timer);

  //######################## KERNEL ########################
  for(int i=0; i<ITERATIONS; i++){
    sa_kernel<<<1,INSTANCES>>>(INSTANCES, DIMENSION, dev_state, dev_decay, dev_point, dev_neighbor, dev_temperature);
    cudaDeviceSynchronize();
    cudaMemcpy(point, dev_point, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyDeviceToHost);
    //Pick the minimal point
    int minimum_index = 0;
    int minimum_value = rastrigin(DIMENSION,point,0);
    for(int k=1; k<INSTANCES; k++){
      int minimum_value_contestant = rastrigin(DIMENSION,point,DIMENSION*k);
      if(minimum_value_contestant<minimum_value){
        minimum_value = minimum_value_contestant;
        minimum_index = k;
      }
    }
    //Copy minimal point
    for(int k=0; k<INSTANCES; k++){
      for(int t=0; t<DIMENSION; t++){
        point[DIMENSION*k+t] = point[DIMENSION*minimum_index+t];
      }
    }
    cudaMemcpy(dev_point, point, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyHostToDevice);
    //printf("Iteration no %d, leading instance: %d\n",i,minimum_index);
  }

  sdkStopTimer(&timer);
  float time = sdkGetTimerValue(&timer);
  sdkDeleteTimer(&timer);

  printf("%f ms\n",time);

  cudaMemcpy(point, dev_point, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyDeviceToHost);
  for(int i=0; i<INSTANCES; i++){
    //for(int d=0; d<DIMENSION; d++) printf("%f\n",point[i*DIMENSION+d]);
    printf("RASTRIGIN %d: %f\n",i,rastrigin(DIMENSION,point,DIMENSION*i));
  }

  delete [] decay;
  delete [] neighbor;
  delete [] point;
  delete [] temperature;
  cudaFree(dev_decay);
  cudaFree(dev_neighbor);
  cudaFree(dev_point);
  cudaFree(dev_temperature);
  return 0;
}
