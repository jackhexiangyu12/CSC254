/*
    Starter code for assignment 6, CSC 2/454, Fall 2019

    Provides skeleton of code for a simple hierarchy of set
    abstractions.

    Everything but /main/ should be moved to a .h file, which should
    then be #included from here.
*/

#include <set>
#include <iostream>
#include <string.h>
#include <type_traits>
using std::set;
using std::cout;
using std::string;

// Naive comparator.
// Provides a default for any type that has an operator<
// and an operator==.
//
template<typename T>
class comp {
  public:
    bool precedes(const T& a, const T& b) const {
        // replace this line:
        // (void) a;  (void) b;  return true;
        return a < b;
    }
    bool equals(const T& a, const T& b) const {
        // replace this line:
        // (void) a;  (void) b;  return true;
        return a == b;
    }
};

// Abstract base class from which all sets are derived.
//
template<typename T, typename C = comp<T>>
class simple_set {
  public:
    virtual ~simple_set<T, C>() { }
        // destructor should be virtual so that we call the right
        // version when saying, e.g.,
        // simple_set* S = new derived_set(args);
        //  ...
        // delete S;
    virtual simple_set<T, C>& operator+=(const T item) = 0;
        // inserts item into set
        // returns a ref so you can say, e.g.
        // S += a += b += c;
    virtual simple_set<T, C>& operator-=(const T item) = 0;
        // removes item from set, if it was there (otherwise does nothing)
    virtual bool contains(const T& item) const = 0;
        // indicates whether item is in set
};

//---------------------------------------------------------------

// Example of a set that implements the simple_set interface.
// Borrows the balanced tree implementation of the standard template
// library.  Note that you are NOT to use any standard library
// collections in your code (though you may use strings and streams).
//
template<typename T>
class std_simple_set : public virtual simple_set<T>, protected set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
  public:
    virtual ~std_simple_set<T>() { }  // will invoke std::~set<T>()
    virtual std_simple_set<T>& operator+=(const T item) {
        set<T>::insert(item);
        return *this;
    }
    virtual std_simple_set<T>& operator-=(const T item) {
        (void) set<T>::erase(item);
        return *this;
    }
    virtual bool contains(const T& item) const {
        return (set<T>::find(item) != set<T>::end());
    }
};

//---------------------------------------------------------------

// Characteristic array implementation of set.
// Requires instantiation with guaranteed low and one-more-than-high
// bounds on elements that can be placed in the set.  Should compile
// and run correctly for any element class T that can be cast to int.
// Throws out_of_bounds exception when appropriate.
//
class out_of_bounds { };    // exception
template<typename T>
class carray_simple_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
    // You'll need some data members here.
  private:
    T low;
    T high;
    bool* data;
    out_of_bounds err;
  public:
    // fill in these methods:
    carray_simple_set(const T l, const T h) {   // constructor
        // replace this line:
        // (void) l;  (void) h;
        if (l > h) throw err;
        low = l;
        high = h;
        data = new bool[high-low];
    }
    virtual ~carray_simple_set() {              // destructor
        // your code here
        delete [] data;
    }
    virtual carray_simple_set<T>& operator+=(const T item) {
        // replace this line:
        // (void) item;  return *this;
        if ((item < low) || (item >= high)) throw err;
        data[(int)item - (int)low] = true;
        return *this;
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        // replace this line:
        // (void) item;  return *this;
        if ((item < low) || (item >= high)) throw err;
        data[(int)item - (int)low] = false;
        return *this;
    }
    virtual bool contains(const T& item) const {
        // replace this line:
        // (void) item;  return true;
        if ((item < low) || (item >= high)) throw err;
        return data[(int)item - (int)low] == true;
    }
};

//---------------------------------------------------------------

// Naive hash function object.
// Provides a default for any type that can be cast to int.
//
template<typename T>
class cast_to_int {
  public:
    int operator()(const T n) {
        return (int) n;
    }
};

