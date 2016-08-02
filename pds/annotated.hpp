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

#include <pds/sketch.hpp>
#include <pds/hash.hpp>
#include <pds/tuple.hpp>

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>

#include <experimental/optional>

namespace pds {

    using namespace std::experimental;

    //
    // annotated types... 
    //

    template <typename T, typename A>
    struct annotated
    {   
        annotated(T n = T{}, A i = {})
        : value(n)
        , info(std::move(i))
        { }

        T value;
        A info;
    };

    template <typename T, typename A>
    auto annotate(T const &value, A info = {})
    {
        return annotated<T, A>(value, std::move(info));
    }

    
    template <typename CharT, typename Traits, typename T, typename A>
    typename std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT,Traits>& out, annotated<T, A> const& c)
    {
        return out << "{ value:" << c.value << " annotation: " << c.info << " }";
    }

} // namespace pds
