// ====------ onedpl_test_vector.cpp---------- -*- C++ -* ----===////
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// ===----------------------------------------------------------------------===//
#include <oneapi/dpl/execution>
#include "dpct/dpct.hpp"
#include "dpct/dpl_utils.hpp"
#include <iostream>

template <typename _T>
using Vector = dpct::device_vector<_T>;

template<typename String, typename _T1, typename _T2>
int ASSERT_EQUAL(String msg, _T1&& X, _T2&& Y) {
    if(X!=Y) {
        std::cout << "FAIL: " << msg << " - (" << X << "," << Y << ")" << std::endl;
        return 1;
    }
    else {
        std::cout << "PASS: " << msg << std::endl;
        return 0;
    }
}

template<typename String, typename _T1, typename _T2>
int ASSERT_ARRAY_EQUAL(String msg, _T1&& X, _T2&& Y) {
    if(X!=Y) {
        std::cout << "FAIL: " << msg << " - (" << X << "," << Y << ")" << std::endl;
        return 1;
    }
    return 0;
}

int test_passed(int failing_elems, std::string test_name) {
    if (failing_elems == 0) {
        std::cout << "PASS: " << test_name << std::endl;
        return 0;
    }
    return 1;
}

int main() {

    // used to detect failures
    int failed_tests = 0;
    int num_failing = 0;
    std::string test_name = "";

    //testing initialization of size zero (should not crash / throw)
    Vector<int> vempty(0);
    
    using T = int;
    std::vector<T> v_base(6);// { 0, 1, 2, 3, 4, 5 };
    v_base[0] = 0; v_base[1] = 1; v_base[2] = 2;
    v_base[3] = 3; v_base[4] = 4; v_base[5] = 5;
    Vector<T> v0 {};
    v0 = v_base;
    Vector<T> v1(v_base);
    // TODO: Extending host vector test.
    Vector<T> v2(v1);
    Vector<int> v3 {};
    v3 = v2;
    v3.clear();
#ifdef _VERBOSE
    std::cout << "v3.size() after clearing = " << v3.size() << std::endl; // expected: 0
#else
    failed_tests += ASSERT_EQUAL("v3.size() = 0", v3.size(), 0);
#endif
    v3.assign(5, -1);
    dpct::get_default_queue().wait();
#ifdef _VERBOSE
    std::cout << "v3.size() after asssigning = " << v3.size() << ", v3[3] = " << v3[3] << std::endl; // expected: 5, -1
#else
    failed_tests += ASSERT_EQUAL("v3.size() = 5", v3.size(), 5);
    failed_tests += ASSERT_EQUAL("v3[3] = -1", v3[3], -1);
#endif
    Vector<T> v4(6);
    v4 = v2;
    v4.erase(v4.begin());
#if 0
    v4.insert(v4.begin(), -111);
    v4.insert(v4.begin(), 2, -111);
    v4.insert(v4.begin(), v2.begin(), v2.begin() + 2);
#else
    v4.insert(v4.begin(), *(v2.begin()) - 111);
    v4.insert(v4.begin(), 2, *(v2.begin()) - 111);
    v4.insert(v4.begin(), v2.begin(), v2.begin() + 2);
#endif
#ifdef _VERBOSE
    std::cout << "v4.size() = " << v4.size() << std::endl;
    std::cout << "v4: ";
#endif
    v4.swap(v2);
    Vector<T> v5(6, 0);
    v5 = v2;
    dpct::get_default_queue().wait();
#ifdef _VERBOSE
    std::cout << std::endl << "v5: ";
    for (std::size_t i = 0; i < v5.size(); ++i) {
        std::cout << v5[i] << " ";  // expected: 0 1 -111 -111 -111 1 2 3 4 5
    }
    std::cout << std::endl;
#endif
    Vector<T> v6(v5.begin(), v5.end());
    dpct::get_default_queue().wait();
#ifdef _VERBOSE
    std::cout << std::endl << "v6: ";
    for (std::size_t i = 0; i < v6.size(); ++i) {
        std::cout << v6[i] << " ";  // expected: 0 1 -111 -111 -111 1 2 3 4 5
    }
    std::cout << std::endl;
#endif
    v6.push_back(T(0));
    v6.push_back(T(1));
    v6.push_back(T(2));
#ifdef _VERBOSE
    std::cout << "v6.back() = " << v6.back() << std::endl; // expected: 2
#else
    //failed_tests += ASSERT_EQUAL("v6.back() = 2", v6.back(), 2);
#endif
    v6.pop_back();
    v6.reserve(20);
#ifdef _VERBOSE
    if (!v6.empty() && v6.front() == *v6.begin()) {
        std::cout << "v6.size() = " << v6.size() << ", v6.max_size() = " <<
            v6.max_size() << ", v6.capacity() = " << v6.capacity() << std::endl; // expected: 12, 4611686018427387903, 20
        v6.shrink_to_fit();
        std::cout << "new v6.capacity() = " << v6.capacity() << std::endl; // expected: 12
        std::cout << "v6[0] = " << v6[0] << std::endl; // expected: 5
    }
#else
    failed_tests += ASSERT_EQUAL("v6.size() = 12", v6.size(), 12);
    failed_tests += ASSERT_EQUAL("v6.max_size()", v6.max_size(), 4611686018427387903);
    failed_tests += ASSERT_EQUAL("v6.capacity() = 20", v6.capacity(), 20);
    v6.shrink_to_fit();
    failed_tests += ASSERT_EQUAL("v6.capacity() = 12", v6.capacity(), 12);
    failed_tests += ASSERT_EQUAL("v6[0] = 0", v6[0], 0);
#endif
    v6.resize(20, 99);
    auto resize_policy = oneapi::dpl::execution::make_device_policy(dpct::get_default_queue());
    auto sum = std::reduce(resize_policy, v6.begin()+12, v6.end(), 0);
    failed_tests += ASSERT_EQUAL("sum = 792", sum, 792);

    v6.erase(v6.cbegin() + 10, v6.cend());
#ifdef _VERBOSE
    for (std::size_t i = 0; i < v6.size(); ++i) {
        std::cout << v6[i] << " ";  // expected: 5 4 3 2 1 -111 -111 -111 1 0
    }
#else
    //std::vector<T> v6 = v5a;
    test_name = "v6 = v5a";

    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[0], 0);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[1], 1);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[2], -111);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[3], -111);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[4], -111);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[5], 1);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[6], 2);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[7], 3);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[8], 4);
    num_failing += ASSERT_ARRAY_EQUAL(test_name, v6[9], 5);

    failed_tests += test_passed(num_failing, test_name);
    num_failing = 0;
