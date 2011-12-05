#pragma once

#include "MemoryManager.h"

namespace CoreObject
{
	//////////////////////////////////////////////////////////////////////////

	// default memory tracked object with certain memory type to define allocator to use
	template<TMemoryType memType>
	class CMemObject
	{

#ifdef USE_MEMORY_TRACKING
	private:
		// new and new[] should not be unavailable to the application.
		static void* operator new(size_t stSize);
		static void* operator new[](size_t stSize);
#endif

	public:
		static void* operator new(size_t stSize
#ifdef USE_MEMORY_TRACKING
			, const char* pFile, int iLine, const char* pFunction
#endif
			)
		{
			if (stSize == 0)
				stSize = 1;

			void* p = CMemoryManager::GetInstance()->Allocate(stSize, memType
#ifdef USE_MEMORY_TRACKING
				, pFile, iLine, pFunction
#endif
				);

			//if (p == 0)
			//	throw std::bad_alloc();

			return p;
		}
		static void* operator new[](size_t stSize
#ifdef USE_MEMORY_TRACKING
			, const char* pFile, int iLine, const char* pFunction
#endif
			)
		{
			if (stSize == 0)
				stSize = 1;

			void* p = CMemoryManager::GetInstance()->Allocate(stSize, memType
#ifdef USE_MEMORY_TRACKING
				, pFile, iLine, pFunction
#endif
				);

			//if (p == 0)
			//	throw std::bad_alloc();

			return p;
		}

		static void operator delete(void* pMemory)
		{
			if (pMemory)
			{
				CMemoryManager::GetInstance()->Deallocate(pMemory, memType);
			}
		}
		static void operator delete[](void* pMemory)
		{
			if (pMemory)
			{   
				CMemoryManager::GetInstance()->Deallocate(pMemory, memType);
			} 
		}

#ifdef USE_MEMORY_TRACKING
		// Required for exception handling in the compiler
		static void operator delete(void* pMemory, 
			const char* pFile, int iLine, const char* pcFunction) {}
		static void operator delete[](void* pMemory, 
			const char* pFile, int iLine, const char* pFunction) {}
#endif

		// We don't want to hide the placement new and delete functions:
		static void* operator new( size_t stSize, void* p ) { return p; }
		static void* operator new[]( size_t stSize, void* p ) { return p; }
		static void operator delete( void *, void* ) {}
		static void operator delete[]( void *, void* ) {}
	};

	//////////////////////////////////////////////////////////////////////////

	template<typename counter>
	class CSTIncrementPolicy
	{
	public:
		static counter AddRef(counter& refCount) { return ++refCount; }
		static counter DecRef(counter& refCount) { return --refCount; }
	};

	template<typename counter>
	class CMTIncrementPolicy
	{
	public:
		static counter AddRef(counter& refCount) { return InterlockedIncrement((LONG*)&refCount); }
		static counter DecRef(counter& refCount) { return InterlockedDecrement((LONG*)&refCount); }
	};

	//////////////////////////////////////////////////////////////////////////

	// a refcounted memory tracked object with optional increment policy and counter type
	template<TMemoryType memType, class incrementPolicy, typename counter>
	class CBaseRefCountedObject: public CMemObject<memType>
	{
	public:

		CBaseRefCountedObject() : m_refCount(0) {}
		virtual ~CBaseRefCountedObject() {}

		void AddRef() { incrementPolicy::AddRef(m_refCount); }
		void DecRef() { 
			if (incrementPolicy::DecRef(m_refCount) == 0)
				DeleteThis();
		}
		counter GetRefCount() const { return m_refCount; }

	protected:

		// made virtual so a derived object can do something 'special' on deletion
		virtual void DeleteThis()
		{
			delete this;
		}

	private:

		// the actual refcount of the object
		counter m_refCount;
	};

	//////////////////////////////////////////////////////////////////////////

	// smart pointer object used to manage a normal refcounted class T
	template <class T, TMemoryType memType> 
	class CSmartPointer : public CMemObject<memType>
	{
	public:

		CSmartPointer(T* pObject = (T*) 0)
		{
			m_pObject = pObject;
			if (m_pObject)
				m_pObject->AddRef();
		}
		CSmartPointer(const CSmartPointer& other)
		{
			m_pObject = other.m_pObject;
			if (m_pObject)
				m_pObject->AddRef();
		}
		~CSmartPointer()
		{
			if (m_pObject)
				m_pObject->DecRef();
		}

