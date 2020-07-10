#ifndef SMARTPOINTER_H_
#define SMARTPOINTER_H_

template<class T, bool D=false>
struct smart_ptr {
//	private :
		T* pointer;
		bool destruct;
		
//	public :
		smart_ptr() : pointer(NULL), destruct(D) {}
	
		smart_ptr(T* p) : pointer(p), destruct(D) {}
		
		smart_ptr(T* p, bool d) : pointer(p), destruct(d) {}
		
		~smart_ptr() { if (destruct) delete pointer; }
	
//		T* get() { return pointer; }
		
		T* operator->() const throw() { return pointer; }
		
		T& operator*() const throw() { return *pointer; }
		
//		bool getFlag(bool val) { return destruct; }
		
//		void setFlag(bool val) { destruct = val; }
		
		smart_ptr& operator=(T* ptr) {
			if (destruct) delete pointer;
			pointer = ptr;
			destruct = D;
			return *this;
		}
		
		smart_ptr& operator=(smart_ptr<T>& ptr) {
			if (destruct) delete pointer;
			pointer = ptr.pointer;
			destruct = ptr.destruct;
			ptr.destruct = false;
			return *this;
		}
		
		bool operator==(const T* ptr) const {
			return (pointer == ptr);
		}

		bool operator!=(const T* ptr) const {
			return (pointer != ptr);
		}
	
		bool operator!() const {
			return (pointer == NULL);
		}
		
		void set_destruct(bool val) {
			destruct = val;
		}
};

#endif /*SMARTPOINTER_H_*/