// Hash table implementation of set.
// Requires instantiation with guaranteed upper bound on number of elements
// that may be placed in set.  Throws overflow if bound is exceeded.
// Can be instantiated without second generic parameter if element type
// can be cast to int; otherwise requires hash function object.
//
class overflow { };         // exception
template<typename T, typename F = cast_to_int<T>>
class hashed_simple_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
    // You'll need some data members here.
    // I recommend you pick a hash table size p that is a prime
    // number >= n, use F(e) % p as your hash function, and rehash
    // with kF(e) % p after the kth collision.  (But make sure that
    // F(e) is never 0.)
  private:
    int size;
    T* data;
    bool* occupied;
    overflow err;
  public:
    hashed_simple_set(const int n) {    // constructor
        // replace this line:
        // (void) n;
        size = n;
        data = new T[size];
        occupied  =new bool[size];
    }
    virtual ~hashed_simple_set() {      // destructor
        delete [] data;
        delete [] occupied;
    }
    virtual hashed_simple_set<T, F>& operator+=(const T item) {
        // replace this line:
        // (void) item;  return *this;
        int idx = F()(item) % size;
        int origidx = idx;
        while (occupied[idx]) {
            if (data[idx] == item) return *this;
            idx++;
            if (idx == origidx) throw err;
            if (idx == size) idx = 0;
        }
        data[idx] = item;
        occupied[idx] = true;
        return *this;
    }
    virtual hashed_simple_set<T, F>& operator-=(const T item) {
        // replace this line:
        // (void) item;  return *this;
        int idx = F()(item) % size;
        int origidx = idx;
        while (occupied[idx]) {
            if (data[idx] == item) {
                occupied[idx] = false;
                return *this;
            }
            idx++;
            if (idx == origidx) return *this;
            if (idx == size) idx = 0;
        }
        return *this;
    }
    virtual bool contains(const T& item) const {
        // replace this line:
        // (void) item;  return false;
        int idx = F()(item) % size;
        int origidx = idx;
        while (data[idx] != item || !occupied[idx]) {
            idx++;
            if (idx == origidx) return false;
            if (idx == size) idx = 0;
        }
        return true;
    }
};

//---------------------------------------------------------------

// Sorted array implementation of set; supports binary search.
// Requires instantiation with guaranteed upper bound on number of
// elements that may be placed in set.  Throws overflow if bound is
// exceeded.
//
template<typename T, typename C = comp<T>>
class bin_search_simple_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
    // You'll need some data members here.
  private:
    int maxsize;
    int currsize;
    T* data;
    C cmp;
    overflow err;

    bool bin_search(int lower, int upper, const T& item) const {
        if (lower >= upper) return false;
        int idx = (upper - lower) / 2 + lower;
        if (cmp.equals(data[idx], item)) return true;
        if (cmp.precedes(item, data[idx])) {
            if (idx == upper) return false;
            return bin_search(lower, idx, item);
        }
        if (cmp.precedes(data[idx], item)) {
            if (idx == lower) return false;
            return bin_search(idx, upper, item);
        }
        return false;
    }
  public:
    // and some methods
    bin_search_simple_set(int n) {
        maxsize = n;
        currsize = 0;
        data = new T[maxsize];
    }
    virtual ~bin_search_simple_set() {
        delete [] data;
    }
    virtual bin_search_simple_set<T, C>& operator+=(const T item) {
        if (currsize == maxsize) throw err;
        if (currsize == 0 || cmp.precedes(data[currsize-1], item)) {
            data[currsize] = item;
            currsize++;
        } else {
            for (int i = 0; i < currsize; i++) {
                if (cmp.equals(data[i], item)) break;
                else if (cmp.precedes(item, data[i])) {
                    T thisdata = item;
                    for (int j = i; j < currsize+1; j++) {
                        T currdata = data[j];
                        data[j] = thisdata;
                        thisdata = currdata;
                    }
                    currsize++;
                    break;
                }
            }
        }
        return *this;
    }
    virtual bin_search_simple_set<T, C>& operator-=(const T item) {
        if (currsize == 0) return *this;
        for (int i = 0; i < currsize; i++) {
            if (cmp.equals(data[i], item)) {
                for (int j = i; j < currsize; j++) {
                    data[j] = data[j+1];
                }
                currsize--;
                break;
            }
        }
        return *this;
    }
    virtual bool contains(const T& item) const {
        return bin_search(0, currsize, item);
    }
};

//===============================================================
// RANGE SETS

// Function object for incrementing.
// Provides a default for any integral type.
//
template<typename T>
class increment {
    static_assert(std::is_integral<T>::value, "Integral type required.");
  public:
    T operator()(T a) const {
        return ++a;
    }
};