		operator T*() const { return m_pObject; }
		operator const T*() const { return m_pObject; }
		T& operator*() const { return m_pObject; }
		T* operator->() const { return m_pObject; }
		T* Get() const { return m_pObject; }

		CSmartPointer& operator=(const CSmartPointer& ptr)
		{
			if (m_pObject != ptr.m_pObject)
			{
				if (m_pObject)
					m_pObject->DecRef();
				m_pObject = ptr.m_pObject;
				if (m_pObject)
					m_pObject->AddRef();
			}
			return *this;
		}
		CSmartPointer& operator=(T* pObject)
		{
			if (m_pObject != pObject)
			{
				if (m_pObject)
					m_pObject->DecRef();
				m_pObject = pObject;
				if (m_pObject)
					m_pObject->AddRef();
			}
			return *this;
		}

		void Reset()
		{
			CSmartPointer<T>().Swap(*this);
		}
		void Reset(T* pOther)
		{
			CSmartPointer<T>()(pOther).Swap(*this);
		}
		void Swap(CSmartPointer<T, memType>& other)
		{
			stl_swap(m_pObject, other.m_pObject);
		}

		operator bool() const { return m_pObject != NULL; };
		bool operator !() const { return m_pObject == NULL; };
		bool operator==(const T* pObject) const { return m_pObject == pObject; }
		bool operator==(T* pObject) const { return m_pObject == pObject; }
		bool operator==(const CSmartPointer& other) const { m_pObject == other.m_pObject; }
		bool operator!=(const T* pObject) const { return m_pObject != pObject; }
		bool operator!=(T* pObject) const { return m_pObject != pObject; }
		bool operator!=(const CSmartPointer& other) const { m_pObject != other.m_pObject; }
		bool  operator <(const T* pOther) const { return m_pObject < pOther; };
		bool  operator >(const T* pOther) const { return m_pObject > pOther; };

	protected:

		// the managed pointer
		T* m_pObject;
	};	

	template <class T, TMemoryType memType> 
	inline bool operator ==(const CSmartPointer<T, memType>& smartPointer, int null)	
	{
		//assert(!null);
		return !(bool)smartPointer;	
	}
	template <class T, TMemoryType memType> 
	inline bool operator !=(const CSmartPointer<T, memType>& smartPointer, int null)
	{
		//assert(!null);
		return (bool)smartPointer;	
	}
	template <class T, TMemoryType memType> 
	inline bool operator ==(int null, const CSmartPointer<T, memType>& smartPointer)
	{
		//assert(!null);
		return !(bool)smartPointer;	
	}
	template <class T, TMemoryType memType> 
	inline bool operator !=(int null, const CSmartPointer<T, memType>& smartPointer)
	{
		//assert(!null);
		return (bool)smartPointer;	
	}

	#define SmartPointer(classname, memType) \
		class classname; \
		typedef CoreObject::CSmartPointer<classname, memType> classname##Ptr

	// Use for casting a smart pointer of one type to a pointer or smart pointer of another type.
	#define SmartPointerCast(type, smartptr) ((type*) (void*) (smartptr))

	//////////////////////////////////////////////////////////////////////////

	// multithreaded ref counted object which uses unsigned int as refcounter
	template <TMemoryType memType> 
	class CRefMTObject : public CBaseRefCountedObject<memType, CMTIncrementPolicy<unsigned int>, unsigned int>
	{
	};

	// multithreaded ref counted object which uses unsigned int as refcounter
	template <TMemoryType memType> 
	class CRefSTObject : public CBaseRefCountedObject<memType, CSTIncrementPolicy<unsigned int>, unsigned int>
	{
	};
}

#define CoreSmartPointer(classname) \
	typedef CoreObject::CSmartPointer<classname, MEMORY_TYPE_DEFAULT> classname##Ptr

// define default core object which are used by the engine internally, and store the data
// in the default memory allocator
class CCoreMemObject : public CoreObject::CMemObject<MEMORY_TYPE_DEFAULT>
{
};
class CCoreRefMTObject : public CoreObject::CRefMTObject<MEMORY_TYPE_DEFAULT>
{
};
class CCoreRefSTObject : public CoreObject::CRefSTObject<MEMORY_TYPE_DEFAULT>
{
};
