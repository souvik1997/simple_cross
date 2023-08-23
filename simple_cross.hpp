//
//  simple_cross.h
//  simple_cross
//
//  Created by Souvik Banerjee on 8/22/23.
//

#ifndef simple_cross_h
#define simple_cross_h

#include <list>
#include <map>
#include <string>

#include "Order.hpp"

/* String output type */
typedef std::list<std::string> results_t;

/**
 * @brief Order book structure
 * @discussion Contains separate structures for buy and sell orders. The orders are referenced by
 * pointer; the actual Order structure is stored in SimpleCross's activeOrders.
 */
struct OrderBook {
    struct OrderPtrComparator {
        inline bool operator()(const Order* lhs, const Order* rhs) const
        {
            return (*lhs) < (*rhs);
        }
    };

    /** @brief Buy orders */
    std::set<Order*, OrderPtrComparator> buys;
    /** @brief Sell orders */
    std::set<Order*, OrderPtrComparator> sells;
};

class SimpleCross {
    /** @brief Mapping from order ID to order */
    std::map<OID, Order> activeOrders;

    /** @brief Mapping from symbol to `OrderBook` */
    std::map<std::string, OrderBook> books;

public:
    results_t action(const std::string& line);
};

#endif /* simple_cross_h */
