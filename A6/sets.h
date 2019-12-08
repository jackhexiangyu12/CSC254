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
        (void) a;  (void) b;  return true;
    }
    bool equals(const T& a, const T& b) const {
        // replace this line:
        (void) a;  (void) b;  return true;
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
  public:
    // fill in these methods:
    carray_simple_set(const T l, const T h) {   // constructor
        // replace this line:
        (void) l;  (void) h;
    }
    virtual ~carray_simple_set() {              // destructor
        // your code here
    }
    virtual carray_simple_set<T>& operator+=(const T item) {
        // replace this line:
        (void) item;  return *this;
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        // replace this line:
        (void) item;  return *this;
    }
    virtual bool contains(const T& item) const {
        // replace this line:
        (void) item;  return true;
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
  public:
    hashed_simple_set(const int n) {    // constructor
        // replace this line:
        (void) n;
    }
    virtual ~hashed_simple_set() { }    // destructor
    virtual hashed_simple_set<T, F>& operator+=(const T item) {
        // replace this line:
        (void) item;  return *this;
    }
    virtual hashed_simple_set<T, F>& operator-=(const T item) {
        // replace this line:
        (void) item;  return *this;
    }
    virtual bool contains(const T& item) const {
        // replace this line:
        (void) item;  return false;
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
  public:
    // and some methods
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
    range(const T l, const bool linc, const T h, const bool hinc)
            : L(l), Linc(linc), H(h), Hinc(hinc), cmp() {
        if (cmp.precedes(h, l)
            || (cmp.equals(l, h) && (!Linc || !Hinc))) throw empty_range;
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
    // You may also find it useful to define the following:
    // bool precedes(const range<T, C>& other) const { ...
    // bool overlaps(const range<T, C>& other) const { ...
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

//---------------------------------------------------------------

// insert an appropriate hashed_range_set declaration here

//---------------------------------------------------------------

// insert an appropriate bin_search_range_set declaration here

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