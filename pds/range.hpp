/******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-15 Nicola Bonelli <nicola@pfq.io>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <numeric>
#include <limits>

#include <experimental/optional>

namespace pds {

    using std::experimental::optional;
    using std::experimental::nullopt;

    template <typename T>
    struct range_iterator
    {
        range_iterator(T min, T max)
        : min_(min)
        , max_(max)
        , value_(min)
        { }

        range_iterator()
        : min_()
        , max_()
        , value_()
        { }

        T
        operator*() const
        {
            return *value_;
        }

        range_iterator &
        operator++()
        {
            if (value_) {
                if (*value_ != max_)
                    (*value_)++;
                else
                    value_ = nullopt;
            }

            return *this;
        }

        friend 
        bool operator==(const range_iterator &lhs, const range_iterator &rhs)
        {
            return lhs.value_ == rhs.value_;
        }
        
        friend 
        bool operator!=(const range_iterator &lhs, const range_iterator &rhs)
        {
            return lhs.value_ != rhs.value_;
        }
        
        T min_;
        T max_;
        mutable optional<T> value_;
    };


    //
    // numeric generator
    //
    
    template <typename T>
    struct numeric_range
    {
        using value_type = T;

        numeric_range(T min, T max)
        : min_(min)
        , max_(max)
        { 
            if (min < max)
                value_ = min;
            else
                value_ = nullopt;
        }

        T min_;
        T max_;
        mutable optional<T> value_;

        optional<T> 
        operator()() const
        {
            auto r = value_;
            
            if (value_) {
                if (*value_ != max_)
                    (*value_)++;
                else
                    value_ = nullopt;
            }

            return r;
        }

        range_iterator<T>
        begin() const
        {
            return range_iterator<T>(min_, max_);
        }
        
        range_iterator<T>
        end() const
        {
            return range_iterator<T>();
        }
        
        range_iterator<T>
        cbegin() const
        {
            return range_iterator<T>(min_, max_);
        }
        
        range_iterator<T>
        cend() const
        {
            return range_iterator<T>();
        }
    };


    template <typename T, typename = void> struct make_range;
    
    template <typename T>
    struct make_range<T, std::enable_if_t<std::is_integral<T>::value>>
    {
        static auto run()
        {
            return numeric_range<T>{std::numeric_limits<T>::min(), std::numeric_limits<T>::max()};
        }
    };

} // namespace pds
