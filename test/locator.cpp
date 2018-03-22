#include "locator.hpp"
#include <iostream>
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <functional>

void test_keep_each();
void test_swap_category();
void test_swap_label();
void test_combinations();
void test_resize();
void test_append();
void test_collapse();
void test_append_single();
void test_eq_contents();
void test_add_category();
void test_add_label();
void test_locate();
void test_find_one();
void test_keep();
void test_keep2();
void test_set_category();
void test_set_category_mult_labels();
void test_set_category_mult_categories();
void test_set_category_mult_categories2();
void test_empty_and_clear();
void test_rm_category();
double test_locate_speed(uint32_t sz);
double test_add_label_speed(uint32_t sz);
double test_add_label_speed_with_size_hint(uint32_t sz);
double test_dynamic_array_bit_array(uint32_t sz);
double test_vector_bit_array(uint32_t sz);
double test_add_category_speed(uint32_t n_categories);
void test_arr_insert_search_speed();
void compare_binary_to_linear_search();
void compare_binary_to_linear_search(uint32_t sz, uint32_t n_iters);
void compare_map_to_binary_search();

util::bit_array get_randomly_filled_array(uint32_t sz, uint32_t n_true);

int main(int argc, char* argv[])
{
    std::cout << "BEGIN LOCATOR" << std::endl;
    using util::profile::simple;
    
    test_keep_each();
    test_swap_label();
    test_swap_category();
    test_combinations();
    test_resize();
    test_set_category_mult_labels();
    test_collapse();
    test_set_category_mult_categories2();
    test_append();
    test_append_single();
    test_eq_contents();
    test_keep2();
    test_add_label();
    test_add_category();
    test_find_one();
    test_keep();
    test_rm_category();
    test_set_category();
    test_empty_and_clear();
    test_locate();

    std::cout << "Profiling ... " << std::endl;

    simple(std::bind(test_locate_speed, 1e3), "find (1000 elements)", 1e3);
    simple(std::bind(test_add_label_speed, 1e4), "add label (- hint) (10000 labels)", 1e2);
    simple(std::bind(test_add_label_speed_with_size_hint, 1e4), "add label (+ hint) (10000 labels)", 1e2);
    simple(std::bind(test_add_category_speed, 1e3), "add category (1000 categories)", 1e2);
    
    std::cout << "END LOCATOR" << std::endl;
    
    return 0;
}

util::bit_array get_randomly_filled_array(uint32_t sz, uint32_t n_true)
{
    if (n_true > sz)
    {
        n_true = sz;
    }
    
    util::bit_array arr(sz, false);
    
    for (uint32_t i = 0; i < n_true; i++)
    {
        arr.place(true, rand() % sz);
    }
    
    return arr;
}

void test_keep_each()
{
    using namespace util;
    
    locator loc;
    
    loc.require_category(0);
    loc.require_category(1);
    loc.require_category(2);
    
    uint32_t sz = 4000;
    
    loc.set_category(0, 1, bit_array(sz, true));
    loc.set_category(0, 2, get_randomly_filled_array(sz, 200));
    
    loc.set_category(1, 3, bit_array(sz, true));
    loc.set_category(1, 4, get_randomly_filled_array(sz, 200));
    
    loc.set_category(2, 5, bit_array(sz, true));
    loc.set_category(2, 6, get_randomly_filled_array(sz, 200));
    loc.set_category(2, 7, get_randomly_filled_array(sz, 200));
    
    types::entries_t combs;
    combs.push(0);
    combs.push(1);
    
    bool exists;
    
    loc.keep_each(combs, &exists);
    
    assert(exists);
    
    std::cout << "OK - test_keep_each()" << std::endl;
}

void test_swap_category()
{
    using namespace util;
    
    locator loc;
    
    loc.require_category(0);
    
    uint32_t res = loc.swap_category(1, 2);
    
    assert(res == locator_status::CATEGORY_DOES_NOT_EXIST);
    
    res = loc.swap_category(0, 1);
    
    assert(res == locator_status::OK);
    
    assert(loc.n_categories() == 1);
    assert(loc.has_category(1));
    assert(!loc.has_category(0));
    
    
    loc.set_category(1, 2, bit_array(100, true));
    
    assert(loc.find(2).tail() == 100);
    
    loc.set_category(1, 3, get_randomly_filled_array(100, 20));
    
    res = loc.swap_category(1, 3);
    
    bool exists;
    uint32_t cat = loc.which_category(2, &exists);
    
    assert(exists);
    assert(cat == 3);
    
    cat = loc.which_category(3, &exists);
    assert(exists);
    assert(cat == 3);
}