#endif
    { // Simple test of DPCT vector with PSTL algorithm
        // create buffer
        std::vector<int64_t> src(8);
        src[0] = -1; src[1] = 2; src[2] = -3; src[3] = 4; src[4] = -5; src[5] = 6; src[6] = -7; src[7] = 8;

        dpct::device_vector<int64_t> dst = src;
        dpct::get_default_queue().wait();
        src[0] = 99;
        failed_tests += ASSERT_EQUAL("src[0] = 99", src[0], 99);
        //TODO: FIX ME, double free or corruption error caused by next line.
        //failed_tests += ASSERT_EQUAL("dst[0] = -1", dst[0], -1);
        std::fill(src.begin(), src.end(), 0);

        // create policy using the default queue to ensure algorithms execute in same context as
        // USM allocations
        auto new_policy = oneapi::dpl::execution::make_device_policy(dpct::get_default_queue());

        // call algorithm:
        std::transform(new_policy, dst.begin(), dst.end(), dst.begin(), std::negate<int64_t>());

        std::copy(new_policy, dst.begin(), dst.end(), src.begin());
        std::fill(new_policy, dst.begin(), dst.end(), 101);
        dpct::get_default_queue().wait();

        failed_tests += ASSERT_EQUAL("dst.size() = 8", dst.size(), 8);
        //failed_tests += ASSERT_EQUAL("dst[0] = 101", dst[0], 101);

        test_name = "simple test of dpct vector";

        num_failing += ASSERT_ARRAY_EQUAL(test_name, src.size(), 8);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[0], 1);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[1], -2);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[2], 3);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[3], -4);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[4], 5);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[5], -6);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[6], 7);
        num_failing += ASSERT_ARRAY_EQUAL(test_name, src[7], -8);

        failed_tests += test_passed(num_failing, test_name);
    }

    std::cout << std::endl << failed_tests << " failing test(s) detected." << std::endl;
    if (failed_tests == 0) {
        return 0;
    }
    return 1;
}
