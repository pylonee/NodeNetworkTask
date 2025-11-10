#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/*
* -- Возможные действия узла --
* 
* CREATE_EVENT - генерирует случайное число и отправляет подписчикам
* SUBSCRIBE - узел подписывается на соседа своего соседа
* UNSUBSCRIBE - узел отписывается от одного из соседей
* CREATE_NODE - узел создаёт новый узел и подписываетс яна него
* NOTHING - узел ни чего не делает
*/

enum class Action {
	CREATE_EVENT,
	SUBSCRIBE,
	UNSUBSCRIBE,
	CREATE_NODE,
	NOTHING
};


// Узел сети
class Node {
private:
	string id; // уникальный идентификатор
	vector<Node*> subscriptions; // узлы на которые подписан данный узел
	vector<Node*> subscribers; // узлы, которые подписаны на данный узел
	unordered_map<Node*, int> eventCount; // N - количество событий полученных от каждого отправителя
	unordered_map<Node*, int> eventSum; // S - сумма всех чисел от каждого отправителя
	unordered_map<Node*, string> handlers; // обработчики N (количество событий) или S (сумма чисел)
	vector<Node*> addNewNodes; // Временное хранилище узлов

	void createRandEvent(); // создаёт и отправляет случайное событие
	void subToNeighborsNeighbor(); // подписаться на соседа соседа
	void unsubFromNeighbor(); // отписаться от соседа
	void createNewNode(); // создаёт новый узел

	// вспомогательный метод для выбора действия
	Action chooseRandAction(double pCreate, double pSub, double pUnsub, double pNewNode, double pNothing) const;

public:
	// конструктор узла
	Node(const string& nodeId) : id(nodeId) {}

	// метод обновления узла
	void update(double pCreate, double pSub, double pUnsub, double pNewNode, double pNothing);

	// выбор обработчика. вызывается, когда узел получает событие от другого узла
	// sender - указатель на узел отправителя
	// data - случайно сгенерированное узлом отправителем число
	void chooseEvent(Node* sender, int data);

	// геттеры
	const string& getId() const { return id; }
	const vector<Node*>& getSubscriptions() const { return subscriptions; }
	const vector<Node*>& getSubscriberts() const { return subscribers; }
	vector<Node*>& getAddNewNodes() { return addNewNodes; }

	bool isSubscribedTo(Node*) const; // проверка подписан ли узел на передаваемый узел
	void unsubFromAll(); // отписаться от всех узлов (перед удалением)

	void addSubscription(Node* target, const string& handlerType); // добавить подписку
	void delSubscription(Node* target); // удалить подписку
	void addSubscriber(Node* sub); // добавить подписчика
	void delSubscriber(Node* sub); // удалить подписчика

	void setHandler(Node* target, const string& handlerType); // обработчик
};