void test_swap_label()
{
    using namespace util;
    
    locator loc;
    
    loc.require_category(0);
    loc.set_category(0, 1, bit_array(100, true));
    
    bit_array idx2 = get_randomly_filled_array(100, 20);
    
    loc.set_category(0, 2, idx2);
    
    loc.require_category(1);
    loc.set_category(1, 3, bit_array(100, true));
    
    uint32_t res = loc.swap_label(10, 20);
    
    assert(res == locator_status::LABEL_DOES_NOT_EXIST);
    
    res = loc.swap_label(2, 3);
    
    assert(res == locator_status::LABEL_EXISTS);
    
    res = loc.swap_label(2, 4);
    
    assert(res == locator_status::OK);
    
    auto inds = loc.find(4);
    auto labs = loc.get_labels();
    
    assert(inds.tail() == idx2.sum());
    
    assert(contains(labs.unsafe_get_pointer(), labs.tail(), 4u));
    assert(!contains(labs.unsafe_get_pointer(), labs.tail(), 2u));
    
    bool exists;
    uint32_t new_cat = loc.which_category(4u, &exists);
    
    assert(exists);
    assert(new_cat == 0);
    
    new_cat = loc.which_category(2u, &exists);
    assert(!exists);
    
}

void test_combinations()
{
    using namespace util;
    
    locator loc;
    
    loc.require_category(0);
    
    bit_array index(1000, false);
    
    index.place(true, 1);
    
    loc.set_category(0, 1, index);
    
    index.place(true, 2);
    
    loc.set_category(0, 2, index);
    
    types::entries_t cats;
    cats.push(0);
    
    bool exists;
    
    auto res = loc.find_all(cats, &exists);
    
    assert(exists);
    assert(res.combinations.tail() == 2);
    
    //
    //
    //
    
    locator loc2;
    loc2.require_category(0);
    loc2.require_category(1);
    
    bit_array index2(1000, true);
    bit_array index3(1000, false);
    
    index3.place(true, 2);
    
    loc2.set_category(0, 1, index2);
    
    types::entries_t cats2;
    cats2.push(0);
    cats2.push(1);
    
    res = loc2.find_all(cats2, &exists);
    
    assert(exists);
    assert(res.combinations.tail() == 0);
    
    loc2.set_category(0, 2, index3);
    
//    loc2.set_category(1, 3, index2);
    loc2.set_category(1, 4, index3);
    
    res = loc2.find_all(cats2, &exists);
    
    assert(exists);
    
    assert(res.combinations.tail() == 4);
    
}

void test_resize()
{
    using namespace util;
    
    locator loc;
    
    loc.require_category(0);
    loc.set_category(0, 1, bit_array(1000, true));
    
    std::unordered_map<uint32_t, locator> loc_map;
    loc_map[0] = loc;
    
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        locator& loca = loc_map[0];
        locator locb = loca;
        
        uint32_t new_size = rand() % 10000;
        
        locb.resize(new_size);
        
        assert(locb.size() == new_size);
        
        if (new_size == 0)
        {
            assert(locb.n_labels() == 0);
        }
        else
        {
            assert(locb.n_labels() == 1);
        }
        
        loc_map[i] = std::move(locb);
    }
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        loc_map.erase(i);
    }
}

void test_collapse()
{
    using namespace util;
    
    for (uint32_t i = 0; i < 1000; i++)
    {
        locator loc;
        uint32_t sz = 1000;
        
        bit_array index(sz, false);
    
        index.place(true, 100);
        index.place(true, 102);
        index.place(true, 103);
    
        loc.require_category(0);
    
        loc.set_category(0, 1, index);
    
        index.fill(false);
    
        index.place(true, 80);
        index.place(true, 81);
    
        loc.set_category(0, 2, index);
    
        assert(loc.n_labels() == 2);
    
        assert(loc.collapse_category(1) == locator_status::CATEGORY_DOES_NOT_EXIST);
        assert(loc.n_labels() == 2);
    
        assert(loc.collapse_category(0) == locator_status::OK);
    
        assert(loc.n_labels() == 1);
    
        assert(!loc.has_label(1));
        assert(!loc.has_label(2));
    
        auto labs = loc.get_labels();
        
        assert(loc.count(labs.at(0)) == sz);
    }
}

