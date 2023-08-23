//
//  Order.cpp
//  simple_cross
//
//  Created by Souvik Banerjee on 8/21/23.
//

#include "Order.hpp"

Order::Order(OID _oid, std::string _symbol, OrderSide _side, uint16_t _quantity, Price _price)
    : oid(_oid)
    , symbol(_symbol)
    , side(_side)
    , quantity(_quantity)
    , price(_price)
{
}

std::strong_ordering
Order::operator<=>(const Order& other) const
{
    /* Compare on price if it is different */
    if (price != other.price) {
        /* Buy orders should be ordered greatest to least,
         * sell orders are ordered least to greatest */
        return side == OrderSide::Buy ? other.price <=> price : price <=> other.price;
    }
    return oid <=> other.oid;
}
