//
//  Price.cpp
//  simple_cross
//
//  Created by Souvik Banerjee on 8/21/23.
//

#include "Price.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

constexpr size_t INT_PART_DIGITS = 7;
constexpr size_t FRAC_PART_DIGITS = 5;

std::strong_ordering
Price::operator<=>(const Price& other) const
{
    if (intPart != other.intPart) {
        return intPart <=> other.intPart;
    } else {
        return fracPart <=> other.fracPart;
    }
}

std::istream&
operator>>(std::istream& in, Price& price)
{
    /* Get integer and fractional parts as strings */
    std::string intPartStr;
    std::string fracPartStr;
    /* '.' is the separator */
    std::getline(in >> std::ws, intPartStr, '.');
    if (in.fail()) {
        return in;
    }
    in >> fracPartStr;
    if (in.fail()) {
        return in;
    }

    if (intPartStr.size() == 0) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (*intPartStr.begin() == '-') {
        in.setstate(std::ios::failbit);
        return in;
    }

    /* NOTE: The specification requires that prices have 5
     * digits after the decimal point. However the prices in
     * the provided actions.txt do not have 5 digits after
     * the decimal point. actions.txt has been corrected. */
    if (fracPartStr.size() != FRAC_PART_DIGITS) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (*fracPartStr.begin() == '-') {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (intPartStr.size() > INT_PART_DIGITS) {
        in.setstate(std::ios::failbit);
        return in;
    }

    /* Get integer and fractional parts as integers */
    std::istringstream intPartStream(intPartStr);
    std::istringstream fracPartStream(fracPartStr);

    intPartStream >> price.intPart;
    if (intPartStream.fail()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    fracPartStream >> price.fracPart;
    if (fracPartStream.fail()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    return in;
}

std::string
to_string(const Price& p)
{
    std::stringstream ss;
    /* Always print 5 decimal places */
    ss << p.intPart << "." << std::setfill('0') << std::setw(FRAC_PART_DIGITS) << p.fracPart;
    return ss.str();
}
