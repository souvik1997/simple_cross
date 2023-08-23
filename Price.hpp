//
//  Price.hpp
//  simple_cross
//
//  Created by Souvik Banerjee on 8/21/23.
//

#ifndef Price_hpp
#define Price_hpp

#include <compare>
#include <cstdint>
#include <iostream>

/**
 * @brief Data structure to represent order prices in 7.5 format (7.5 format means up to 7 digits before the decimal and exactly 5 digits after the decimal)
 * @discussion Instead of storing order prices as a `double` or `float`,
 * the `Price` struct stores the integer part and fractional part separately. This
 * is to avoid potential precision loss due to using floating point types.
 * `Price` does not support arithmetic operations like addition, subtraction, etc.
 * because they are not needed for this application, though they can be easily added
 * in the future.
 */
struct Price {
    /** @brief Integer part of the price. We need to store 7 decimal digits */
    uint32_t intPart;
    /** @brief Fractional part of the price. We need to store 5 decimal digits */
    uint32_t fracPart;

    /** @brief Compare against another `Price` */
    std::strong_ordering operator<=>(const Price& other) const;

    /** @brief Default equality operator */
    bool operator==(const Price& other) const = default;

    /** @brief Operator to parse input into a `Price` instance */
    friend std::istream& operator>>(std::istream& in, Price& price);
};

/**
 * @brief Convert a `Price` instance to a string
 * @discussion This is intentionally named the same as `std::to_string()` in order to take advantage of ADL.
 */
std::string
to_string(const Price& p);

#endif /* Price_hpp */
