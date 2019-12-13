#include "sets.h"

// int main() {

//     // Some miscellaneous code to get you started on testing your sets.
//     // The following should work:

//     std_simple_set<int> R;
//     R += 3;
//     cout << "3 is " << (R.contains(3) ? "" : "not ") << "in R\n";
//     cout << "5 is " << (R.contains(5) ? "" : "not ") << "in R\n";

//     simple_set<double>* S = new std_simple_set<double>();
//     *S += 3.14;
//     cout << "pi is " << (S->contains(3.14) ? "" : "not ") << "in S\n";
//     cout << "e is " << (S->contains(2.718) ? "" : "not ") << "in S\n";

//     std_simple_set<string> U;
//     U += "hello";
//     cout << "\"hello\" is " << (U.contains("hello") ? "" : "not ") << "in U\n";
//     cout << "\"foo\" is " << (U.contains("foo") ? "" : "not ") << "in U\n";

//     range<string> r1("a", true, "f", true);
//     cout << "\"b\" is " << (r1.contains("b") ? "" : "not ") << "in r1\n";
//     cout << "\"aaa\" is " << (r1.contains("aaa") ? "" : "not ") << "in r1\n";
//     cout << "\"faa\" is " << (r1.contains("faa") ? "" : "not ") << "in r1\n";

//     range<const char*, lexico_less> r2("a", true, "f", true);
//     cout << "\"b\" is " << (r2.contains("b") ? "" : "not ") << "in r2\n";
//     cout << "\"aaa\" is " << (r2.contains("aaa") ? "" : "not ") << "in r2\n";
//     cout << "\"faa\" is " << (r2.contains("faa") ? "" : "not ") << "in r2\n";

//     // The following will not work correctly yet:

//     range_set<int>* X = new std_range_set<int>();
//     *X += range<int>(5, true, 8, false);
//     if (X->contains(4)) cout << "4 is in X\n";
//     if (X->contains(5)) cout << "5 is in X\n";      // should print
//     if (X->contains(6)) cout << "6 is in X\n";      // should print
//     if (X->contains(7)) cout << "7 is in X\n";      // should print
//     if (X->contains(8)) cout << "8 is in X\n";
//     if (X->contains(9)) cout << "9 is in X\n";
//     *X -= range<int>(6, true, 10, false);
//     if (X->contains(4)) cout << "4 is now in X\n";
//     if (X->contains(5)) cout << "5 is now in X\n";      // should print
//     if (X->contains(6)) cout << "6 is now in X\n";
//     if (X->contains(7)) cout << "7 is now in X\n";
//     if (X->contains(8)) cout << "8 is now in X\n";
//     if (X->contains(9)) cout << "9 is now in X\n";

//     simple_set<weekday>* V = new carray_simple_set<weekday>(mon, (weekday)5);

//     hashed_simple_set<int, cast_to_int<int>> H(100);

//     bin_search_simple_set<double> J(100);

//     simple_set<weekday>* set3 = new carray_simple_set<weekday>(mon, (weekday)5);

//     simple_set<int>* set = new hashed_simple_set<int, cast_to_int<int>>(100);

// //    bin_search_simple_set<double> J(100);

//     simple_set<int>* set2 = new carray_simple_set<int>(1,5);
//     *set += (weekday) 1;
//     *set += (weekday) 2;
//     *set -= (weekday) 2;
//     if(set->contains((weekday)1)) cout << "mon is in set\n";
//     if(set->contains((weekday)2)) cout << "tue is in set\n";
//     if(set->contains((weekday)3)) cout << "wed is in set\n";
// }

#include <assert.h>

/**
 * The following is provided ONLY to make sure your implementations compiles
 * The code added by the TAs is not intended to be examples of good c++ code.
 *
 * Assert statemets terminate the program if the experesion is not true (non-zero).
 * All asserts written here should pass. When actually testing code for correctness,
 * we will not be using asserts (so early failing tests won't cause later tests to not run)
 *
 */

void carray_simple_set_tests() {
    carray_simple_set<int> i(0, 100);
    for (int x = 0; x < 100; x+=3)
      i += x;
    i -= 6;
    i += 7;
    assert(i.contains(3));
    assert(i.contains(7));
    assert(!i.contains(6));
}

void carray_range_set_tests() {
    carray_range_set<int> s(0, 100);
    s += range<int>(4, true, 9, false);
    s += range<int>(11, true, 15, true);
    s -= range<int>(15, true, 15, true);
    assert(!s.contains(15));
    assert(s.contains(4));
    assert(!s.contains(98));
    range_set<char> * d = new carray_range_set<char>('a', 'z');
    *d += range<char>('a', true, 'd', false);
    *d += range<char>('g', true, 'i', false);
    *d -= range<char>('b', true, 'c', false);
    assert(!d->contains('y'));
    assert(d->contains('a'));
    assert(!d->contains('d'));
}

void hash_simple_tests() {
    hashed_simple_set<int> s(100);
    s += 4; s+= 15; s+= 3;
    assert(s.contains(15));
    assert(s.contains(4));
    assert(!s.contains(98));
    hashed_simple_set<char> d(30);
    d += 'd';
    d += 'z';
    d -= 'd';
    assert(d.contains('z'));
    assert(!d.contains('d'));
    assert(!d.contains('a'));
}

void hash_range_tests() {
    hashed_range_set<int> s(100);
    s += range<int>(4, true, 9, false);
    s += range<int>(11, true, 15, true);
    s -= range<int>(15, true, 15, true);
    assert(!s.contains(15));
    assert(s.contains(4));
    assert(!s.contains(98));
    range_set<char> * d = new hashed_range_set<char>(30);
    *d += range<char>('a', true, 'd', false);
    *d += range<char>('g', true, 'i', false);
    *d -= range<char>('b', true, 'c', false);
    assert(!d->contains('z'));
    assert(d->contains('a'));
    assert(!d->contains('d'));
}

class double_cmp {
  public:
  bool precedes(double a, double b) const {
    return a < b;
  }
  bool equals(double a, double b) const {
    return a == b;
  }
};

void bin_search_simple_set_tests() {
  bin_search_simple_set<double, double_cmp> s(10);
  s += 3.7;
  s += 8.6;
  assert(s.contains(3.7));
  assert(!s.contains(4.2));
  s -= 3.7;
  assert(!s.contains(3.7));
}

void bin_search_range_set_tests() {
  bin_search_range_set<double, double_cmp> s(10);
  // cout << "okhere\n";
  s += range<double, double_cmp>(2.0, true, 4.0, true);
  s += range<double, double_cmp>(6.0, true, 8.0, true);
  s += range<double, double_cmp>(10.0, true, 11.0, true);
  assert(!s.contains(5.0));
  assert(s.contains(3.7));
  s += range<double, double_cmp>(1.0, false, 10.7, false);
  assert(s.contains(5.0));
  s -= range<double, double_cmp>(3, false, 6.6, false);
  assert(!s.contains(5.0));
  assert(s.contains(2.0));
}

int main() {
  carray_simple_set_tests();
  carray_range_set_tests();
  hash_simple_tests();
  hash_range_tests();
  bin_search_simple_set_tests();
  bin_search_range_set_tests();
}