#include "network.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main() {
	cout << "--- Network Simulation Start ---" << endl;

	/*
	* Network with params:
	* - initial size: 5 nodes
	* - actions probability (summ approximately 1.0)
	* 
	* -- Create action: 40% (0.4)
	* -- Subscribe 30% (0.3)
	* -- Unsubscribe 10% (0.1)
	* -- Create new node 10% (0.1)
	* -- Do nothing 10% (0.1)
	* 0.4 + 0.3 + 0.1 + 0.1 + 0.1 = 1.0
	* 
	* Сеть с параметрами:
	* - начальный размер: 5 nodes
	* - вероятности действий (сумма должна быть примерно равна 1.0)
	* 
	* -- Создать событие: 40% (0.4)
	* -- Подписаться на узел 30% (0.3)
	* -- Отписаться от узла 10% (0.1)
	* -- Создать новый узел 10% (0.1)
	* -- Ни чего не делать 10% (0.1)
	* 0.4 + 0.3 + 0.1 + 0.1 + 0.1 = 1.0
	*/

	Network network(5, 0.4, 0.3, 0.1, 0.1, 0.1);

	int cycle = 0; // счётчикк циклов

	const int maxCycles = 100; // максимум циклов, чтобы симуляция не продолжалось бескочено (вероятность то есть)

	while (network.needContinue() && cycle < maxCycles) {
		cout << "\n\n --- Cycle" << cycle << " --- " << endl;
		cout << "Nodes in network: " << network.getNodeCount() << endl;

		network.update(); // обновление сети - один полный цикл

		this_thread::sleep_for(chrono::milliseconds(1000)); // задержка на 1 секунду

		cycle++;
	}

	// Проверка причины остановки симуляции
	if (network.needContinue()) {
		cout << "\nNetwork stopped after " << maxCycles << " cycles" << endl;
		cout << "Remaining nodes: " << network.getNodeCount() << endl;
	}
	else {
		cout << "\n Network die after " << cycle << " cycles" << endl;
	}

	cout << "--- Network simulation finish ---" << endl;

	return 0;
}