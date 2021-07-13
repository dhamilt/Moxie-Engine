#pragma once
#include <map>
#include <vector>
#include <iterator>
template<typename T> 
class ObjectPool
{
public:
	ObjectPool<T>() {};
	ObjectPool<T>(int _minPoolSize, int _maxPoolSize) :maxPoolSize(_maxPoolSize), minPoolSize(_minPoolSize){};
	void GetItem(T& item);
	T RemoveItem(T item);
	std::vector<T> RemoveAllObjects();
	void AddToPool(T item);
	int Find(T item);
	bool DeactivateInPool(T item);
	void Shrink();

private:
	int minPoolSize, maxPoolSize;
	std::map<T, bool>pool;
	bool shrink;
	typename std::map<T, bool>::iterator it;
};

template<typename T>
inline void ObjectPool<T>::GetItem(T& item)
{
	// loop through the pool
	for (it = pool.begin(); it != pool.end(); it++)
	{
		// find the first object that is inactive
		if (!it->second)
		{
			// Set active
			it->second = true;
			// Return it
			item = it->first;
			// And exit the function
			return;
		}
	}
	// otherwise return nothing
	item = NULL;
}

template<typename T>
inline T ObjectPool<T>::RemoveItem(T item)
{
	it = pool.begin();
	while (it != pool.end())
	{
		if (it->first == item)
		{
			T temp = it->first;
			pool.erase(it);
			return temp;
		}
		it++;
	}
	return NULL;
}

template<typename T>
inline std::vector<T> ObjectPool<T>::RemoveAllObjects()
{
	std::vector<T> result;
	
	it = pool.begin();
	while (it != pool.end())
	{
		result.push_back(it->first);
		it++;
	}
	pool.erase(pool.begin(), pool.end());
	return result;
}

template<typename T>
inline void ObjectPool<T>::AddToPool(T item)
{
	if (pool.size() < maxPoolSize)
		pool.insert(std::pair<T, bool>( item, true));
}

template<typename T>
inline int ObjectPool<T>::Find(T item)
{
	int i = 0;
	for (it = pool.begin(); it != pool.end(); it++)
	{
		if (it->first == item)
			return i;

		i++;
	}
	return -1;
}

template<typename T>
inline bool ObjectPool<T>::DeactivateInPool(T item)
{
	for (it = pool.begin(); it != pool.end(); it++)
	{
		if (it->first == item)
		{
			it->second = false;
			return true;
		}
	}
	return false;
}

template<typename T>
inline void ObjectPool<T>::Shrink()
{
	if (pool.size() > minPoolSize)
	{
		std::vector<int> removalIndices;
		int index = 0;		 
		it = pool.begin();

		// while looping through the pool
		while ( it != pool.end() )
		{
			// if there are objects that can be removed from the pool
			if(pool.size() - removalIndices.size() > minPoolSize)
			{
				// if current object is inactive
				if (it->second == false)
					// remove it
					removalIndices.push_back(index);

			}
			else
				break;
			it++;
			index++;
		}

		// Remove the objects at the indices
		for (int i = 0; i < removalIndices.size(); i++)
			pool.erase(pool.begin() + removalIndices[i]);
	}
}
