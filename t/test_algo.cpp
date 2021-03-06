#include "t.hpp"
#include "ttl/utility.hpp"
#include "ttl/functional.hpp"
#include "ttl/algorithm.hpp"

struct less_than
{
   int v;
   less_than(int v_): v(v_) {}
   bool operator()(int o) const { return o < v; }
};

struct equal_to
{
   int v;
   equal_to(int v_): v(v_) {}
   bool operator()(int o) const { return o == v; }
};

static void print_ints(const char *title, const int *first, const int *last)
{
   fputs(title, stdout);
   while (first != last)
      printf(" %d", *first++);
   fputs(".\n", stdout);
}

struct printer
{
   void operator()(const int &v) { printf(" %d", v); }
};

struct counter
{
   int value;
   counter(int start): value(start) {}
   int operator()() { return value++; }
};

void test()
{
   assert(ttl::min(1,2) == 1);
   assert(ttl::max(1,2) == 2);
   assert(ttl::minmax(1,2) == ttl::minmax(2,1));
   assert(ttl::minmax(2,1).first == 1);
   assert(ttl::minmax(2,1).second == 2);

   int *p;

   int a0[] = {1,2,3,4};
   int a1[countof(a0)] = {0};
   assert(ttl::equal(a1, a1 + countof(a1), a0, ttl::less<int>()));
   assert(ttl::mismatch(a1, a1 + countof(a1), a0) == ttl::make_pair(a1, a0));
   assert(ttl::mismatch(a1, a1 + countof(a1), a0, a0 + countof(a0)) == ttl::make_pair(a1, a0));

   assert(ttl::copy_n(a0, countof(a0) / 2, a1) == a1 + countof(a0) / 2);
   assert(ttl::mismatch(a1, a1 + countof(a1), a0) == ttl::make_pair(a1 + countof(a0)/2, a0 + countof(a0)/2));
   assert(ttl::mismatch(a1, a1 + countof(a1), a0, a0 + countof(a0)) == ttl::make_pair(a1 + countof(a0)/2, a0 + countof(a0)/2));

   assert(ttl::transform(a0, a0 + countof(a0), a1, less_than(3)) == a1 + countof(a0));
   print_ints("transform(unary) :", a1, a1 + countof(a1));
   assert(a1[0] == 1 && a1[1] == 1 && a1[2] == 0 && a1[3] == 0);
   assert(ttl::fill_n(a1, countof(a1), 0) == a1 + countof(a1));
   a1[2] = a1[3] = 10;
   assert(ttl::transform(a0, a0 + countof(a0), a1, a1, ttl::less<int>()) == a1 + countof(a0));
   print_ints("transform(binary):", a1, a1 + countof(a1));
   assert(a1[0] == 0 && a1[1] == 0 && a1[2] == 1 && a1[3] == 1);

   assert(ttl::copy(a0, a0 + countof(a0), a1) == a1 + countof(a0));
   assert(ttl::equal(a0, a0 + countof(a0), a1));
   assert(ttl::equal(a1, a1 + countof(a1), a0, a0 + countof(a0), ttl::equal_to<int>()));
   assert(ttl::mismatch(a1, a1 + countof(a1), a0) == ttl::make_pair(a1 + countof(a1), a0 + countof(a0)));
   assert(ttl::mismatch(a1, a1 + countof(a1), a0, a0 + countof(a0)) == ttl::make_pair(a1 + countof(a1), a0 + countof(a0)));
   print_ints("copy:", a1, a1 + countof(a1));

   ttl::fill_n(a1, countof(a1), 0);
   ttl::copy_n(a0, 2, a1);
   assert(a0[0] == a1[0] && a1[2] == 0);
   print_ints("copy_n:", a1, a1 + countof(a1));

   ttl::fill_n(a1, countof(a1), 0);
   assert(ttl::copy_if(a0, a0 + countof(a0), a1, less_than(3)) == a1 + 2);
   assert(a0[0] == a1[0] && a1[2] == 0);
   print_ints("copy_if:", a1, a1 + countof(a1));

   ttl::fill_n(a1, countof(a1), 0);
   assert(ttl::copy_backward(a0 + 2, a0 + countof(a0), a1 + countof(a1)) == a1 + 2);
   assert(a0[2] == a1[2] && a1[0] == 0);
   print_ints("copy_backward:", a1, a1 + countof(a1));

   p = ttl::remove_copy(a0, a0 + countof(a0), a1, 3);
   print_ints("remove_copy(3):", a1, p);
   assert(ttl::find(a1, p, 3) == p);

   p = ttl::remove_copy_if(a0, a0 + countof(a0), a1, less_than(3));
   print_ints("remove_copy_if(< 3):", a1, p);
   assert(ttl::find(a1, p, 1) == p);
   assert(ttl::find(a1, p, 2) == p);
   assert(ttl::find(a1, p, 3) == a1);

   ttl::generate(a1, a1 + countof(a1), counter(1));
   assert(a1[0] == 1 && a1[1] == 2 && a1[2] == 3 && a1[3] == 4);
   assert(ttl::generate_n(a1, 2, counter(2)) == a1 + 2);
   assert(a1[0] == 2 && a1[1] == 3 && a1[2] == 3 && a1[3] == 4);

   int a2[] = {1,2,2,3,4,6};
   int *a2end = a2 + countof(a2);
   print_ints("a2:", a2, a2end);
   printf("a2 min: %d\n", *ttl::min_element(a2, a2end));
   assert(ttl::min_element(a2, a2end) == a2);
   printf("a2 max: %d\n", *ttl::max_element(a2, a2end));
   assert(ttl::max_element(a2, a2end) == a2end - 1);
   assert(ttl::minmax_element(a2, a2end).first == ttl::min_element(a2, a2end));
   assert(ttl::minmax_element(a2, a2end).second == ttl::max_element(a2, a2end));

   assert(ttl::count(a2, a2end, 2) == 2);
   assert(ttl::count_if(a2, a2end, less_than(2)) == 1);
   assert(ttl::any_of(a2, a2end, equal_to(4)));
   assert(ttl::none_of(a2, a2end, equal_to(5)));
   assert(*ttl::adjacent_find(a2, a2end) == 2);
   assert(*ttl::adjacent_find(a2, a2end, ttl::equal_to<int>()) == 2);

   assert(ttl::search(a2, a2end, a2, a2 + 1) == a2);
   assert(ttl::search(a2, a2end, a2 + 1, a2 + 3) == a2 + 1);
   assert(ttl::search(a2, a2end, a0, a0 + 2) == a2);
   assert(ttl::search(a2, a2end, a0, a0 + 3) == a2end);

   assert(ttl::search_n(a2, a2end, 1, 2) == a2 + 1);
   assert(ttl::search_n(a2, a2end, 2, 2) == a2 + 1);
   assert(ttl::search_n(a2, a2end, 2, 2, ttl::equal_to<int>()) == a2 + 1);
   assert(ttl::search_n(a2, a2end, 3, 2) == a2end);
   assert(*ttl::search_n(a2, a2end, 2, 2) == 2);
   assert(ttl::search_n(a2, a2end, 1, 1) == a2);
   assert(ttl::search_n(a2, a2end, 2, 1) == a2end);
   assert(ttl::search_n(a2, a2end, 0, 1) == a2);
   assert(ttl::search_n(a2, a2end, 1, 6) == a2end - 1);
   assert(ttl::search_n(a2, a2end, a2end - a2 + 1, 2) == a2end);
   assert(ttl::search_n(a2, a2end, -1, 2) == a2);
   assert(ttl::search_n(a2, a2end, INT_MAX, 2) == a2end);

   fputs("for_each", stdout);
#if __cplusplus >= 201103L // C++11
   fputs("(lambda):", stdout);
   ttl::for_each(a2, a2end, [](const int &v){ printf(" %d", v); } )(-1);
#else
   fputs("(class inst):", stdout);
   ttl::for_each(a2, a2end, printer())(-1);
#endif
   fputs("\n", stdout);

   assert(ttl::is_sorted(a2, a2end) == true);
   assert(ttl::is_sorted_until(a2, a2end) == a2end);
   {
      int merged[countof(a2) + countof(a0)];
      ttl::merge(a0, a0 + countof(a0), a2, a2 + countof(a2), merged);
      print_ints("merged:", merged, merged + countof(merged));
      assert(ttl::is_sorted(merged, merged + countof(merged)) == true);
   }

   p = ttl::lower_bound(a2, a2end, 2);
   printf("a2 dup lower_bound: [%ld\n", (long)(p - a2));
   assert(p - a2 == 1 && *p == 2);
   p = ttl::lower_bound(a2, a2end, 5); // missing, in-range
   assert(p == a2 + 5);
   p = ttl::lower_bound(a2, a2end, 7); // missing, out-of-range
   assert(p == a2end);

   p = ttl::upper_bound(a2, a2end, 2);
   printf("a2 dup upper_bound: %ld)\n", (long)(p - a2));
   assert(p - a2 == 3);
   p = ttl::upper_bound(a2, a2end, 5); // missing, in-range
   assert(p == a2 + 5);
   p = ttl::upper_bound(a2, a2end, 7); // missing, out-of-range;
   assert(p == a2end);

   ttl::pair<int *, int *> r = ttl::equal_range(a2, a2end, 2);
   printf("a2 dup equal_range: [%ld,%ld)\n", (long)(r.first - a2), (long)(r.second - a2));
   assert(ttl::all_of(r.first, r.second, equal_to(2)));
   assert(r.second - r.first == 2);
   assert(*r.first == 2);
   assert(*(r.first + 1) == 2);

   r = ttl::equal_range(a2, a2end, 5);
   assert(r.first == r.second);
   assert(r.first == a2 + 5);

   r = ttl::equal_range(a2, a2end, 7);
   assert(r.first == r.second);
   assert(r.first == a2end);

   p = ttl::remove(a2, a2end, 2);
   print_ints("a2 remove 2:", a2, p);
   assert(ttl::find(a2, p, 2) == p);
   assert(ttl::find(a2, p, 6) + 1 == p);
   assert(a2[1] == 3);
   assert(*ttl::find(a2, p, 6) == 6);
   a2end = p;

   p = ttl::remove_if(a2, a2end, less_than(2));
   print_ints("a2 remove less than 2:", a2, p);
   assert(ttl::find(a2, p, 2) == p);
   assert(ttl::find(a2, p, 6) + 1 == p);
   assert(a2[0] == 3);
   assert(*ttl::find(a2, p, 6) == 6);
   a2end = p;

   {
      printf("reverse\n");
      int numbers[] = {1,2,3,4,5,6,7};
      int tmp[countof(numbers)];
      ttl::reverse_copy(numbers, numbers + countof(numbers), tmp);
      ttl::reverse(tmp, tmp + countof(tmp));
      assert(ttl::equal(tmp, tmp + countof(tmp), numbers, numbers + countof(numbers)));

      // "three reverses" array rotation
      int fourth = tmp[4];
      ttl::reverse(tmp, tmp + 4);
      ttl::reverse(tmp + 4, tmp + countof(tmp));
      ttl::reverse(tmp, tmp + countof(tmp));
      print_ints("rotate array left by 4:", tmp, tmp + countof(tmp));
      assert(tmp[0] == fourth);
      ttl::reverse(tmp + 3, tmp + countof(tmp));
      ttl::reverse(tmp, tmp + 3);
      ttl::reverse(tmp, tmp + countof(tmp));
      print_ints("rotate array right by 4:", tmp, tmp + countof(tmp));
      assert(tmp[4] == fourth);
   }
}
