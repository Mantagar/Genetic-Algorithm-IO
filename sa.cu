#include <stdio.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <stdlib.h>
#include <random>
#include <math.h>
#include <unistd.h>
#define PI 3.1415926535
#define RANGE 600

__host__ __device__ double ackley(int DIMENSION, double *point, int offset);//RANGE = 1
__host__ __device__ double griewangk(int DIMENSION, double *point, int offset);//RANGE = 600
__host__ __device__ double schwefel(int DIMENSION, double *point, int offset);//RANGE = 500
__host__ __device__ double dejong(int DIMENSION, double *point, int offset);//RANGE = 5.12
__host__ __device__ double rastrigin(int DIMENSION, double *point, int offset);//RANGE = 5.12

//Replace with a predefined function and set RANGE accordingly 
__host__ __device__ double fitness(int DIMENSION, double *point, int offset){
  return griewangk(DIMENSION, point, offset);
}

__host__ __device__ double ackley(int DIMENSION, double *point, int offset){
  double sum_result1 = 0;
  double sum_result2 = 0;
  for(int i=0; i<DIMENSION; i++){
    sum_result1 += point[offset+i]*point[offset+i];
    sum_result2 += cos(2*PI*point[offset+i]);
  }
  return -20*exp(-0.2*sqrt(sum_result1/DIMENSION))-exp(sum_result2/DIMENSION)+20+exp(1.);
}

__host__ __device__ double griewangk(int DIMENSION, double *point, int offset){
  double sum_result = 0;
  double product_result = 1;
  for(int i=0; i<DIMENSION; i++){
    sum_result += point[offset+i]/4000*point[offset+i];
    product_result *= cos(point[offset+i]/sqrt((double)i+1));
  }
  return sum_result-product_result+1;
}

__host__ __device__ double schwefel(int DIMENSION, double *point, int offset){
  double result = 0;
  for(int i=0; i<DIMENSION; i++){
    result -= point[offset+i]*sin(sqrt(abs(point[offset+i])));
  }
  return result+DIMENSION*418.9829;//shifted to obtain 0 minimum
}

__host__ __device__ double dejong(int DIMENSION, double *point, int offset){
  double result = 0;
  for(int i=0; i<DIMENSION; i++){
    result += point[offset+i]*point[offset+i];
  }
  return result;
}

__host__ __device__ double rastrigin(int DIMENSION, double *point, int offset){
  double result = DIMENSION*10;
  for(int i=0; i<DIMENSION; i++){
    result += point[offset+i]*point[offset+i]-10*cos(2*PI*point[offset+i]);
  }
  return result;
}

__global__ void curand_init_kernel(curandState *state, int seed){
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  curand_init(seed, tid, 0, &state[tid]);
}

__global__ void sa_kernel(int INSTANCES, int DIMENSION, int GPU_ITERATIONS, curandState *state, double *decay, double *point, double *neighbor, double *temperature) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if(tid < INSTANCES) {
    for(int i=0; i<GPU_ITERATIONS; i++){
      int axis = trunc(curand_uniform_double(&state[tid])*DIMENSION);
      neighbor[tid*DIMENSION+axis] = (curand_uniform_double(&state[tid])*2-1)*RANGE;
      double point_fitness=fitness(DIMENSION, point, DIMENSION*tid);
      double neighbor_fitness=fitness(DIMENSION, neighbor, DIMENSION*tid);
      double change_prob = 2; //ensure change
       if(neighbor_fitness>point_fitness) change_prob = exp((point_fitness-neighbor_fitness)/temperature[tid]);
      if(change_prob > curand_uniform_double(&state[tid]))
        point[tid*DIMENSION+axis] = neighbor[tid*DIMENSION+axis];
      else
        neighbor[tid*DIMENSION+axis] = point[tid*DIMENSION+axis];
      temperature[tid]*=decay[tid];
    }
  }
}

int main(int argc, char **argv) {
  if(argc<5) {
    printf("Required arguments:\nINSTANCES - less than 1024\nDIMENSION - size of a problem\nITERATIONS - number of times the data will be synchronized with GPUs\nGPU_ITERATIONS - number of iterations per core in a single synchronization step\n");
    return 0;
  }
  const int INSTANCES = atoi(argv[1]);
  const int DIMENSION = atoi(argv[2]);
  const int ITERATIONS = atoi(argv[3]);
  const int GPU_ITERATIONS = atoi(argv[4]);
  time_t time_measure = time(0);
  
  const int RANDOM_SEED = time(0)*getpid();

  curandState* dev_state;
  cudaMalloc((void**) &dev_state, INSTANCES * sizeof(curandState));
  curand_init_kernel<<<1,INSTANCES>>>(dev_state, RANDOM_SEED);
  
  std::default_random_engine generator(static_cast<long unsigned int>(RANDOM_SEED));
  std::uniform_real_distribution<double> decay_distribution(0.9,1.0);
  std::uniform_real_distribution<double> point_distribution(-RANGE,RANGE);
  
  double *decay = new double[INSTANCES];
  for(int i=0; i<INSTANCES; i++){
    decay_distribution.reset();
    decay[i] = decay_distribution(generator);
    //printf("DECAY FACTOR %d: %f\n",i,decay[i]);
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

  double *temperature = new double[INSTANCES];
  for(int i=0; i<INSTANCES; i++){
    temperature[i] = 100.0;
  }
  double *dev_temperature;
  cudaMalloc((void**)&dev_temperature, INSTANCES * sizeof(double));
  cudaMemcpy(dev_temperature, temperature, INSTANCES * sizeof(double), cudaMemcpyHostToDevice);
  
  //######################## KERNEL ########################
  for(int i=0; i<ITERATIONS; i++){
    sa_kernel<<<1,INSTANCES>>>(INSTANCES, DIMENSION, GPU_ITERATIONS, dev_state, dev_decay, dev_point, dev_neighbor, dev_temperature);
    cudaDeviceSynchronize();
    cudaMemcpy(point, dev_point, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyDeviceToHost);
    //Pick the minimal point
    int minimum_index = 0;
    double minimum_value = fitness(DIMENSION,point,0);
    for(int k=1; k<INSTANCES; k++){
      double minimum_value_contestant = fitness(DIMENSION,point,DIMENSION*k);
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
  }
  
  cudaMemcpy(point, dev_point, INSTANCES * DIMENSION * sizeof(double), cudaMemcpyDeviceToHost);
  printf("%d\t%.10f\n", time(0)-time_measure, fitness(DIMENSION,point,0));

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
