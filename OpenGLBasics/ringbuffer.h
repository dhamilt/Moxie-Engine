#pragma once



template<typename T>
struct Node
{
	Node() = delete;
	Node(T value, int id) { data = &value; _id = id; }
	~Node() { if(next) next = nullptr; if(prev) prev = nullptr; data = nullptr; }
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
class ringbuffer
{
public:
	ringbuffer<T>();
	ringbuffer<T>(int _capacity);
	void Insert(T value, int index);
	void Add(T value);
	void RemoveAt(int index);
	void RemoveAt(int begin, int end);
	int Find(T value);
	T& operator[](int index);
	


private:
	int capacity;
	int size;
	Node* head = nullptr;
	Node* tail = nullptr;
	static int id = 0;
};

template<typename T>
inline ringbuffer<T>::ringbuffer()
{

}

template<typename T>
inline ringbuffer<T>::ringbuffer(int _capacity)
{
	capacity = _capacity;
}

template<typename T>
inline void ringbuffer<T>::Insert(T value, int index)
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
inline void ringbuffer<T>::Add(T value)
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
inline void ringbuffer<T>::RemoveAt(int index)
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

	previous->next = next;
	next->prev = previous;

	delete current;
}

template<typename T>
inline void ringbuffer<T>::RemoveAt(int begin, int end)
{
	assert(end < size);


}
