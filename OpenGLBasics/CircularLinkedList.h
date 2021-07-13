#pragma once



template<typename T>
struct Node
{
	Node() = delete;
	Node(T value, int id) { data = &value; _id = id; }
	~Node() { if(this->next) next = nullptr; if(this->prev) prev = nullptr; this->data = nullptr; }
	Node* next;
	Node* prev;

	int _id;

	T* data;
	bool operator==(const Node* node)
	{
		return this->_id == node->_id;
	}
};

template<typename T>
class CircularLinkedList
{
public:
	CircularLinkedList<T>();
	CircularLinkedList<T>(int _capacity);
	~CircularLinkedList();
	void Insert(T value, int index);
	void Add(T value);
	void RemoveAt(int index);
	void RemoveAt(int begin, int count);
	int Find(T value);
	T*& operator[](int index);
	


private:
	int capacity;
	int size;
	Node* head = nullptr;
	Node* tail = nullptr;
	static int id = 0;
};

template<typename T>
inline CircularLinkedList<T>::CircularLinkedList()
{

}

template<typename T>
inline CircularLinkedList<T>::CircularLinkedList(int _capacity)
{
	capacity = _capacity;
}

template<typename T>
inline CircularLinkedList<T>::~CircularLinkedList()
{
	RemoveAt(0, size);
}

template<typename T>
inline void CircularLinkedList<T>::Insert(T value, int index)
{
	assert(index < size);

	int i = 0;
	Node* current = head;
	while (i < index)
	{
		
		if (i == index)
		{
			Node* previous = current->prev;
			Node* next = current->next;
			
			Node* temp = new Node(value, id);			
			id++;			
			temp->prev = previous;
			temp->next = next;

			previous->next = temp;
			next->prev = temp;
		}
		i++;
		current = current->next;
	}
	size++;
}

template<typename T>
inline void CircularLinkedList<T>::Add(T value)
{
	Node* temp = new Node(value, id);
	id++;

	if (head == nullptr)
	{
		head = temp;		
		return;
	}

	if (tail == nullptr)
	{		
		head->next = temp;
		temp->prev = head;
		temp->next = head;
		tail = temp;		
		return;
	}
	else
	{
		temp->next = head;
		temp->prev = tail;
		head->prev = temp;
		tail->next = temp;
		tail = temp;
	}

	size++;

	if (capacity < size)
		capacity++;

}

template<typename T>
inline void CircularLinkedList<T>::RemoveAt(int index)
{
	assert(index < size);

	int i = 0;
	Node* current = head;
	while (i < index)
	{
		i++;
		current = current->next;
	}
	
	Node* previous = current->prev;
	Node* next = current->next;

	if(previous)
		previous->next = next;

	if(next)
		next->prev = previous;

	delete current;
}

template<typename T>
inline void CircularLinkedList<T>::RemoveAt(int begin, int count)
{
	assert(begin + count < size);

	Node* current = head;
	Node* previous = current->prev;
	Node* next = current->next;
	int i = 0;

	while (i < begin + count)
	{
		if (i >= begin)
		{
			if(previous)
				previous->next = next;

			if(next)
				next->prev = previous;

			delete current;
			size--;
		}
		i++;
		current = next;
		previous = current->prev;
		next = current->next;
	}


}

template<typename T>
inline int CircularLinkedList<T>::Find(T value)
{
	int i = 0;
	Node* current = head;
	while(i < size)
	{
		if (*(current->data) == value)
			return i;

		i++;
		current = current->next;
	}
	
	i = -1;
	return i;
}

template<typename T>
inline T*& CircularLinkedList<T>::operator[](int index)
{
	assert(index < size);
	int i = 0;
	Node* current = head;

	while (i <= index)
	{
		if (i == index)
			return current->data;

		i++;
		current = current->next;
	}
	return NULL;
}
