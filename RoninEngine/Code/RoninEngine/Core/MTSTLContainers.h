#pragma once

#include "CoreMemObject.h"

#include "ThreadSyncPrimitives.h"

// Multi-Thread safe vector container, can be used instead of stl_vector.
// Every operation is wrapped via a critical section
// (base version taken from CryEngine Free SDK CryCommon, see CryMT::vector)

template <class T>
class mt_stl_vector : public CCoreMemObject
{
public:
	typedef	CAutoCriticalSection AutoLock;

	CCriticalSection& get_lock() const { return m_CriticalSection; }

	//////////////////////////////////////////////////////////////////////////

	mt_stl_vector() {}
	mt_stl_vector( const mt_stl_vector<T> &rOther )
	{		
		AutoLock lock1(m_CriticalSection); 
		AutoLock lock2(rOther.m_CriticalSection); 

		m_vector = rOther.m_vector;
	}
	mt_stl_vector& operator=( const mt_stl_vector<T> &rOther )
	{		
		if( this == &rOther )
			return *this;

		AutoLock lock1(m_CriticalSection); 
		AutoLock lock2(rOther.m_CriticalSection); 

		m_vector = rOther.m_vector;

		return *this;
	}

	bool   empty() const { AutoLock lock(m_CriticalSection); return m_vector.empty(); }
	int    size() const  { AutoLock lock(m_CriticalSection); return m_vector.size(); }
	void   resize( int sz ) { AutoLock lock(m_CriticalSection); m_vector.resize( sz ); }
	void   reserve( int sz ) { AutoLock lock(m_CriticalSection); m_vector.reserve( sz ); }
	size_t capacity() const { AutoLock lock(m_CriticalSection); return m_vector.size(); }
	void   clear() { AutoLock lock(m_CriticalSection); m_vector.clear(); }
	T&	   operator[]( size_t pos ) { AutoLock lock(m_CriticalSection); return v[pos]; }
	const T& operator[]( size_t pos ) const { AutoLock lock(m_CriticalSection); return v[pos]; }
	const T& front() const { AutoLock lock(m_CriticalSection); return m_vector.front(); }
	const T& back() const { AutoLock lock(m_CriticalSection); 	return m_vector.back(); }
	T& back() { AutoLock lock(m_CriticalSection); 	return m_vector.back(); }
	void free_memory() { AutoLock lock(m_CriticalSection); m_vector.free_memory(); }

	void push_back( const T& x ) { AutoLock lock(m_CriticalSection); return m_vector.push_back(x); }
	void pop_back() { AutoLock lock(m_CriticalSection); 	return m_vector.pop_back(); }

	//////////////////////////////////////////////////////////////////////////

	template <class Func>
	void sort( const Func &compare_less ) 
	{ 
		AutoLock lock(m_CriticalSection); 
		stl_sort( m_vector.begin(),m_vector.end(),compare_less ); 
	}

	template <class Iter>
	void append( const Iter &startRange,const Iter &endRange ) 
	{ 
		AutoLock lock(m_CriticalSection); 
		m_vector.insert( m_vector.end(), startRange,endRange ); 
	}

	void swap(stl_vector<T> &vec) 
	{ 
		AutoLock lock(m_CriticalSection); 
		m_vector.swap(vec); 
	}

	//////////////////////////////////////////////////////////////////////////

	bool try_pop_front( T& returnValue )
	{
		AutoLock lock(m_CriticalSection); 
		if (!m_vector.empty())
		{
			returnValue = m_vector.front();
			m_vector.erase(m_vector.begin());
			return true;
		}
		return false;
	};
	bool try_pop_back( T& returnValue )
	{
		AutoLock lock(m_CriticalSection); 
		if (!m_vector.empty())
		{
			returnValue = m_vector.back();
			m_vector.pop_back();
			return true;
		}
		return false;
	};

	//////////////////////////////////////////////////////////////////////////

	template <typename FindFunction,typename KeyType>
	bool find_and_copy( FindFunction findFunc,const KeyType &key,T &foundValue ) const
	{
		AutoLock lock(m_CriticalSection);
		if(!m_vector.empty())
		{
			typename std::vector<T>::const_iterator it;
			for (it = m_vector.begin(); it != m_vector.end(); ++it)
			{
				if (findFunc(key,*it))
				{
					foundValue = *it;
					return true;
				}
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	bool try_remove( const T& value )
	{
		AutoLock lock(m_CriticalSection);
		if(!m_vector.empty())
		{
			typename std::vector<T>::iterator it = std::find(m_vector.begin(), m_vector.end(), value);
			if(it != m_vector.end())
			{
				m_vector.erase(it);
				return true;
			}
		}
		return false;
	};

private:
	
	// the actual STL_Vector
	stl_vector<T> m_vector;

	// needs to be mutable because it has to operate on a const 
	// object as well
	mutable	CCriticalSection m_CriticalSection;
};

