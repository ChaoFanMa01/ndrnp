#ifndef NDRNP_MISCELLANEOUS_H
#define NDRNP_MISCELLANEOUS_H

#include <linux/types.h>      // ssize_t
#include <utility>            // move()
#include <functional>         // less(), less_equal(), ...
#include <random>

namespace ndrnp {
    // function predeclarations.
    template <class Iter, typename Cmp>
    void insertion_sort(const Iter&, const Iter&, Cmp);

    template <class Iter>
    void insertion_sort_ascending(const Iter&, const Iter&);

    template <class Iter>
    void insertion_sort_descending(const Iter&, const Iter&);

    template <class Iter, typename Cmp>
    ssize_t index_of_min_or_max(const Iter&, const Iter&, Cmp);

    template <class Iter, typename Cmp>
    Iter min_or_max(const Iter&, const Iter&, Cmp);

    template <class Iter>
    ssize_t index_of_max(const Iter&, const Iter&);

    template <class Iter>
    ssize_t index_of_min(const Iter&, const Iter&);

    template <class Iter>
    Iter min(const Iter&, const Iter&);

    template <class Iter>
    Iter max(const Iter&, const Iter&);

    template <class Iter, class O>
    ssize_t index_of(const Iter&, const Iter&, const O&);
    // generate a random integer ranging from b to e using
    // the default random engine.
    int rand_range(std::default_random_engine&, 
                   const int&, const int&);
    double rand_range(std::default_random_engine&,
                      const double&, const double&);

    // function definitions.
    template <class Iter, typename Cmp>
    void insertion_sort(const Iter& b, const Iter& e, Cmp cmp) {
        if (b == e)
            return;
        for (Iter itr = b + 1; itr != e; ++itr) {
            Iter p;
            auto temp = *itr;
            for (p = itr; p != b && cmp(temp, *(p - 1)); --p)
                *p = *(p - 1);
            *p = temp;
        }
    }

    template <class Iter>
    void insertion_sort_ascending(const Iter& b, const Iter& e) {
        insertion_sort(b, e, std::less<decltype(*b)>());
    }

    template <class Iter>
    void insertion_sort_descending(const Iter& b, const Iter& e) {
        insertion_sort(b, e, std::greater<decltype(*b)>());
    }

    template <class Iter, typename Cmp>
    ssize_t index_of_min_or_max(const Iter& b, const Iter& e, Cmp cmp) {
        if (b == e)
            return -1;
        auto temp = *b;
        ssize_t m = 0, cnt = 1;
        for (Iter iter = b + 1; iter != e; ++iter, ++cnt)
            if (cmp(*iter, temp)) {
                m = cnt;
                temp = *iter;
            }
        return m;
    }

    template <class Iter, typename Cmp>
    Iter min_or_max(const Iter& b, const Iter& e, Cmp cmp) {
        if (b == e)
            return Iter(nullptr);
        auto temp = *b;
        Iter m = b;
        for (Iter iter = b + 1; iter != e; ++iter)
            if (cmp(*iter, temp)) {
                m = iter;
                temp = *iter;
            }
        return m;
    }

    template <class Iter>
    ssize_t index_of_min(const Iter& b, const Iter& e) {
        return index_of_min_or_max(b, e, std::less<decltype(*b)>());
    }

    template <class Iter>
    ssize_t index_of_max(const Iter& b, const Iter& e) {
        return index_of_min_or_max(b, e, std::greater<decltype(*b)>());
    }

    template <class Iter>
    Iter min(const Iter& b, const Iter& e) {
        return min_or_max(b, e, std::less<decltype(*b)>());
    }

    template <class Iter>
    Iter max(const Iter& b, const Iter& e) {
        return min_or_max(b, e, std::greater<decltype(*b)>());
    }

    template <class Iter, class O>
    ssize_t index_of(const Iter& b, const Iter& e, const O& val) {
        if (b == e)
            return -1;
        ssize_t cnt = 0;
        for (Iter iter = b; iter != e; ++iter, ++cnt)
            if (*iter == val)
                return cnt;
        return -1;
    }

    int
    rand_range(std::default_random_engine& en, 
               const int& b, const int& e) {
        std::uniform_int_distribution<int>   dis(b, e);
        return dis(en);
    }

    double 
    rand_range(std::default_random_engine& en,
               const double& b, const double& e) {
        std::uniform_real_distribution<double> dis(b, e);
        return dis(en);
    }
}

#endif