// Range type.  Uses comp<T> by default, but you can provide your
// own replacement if you want, e.g. for C strings.
//
static const struct { } empty_range;    // exception

template<typename T, typename C = comp<T>>
class range {
    T L;        // represents all elements from L
    bool Linc;  // inclusive?
    T H;        // through H
    bool Hinc;  // inclusive?
    C cmp;      // can't be static; needs explicit instantiation
  public:
    range(const T l = (T)NULL, const bool linc = false, const T h = (T)NULL, const bool hinc = false)
            : L(l), Linc(linc), H(h), Hinc(hinc), cmp() {
        // if (cmp.precedes(h, l)
        //     || (cmp.equals(l, h) && (!Linc || !Hinc))) throw empty_range;
    }
    // no destructor needed
    T low() const { return L; }
    bool closed_low() const { return Linc; }
    T high() const { return H; }
    bool closed_high() const {return Hinc; }
    bool contains(const T& item) const {
        return ((cmp.precedes(L, item) || (Linc && cmp.equals(L, item)))
            && (cmp.precedes(item, H) || (Hinc && cmp.equals(item, H))));
    }
    bool precedes(const range<T, C>& other) const {
        return (cmp.precedes(H, other.L) || ((cmp.equals(H, other.L) && !Hinc && !other.Linc)));
    }
    bool equals(const range<T, C>& other) const {
        return cmp.equals(L, other.L) && cmp.equals(H, other.H) && (Linc == other.Linc) && (Hinc == other.Hinc);
    }
    bool lessThan(const T& item) {
        return (cmp.precedes(H, item)||(cmp.equals(H, item) && !Hinc));
    }
    bool greaterThan(const T& item) {
        return (cmp.precedes(item, L) || (cmp.equals(L, item) && !Linc));
    }
    bool includes (const T& item) const {
        return ((cmp.precedes(L, item) && cmp.precedes(item, H)) || (cmp.equals(L, item) && Linc) || (cmp.equals(H, item) && Hinc));
    }
    // Check if there is overlap
    bool overlaps(const range<T, C>& other) const {
        if ((cmp.precedes(L, other.L) && cmp.precedes(other.L, H)) ||
           (cmp.precedes(L, other.H) && cmp.precedes(other.H, H)) ||
           (cmp.precedes(other.L, L) && cmp.precedes(H, other.H)) ||
           (cmp.equals(L, other.H) && Linc && other.Hinc) ||
           (cmp.equals(H, other.L) && Hinc && other.Linc) ||
           cmp.equals(L, other.L) ||
           cmp.equals(H, other.H)) {
            return true;
        } else {
            return false;
        }
    }
    // Merge if added range overlaps with existing range
    range merge(const range<T, C>& other) const {
        T nl; T nh; bool nlinc; bool nhinc;
        // Check L
        if (cmp.precedes(L, other.L)){
            nl = L;
            nlinc = Linc;
        } else if (cmp.precedes(other.L, L)) {
            nl = other.L;
            nlinc = other.Linc;
        } else {
            nl = L;
            nlinc = Linc || other.Linc;
        }
        // Check H
        if (cmp.precedes(other.H, H)){
            nh = H;
            nhinc = Hinc;
        } else if (cmp.precedes(H, other.H)) {
            nh = other.H;
            nhinc = other.Hinc;
        } else {
            nh = H;
            nhinc = Hinc || other.Hinc;
        }

        return range<T, C>(nl, nlinc, nh, nhinc);
    }
    // Split if deleted range overlaps with existing range
    range* split(const range<T, C>& other) const {
        range<T, C>* result = new range<T, C>[2];
        if ((cmp.precedes(L, other.L) && cmp.precedes(other.L, H)) || 
            (cmp.equals(L, other.L) && Linc) || 
            (cmp.equals(H, other.L) && Hinc)) {
            result[0] = range<T, C>(L, Linc, other.L, other.Linc);
        } else {
            result[0] = range<T, C>(1, false, -1, false);
        }
        if ((cmp.precedes(L, other.H) && cmp.precedes(other.H, H)) || 
            (cmp.equals(L, other.H) && Linc) || 
            (cmp.equals(H, other.H) && Hinc)) {
            result[1] = range<T, C>(other.H, other.Hinc, H, Hinc);
        } else {
            result[1] = range<T, C>(1, false, -1, false);
        }
        return result;
    }
    void print_range() {
        cout << L << ", " << Linc << ", " << H << ", " << Hinc << std::endl;
    }
};

