#include "set.hpp"
#include <assert.h>

class double_cmp {
    public:
    bool precedes(double a, double b) const {
        return a < b;
    }
    bool equals(double a, double b) const {
        return a == b;
    }
};

int main() {

    // Some miscellaneous code to get you started on testing your sets.
    // The following should work:

    // std_simple_set<int> R;
    // R += 3;
    // cout << "3 is " << (R.contains(3) ? "" : "not ") << "in R\n";
    // cout << "5 is " << (R.contains(5) ? "" : "not ") << "in R\n";

    // simple_set<double>* S = new std_simple_set<double>();
    // *S += 3.14;
    // cout << "pi is " << (S->contains(3.14) ? "" : "not ") << "in S\n";
    // cout << "e is " << (S->contains(2.718) ? "" : "not ") << "in S\n";

    // std_simple_set<string> U;
    // U += "hello";
    // cout << "\"hello\" is " << (U.contains("hello") ? "" : "not ") << "in U\n";
    // cout << "\"foo\" is " << (U.contains("foo") ? "" : "not ") << "in U\n";

    // range<string> r1("a", true, "f", true);
    // cout << "\"b\" is " << (r1.contains("b") ? "" : "not ") << "in r1\n";
    // cout << "\"aaa\" is " << (r1.contains("aaa") ? "" : "not ") << "in r1\n";
    // cout << "\"faa\" is " << (r1.contains("faa") ? "" : "not ") << "in r1\n";

    // range<const char*, lexico_less> r2("a", true, "f", true);
    // cout << "\"b\" is " << (r2.contains("b") ? "" : "not ") << "in r2\n";
    // cout << "\"aaa\" is " << (r2.contains("aaa") ? "" : "not ") << "in r2\n";
    // cout << "\"faa\" is " << (r2.contains("faa") ? "" : "not ") << "in r2\n";

    // // The following will not work correctly yet:

    // range_set<int>* X = new std_range_set<int>();
    // *X += range<int>(5, true, 8, false);
    // if (X->contains(4)) cout << "4 is in X\n";
    // if (X->contains(5)) cout << "5 is in X\n";      // should print
    // if (X->contains(6)) cout << "6 is in X\n";      // should print
    // if (X->contains(7)) cout << "7 is in X\n";      // should print
    // if (X->contains(8)) cout << "8 is in X\n";
    // if (X->contains(9)) cout << "9 is in X\n";
    // *X -= range<int>(6, true, 10, false);
    // if (X->contains(4)) cout << "4 is now in X\n";
    // if (X->contains(5)) cout << "5 is now in X\n";      // should print
    // if (X->contains(6)) cout << "6 is now in X\n";
    // if (X->contains(7)) cout << "7 is now in X\n";
    // if (X->contains(8)) cout << "8 is now in X\n";
    // if (X->contains(9)) cout << "9 is now in X\n";

    simple_set<weekday>* V = new carray_simple_set<weekday>(mon, (weekday)5);

    hashed_simple_set<int, cast_to_int<int>> H(100);

    // bin_search_simple_set<double> J(100);

    carray_simple_set<int> s1(0, 100);
    s1 += 50;
    s1 += 15;
    s1 -= 50;
    assert(s1.contains(15));
    assert(!s1.contains(50));
    assert(!s1.contains(51));

    hashed_simple_set<int> s2(100);
    s2 += 50;
    s2 += 15;
    s2 -= 50;
    assert(s2.contains(15));
    assert(!s2.contains(50));
    assert(!s2.contains(51));
    
    hashed_simple_set<char> s3(10);
    s3 += 'a';
    s3 += 'z';
    s3 -= 'a';
    assert(s3.contains('z'));
    assert(!s3.contains('a'));
    assert(!s3.contains('g'));

    bin_search_simple_set<double, double_cmp> s4(10);
    s4 += 3.14;
    s4 += 5.3;
    s4 -= 3.14;
    assert(s4.contains(5.3));
    assert(!s4.contains(1.5));
    assert(!s4.contains(3.14));

    range_set<char>* s5 = new carray_range_set<char>('a', 'z');
    *s5 += range<char>('m', true, 'p', false);
    *s5 += range<char>('a', true, 'f', false);
    *s5 -= range<char>('b', true, 'c', false);
    assert(!s5->contains('y'));
    assert(s5->contains('a'));
    assert(!s5->contains('b'));

    range_set<char>* s6 = new hashed_range_set<char>(30);
    *s6 += range<char>('a', true, 'f', false);
    *s6 += range<char>('m', true, 'p', false);
    *s6 -= range<char>('b', true, 'c', false);
    assert(!s6->contains('z'));
    assert(s6->contains('a'));
    assert(!s6->contains('b'));

    bin_search_range_set<double, double_cmp> s7(10);
    s7 += range<double, double_cmp>(1.0, true, 3.0, true);
    s7 += range<double, double_cmp>(7.0, true, 9.0, false);
    s7 += range<double, double_cmp>(11.0, true, 13.0, true);
    assert(!s7.contains(5.0));
    assert(s7.contains(7.7));
    assert(!s7.contains(9.0));
    s7 -= range<double, double_cmp>(3, false, 8.8, false);
    assert(!s7.contains(5.0));
    assert(s7.contains(2.0));
}