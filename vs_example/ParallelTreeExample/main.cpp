/*
 * Дана задача:
 * Максимизировать f(x) = 5*x0 + 10*x1 + 4*x2 + 3*x3
 * С ограничением 4*x0 + 7*x1 + 2*x2 + 3*x3 <= 10
 * где x1,x2,x3,x4 двоичные (т.е. принадлежат {0,1})
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <ParallelTree.hpp>

using namespace std;

int f(vector<int> x) { // функция минимизации
	return accumulate(x.begin(), x.end(), 0);
}

class ExampleRecord : public Record
{
public:
	vector<int> x;

	ExampleRecord(vector<int> x) : x(x) {}

	bool betterThan(const Record& other) const override {
		const ExampleRecord& otherCast = static_cast<const ExampleRecord&>(other);
		return f(x) < f(otherCast.x);
	}

	std::unique_ptr<Record> clone() const override {
		return std::make_unique<ExampleRecord>(*this);
	}
};

class ExampleNode : public Node {
public:
	vector<int> finalcombination; // получившееся сочетание на каждом листе
	vector<int> N; // из скольки 
	int k; // по сколько 
	int I; // ярус
	int node;

	ExampleNode(vector<int> N, int k) : N(N), k(k), I(0) {}

	std::vector< std::unique_ptr<Node> > process(Record& record) override
	{

		ExampleRecord& recordCast = static_cast<ExampleRecord&>(record);

		std::vector< std::unique_ptr<Node> > childNodes;

		if (I == k) {
			/*cout << endl;
			for (auto i : finalcombination) cout << i << " ";
			cout << endl;*/
			if (f(finalcombination) < f(recordCast.x))
				recordCast.x = finalcombination;
			return childNodes;
		}
		else {
			I += 1;
			vector<int> temp = N;
			for (int i(0); i < N.size() - k + I; ++i) {
				temp = N;
				node = N[i];
				finalcombination.push_back(node);
				//if (accumulate(cbb.begin(), cbb.end(), 0) > recordCast.sum) continue;
				//cout << "+1" << endl;
				temp.erase(temp.begin() + i);
				swap(temp, N);
				childNodes.emplace_back(new ExampleNode(*this));
				swap(temp, N);
				finalcombination.pop_back();
			}
			return childNodes;
		}
	}

	bool hasHigherPriority(const Node& other) const override {
		const ExampleNode& otherCast = static_cast<const ExampleNode&>(other);
		return f(finalcombination) < f(otherCast.finalcombination);
	}
};

int main()
{
	int q = INT_MAX;

	vector<vector<int>> matrix{				// задаем наш граф матрицей смежности
		{ 0, 12,  q, 28,  q,  q,  q},
		{ 12, 0, 10, 43,  q,  q,  q},
		{ q, 10,  0,  q, 10,  q,  q},
		{ 28,43, 17,  0,  q,  q,  q},
		{ q, 31, 10,  q,  0,  8,  q},
		{ q,  q,  q,  q, 14,  0,  6},
		{ q,  q,  q,  q,  q,  6,  0},
	};
	//for (int i(0); i < matrix.size(); ++i) {
	//	for (int j(0); j < matrix.size(); ++j) {
	//		cout << setw(2) << matrix[i][j] << "  ";
	//	}
	//	cout << endl;
	//}

	// с помощью алгоритма Флойда — Уоршелла находим длины кратчайших путей между всеми парами вершин
	for (int k = 0; k < matrix.size(); ++k)
		for (int i = 0; i < matrix.size(); ++i)
			for (int j = 0; j < matrix.size(); ++j)
				if (matrix[i][k] < INT_MAX && matrix[k][j] < INT_MAX)
					if (matrix[i][k] + matrix[k][j] < matrix[i][j])
						matrix[i][j] = matrix[i][k] + matrix[k][j];

	for (int i(0); i < matrix.size(); ++i) {	// выводим полную матрицу смежности
		for (int j(0); j < matrix.size(); ++j) {
			cout << setw(2) << matrix[i][j] << "  ";
		}
		cout << endl;
	}

	cout << endl;
	vector<int> CBB; // создаем вектор, каждое значение которого - сумма кратчайших расстояний от данной вершины до всех остальных
	for (int i = 0; i < matrix.size(); i++) {
		int sum = 0;
		for (int j = 0; j < matrix.size(); j++) {
			sum += matrix[i][j];
		}
		CBB.push_back(sum);
		cout << CBB[i];
		cout << "\n";
	}
	cout << endl << endl;
	//cout << accumulate(CBB.begin(), CBB.end(), 0);
	int k = 3;
	vector<int> resized_for_record = CBB;
	resized_for_record.resize(k);
	ExampleRecord initialRecord({ resized_for_record });
	unique_ptr<ExampleNode> root = make_unique<ExampleNode>(CBB, k);
	unique_ptr<Record> bestSolution = parallelTree(move(root), initialRecord);
	const ExampleRecord* bestSolutionCast = reinterpret_cast<const ExampleRecord*>(bestSolution.get());

	for (int i(0); i < bestSolutionCast->x.size(); ++i) {
		cout << bestSolutionCast->x[i] << "  ";
	}
	//cout << endl << f(bestSolutionCast->x, capacity, bestSolutionCast->x.size());
	cout << endl << "Runtime : " << clock() / 1000.0;

	return 0;
}



