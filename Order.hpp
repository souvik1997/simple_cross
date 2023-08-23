//
//  Order.hpp
//  simple_cross
//
//  Created by Souvik Banerjee on 8/21/23.
//

#ifndef Order_hpp
#define Order_hpp

#include <compare>
#include <cstdint>
#include <optional>
#include <set>
#include <string>

#include "Price.hpp"

/**
 * @brief Enum for order sides.
 * @discussion Orders can be either Buy or Sell orders
 */
enum class OrderSide {
    Buy,
    Sell
};

/** @brief Convenient type alias for order ID (OID) */
using OID = uint32_t;

/**
 * @brief Data structure representing an order
 */
struct Order {
    /** @brief Order ID */
    OID oid;
    /** @brief Order symbol */
    std::string symbol;
    /** @brief Order side (either buy or sell) */
    OrderSide side;
    /** @brief Order quantity */
    uint16_t quantity;
    /** @brief Order price */
    Price price;

    /** @brief Iterator referencing where this order is stored in the order book. */
    std::optional<std::set<Order*>::iterator> book_iter;

    Order(OID oid, std::string symbol, OrderSide side, uint16_t quantity, Price price);

    /** @brief Compare against another `Order` instance */
    std::strong_ordering operator<=>(const Order& other) const;
};

#endif /* Order_hpp */
