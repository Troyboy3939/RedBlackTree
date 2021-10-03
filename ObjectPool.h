#pragma once
#include <vector>
#include <unordered_map>

// T SHOULD NOT BE A POINTER
template<typename T>
class ObjectPool
{
public:
	//state amount initially created, pass in pointer
	ObjectPool(int nInitialAmount = 5)
	{
		static_assert(!std::is_pointer_v<T>,
			"The argument to template must not be a pointer.");

	
		m_aObjects.reserve(nInitialAmount);

		m_nInUse = 0;

		for(int i = 0; i < nInitialAmount; i++)
		{
			m_aObjects.push_back(new T());
		}
	}

	~ObjectPool()
	{
		//delete all objects
		for(int i = 0; i < m_aObjects.size(); i++)
		{
			delete m_aObjects[i];
		}
	}

	void Return(T* pObject)
	{
		//Assign value to false
		m_aUsing.insert_or_assign(pObject, false);
		m_nInUse--;
	}



	T* GetObject()
	{
		auto nSize = m_aObjects.size();

		//if all objects are in use
		if(m_nInUse >= nSize)
		{
			//create new object, copying from the first object
			T* pObj = new T();

			//add to pool
			m_aObjects.push_back(pObj);

			//state in use
			m_aUsing.insert_or_assign(m_aObjects[nSize - 1], true);

			m_nInUse++;

			return pObj;
		}

		//for every 
		for(int i = 0; i < m_aObjects.size(); i++)
		{
			//for every object in m_aObject, check if its in use
			if(m_aUsing[m_aObjects[i]])
			{
				//skip if it is
				continue;
			}

			//if it reaches here its not in use, increment in use
			m_nInUse++;


			m_aUsing[m_aObjects[i]] = true;
			//return object
			return m_aObjects[i];
		}

		return nullptr;

	}

private:
	//stores objects
	std::vector<T*> m_aObjects;

	//keeps track of whats being used
	std::unordered_map<T*,bool> m_aUsing;

	//keeps track of total number -> skip looping through if theres none left -> cerate new one
	int m_nInUse;


};

