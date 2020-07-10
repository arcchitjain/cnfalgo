#ifndef CNFFUNCTIONS
#define CNFFUNCTIONS

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

typedef std::vector<int> intarray;

extern int SIGNAL_OUT_OF_MEMORY;

template<typename T>
inline std::string str(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

template<typename T>
inline std::string str(const T* array, int length) {
	if (length > 0) {
		std::string result = str(*array);
		if (length > 1) result += ", ";
		result+=str(++array,--length);
		return result;
	} else {
		return "";
	}
}

template<typename T>
inline std::string str(const std::vector<T>& array) {
	if (array.size() > 0) {
		std::string result = str(array[0]);
		for (unsigned int i=1; i<array.size(); i++) {
			result += ", " + str(array[i]);	
		}
		return result;
	} else {
		return "";
	}
}

/*
 * Subtracts the elements in list b from those in list a.
 * Only considers the absolute values of the elements.
 * So if list a contains the element X and b contains -X then X will also be removed from
 *  list a.
 */
void subtract(const int* a, int al,const int* b, int bl, int*& r, int& rl);

void subtract(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& r);

void unionl(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& r);

void unionl(std::vector<int>& a, const std::vector<int>& b);

/*
 * Copies the given array into a new array.
 * If the given length of the new array > 0 then the new array
 *  will be of size al+rl.
 * The values of a will be copied to the end of the new array.
 * For example: 
 * 			copy([1,2,3,4],4,NULL,3)
 *  will output: r = [?,?,?,1,2,3,4], rl = 7.
 * 			copy([1,2,3,4],4,NULL,0)
 * 	yields: r = [1,2,3,4], rl = 4.
 * 
 * This behaviour is provided for backward-compatibility.
 */  
void copy(const int* a, int al, int*& r, int& rl);

/*
 * Inserts the new item in the list at the correct position.
 * The list is assumed to be sorted (on absolute value).
 * (if the item-to-be-added is already present in the list then it will still be added,
 *   thus appearing twice in the list.)
 */
void insert(int item, const int* a, int al, int*& r, int& rl);

void insert(int item, const std::vector<int>& alist, std::vector<int>& rlist);

/*
 * Inserts multiple values into a list at the correct position.
 * All input (and output) lists are sorted.
 * Value 0 will not be inserted.
 * itemcount gives the number of non-zeroes in the items-array in stead of the length of the
 *   array.
 */
void insert(int* items, int itemcount, int* a, int al, int*& r, int& rl);

std::string stripExtension(const std::string& fileName);

std::string& param_replace(const std::string& param, std::string text,const std::string& value);

int random_int(int max);

/*
 * Removes the elements in the list at the positions stored in index.
 * Index is assumed to be sorted.
 * arrout contains the new array (of length length-count)
 * value contains the values that were removed (of length count)
 */
void select(const int* index, int count, const int* list, int length, int* values, int* remainder); 

void start_random(unsigned int seed=0);

/*
 * Picks 'count' random numbers from the list 0 ... max-1.
 * The list is ordered.
 */
void pick_random(int max, int count, int* result);

void partition(int a[], int left, int right, int& lp, int &rp);

void quicksort(int a[], int left, int right );

void sort( int a[], int n );

void filter_negative(int* ex, int exl, int*& nex, int& nexl);

inline void filter_negative(int*& ex, int& exl) {
	int* nex = NULL;
	int nexl = 0;
	filter_negative(ex,exl,nex,nexl);
	delete[] ex;
	ex = nex;
	exl = nexl;	
}


/*
 * Filters the even values from the given list.
 */
void filter_even(int* ex, int exl, int*& nex, int& nexl, int restriction=-1);

/*
 * In-place version of the filter_even function.
 */
inline void filter_even(int*& ex, int& exl,int restriction=-1) {
	int* nex = NULL;
	int nexl = 0;
	filter_even(ex,exl,nex,nexl,restriction);
	delete[] ex;
	ex = nex;
	exl = nexl;	
}

#endif 