void test_set_category_mult_labels()
{
    using namespace util;
    
    locator loc;
    
    types::entries_t res(4);
    res.seek_tail_to_start();
    
    loc.require_category(0);
    loc.set_category(0, 1, bit_array(100, true));
    
    res.push(0);
    res.push(1);
    res.push(2);
    res.push(2);
    
    bit_array index(100, false);
    index.place(true, 0);
    index.place(true, 1);
    index.place(true, 2);
    index.place(true, 3);
    
    uint32_t result = loc.set_category(0, res, index);
    
    assert(result == locator_status::OK);
    
    assert(loc.n_labels() == 3);
    
    auto inds = loc.find(2);
    
    assert(inds.tail() == 2 && inds.at(0) == 2 && inds.at(1) == 3);
    
    inds = loc.find(0);
    
    assert(inds.tail() == 1 && inds.at(0) == 0);
    
    index.fill(true);
    
    result = loc.set_category(0, res, index);
    
    assert(result == locator_status::WRONG_NUMBER_OF_INDICES);
    
    locator loc2;
    
    loc2.require_category(0);
    
    types::entries_t in_labs;
    for (uint32_t i = 0; i < 4; i++)
    {
        in_labs.push(i);
    }
    
    result = loc2.set_category(0, in_labs, bit_array(4, true));
    
    assert(result == locator_status::OK);
}

void test_append_single()
{
    using namespace util;
    
    locator loc;
    
    uint32_t n_cats = 10;
    uint32_t amt = uint32_t(1e6);
    
    for (uint32_t i = 0; i < n_cats; i++)
    {
        loc.require_category(i);
    }
    
    for (uint32_t i = 0; i < 1000; i++)
    {
        locator locb;
        
        types::entries_t res;
        
        for (uint32_t j = 0; j < n_cats; j++)
        {
            locb.require_category(j);
            
            uint32_t lab = rand() % amt;
            
            while (loc.has_label(lab) || locb.has_label(lab))
            {
                lab = rand() % amt;
            }
            
            locb.set_category(j, lab, bit_array(1, true));
            
            res.push(lab);
        }
        
        loc.append(locb);
        
        const auto& labs = loc.get_labels();
        uint32_t* labs_ptr = labs.unsafe_get_pointer();
        
        for (uint32_t i = 0; i < labs.tail(); i++)
        {
            auto inds = loc.find(labs_ptr[i]);
            assert(inds.tail() > 0);
        }        
    }
}

void test_append()
{
    using namespace util;
    
    locator loc;
    
    uint32_t sz = 100000;
    
    util::bit_array index0 = get_randomly_filled_array(sz, 20);
    util::bit_array index1 = get_randomly_filled_array(sz, 30);
    
    loc.require_category(0);
    loc.set_category(0, 100, index0);
    loc.set_category(0, 102, index1);
    
    locator loc2;
    
    loc2.require_category(1);
    loc2.set_category(1, 100, index0);
    
    uint32_t res = loc.append(loc2);
    
    assert(res == locator_status::CATEGORIES_DO_NOT_MATCH);
    
    locator loc3 = loc;
    
    loc3.set_category(0, 101, index0);
    
    res = loc.append(loc3);
    
    assert(res == locator_status::OK);
    
    assert(loc.size() == sz * 2);
    
    auto labs = loc.get_labels();
    
    assert(labs.tail() == 3);
    
    uint32_t* labs_ptr = labs.unsafe_get_pointer();
    assert(contains(labs_ptr, labs.tail(), 100u));
    assert(contains(labs_ptr, labs.tail(), 101u));
    assert(contains(labs_ptr, labs.tail(), 102u));

    auto inds1 = loc.find(101);
    
    assert(inds1.tail() == index0.sum());
    
    for (uint32_t i = 0; i < inds1.tail(); i++)
    {
        uint32_t idx = inds1.at(i);
        assert(index0.at(idx-sz));
    }
    
    assert(loc3.size() == sz);
    assert(loc3.n_labels() == 2);
}