// You may find it useful to define derived types with two-argument
// constructors that embody the four possible combinations of open and
// close-ended:
//
// template<typename T, typename C = comp<T>>
// class CCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class COrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OOrange : public range<T, C> { ...

// This is the abstract class from which all range-supporting sets are derived.
//
template<typename T, typename C = comp<T>>
class range_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
  public:
    virtual range_set<T, C>& operator+=(const range<T, C> r) = 0;
    virtual range_set<T, C>& operator-=(const range<T, C> r) = 0;
};

//---------------------------------------------------------------

// As implemented in the standard library, sets contain individual
// elements, not ranges.  (There are range insert and erase operators, but
// (a) they use iterators, (b) they take time proportional to the number of
// elements in the range, and (c) they require, for deletion, that the
// endpoints of the range actually be in the set.  An std_range_set, as
// defined here, avoids shortcomings (a) and (c), but not (b).  Your
// bin_search_range_set should avoid (b), though it will have slow insert
// and remove operations.  A tree_range_set (search tree -- extra credit)
// would have amortized log-time insert and remove for individual elements
// _and_ ranges.
//
template<typename T, typename C = comp<T>, typename I = increment<T>>
class std_range_set : public virtual range_set<T, C>,
                      public std_simple_set<T> {
    // 'virtual' on range_set ensures single copy if multiply inherited
    static_assert(std::is_integral<T>::value, "Integral type required.");
    I inc;
  public:
    // The first three methods below tell the compiler to use the
    // versions of the simple_set methods already found in std_simple_set
    // (given true multiple inheritance it can't be sure it should do that
    // unless we tell it).
    virtual std_simple_set<T>& operator+=(const T item) {
        return std_simple_set<T>::operator+=(item);
    }
    virtual std_simple_set<T>& operator-=(const T item) {
        return std_simple_set<T>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return std_simple_set<T>::contains(item);
    }
    virtual range_set<T>& operator+=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this += i;
        }
        return *this;
    }
    virtual range_set<T>& operator-=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this -= i;
        }
        return *this;
    }
};

//---------------------------------------------------------------

// insert an appropriate carray_range_set declaration here
template<typename T, typename C = comp<T>, typename I = increment<T>>
class carray_range_set : public virtual range_set<T, C>, 
                         public carray_simple_set<T> {
    I inc;
public:
    carray_range_set(const T l, const T h) : carray_simple_set<T>(l, h) { }
    virtual carray_simple_set<T>& operator+=(const T item) {
        return carray_simple_set<T>::operator+=(item);
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        return carray_simple_set<T>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return carray_simple_set<T>::contains(item);
    }
    virtual carray_range_set<T>& operator+=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this += i;
        }
        return *this;
    }
    virtual carray_range_set<T>& operator-=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this -= i;
        }
        return *this;
    }
};

//---------------------------------------------------------------

// insert an appropriate hashed_range_set declaration here
template<typename T, typename F = cast_to_int<T>, typename C = comp<T>, typename I = increment<T>>
class hashed_range_set : public virtual range_set<T, C>, 
                         public hashed_simple_set<T> {
    I inc;
public:
    hashed_range_set(const int n) : hashed_simple_set<T, F>(n) { }
    virtual hashed_simple_set<T>& operator+=(const T item) {
        return hashed_simple_set<T>::operator+=(item);
    }
    virtual hashed_simple_set<T>& operator-=(const T item) {
        return hashed_simple_set<T>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return hashed_simple_set<T>::contains(item);
    }
    virtual hashed_range_set<T>& operator+=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this += i;
        }
        return *this;
    }
    virtual hashed_range_set<T>& operator-=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this -= i;
        }
        return *this;
    }
};

//---------------------------------------------------------------

