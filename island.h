class Island {

  int dim, size;
  std::vector<std::vector<double>> population;
  std::vector<double> scores;
  double (*initFunc)();
  double (*fitFunc)(int, double*);
  int idx1;
  int idx2;
  std::vector<double> std;
  std::vector<double> mean;

  void eval();
  void select();
  void crossover();
  void mutate(double mutationProb);

public:

  Island(int dim, int size, double (*initFunc)(), double (*fitFunc)(int, double*));
  void init();
  void next(double mutationProb);
  double getBestScore();
  std::vector<double> getRandomRepresentative();
  void addToPopulation(std::vector<double> rep);
  void updateMetrics();
  std::vector<double> getMean();
  std::vector<double> getStd();
};