void test_eq_contents()
{
    using namespace util;
    
    locator loc;
    
    uint32_t sz = 100;
    
    util::bit_array index0 = get_randomly_filled_array(sz, 20);
    util::bit_array index1 = get_randomly_filled_array(sz, 30);
    util::bit_array check = get_randomly_filled_array(sz, 0);
    
    util::bit_array::unchecked_dot_eq(check, index0, index1, 0, index0.size());
    
    //  ensure the index is not the same to begin with
    while (check.all())
    {
        check.fill(false);
        
        util::bit_array index1 = get_randomly_filled_array(sz, 30);
        util::bit_array::unchecked_dot_eq(check, index0, index1, 0, index0.size());
    }
    
    util::bit_array index0_copy = index0;
    
    loc.require_category(0);
    loc.require_category(1);
    
    loc.set_category(0, 10, index0);
    loc.set_category(1, 11, index1);
    
    locator loc2 = loc;
    
    locator loc3;
    locator loc4;
    
    loc4.require_category(0);
    loc4.require_category(1);
    
    loc4.set_category(0, 10, index0);
    loc4.set_category(1, 11, index1);
    
    assert(loc2 == loc);
    assert(loc != loc3);
    assert(loc4 == loc);
    
    locator loc5 = loc4;
    
    assert(loc4 == loc5);
    
    loc5.set_category(1, 12, index1);
    
    assert(loc4 != loc5);
}

void test_keep2()
{
    using namespace util;
    
    locator loc;
    
    loc.require_category(100);
    loc.set_category(100, 100, bit_array(100, true));
    
    types::entries_t indices(2);
    
    indices.place(0, 0);
    indices.place(1, 1);
    
    loc.keep(indices);
    
    assert(loc.size() == 2);
    
}

void test_rm_category()
{
    using namespace util;
    
    locator loc;
    uint32_t res;
    
    res = loc.require_category(0);
    
    assert(loc.has_category(0));
    
    res = loc.rm_category(0);
    
    assert(res == locator_status::OK);
    
    res = loc.rm_category(0);
    
    assert(res == locator_status::CATEGORY_DOES_NOT_EXIST);
    
    assert(!loc.has_category(0));
    
    res = loc.set_category(0, 10, bit_array(100, true));
    
    assert(res == locator_status::CATEGORY_DOES_NOT_EXIST);
    
    loc.require_category(0);
    
    res = loc.set_category(0, 100, bit_array(100, true));
    
    assert(res == locator_status::OK);
    
    const types::entries_t& labs = loc.get_labels();
    
    assert(labs.tail() == 1 && labs.at(0) == 100);
    
    res = loc.rm_category(0);
    
    assert(labs.tail() == 0);
    
    assert(loc.find(100).tail() == 0);
    
    locator loc3;
    
    loc3.require_category(0);
    loc3.set_category(0, 100, bit_array(100, true));
    
    loc3.rm_category(0);
    
    locator loc2;
    
    uint32_t loc_sz = 100;
    uint32_t n_cats = 10;
    uint32_t n_labs = 1000;
    
    for (uint32_t i = 0; i < 1000; i++)
    {
        for (uint32_t j = 0; j < n_labs; j++)
        {
            bit_array index = get_randomly_filled_array(loc_sz, loc_sz / 2);
            uint32_t cat = rand() % n_cats;
            uint32_t lab = j;
            
            loc2.require_category(cat);
            uint32_t res = loc2.set_category(cat, lab, index);
            
            assert(res == locator_status::OK);
        }
        
        types::entries_t categories = loc2.get_categories();
        
        for (uint32_t j = 0; j < categories.tail(); j++)
        {
            uint32_t res = loc2.rm_category(categories.at(j));
            
            assert(res == locator_status::OK);
        }
        
        assert(loc2.n_categories() == 0);
        
        categories = loc2.get_categories();
        
        assert(categories.tail() == 0);
        
        const types::entries_t& labs = loc2.get_labels();
        
        assert(labs.tail() == 0);
    }
}

