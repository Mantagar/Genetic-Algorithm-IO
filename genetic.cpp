#include <iostream>

class Genetic
{
public:
	Genetic(int populationSize) {

	}
	void run() {
		Init();
		Eval();
		do {
			Select();
			Crossover();
			Mutate();
			Eval();
		} while(ShouldContinue());

	}
private:
	void Init() {

	}
	void Eval() {

	}
	bool ShouldContinue() {
		return false;
	}
	void Select() {

	}
	void Crossover() {

	}
	void Mutate() {

	}
};

int main() {
	Genetic instance(0);
	instance.run();

	return 0;
}
