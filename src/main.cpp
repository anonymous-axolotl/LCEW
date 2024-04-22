#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <stdlib.h>

#include "pm_wc.hpp"
#include "lcew.hpp"
#include "fast_mm.hpp"

using namespace std;

/**
 * Compute the LCEW at indices `i` and `j` using a linear scan algorithm,
 * using '#' as wildcard.
 */
int naive_lcew_sharp(const vector<int> &t, size_t i, size_t j)
{
    auto matches = [&](int a, int b) -> bool
    {
        return a == b || a == DEFAULT_WILDCARD || b == DEFAULT_WILDCARD;
    };

    size_t r = 0;
    while (i + r < t.size() && j + r < t.size() && matches(t[i + r], t[j + r]))
        ++r;

    return r;
}

/**
 * Check that the LCEW data structure is correct on `txt`.
 */
void test_lcew(vector<int> &txt, int t)
{
    Lcew ds(txt, t);
    int n = txt.size();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int a = ds.lcew(i, j);
            int b = naive_lcew_sharp(txt, i, j);
            assert(a == b);
        }
    }
}

/**
 * Check that the LCEW data structure is correct on the string `s`.
 */
void test_lcew_str(string &s, int t)
{
    vector<int> txt = vector<int>(s.begin(), s.end());
    test_lcew(txt, t);
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

template <class RNG>
void random_test(size_t nb_str, size_t str_len, size_t it, RNG &rng)
{
    for (size_t it_s = 0; it_s < nb_str; it_s++)
    {
        vector<int> txt = random_str(str_len, rng);
        std::uniform_int_distribution<> distrib(0, str_len - 1);
        int t = distrib(rng) + 1;
        Lcew ds(txt, t);
        for (size_t it_q = 0; it_q < it; it_q++)
        {
            int i = distrib(rng);
            int j = distrib(rng);
            int a = ds.lcew(i, j);
            int b = naive_lcew_sharp(txt, i, j);
            assert(a == b);
        }
    }
}

void simple_mm()
{
    vector<vector<bool>> id_dense = {{true, false}, {false, true}};
    SparseBoolMatrix id = SparseBoolMatrix::from_dense(id_dense);

    auto res = matrix_mult(id, id);

    assert(id == res);
}

template <class RNG>
vector<vector<bool>> random_bool_mat(int n, double density, RNG &rng)
{
    bernoulli_distribution b(density);
    vector<vector<bool>> res(n, vector<bool>(n, false));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            res[i][j] = b(rng);

    return res;
}

vector<vector<bool>> mat_mul(vector<vector<bool>> &a, vector<vector<bool>> &b)
{
    size_t n = a.size();
    vector<vector<bool>> res(n, vector<bool>(n, false));
    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            for (size_t k = 0; k < n; k++)
                res[i][j] = res[i][j] || (a[i][k] && b[k][j]);
    return res;
}

template <class RNG>
double test_mm(size_t n, double p, RNG &rng)
{
    vector<vector<bool>> a = random_bool_mat(n, p, rng);
    vector<vector<bool>> b = random_bool_mat(n, p, rng);
    SparseBoolMatrix a_s = SparseBoolMatrix::from_dense(a);
    SparseBoolMatrix b_s = SparseBoolMatrix::from_dense(b);

    chrono::steady_clock sc;
    auto start = sc.now();
    auto check_dense = mat_mul(a, b);
    auto end = sc.now();
    auto time_rep = static_cast<chrono::duration<double>>(end - start).count();
    return time_rep;
    /*    SparseBoolMatrix check = SparseBoolMatrix::from_dense(check_dense);

        auto start = sc.now();
        SparseBoolMatrix res = matrix_mult(a_s, b_s);
        auto end = sc.now();
        auto time_rep = static_cast<chrono::duration<double>>(end - start).count();

        assert(res == check);
    */
    return time_rep;
}

size_t nb_rep(2);

/**
 * Generate figures from the paper
 */
void run_tests(size_t max_size, size_t step)
{
    random_device rd;
    mt19937 rng(rd());
    // for (float p = 0.1; p <= 1; p += 0.1)
    float p = 40;
    {
        string name_file = "results_" + to_string(p) + ".csv";
        ofstream outFile;
        outFile.open(name_file);

        if (outFile.is_open())
        {
            for (size_t n = step; n <= max_size; n += step)
            {
                for (size_t i = 0; i <= nb_rep; i++)
                {
                    cout << n << "," << p << "," << i << endl;

                    double time_total = test_mm(n, p / (n), rng);
                    outFile << n << "," << p << "," << time_total << endl;
                }
            }
            outFile.close();
        }
        else
        {
            cout << "Unable to open files: " << name_file << endl;
        }
    }
}

int main()
{
    // random_device rd;
    // mt19937 rng(rd());
    // test_pm_wc_jump(100, rng);

    size_t max_size = 2000;
    size_t step = 500;

    run_tests(max_size, step);
    return 0;
}