void test_set_category_mult_categories2()
{
    using namespace util;
    
    locator loc;
    
    uint32_t iters = 10000;
    dynamic_array<uint32_t> prev_labs;
    
    prev_labs.push(0);
    prev_labs.push(1);
    
    for (uint32_t i = 0; i < iters; i++)
    {
        
        uint32_t lab = rand() % iters;
        uint32_t* lab_ptr = prev_labs.unsafe_get_pointer();
        
        while (i > 0 && contains(lab_ptr, prev_labs.tail(), lab))
        {
            lab = rand() % iters;
        }
        
        loc.require_category(0);
        loc.require_category(1);
        loc.require_category(2);
        loc.require_category(3);
        
        loc.set_category(2, 0, bit_array(true, 100));
        loc.set_category(3, 1, bit_array(true, 100));
        
        loc.set_category(0, lab, bit_array(true, 100));
        
        assert(loc.n_labels() == 3);
        
        const types::entries_t& labs = loc.get_labels();
        
        assert(labs.tail() == 3);
        
        uint32_t* own_lab_ptr = labs.unsafe_get_pointer();
        
        assert(contains(own_lab_ptr, labs.tail(), 0u));
        assert(contains(own_lab_ptr, labs.tail(), lab));
        
        prev_labs.push(lab);
    }
    
    locator loc2;
    
    loc2.require_category(0);
    loc2.require_category(1);
    
    uint32_t sz = 10;
    
    util::bit_array index(sz, false);
    
    for (uint32_t i = 0; i < sz/2; i++)
    {
        index.place(true, i);
    }
    
    loc2.set_category(0, 10, index);
    
    index.fill(false);
    
    for (uint32_t i = sz/2; i < sz; i++)
    {
        index.place(true, i);
    }
    
    loc2.set_category(0, 7, index);
    
    types::entries_t labs = loc2.get_labels();
    
    assert(labs.tail() == 2 && labs.tail() == loc2.n_labels());
    
    types::entries_t search;
    search.push(7);
    
    types::entries_t num_inds = loc2.find(search);
    
    assert(num_inds.tail() == sz/2);
    
    search.push(10);
    
    num_inds.seek_tail_to_start();
    
    num_inds = loc2.find(search);
    
    assert(num_inds.tail() == sz);
}

void test_set_category_mult_categories()
{
    using namespace util;
    
    locator loc;
    
    uint32_t sz = 100;
    uint32_t prev_lab;
    uint32_t prev_cat;
    uint32_t n_cats = 100;
    uint32_t iters = 10000;
    
    dynamic_array<uint32_t> prev_labs;
    
    for (uint32_t i = 0; i < iters; i++)
    {
        uint32_t lab = rand() % iters;
        uint32_t cat = rand() % n_cats;
        
        loc.require_category(cat);
        
        uint32_t* lab_ptr = prev_labs.unsafe_get_pointer();
        
        while (i > 0 && contains(lab_ptr, i, lab))
        {
            lab = rand() % iters;
        }
        
        uint32_t res = loc.set_category(cat, lab, bit_array(sz, true));
        
        if (res != locator_status::OK)
        {
            assert(res == locator_status::LABEL_EXISTS_IN_OTHER_CATEGORY);
            std::cout << "OK" << std::endl;
        }
        
        assert(res == locator_status::OK);
        
        types::entries_t result = loc.find(lab);
        assert(result.tail() == sz);
        
        types::entries_t labs = loc.get_labels();
        
        if (labs.tail() != loc.n_categories())
        {
            std::cout << "Iter is: " << i << std::endl;
            std::cout << "Tail is: " << labs.tail() << std::endl;
            std::cout << "N labels is: " << loc.n_labels() << std::endl;
            std::cout << "Label is: " << labs.at(0) << std::endl;
            std::cout << "Categories are: " << loc.n_categories() << std::endl;
        }
        
        assert(labs.tail() == loc.n_categories());
        
        if (i > 0)
        {
            result = loc.find(prev_lab);
            
            if (prev_cat == cat)
            {
                assert(result.tail() == 0);
            }
            else
            {
                assert(result.tail() == sz);
            }
        }
        
        prev_cat = cat;
        prev_lab = lab;
        prev_labs.push(lab);
    }
}

