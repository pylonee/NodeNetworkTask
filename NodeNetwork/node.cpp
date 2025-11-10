#include "node.h"
#include "Utils_rand.h"
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

void Node::update(double pCreate, double pSub, double pUnsub, double pNewNode, double pNothing) {
	// выбираем случайное действие в зависимости от вероятности
	Action action = chooseRandAction(pCreate, pSub, pUnsub, pNewNode, pNothing);

	// вызываем соответствующий метод
	switch (action) {
	case Action::CREATE_EVENT:
		createRandEvent();
		break;
	case Action::SUBSCRIBE:
		subToNeighborsNeighbor();
		break;
	case Action::UNSUBSCRIBE:
		unsubFromNeighbor();
		break;
	case Action::CREATE_NODE:
		createNewNode();
		break;
	case Action::NOTHING:
		// ни чего не делать
		break;
	}
}

// обработка входящего события от другого узла
void Node::chooseEvent(Node* sender, int data) {
	eventCount[sender]++; // счётчик событий
	eventSum[sender] += data; // счётчик суммы

	// получаем тип обработчика. Если такого отправителя нет, то вернется пустая сторка
	string handler = handlers[sender];

	// вывод в зависимости от обработчика
	if (handler == "N") {
		cout << sender->getId() << " -> " << id << ": N = " << eventCount[sender] << endl;
	}
	else if (handler == "S") {
		cout << sender->getId() << " -> " << id << ": S = " << eventSum[sender] << endl;
	}
}

// метод создания случайного события
void Node::createRandEvent() {
	uniform_int_distribution<> dis(1, 100); // распределение от 1 до 100
	int randData = dis(gen); // генерируем случайное число 

	// отправляем событие всем подписчикам
	for (Node* sub : subscribers) {
		sub->chooseEvent(this, randData);
	}
}

// подписка на соседа соседа
void Node::subToNeighborsNeighbor() {
	vector<Node*> candidates; // на кого можно будет подписаться

	// все узлы, на которые подписан текущий узел (сосоеди)
	for (Node* neighbor : subscriptions) {
		// все подписки каждого соседа (соседи соседа)
		for (Node* neibhorsNeighbor : neighbor->getSubscriptions()) {
			// проверяем, что сосед соседа НЕ текущий узел (нельхя подписаться на самого себя)
			// проверяем, что текущий узел не подписан на этого соседа
			if (neibhorsNeighbor != this && !isSubscribedTo(neibhorsNeighbor)) {
				candidates.push_back(neibhorsNeighbor); // добавляем узел к кандидатам
			}
		}
	}

	// если есть на кого подписаться, то выбираем случайного соседа и подписываеся
	if (!candidates.empty()) {
		uniform_int_distribution<> dis(0, candidates.size() - 1);
		Node* target = candidates[dis(gen)];

		subscriptions.push_back(target); // подписки текущего узла
		target->subscribers.push_back(this); // добавляем текущий узел в подписчиков соседа

		// закрепляем обработчик
		uniform_int_distribution<> handlerDis(0, 1);
		handlers[target] = (handlerDis(gen) == 0) ? "N" : "S";

		eventCount[target] = 0;
		eventSum[target] = 0;
	}
}

// метод отписки от случайного соседа
void Node::unsubFromNeighbor() {
	// проверка на кого подписан текущий узел
	if (!subscriptions.empty()) {
		// выбор случайного соседа
		uniform_int_distribution<> dis(0, subscriptions.size() - 1);
		Node* target = subscriptions[dis(gen)];
		
		// поиск и удаление соседа из подписок текущего узла
		auto it = find(subscriptions.begin(), subscriptions.end(), target);

		if (it != subscriptions.end()) {
			subscriptions.erase(it);
		}

		// поиск и удаление текущего узла из подписчиков соседа
		auto subIt = find(target->subscribers.begin(), target->subscribers.end(), this);

		if (subIt != target->subscribers.end()) {
			target->subscribers.erase(subIt);
		}

		// статистика соседа больше не нужна
		eventCount.erase(target);
		eventSum.erase(target);
		handlers.erase(target);
	}
}

// метод создания нового узла
void Node::createNewNode() {
	static int newNodeCounter = 0; // статическая переменная для уникального счетчика количества узлов

	string newId = "Node_" + to_string(newNodeCounter++);

	Node* newNode = new Node(newId);		// новый узел
	addNewNodes.push_back(newNode);			// добавляем в список ожидаюзих на добавление
	subscriptions.push_back(newNode);		// текущий узел добавляет в подписки новый
	newNode->subscribers.push_back(this);	// текущий узел добавляется в подписчики нового узла

	// назначаем случайный обработчик
	uniform_int_distribution<> dis(0, 1);
	handlers[newNode] = (dis(gen) == 0) ? "N" : "S";
	eventCount[newNode] = 0;
	eventSum[newNode] = 0;
}


// метод выбора случайного действия по вероятность
Action Node::chooseRandAction(double pCreate, double pSub, double pUnsub, double pNewNode, double pNothing) const {
	discrete_distribution<> dis({ pCreate, pSub, pUnsub, pNewNode, pNothing });
	return static_cast<Action>(dis(gen));
}

// метод проверки подписан ли текущий узел на указанный
bool Node::isSubscribedTo(Node* node) const {
	return find(subscriptions.begin(), subscriptions.end(), node) != subscriptions.end();
}

// метод отписки от всех узлов
void Node::unsubFromAll() {
	// проходим по всем на кого подписан текущий узел
	for (Node* subscription : subscriptions) {
		// ещем в подписчиках текущий узел
		auto it = find(subscription->subscribers.begin(), subscription->subscribers.end(), this);
		// если есть, то удаляем текущий узел из подписчиков
		if (it != subscription->subscribers.end()) {
			subscription->subscribers.erase(it);
		}
	}
	// очистка статистики и подписок текущего узла
	subscriptions.clear();
	subscribers.clear();

	eventCount.clear();
	eventSum.clear();
	handlers.clear();
}

// метод добавления подписки
void Node::addSubscription(Node* target, const string& handlerType) {
	// добавлем target в подписки и прописываем поля
	subscriptions.push_back(target);
	handlers[target] = handlerType;
	eventCount[target] = 0;
	eventSum[target] = 0;
}

// метод удаления подписки
void Node::delSubscription(Node* target) {
	// ищем target среди подписок и удаляем, если нашли
	auto it = find(subscriptions.begin(), subscriptions.end(), target);
	if (it != subscriptions.end()) {
		subscriptions.erase(it);
	}
}

// метод добавления подписчика
void Node::addSubscriber(Node* sub) {
	subscribers.push_back(sub);
}

// метод удаления подписчика
void Node::delSubscriber(Node* sub) {
	// ищем sub среди подписчиков и удаляем, если нашли
	auto it = find(subscribers.begin(), subscribers.end(), sub);
	if (it != subscribers.end()) {
		subscribers.erase(it);
	}
}

// медот назначения обработчика
void Node::setHandler(Node* target, const string& handlerType) {
	handlers[target] = handlerType;
}