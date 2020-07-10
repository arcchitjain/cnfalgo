#include "functions.h"
#include <iostream>
#include <sstream>
#include <cassert>

int SIGNAL_OUT_OF_MEMORY=-5;


void subtract(const int* a, int al, const int* b, int bl, int*& r, int& rl) {
	int l = rl;
	if (al == 0) {
		r = new int[rl];
	} else if (bl == 0) {
		subtract(a+1,al-1,b,bl,r,++rl);
		*(r+l) = *a;
	} else if (abs(*a) == abs(*b)) {
		subtract(a+1,al-1,b+1,bl-1,r,rl);
	} else if (abs(*a) > abs(*b)) {
		subtract(a,al,b+1,bl-1,r,rl);
	} else {
		subtract(a+1,al-1,b,bl,r,++rl);
		*(r+l) = *a;
	}
}

void subtract(const std::vector<int>& alist, const std::vector<int>& blist, std::vector<int>& rlist) {
	rlist.reserve(alist.size());
	
	unsigned int a = 0;
	unsigned int b = 0;
	
	while (a < alist.size()) {
		assert(a<1 || abs(alist[a-1]) < abs(alist[a]));
		if (b >= blist.size()) {
			rlist.push_back(alist[a]);
			a++;
		} else {
			assert(b<1 || abs(blist[b-1]) < abs(blist[b]));
			int aval = abs(alist[a]);
			int bval = abs(blist[b]);
			if (aval == bval) {
				a++; b++;
			} else if (aval < bval) {
				rlist.push_back(alist[a]);
				a++;
			} else { // (aval > bval)
				b++;
			}
		}
	}	
}

void unionl(const std::vector<int>& alist, const std::vector<int>& blist, std::vector<int>& r) {
	unsigned int a = 0;
	unsigned int b = 0;
	
	while (a < alist.size() || b < blist.size()) {
		if (a >= alist.size()) {
			r.push_back(blist[b]); b++;
		} else {
			assert(a<1 || abs(alist[a-1]) < abs(alist[a]));
			if (b >= blist.size()) {
				r.push_back(alist[a]); a++;
			} else {
				assert(b<1 || abs(blist[b-1]) < abs(blist[b]));
				if (abs(alist[a]) == abs(blist[b])) {
					r.push_back(alist[a]); a++; b++;
				} else if (abs(alist[a]) < abs(blist[b])) {
					r.push_back(alist[a]); a++;
				} else {
					r.push_back(blist[b]); b++;
				}
			}
		}
	}

}

void unionl(std::vector<int>& alist, const std::vector<int>& blist) {
	std::vector<int> tmp = alist;
	alist.clear();
	unionl(tmp,blist,alist);
}

void copy(const int* a, int al, int*& r, int& rl) {
	r = new int[al+rl];
	for (int i=0; i<al; i++) r[i+rl]=a[i];
	rl = al+rl;
}

/*
void copy(const int* a, int al, int*& r, int& rl) {
	if (al == 0) 
		r = new int[rl];
	else {
		int l = rl;
		copy(a+1,al-1,r,++rl);
		*(r+l) = *a;
	}
}
*/

void insert(int* items, int itemcount, int* a, int al, int*& r, int& rl) {
	r = new int[al+itemcount];
	rl = al+itemcount;
	
	int j=0; // index in list items
	int ins=0; // number of inserted items
	for (int i=0; i<rl; i++) {
		while (ins < itemcount && items[j] == 0) j++;
		
		if (ins >= itemcount) { // all items inserted
			r[i] = a[i-ins]; // just copy
		} else { // some items not inserted yet
			if (i >= al || abs(a[i]) > abs(items[j])) { // insert item
				r[i] = items[j];
				ins++; j++;
			} else { // copy item
				r[i] = a[i-ins];
			}
		}
	}

}

void insert(int item, const std::vector<int>& alist, std::vector<int>& rlist) {
	rlist = alist;
	std::vector<int>::iterator i = rlist.begin();
	while (i != rlist.end()) {
		if (abs(*i) > abs(item)) {
			rlist.insert(i,item);
			return;	
		}
		i++;
	}
	rlist.push_back(item);
}