void test_set_category()
{
    using namespace util;
    
    locator loc;
    
    loc.add_category(0);
    uint32_t sz = 100;
    uint32_t prev_lab;
    
    for (uint32_t i = 0; i < 10000; i++)
    {
        uint32_t lab = rand() % 10000;
        
        while (i > 0 && lab == prev_lab)
        {
            lab = rand() % 10000;
        }
        
        loc.set_category(0, lab, bit_array(sz, true));
        types::entries_t result = loc.find(lab);
        assert(result.tail() == sz);
        
        types::entries_t labs = loc.get_labels();
        
        assert(labs.tail() > 0 && labs.at(0) == lab);
        
        if (i > 0)
        {
            result = loc.find(prev_lab);
            assert(result.tail() == 0);
        }
        
        prev_lab = lab;
    }
}

void test_empty_and_clear()
{
    using namespace util;
    
    locator loc;
    locator loc2;
    uint32_t sz = 100;
    
    assert(loc.is_empty());
    
    loc.add_category(0);
    loc.set_category(0, 10, bit_array(sz, true));
    
    assert(loc.find(10).tail() == sz);
    
    loc.empty();
    
    assert(loc.has_category(0));
    
    loc2.add_category(0);
    loc2.set_category(0, 10, bit_array(sz, true));
    loc2.clear();
    
    assert(!loc2.has_category(0));
    
    loc2.add_category(0);
    loc2.set_category(0, 10, bit_array(sz, true));
    
    assert(loc2.has_category(0));
    assert(loc2.find(10).tail() == sz);
    
    loc2.add_category(10);
    loc2.set_category(10, 100, bit_array(sz, true));
    
    assert(loc2.find(100).tail() == sz);
    
    assert(loc2.n_labels() == 2);
    assert(loc2.n_categories() == 2);
}

void test_keep()
{
    using namespace util;
    
    locator loc;
    uint32_t res;
    uint32_t ok = locator_status::OK;
    uint32_t sz = 100;
    
    dynamic_array<uint32_t> at_indices;
    
    res = loc.keep(at_indices);
    
    assert(res == ok);
    
    assert(loc.is_empty());
    
    loc.add_category(0);
    
    loc.set_category(0, 10, bit_array(sz, true));
    
    bit_array index2(sz, false);
    index2.place(true, 10);
    index2.place(true, 20);
    
    loc.set_category(0, 11, index2);
    
    assert(!loc.is_empty());
    assert(loc.n_labels() == 2);
    assert(loc.size() == sz);
    
    at_indices.push(0);
    at_indices.push(1);
    
    loc.keep(at_indices);
    
    assert(loc.size() == 2);
    
    assert(loc.n_labels() == 1);
    
    const types::entries_t& labs = loc.get_labels();
    
    assert(labs.tail() == 1);
    assert(labs.at(0) == 10);
    
    loc.set_category(0, 11, bit_array(loc.size(), true));
    
    assert(loc.n_labels() == 1);
    
    assert(labs.at(0) == 11);
    
}

void test_find_one()
{
    using namespace util;
    
    locator loc;
    
    types::entries_t res = loc.find(0);
    
    assert(res.tail() == 0);
    
    loc.add_category(10);
    loc.set_category(10, 1, bit_array());
    res = loc.find(1);
    
    assert(res.tail() == 0);
    
    loc.set_category(10, 1, bit_array(1, true));
    
    res = loc.find(1);
    
    assert(res.tail() == 1);
}

