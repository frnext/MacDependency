
// Copyright 2005-2008 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_UNORDERED_TEST_HELPERS_RANDOM_VALUES_HEADER)
#define BOOST_UNORDERED_TEST_HELPERS_RANDOM_VALUES_HEADER

#include "./list.hpp"
#include <algorithm>
#include <boost/mpl/if.hpp>
#include "./generators.hpp"
#include "./metafunctions.hpp"

namespace test
{
    typedef enum {
        default_generator,
        generate_collisions
    } random_generator;

    template <class X>
    struct unordered_generator_set
    {
        typedef BOOST_DEDUCED_TYPENAME X::value_type value_type;

        random_generator type_;

        unordered_generator_set(random_generator type)
            : type_(type) {}

        template <class T>
        void fill(T& x, std::size_t len) {
            value_type* value_ptr = 0;
            int* int_ptr = 0;

            for(std::size_t i = 0; i < len; ++i) {
                value_type value = generate(value_ptr);

                for(int count =
                    type_ == generate_collisions ?
                    generate(int_ptr) % 10 : 1;
                    count; --count) {
                    x.push_back(value);
                }
            }
        }
    };

    template <class X>
    struct unordered_generator_map
    {
        typedef BOOST_DEDUCED_TYPENAME X::key_type key_type;
        typedef BOOST_DEDUCED_TYPENAME X::mapped_type mapped_type;

        random_generator type_;

        unordered_generator_map(random_generator type)
            : type_(type) {}

        template <class T>
        void fill(T& x, std::size_t len) {
            key_type* key_ptr = 0;
            mapped_type* mapped_ptr = 0;
            int* int_ptr = 0;

            for(std::size_t i = 0; i < len; ++i) {
                key_type key = generate(key_ptr);

                for(int count =
                    type_ == generate_collisions ?
                    generate(int_ptr) % 10 : 1;
                    count; --count) {
                    x.push_back(std::pair<key_type const, mapped_type>(key, generate(mapped_ptr)));
                }
            }
        }
    };

    template <class X>
    struct unordered_generator_base
        : public boost::mpl::if_<
            test::is_set<X>,
            test::unordered_generator_set<X>,
            test::unordered_generator_map<X> >
    {
    };

    template <class X>
    struct unordered_generator : public unordered_generator_base<X>::type
    {
        typedef BOOST_DEDUCED_TYPENAME unordered_generator_base<X>::type base;

        unordered_generator(random_generator const& type = default_generator)
            : base(type) {}
    };

    template <class X>
    struct random_values
        : public test::list<BOOST_DEDUCED_TYPENAME X::value_type>
    {
        random_values(int count, test::random_generator const& generator =
            test::default_generator)
        {
            static test::unordered_generator<X> gen(generator);
            gen.fill(*this, count);
        }
    };
}

#endif
