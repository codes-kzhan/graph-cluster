// -----------------------------------------------------------------------------
// Local Search Non-negative Matrix Factorization
// By Twan van Laarhoven
// License: BSD style, see file LICENSE
// -----------------------------------------------------------------------------

#ifndef HEADER_SPARSE_VECTOR
#define HEADER_SPARSE_VECTOR

#include "util.hpp"
#include "sparse_matrix.hpp"
#include <vector>

namespace lso_cluster {

// -----------------------------------------------------------------------------
// Data structures
// -----------------------------------------------------------------------------

struct SparseItem {
	clus_t clus;
	double weight;
	SparseItem() {}
	SparseItem(clus_t clus, double weight) : clus(clus), weight(weight) {}
};
inline bool operator < (SparseItem const& x, clus_t y) {
	return x.clus < y;
}
inline bool operator < (clus_t x, SparseItem const& y) {
	return x < y.clus;
}
inline bool operator < (SparseItem const& x, SparseItem const& y) {
	return x.clus < y.clus;
}

#if __cplusplus < 201103L
// Check that a list is sorted
template <class ForwardIterator> inline bool is_sorted(ForwardIterator first, ForwardIterator last) {
	if (first==last) return true;
	ForwardIterator next = first;
	while (++next!=last) {
		if (*next<*first) return false;
		++first;
	}
	return true;
}
#endif

// A sparse vector is a vector of SparseItems, sorted by key
struct SparseVector : private std::vector<SparseItem> {
  public:
	using std::vector<SparseItem>::iterator;
	using std::vector<SparseItem>::const_iterator;
	using std::vector<SparseItem>::begin;
	using std::vector<SparseItem>::end;
	using std::vector<SparseItem>::clear;
	using std::vector<SparseItem>::empty;
	
	// this can invalidate the order, call sort() to fix it
	// maybe a conversion function would be nicer?
	using std::vector<SparseItem>::push_back;
	using std::vector<SparseItem>::pop_back;
	using std::vector<SparseItem>::back;
	
	// lookup a value
	inline double operator () (clus_t i) const;
	inline bool contains(clus_t i) const;
	
	// re-sort by cluster-id/key
	inline void sort() {
		std::sort(this->begin(), this->end());
	}
	
	// insert a value, it must not yet exist
	inline void insert(clus_t k, double weight);
	// remove an item, return the old weight
	inline double remove(clus_t k);
	
	inline size_t nnz() const { return size(); }
};

std::ostream& operator << (std::ostream& out, SparseVector const& vec) {
	out << "[";
	for (SparseVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it) {
		if (it != vec.begin()) out << ", ";
		out << it->clus << ":" << it->weight;
	}
	return out << "]";
}

double SparseVector::operator () (clus_t k) const {
	assert(is_sorted(this->begin(),this->end()));
	/*if (!is_sorted(this->begin(),this->end())) {
		octave_stdout << "not sorted! " << *this << endl;
		throw "bork";
	}*/
	
	const_iterator it = std::lower_bound(this->begin(),this->end(),k);
	if (it != this->end() && it->clus == k) {
		return it->weight;
	} else {
		return 0.;
	}
}
bool SparseVector::contains(clus_t k) const {
	const_iterator it = std::lower_bound(this->begin(),this->end(),k);
	return (it != this->end() && it->clus == k);
}

void SparseVector::insert(clus_t k, double weight) {
	iterator it = std::lower_bound(this->begin(),this->end(),k);
	assert(it == this->end() || it->clus != k);
	std::vector<SparseItem>::insert(it, SparseItem(k,weight));
}
double SparseVector::remove(clus_t k) {
	iterator it = std::lower_bound(this->begin(),this->end(),k);
	assert(it != this->end() && it->clus == k);
	double weight = it->weight;
	std::vector<SparseItem>::erase(it);
	return weight;
}

double dot(SparseVector const& x, SparseVector const& y) {
	double sum = 0.;
	for (SparseVector::const_iterator it1 = x.begin(), it2 = y.begin() ; it1 != x.end() && it2 != y.end() ; ) {
		if (it1->clus < it2->clus) {
			++it1;
		} else if (it1->clus > it2->clus) {
			++it2;
		} else {
			sum += it1->weight * it2->weight;
			++it1; ++it2;
		}
	}
	return sum;
}

double sumsq(SparseVector const& x) {
	double sum = 0.;
	for (SparseVector::const_iterator it = x.begin() ; it != x.end() ; ++it) {
		sum += it->weight * it->weight;
	}
	return sum;
}

void operator += (std::vector<double>& x, SparseVector const& y) {
	for (SparseVector::const_iterator it = y.begin() ; it != y.end() ; ++it) {
		x[it->clus] += it->weight;
	}
}

// -----------------------------------------------------------------------------
}
#endif