void compare_binary_to_linear_search(uint32_t sz, uint32_t n_iters)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    util::dynamic_array<double> times(n_iters);
    
    times.seek_tail_to_start();
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        dynamic_array<uint32_t> arr(sz);
    
        arr.seek_tail_to_start();
    
        for (uint32_t j = 0; j < sz; j++)
        {
            arr.push(j);
        }
    
        uint32_t* ptr = arr.unsafe_get_pointer();
    
        uint32_t search_for = sz-1;
        bool was_found;
        uint32_t at_linear_index;
        uint32_t at_binary_index;
        
        t1 = high_resolution_clock::now();
        util::unchecked_linear_search(ptr, sz, search_for, &was_found, &at_linear_index);
        t2 = high_resolution_clock::now();
        
        assert(was_found);
        
        double linear_time = profile::ellapsed_time_s(t1, t2);
        
        t1 = high_resolution_clock::now();
        util::unchecked_binary_search(ptr, sz, search_for, &was_found, &at_binary_index);
        t2 = high_resolution_clock::now();
        
        double binary_time = profile::ellapsed_time_s(t1, t2);
        
        double speed_binary_vs_linear = binary_time - linear_time;
        
        times.push(speed_binary_vs_linear);
        
        assert(was_found);
        assert(at_binary_index == at_linear_index);
    }
    
    double sum = 0.0;
    double* times_ptr = times.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        sum += times_ptr[i];
    }
    
    double mean = (sum / n_iters) * 10000.0;
    
    std::cout << "Mean difference (binary - linear) " << "(" << sz << " elements)" << ": " << mean;
    std::cout << " (µs)" << std::endl;
    std::cout << "--" << std::endl;
}

void compare_binary_to_linear_search()
{
    using namespace util;
    
    dynamic_array<uint32_t> szs;
    
    szs.push(1e1);
    szs.push(1e2);
    szs.push(1e3);
    szs.push(1e4);
    szs.push(1e5);
    szs.push(1e6);
    szs.push(1e7);
    
    for (uint32_t i = 0; i < szs.tail(); i++)
    {
        uint32_t sz = szs.at(i);
        compare_binary_to_linear_search(sz, 100);
    }
}

double test_add_category_speed(uint32_t n_categories)
{
    using namespace std::chrono;
    using namespace util;
    
    util::locator loc;
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < n_categories; i++)
    {
        loc.add_category(i);
    }
    
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    
    return util::profile::ellapsed_time_s(t1, t2);
}

double test_add_label_speed_with_size_hint(uint32_t sz)
{
    using namespace std::chrono;
    using namespace util;
    
    util::locator loc(sz);
    util::bit_array index(10000, true);
    
    uint32_t n_categories = 100;
    
    for (uint32_t i = 0; i < n_categories; i++)
    {
        loc.add_category(i);
    }
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        uint32_t cat = rand() % n_categories;
        loc.set_category(cat, i, index);
    }
    
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    
    return util::profile::ellapsed_time_s(t1, t2);
}

double test_add_label_speed(uint32_t sz)
{
    using namespace std::chrono;
    using namespace util;
    
    util::locator loc;
    util::bit_array index(10000, false);
    
    uint32_t n_categories = 100;
    
    for (uint32_t i = 0; i < n_categories; i++)
    {
        loc.add_category(i);
    }
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        uint32_t cat = rand() % n_categories;
        
        for (uint32_t j = 0; j < 100; j++)
        {
            uint32_t idx = rand() % sz;
            index.place(true, idx);
        }
        
        loc.set_category(cat, i, index);
    }
    
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    
    return util::profile::ellapsed_time_s(t1, t2);
}

double test_vector_bit_array(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    std::vector<bit_array> vec(sz);
    
    t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        bit_array index(10000);
        vec.push_back(index);
    }
    
    t2 = high_resolution_clock::now();
    
    return profile::ellapsed_time_s(t1, t2);
}

double test_dynamic_array_bit_array(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    dynamic_array<bit_array, dynamic_allocator<bit_array>> array(sz);
    
    array.seek_tail_to_start();
    
    t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        bit_array index(10000);
        array.push(index);
    }
    
    t2 = high_resolution_clock::now();
    
    return profile::ellapsed_time_s(t1, t2);
}

double test_locate_speed(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    locator loc;
    uint32_t n_true = sz / 1e2;
    uint32_t n_search = 1000;
    
    uint32_t n_categories = 7;
    uint32_t n_labels = 100;
    
    for (uint32_t i = 0; i < n_categories; i++)
    {
        loc.add_category(i);
    }
    
    util::bit_array index(sz);
    
    for (uint32_t i = 0; i < n_labels; i++)
    {
        index.fill(false);
        
        uint32_t cat = rand() % n_categories;
        
        for (uint32_t j = 0; j < n_true; j++)
        {
            uint32_t idx = rand() % sz;
            index.place(true, idx);
        }
        
        uint32_t label = i;
        
        loc.set_category(cat, label, index);
    }
    
    types::entries_t search_for;
    
    for (uint32_t i = 0; i < n_search; i++)
    {
        search_for.push(i);
    }
    
    t1 = high_resolution_clock::now();
    
    types::entries_t res = loc.find(search_for);
    
    t2 = high_resolution_clock::now();
    
    return profile::ellapsed_time_s(t1, t2);
}

