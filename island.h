class Island {

  int dim, size;
  vector<vector<double>> population;
  vector<double> scores;
  double (*initFunc)();
  double (*fitFunc)(int, double*);
  int idx1;
  int idx2;
  vector<double> std;
  vector<double> mean;

  void eval();
  void select();
  void crossover();
  void mutate(double mutationProb);

public:

  Island(int dim, int size, double (*initFunc)(), double (*fitFunc)(int, double*));
  void init();
  void next(double mutationProb);
  double getBestScore();
  vector<double> getRandomRepresentative();
  void addToPopulation(vector<double> rep);
  void updateMetrics();
  vector<double> getMean();
  vector<double> getStd();
};