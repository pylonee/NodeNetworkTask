#pragma once
#include "node.h"
#include <vector>

using namespace std;

// Класс сети. Управление взаимодействиями узлов
class Network {
private:
	vector<Node*> nodes;		// все узлы сети

	double probCreateEvent;		// вероятность создания события
	double probSubscribe;		// вероятность подписки на соседа соседа
	double probUnsubscribe;		// вероятность отписки соседа
	double probCreateNode;		// вероятность создания нового узла
	double probNothing;			// вероятность ни чего не делать

	void initNetwork(int size); // создание сети
	void removeEmptyNodes();	// удаление пустых узлов (нет подписчиков = пустой)

public:
	// конструктор сети
	// получает начальное количество узлов и вероятности событий
	Network(int initialSize, double pCreate, double pSub, double pUnsub, double pNewNode, double pNothing);

	// деструктор сети ( для освобождения памяти)
	~Network();

	void update();	// метод обновления сети
	bool needContinue() const;	// метод для проверки нужно ли продолжать работу
	size_t getNodeCount() const { return nodes.size(); }	// текущее количество узлов
};