void test_locate()
{
    using namespace util;
    
    uint32_t n_labs = 20;
    uint32_t sz = 1e4;
    
    locator loc(n_labs);
    
    types::entries_t all_labels(n_labs);
    all_labels.seek_tail_to_start();
    
    for (uint32_t i = 0; i < n_labs; i++)
    {
        loc.add_category(i);
        loc.set_category(i, i, bit_array(sz, true));
        
        all_labels.push(i);
    }
    
    types::entries_t result = loc.find(all_labels);
    
    assert(result.tail() == sz);
}

void test_locate2()
{
    using namespace util;
    
    locator loc;
    uint32_t sz = 100;
    
    bit_array index0(sz);
    
    index0.fill(false);
    
    bit_array index1 = index0;
    bit_array index2 = index0;
    bit_array index3 = index0;
    
    index0.place(true, 99);
    index1.place(true, 99);
    index2.place(true, 0);
    index3.place(true, 0);
    
    uint32_t cat0 = 0;
    uint32_t cat1 = 1;
    
    loc.add_category(cat0);
    loc.add_category(cat1);
    
    loc.set_category(cat0, 10, index0);
    loc.set_category(cat0, 11, index1);
    loc.set_category(cat0, 12, index2);
    
    loc.set_category(cat1, 20, index3);
    
    types::entries_t labels0 = types::entries_t();
    types::entries_t labels1 = types::entries_t();
    types::entries_t labels2 = types::entries_t();
    types::entries_t labels3 = types::entries_t();
    types::entries_t labels4 = types::entries_t();
    
    labels0.push(11);
    
    labels1.push(10);
    labels1.push(11);
    
    labels2.push(10);
    labels2.push(11);
    labels2.push(12);
    
    labels3.push(20);
    labels3.push(12);
    
    labels4.push(20);
    labels4.push(10);
    
    types::entries_t res0 = loc.find(labels0);
    
    assert(res0.tail() == 1 && res0.at(0) == 99);
    
    types::entries_t res1 = loc.find(labels1);
    
    assert(res1.tail() == 0);
    
    types::entries_t res2 = loc.find(labels2);
    
    assert(res2.tail() == 2 && res2.at(0) == 0 && res2.at(1) == 99);
    
    types::entries_t res3 = loc.find(labels3);
    
    assert(res3.tail() == 1 && res3.at(0) == 0);
    
    types::entries_t res4 = loc.find(labels4);
    
    assert(res4.tail() == 0);
    
    index0.place(true, 70);
    res0 = loc.find(labels0);
    assert(res0.tail() == 1 && res0.at(0) == 99);
}

void test_add_label()
{
    using namespace util;
    
    locator loc;
    uint32_t result;
    bit_array index(100, true);
    
    uint32_t label = 10;
    uint32_t category = 100;
    
    result = loc.set_category(category, label, index);
    
    assert(result == locator_status::CATEGORY_DOES_NOT_EXIST);
    
    result = loc.add_category(category);
    
    assert(result == locator_status::OK);
    
    result = loc.set_category(category, label, index);
    
    assert(result == locator_status::OK);
    
    assert(loc.n_labels() == 1);
    
    index.place(false, 99);
    
    result = loc.set_category(category, 9, index);
    
    assert(result == locator_status::OK);
    
    assert(loc.n_labels() == 2);
    
    types::entries_t labels = loc.get_labels();
}

void test_add_category()
{
    using namespace util;
    
    locator loc;
    uint32_t result;
    
    result = loc.add_category(10);
    
    assert(result == locator_status::OK);
    assert(loc.has_category(10));
    
    result = loc.add_category(10);
    
    assert(result == locator_status::CATEGORY_EXISTS);
    
    result = loc.add_category(11);
    
    assert(loc.has_category(11));
}
