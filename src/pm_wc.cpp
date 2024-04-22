#include "pm_wc.hpp"
#include "ntt.hpp"
#include <iostream>
#include <cassert>

template <class T, class F>
vector<unsigned> vec_map(const vector<T> &v, F &&f)
{
    vector<unsigned> res(v.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        res[i] = f(v[i]);
    }
    return res;
}

template <class T>
ostream &operator<<(ostream &os, const vector<T> &v)
{
    os << "[";
    for (const auto &x : v)
    {
        os << x << ", ";
    }
    os << "]";
    return os;
}

vector<bool> pm_wc(const vector<int> &pat, const vector<int> &text, const unordered_set<int> &wc)
{
    int n = text.size();
    int m = pat.size();
    // Flip t before FFT
    vector<int> t_rev(text.rbegin(), text.rend());
    auto wc_zero = [&](int c)
    { return wc.contains(c) ? 0 : c; };
    vector<unsigned> p = vec_map(pat, wc_zero);
    vector<unsigned> t = vec_map(t_rev, wc_zero);

    auto p3 = vec_map(p, [](int i)
                      { return i * i * i; });
    auto p2 = vec_map(p, [](int i)
                      { return i * i; });
    auto t3 = vec_map(t, [](int i)
                      { return i * i * i; });
    auto t2 = vec_map(t, [](int i)
                      { return i * i; });
    auto conv1 = conv(p3, t);
    auto conv2 = conv(p2, t2);
    auto conv3 = conv(p, t3);

    for (int i = 0; i < n; i++)
    {
        conv1[i] += -2 * conv2[i] + conv3[i];
    }

    vector<bool> res(n, false);
    for (int j = m - 1; j < n; j++)
    {
        res[n - j - 1] = conv1[j] == 0;
    }

    return res;
}

vector<bool> pm_wc_naive(vector<int> &p, vector<int> &t, unordered_set<int> &wc)
{
    int m = p.size();
    int n = t.size();

    auto matches = [&](int a, int b) -> bool
    {
        return a == b || wc.contains(a) || wc.contains(b);
    };

    auto matches_substr = [&](int i) -> bool
    {
        for (int j = 0; j < m; j++)
        {
            if (i + j >= n)
                return false;
            if (!matches(p[j], t[i + j]))
                return false;
        }
        return true;
    };

    vector<bool> res(n, false);
    for (int i = 0; i < n; i++)
    {
        res[i] = matches_substr(i);
    }

    return res;
}

vector<bool> pm_wc_jump(
    int p_start, int m,
    vector<int> &t, unordered_set<int> &wc,
    vector<int> &next_tr)
{
    int n = t.size();

    auto matches = [&](int a, int b) -> bool
    {
        return a == b || wc.contains(a) || wc.contains(b);
    };

    auto matches_substr = [&](int i) -> bool
    {
        for (int j = 0; j < m; j++)
        {
            if (i + j >= n)
                return false;

            if (wc.contains(t[p_start + j]))
                j += next_tr[p_start + j];

            if (i + j >= n)
                return false;

            if (wc.contains(t[i + j]))
                j += next_tr[i + j];

            if (!matches(t[p_start + j], t[i + j]))
                return false;
        }
        return true;
    };

    vector<bool> res(n, false);
    for (int i = 0; i < n; i++)
    {
        res[i] = matches_substr(i);
    }

    return res;
}

void check_pm_wc(vector<int> &p, vector<int> &t)
{
    unordered_set<int> wc = {'#'};
    auto res_exact = pm_wc_naive(p, t, wc);
    auto res_fft = pm_wc(p, t, wc);
    assert(res_exact == res_fft);
}

template <class RNG>
vector<int> random_str(size_t len, RNG &rng)
{
    vector<int> res;
    res.reserve(len);
    std::uniform_int_distribution<> distrib(1, 255);
    for (size_t i = 0; i < len; i++)
    {
        int c = distrib(rng);
        c = (c > 127) ? 35 : c;
        res.push_back(c);
    }
    return res;
}

void test_pm_wc(int it, mt19937 &rng)
{
    for (int i = 0; i < it; i++)
    {
        vector<int> p = random_str(10, rng);
        vector<int> t = random_str(100, rng);
        check_pm_wc(p, t);
    }
}

vector<int> get_next_tr(const vector<int> &t, unordered_set<int> &wc)
{
    int n = t.size();
    vector<int> res(n, 0);
    res[n - 1] = 0;
    for (int i = n - 2; i > 0; i--)
    {
        if (wc.contains(t[i - 1]) && !wc.contains(t[i]))
            res[i] = 0;
        else
            res[i] = res[i + 1] - 1;
    }

    if (n > 0)
        res[0] = res[1] + 1;

    return res;
}

void check_pm_wc_jump(int p_start, int l, vector<int> &t)
{
    unordered_set<int> wc = {'#'};
    vector<int> p(t.begin() + p_start, t.begin() + p_start + l);
    auto next_tr = get_next_tr(t, wc);
    auto res_exact = pm_wc_naive(p, t, wc);
    auto res_fft = pm_wc_jump(p_start, l, t, wc, next_tr);
    assert(res_exact == res_fft);
}

void test_pm_wc_jump(int it, mt19937 &rng)
{
    for (int i = 0; i < it; i++)
    {
        vector<int> t = random_str(100, rng);
        for (int p_start = 0; p_start < 90; p_start++)
        {
            check_pm_wc_jump(p_start, 10, t);
        }
    }
}
