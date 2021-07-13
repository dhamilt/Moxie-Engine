#pragma once
#include <mutex>
#include <memory>

/// <summary>
/// Template class for creating a buffer of data
/// that allocate itself in a circular format. 
/// This class will also overwrite itself in the same format.
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class circular_buffer
{
public:
	explicit circular_buffer<T>();
	explicit circular_buffer<T>(size_t _capacity);


public:
	void put(T item);
	T get();
	void clear();
	bool empty() const;
	bool full()	const;
	size_t capacity() const;
	size_t size() const;

private:
	std::mutex _mutex;
	std::unique_ptr<T[]>buffer;
	size_t head = 0;
	size_t tail = 0;
	size_t max_size;
	bool _full = false;
};

template<typename T>
inline circular_buffer<T>::circular_buffer()
{
}

template<typename T>
inline circular_buffer<T>::circular_buffer(size_t _capacity)
{
	max_size = _capacity;
}

template<typename T>
inline void circular_buffer<T>::put(T item)
{
	std::lock_guard<std::mutex> lock(_mutex);

	buffer[head] = item;
	// if the buffer is full
	if (_full)
		// resize the tail half 
		tail = (tail + 1) % max_size;
	
	head = (head + 1) % max_size;
	_full = head == tail;
}

template<typename T>
inline T circular_buffer<T>::get()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if(empty())
		return T();

	// Read data in the buffer and 
	// move down the tail to make more space
	auto val = buffer[tail];
	_full = false;
	tail = (tail + 1) % max_size;

	return val;
}

template<typename T>
inline void circular_buffer<T>::clear()
{
	// lock thread until data has been cleared
	std::lock_guard<std::mutex> lock(_mutex);
	head = tail;
	_full = false;
}

template<typename T>
inline bool circular_buffer<T>::empty() const
{
	// if the head and tail of the buffer are equal
	// while not being full, the buffer is empty
	return (!_full && (head == tail));
}

template<typename T>
inline bool circular_buffer<T>::full() const
{
	return _full;
}

template<typename T>
inline size_t circular_buffer<T>::capacity() const
{
	return max_size;
}

template<typename T>
inline size_t circular_buffer<T>::size() const
{
	size_t size = max_size;

	// if buffer isnt full
	if (!_full)
	{
		// If the head half of the buffer is bigger
		// or the same size of the tail half		
		if (head >= tail)
			// size is the difference between the halves
			size = head - tail;
		// otherwise the size is the difference 
		// plus the max size
		else
			size = max_size + head - tail;
	}
	return size;
}