// non recursive version
void insert(int item, const int* a, int al, int*& r, int& rl) {
	if (item == 0) {
		copy(a,al,r,rl);
	} else {
		bool inserted = false;
		rl = al+1;
		r = new int[rl];
		for (int i=0; i<rl; i++) {
			if (inserted) { // element already inserted in list, just copy
				r[i] = a[i-1];
			} else { // element not inserted yet
				if (i >= al || abs(a[i]) > abs(item)) { // insert item
					r[i] = item;
					inserted = true;
				} else { // just copy
					r[i] = a[i];	
				}
			}
		}
	}
}


 // recursive version
/*
void insert(int item, const int* a, int al, int*& r, int& rl) {
	if (al == 0) {
		r = new int[++rl];
		*(r+rl-1) = item;
	} else if (abs(*a) < abs(item)) {
		int l = rl;
		insert(item,a+1,al-1,r,++rl);
		*(r+l) = *a;
	} else {
		int l = rl;
		copy(a,al,r,++rl);
		*(r+l) = item;
	}
}
*/


std::string stripExtension(const std::string& fileName) { 
	const int length = static_cast<int>(fileName.length());
	for (int i=0; i!=length; ++i) { 
		if (fileName[i]=='.') { 
			return fileName.substr(0,i) + "_" + fileName.substr(i+1); 
		} 
	} 
	return fileName; 
}

std::string& param_replace(const std::string& param, std::string text,const std::string& value) {
	int index = static_cast<int>(text.find(param,0));
	return text.replace(index,1,value); 
	
}

//void select(const int* index, int count, const int* list, int length, int* values, int* remainder);
void select(const int* index, int count, const int* list, int length, int* values, int* remainder) {
	
	int c=0;
	for (int i=0; i<length; i++) {
		if (c < count && i == index[c]) {
			// delete current value
			values[c] = list[i];
			c++;
		} else {
			// keep current value
			remainder[i-c] = list[i];
		}
	}
}
void start_random(unsigned int seed) {
	srand(seed);	
}

int random_int(int max) {
	return rand() % max;
}

void pick_random(int max, int count, int* result) {
	for (int i=0; i<count; i++) result[i] = -1;
	int n=0;
	while (n<count) {
		int r = random_int(max);
		bool found=false;
		for (int i=0; i<count; i++) { 
			if (r==result[i]) {
				found=true;
				break;	
			}
		}
		if (!found) {
			result[n] = r;
			n++;
		}
			
	};
	
	sort(result,count);	
}

void partition(int a[], int left, int right, int& lp, int &rp)
{
    int i = left + 1; 
    int j = left + 1;
    int x = a[left];
    while (j <= right) {
       if (a[j] < x) {
          int temp = a[j];
          a[j] = a[i];
          a[i] = temp;
          i++;
       }
       j++;
    }
    a[left] = a[i-1];
    a[i-1] = x;
    lp = i - 2;
    rp = i;
}

void quicksort(int a[], int left, int right ) {
    if (left < right) {
       int lp, rp;
       partition( a, left, right, lp, rp );
       quicksort(a, left, lp);
       quicksort(a, rp, right);
    }
}

void sort( int a[], int n )
{
    quicksort(a, 0, n-1);
}

void filter_negative(int* ex, int exl, int*& nex, int& nexl) {
	if (exl == 0) {
		nex = new int[nexl];
	} else {
		if (ex[0] < 0) {
			filter_negative(ex+1,exl-1,nex,nexl);	
		} else {
			int i=nexl;
			filter_negative(ex+1,exl-1,nex,++nexl);
			nex[i] = ex[0];
		}
	}
}

void filter_even(int* ex, int exl, int*& nex, int& nexl, int restriction) {
	if (exl == 0) {
		nex = new int[nexl];
	} else {
		if (ex[0] % 2 == 0 && (restriction <= 0 || ex[0] < restriction)) {
			filter_even(ex+1,exl-1,nex,nexl,restriction);	
		} else {
			int i=nexl;
			filter_even(ex+1,exl-1,nex,++nexl,restriction);
			nex[i] = ex[0];
		}
	}
}
