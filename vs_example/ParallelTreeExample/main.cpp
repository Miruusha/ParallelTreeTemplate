#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <ctime>
#include <ParallelTree.hpp>

using namespace std;

int f(vector<vector<int>> x, vector<int> p) { // функция минимизации
	int s = 0;
	int shortest_way = INT_MAX;
	for (int i(0); i < x.size(); ++i) {
		shortest_way = INT_MAX;
		for (auto j : p) {
			if (shortest_way > x[i][j]) shortest_way = x[i][j];
		}
		s += shortest_way;
	}
	return s;
}

class ExampleRecord : public Record
{
public:
	vector<vector<int>> x;
	vector<int> p;

	ExampleRecord(vector<vector<int>> x,vector<int> p) : x(x), p(p) {}

	bool betterThan(const Record& other) const override {
		const ExampleRecord& otherCast = static_cast<const ExampleRecord&>(other);
		return f(x,p) < f(otherCast.x, otherCast.p);
	}

	std::unique_ptr<Record> clone() const override {
		return std::make_unique<ExampleRecord>(*this);
	}
};
	
class ExampleNode : public Node {
public:
	vector<vector<int>> x;
	vector<int> p; // получившееся сочетание на каждом листе
	vector<int> N; // из скольки 
	int k; // по сколько 
	int I; // ярус
	int node;

	ExampleNode(vector<vector<int>> x,vector<int> N, int k) : x(x), N(N), k(k), I(0) {}

	std::vector< std::unique_ptr<Node> > process(Record& record) override
	{

		ExampleRecord& recordCast = static_cast<ExampleRecord&>(record);

		std::vector< std::unique_ptr<Node> > childNodes;

		if (I == k) {
			if (f(x, p) < f(recordCast.x, recordCast.p))
				recordCast.p = p;
			return childNodes;
		}
		else {
			I += 1;
			vector<int> temp = N;
			for (int i(0); i < N.size() - k + I; ++i) {
				temp = N;
				node = N[i];
				p.push_back(node);
				temp.erase(temp.begin() + i);
				swap(temp, N);
				childNodes.emplace_back(new ExampleNode(*this));
				swap(temp, N);
				p.pop_back();
			}
			return childNodes;
		}
	}

	bool hasHigherPriority(const Node& other) const override {
		const ExampleNode& otherCast = static_cast<const ExampleNode&>(other);
		return f(x,p) < f(otherCast.x, otherCast.p);
	}
};

int main()
{
	int q = INT_MAX;

	//vector<vector<int>> matrix{				// задаем наш граф матрицей смежности
	//	{ 0, 12,  q, 28,  q,  q,  q},
	//	{ 12, 0, 10, 43,  q,  q,  q},
	//	{ q, 10,  0,  q, 10,  q,  q},
	//	{ 28,43, 17,  0,  q,  q,  q},
	//	{ q, 31, 10,  q,  0,  8,  q},
	//	{ q,  q,  q,  q, 14,  0,  6},
	//	{ q,  q,  q,  q,  q,  6,  0},
	//};
	vector<vector<int>> matrix{				// задаем наш граф матрицей смежности
		//1   2   3   4   5   6   7   8   9   10
		{ 0,  q,  1,  q,  q,  q,  q,  q,  q,  q}, //1
		{ q,  0,  4,  q,  q,  q,  7,  q,  3,  q}, //2
		{ 1,  4,  0,  9,  q,  q,  q,  q,  q,  q}, //3
		{ q,  q,  9,  0,  9,  q,  7,  1,  q,  q}, //4
		{ q,  q,  q,  9,  0,  2,  q,  3,  q,  q}, //5
		{ q,  q,  q,  q,  2,  0,  q,  q,  q,  q}, //6
		{ q,  7,  q,  7,  q,  q,  0,  q,  q,  q}, //7
		{ q,  q,  q,  1,  3,  q,  q,  0,  q,  5}, //8
		{ q,  3,  q,  q,  q,  q,  q,  q,  0,  8}, //9
		{ q,  q,  q,  q,  q,  q,  q,  5,  8,  0}, //10
	};

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

	cout << endl << endl;
	int k = 3;
	vector<int> N;
	for (int i(0); i < matrix.size(); ++i) N.push_back(i);
	vector<int> resized_for_record = N;
	resized_for_record.resize(k);
	auto t1 = std::chrono::high_resolution_clock::now();
	ExampleRecord initialRecord(matrix, resized_for_record);
	unique_ptr<ExampleNode> root = make_unique<ExampleNode>(matrix, N, k);
	unique_ptr<Record> bestSolution = parallelTree(move(root), initialRecord, 4);
	const ExampleRecord* bestSolutionCast = reinterpret_cast<const ExampleRecord*>(bestSolution.get());
	auto t2 = std::chrono::high_resolution_clock::now();
	auto tt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << endl << "Runtime : " << tt<< endl;
	for (int i(0); i < bestSolutionCast->p.size(); ++i) {
		cout << bestSolutionCast->p[i] + 1 << "  ";
	}
	

	return 0;
}