// insert an appropriate bin_search_range_set declaration here
template<typename T, typename C = comp<T>>
class bin_search_range_set : public virtual range_set<T, C>, 
                         public bin_search_simple_set<T, C> {
    C cmp;
    int maxsize;
    int currsize;
    range<T, C>* data;
    overflow err;
private:
    void print_ranges() {
        for(int i=0; i<currsize; i++) {
            data[i].print_range();
        }
    }
    int bin_search(int lower, int upper, const T& item) const {
        if (lower >= upper) return -1;
        int idx = (upper - lower) / 2 + lower;
        if (data[idx].includes(item)) {
            return idx;
        }
        else if (data[idx].greaterThan(item)) {
            if (idx == upper) return -1;
            return bin_search(lower, idx, item);
        }
        else if (data[idx].lessThan(item)) {
            if (idx == lower) return -1;
            return bin_search(idx, upper, item);
        }
        return -1;
    }
public:
    bin_search_range_set(const int n) : bin_search_simple_set<T, C>(n){
        maxsize = n;
        currsize = 0;
        data = new range<T, C>[maxsize];
    }
    virtual bin_search_simple_set<T, C>& operator+=(const T item) {
        return bin_search_simple_set<T, C>::operator+=(item);
    }
    virtual bin_search_simple_set<T, C>& operator-=(const T item) {
        return bin_search_simple_set<T, C>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return bin_search(0, currsize, item) != -1;
    }
    virtual bin_search_range_set<T, C>& operator+=(const range<T, C> r) {
        // for (T i = (r.closed_low() ? r.low() : inc(r.low()));
        //         r.contains(i); i = inc(i)) {
        //     *this += i;
        // }
        // return *this;
        if (currsize == maxsize) throw err;
        if (currsize == 0 || data[currsize-1].precedes(r)) {
            data[currsize] = r;
            currsize++;
            return *this;
        }
        // Check if precedes
        bool hasPrecede = false;
        int idxPrecede = 0;
        for (int i = 0; i < currsize; i++) {
            if (data[i].equals(r)) return *this;
            else if (r.precedes(data[i])) {
                hasPrecede = true;
                idxPrecede = i+1;
                break;
            }
        }
        // Check if overlaps
        range<T, C> copyr = r;
        bool hasOverlap = false;
        int idxOverlap = 0;
        for (int i = idxPrecede; i < currsize; i++) {
            if (data[i].overlaps(copyr)) {
                hasOverlap = true;
                copyr.merge(data[i]);
                data[i] = range<T, C>(1, false, -1, false);
            } else {
                if (hasOverlap) {
                    idxOverlap = i;
                    break;
                }
            }
        }
        // Insert
        int idxInsert = currsize;
        if (hasPrecede) idxInsert = idxPrecede;
        else if (hasOverlap) idxInsert = idxOverlap;
        range<T, C> thisrange = copyr;
        for (int j = idxInsert; j < currsize+1; j++) {
            range<T, C> currrange = data[j];
            data[j] = thisrange;
            thisrange = currrange;
        }
        currsize++;
        // Clean up
        range<T, C>* newdata = new range<T, C>[maxsize];
        int newsize = 0;
        for (int i = 0; i < currsize; i++) {
            if (data[i].low() < data[i].high()) {
                newdata[newsize] = data[i];
                newsize++;
            }
        }
        data = newdata;
        currsize = newsize;
        return *this;
    }
    virtual bin_search_range_set<T, C>& operator-=(const range<T, C> r) {
        // for (T i = (r.closed_low() ? r.low() : inc(r.low()));
        //         r.contains(i); i = inc(i)) {
        //     *this -= i;
        // }
        // return *this;
        range<T, C>* newdata = new range<T, C>[maxsize];
        int newsize = 0;
        for (int i = 0; i < currsize; i++) {
            if (data[i].overlaps(r)) {
                range<T, C>* splits = data[i].split(r);
                if (splits[0].low() < splits[0].high()) {
                    newdata[newsize] = splits[0];
                    newsize++;
                }
                if (splits[1].low() < splits[1].high()) {
                    newdata[newsize] = splits[1];
                    newsize++;
                }
            } else {
                newdata[newsize] = data[i];
                newsize++;
            }
        }
        data = newdata;
        currsize = newsize;
        return *this;
    }
};

//===============================================================

// comparator for C strings
//
class lexico_less {
  public:
    bool precedes(const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }
    bool equals(const char *a, const char *b) const {
        return strcmp(a, b) == 0;
    }
};

typedef enum{mon, tue, wed, thu, fri} weekday;