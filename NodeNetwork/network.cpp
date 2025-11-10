#include "network.h"
#include<iostream>
#include<random>
#include"Utils_rand.h"


// конструктор сети
Network::Network(int initialSize, double pCreate, double pSub, double pUnsub, double pNewNode, double pNothing) :
	probCreateEvent(pCreate),
	probSubscribe(pSub),
	probUnsubscribe(pUnsub),
	probCreateNode(pNewNode),
	probNothing(pNothing) {

	// создание сети с начальными параметрами
	initNetwork(initialSize);
}

// деструктор сети
Network::~Network() {
	// удаляем все узлы для очистки памяти
	for (Node* node : nodes) {
		delete node;
	}
}

// метод создания сети и назначение начальных случайных подписок
void Network::initNetwork(int size) {
	// колчество узлов = size
	for (int i = 0; i < size; i++) {
		string nodeId = "Node_" + to_string(i);		// создаём уникальные ID и имена узлов
		nodes.push_back(new Node(nodeId));			// добавляем узел в список всех узлов сети
	}
	uniform_int_distribution<> dis(0, size - 1);	// случайный выбор индекса узла (от 0 до переданного количства узлов)
	uniform_int_distribution<> handlerDis(0, 1);	// случайный выбор обработчика

	// для всех узлов
	for (Node* node : nodes) {
		// количесво новых подписок текущего узла, получится 1 или 2 подписки
		int subscriptionCount = dis(gen) % 2 + 1;
		
		// подписываем узел на узлы (количество подписок = subscriptionCount)
		for (int j = 0; j < subscriptionCount; ++j) {
			Node* target = nodes[dis(gen)]; // случайный узел

			// нельзя подписаться на самого себя
			// нельзя подписаться, если уже подписан на этот узел
			if (target != node && !node->isSubscribedTo(target)) {
				// подписываемся на узел, случайно выбираем обработчик
				node->addSubscription(target, (handlerDis(gen) == 0) ? "N" : "S");
				// добавляем текущий узел в подписчики узла
				target->addSubscriber(node);
			}
		}
	}
}

// метод обновления сети - один цикл симуляции
void Network::update() {
	vector<Node*> newNodes;		// новый узлы созданные в этом цикле
	vector<Node*> delNodes;		// узлы для удаления

	// ШАГ 1
	// сначала каждый узел выполняет одно действие
	for (Node* node : nodes) {
		node->update(probCreateNode, probSubscribe, probUnsubscribe, probCreateNode, probNothing);

		// узел мог создать новый узел, добавляем его в список новых узлов текущего узла
		auto& addNodes = node->getAddNewNodes();
		// добавляем новые узлы в список новых узлов текущего цикла
		newNodes.insert(newNodes.end(), addNodes.begin(), addNodes.end());
		// у текущего узла очищаем список созданных им узлов
		addNodes.clear();
	}

	// ШАГ 2
	// все новые созданные узлы добавляются в сеть (делаем в конце, потому что новые узлы не должны выполнять действия в текущем цикле)
	nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());

	// ШАГ 3
	// удаляем пустые узлы (без подписчиков)
	removeEmptyNodes();
}

// метод удаления пустых узлов
void Network::removeEmptyNodes() {
	vector<Node*> nodesToRemove;

	// добавляем в список на удаление, все узлы у которых нет подписчиков
	for (Node* node : nodes) {
		if (node->getSubscriberts().empty()) {
			nodesToRemove.push_back(node);
		}
	}

	// удаляем все узлы из списка на удаление
	for (Node* node : nodesToRemove) {
		// отписываемся от всех подписок текущего узла
		node->unsubFromAll();

		// ищем текущий узел в списке узлов сети и удаляем из списка
		auto it = find(nodes.begin(), nodes.end(), node);

		if (it != nodes.end()) {
			nodes.erase(it);
		}

		// удаляем узел
		delete node;
	}
}

// проверка надо ли продолжть работу
bool Network::needContinue() const {
	// если список узлов пустой, то надо закончить выполнение
	return !nodes.empty();
}