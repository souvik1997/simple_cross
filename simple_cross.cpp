/*
 SimpleCross - a process that matches internal orders

 Overview:
 * Accept/remove orders as they are entered and keep a book of
 resting orders
 * Determine if an accepted order would be satisfied by previously
 accepted orders (i.e. a buy would cross a resting sell)
 * Output (print) crossing events and remove completed (fully filled)
 orders from the book

 Inputs:
 A string of space separated values representing an action.  The number of
 values is determined by the action to be performed and have the following
 format:

 ACTION [OID [SYMBOL SIDE QTY PX]]

 ACTION: single character value with the following definitions
 O - place order, requires OID, SYMBOL, SIDE, QTY, PX
 X - cancel order, requires OID
 P - print sorted book (see example below)

 OID: positive 32-bit integer value which must be unique for all orders

 SYMBOL: alpha-numeric string value. Maximum length of 8.

 SIDE: single character value with the following definitions
 B - buy
 S - sell

 QTY: positive 16-bit integer value

 PX: positive double precision value (7.5 format)

 Outputs:
 A list of strings of space separated values that show the result of the
 action (if any).  The number of values is determined by the result type and
 have the following format:

 RESULT OID [SYMBOL [SIDE] (FILL_QTY | OPEN_QTY) (FILL_PX | ORD_PX)]

 RESULT: single character value with the following definitions
 F - fill (or partial fill), requires OID, SYMBOL, FILL_QTY, FILL_PX
 X - cancel confirmation, requires OID
 P - book entry, requires OID, SYMBOL, SIDE, OPEN_QTY, ORD_PX (see example below)
 E - error, requires OID. Remainder of line represents string value description of the error

 FILL_QTY: positive 16-bit integer value representing qty of the order filled by
 this crossing event

 OPEN_QTY: positive 16-bit integer value representing qty of the order not yet filled

 FILL_PX:  positive double precision value representing price of the fill of this
 order by this crossing event (7.5 format)

 ORD_PX:   positive double precision value representing original price of the order (7.5 format)
 (7.5 format means up to 7 digits before the decimal and exactly 5 digits after the decimal)

 Conditions/Assumptions:
 * The implementation should be a standalone Linux console application (include
 source files, testing tools and Makefile in submission)
 * The use of third party libraries is not permitted.
 * The app should respond to malformed input and other errors with a RESULT
 of type 'E' and a descriptive error message
 * Development should be production level quality. Design and
 implementation choices should be documented
 * Performance is always a concern in software, but understand that this is an unrealistic test.
 Only be concerned about performance where it makes sense to the important sections of this application (i.e. reading actions.txt is not important).
 * All orders are standard limit orders (a limit order means the order remains in the book until it
 is either canceled, or fully filled by order(s) for its same symbol on the opposite side with an
 equal or better price).
 * Orders should be selected for crossing using price-time (FIFO) priority
 * Orders for different symbols should not cross (i.e. the book must support multiple symbols)

 Example session:
 INPUT                                   | OUTPUT
 ============================================================================
 "O 10000 IBM B 10 100.00000"            | results.size() == 0
 "O 10001 IBM B 10 99.00000"             | results.size() == 0
 "O 10002 IBM S 5 101.00000"             | results.size() == 0
 "O 10003 IBM S 5 100.00000"             | results.size() == 2
 | results[0] == "F 10003 IBM 5 100.00000"
 | results[1] == "F 10000 IBM 5 100.00000"
 "O 10004 IBM S 5 100.00000"             | results.size() == 2
 | results[0] == "F 10004 IBM 5 100.00000"
 | results[1] == "F 10000 IBM 5 100.00000"
 "X 10002"                               | results.size() == 1
 | results[0] == "X 10002"
 "O 10005 IBM B 10 99.00000"             | results.size() == 0
 "O 10006 IBM B 10 100.00000"            | results.size() == 0
 "O 10007 IBM S 10 101.00000"            | results.size() == 0
 "O 10008 IBM S 10 102.00000"            | results.size() == 0
 "O 10008 IBM S 10 102.00000"            | results.size() == 1
 | results[0] == "E 10008 Duplicate order id"
 "O 10009 IBM S 10 102.00000"            | results.size() == 0
 "P"                                     | results.size() == 6
 | results[0] == "P 10009 IBM S 10 102.00000"
 | results[1] == "P 10008 IBM S 10 102.00000"
 | results[2] == "P 10007 IBM S 10 101.00000"
 | results[3] == "P 10006 IBM B 10 100.00000"
 | results[4] == "P 10001 IBM B 10 99.00000"
 | results[5] == "P 10005 IBM B 10 99.00000"
 "O 10010 IBM B 13 102.00000"            | results.size() == 4
 | results[0] == "F 10010 IBM 10 101.00000"
 | results[1] == "F 10007 IBM 10 101.00000"
 | results[2] == "F 10010 IBM 3 102.00000"
 | results[3] == "F 10008 IBM 3 102.00000"

 So, for the example actions.txt, the desired output from the application with the below main is:
 F 10003 IBM 5 100.00000
 F 10000 IBM 5 100.00000
 F 10004 IBM 5 100.00000
 F 10000 IBM 5 100.00000
 X 10002
 E 10008 Duplicate order id
 P 10009 IBM S 10 102.00000
 P 10008 IBM S 10 102.00000
 P 10007 IBM S 10 101.00000
 P 10006 IBM B 10 100.00000
 P 10001 IBM B 10 99.00000
 P 10005 IBM B 10 99.00000
 F 10010 IBM 10 101.00000
 F 10007 IBM 10 101.00000
 F 10010 IBM 3 102.00000
 F 10008 IBM 3 102.00000

 */

// Stub implementation and example driver for SimpleCross.
// Your crossing logic should be accesible from the SimpleCross class.
// Other than the signature of SimpleCross::action() you are free to modify as needed.
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <variant>

#include "Order.hpp"
#include "Price.hpp"
#include "simple_cross.hpp"

/**
 * @brief Result for parsing input
 */
enum class InputParseResult {
    /** @brief Input was parsed successfully */
    Success,
    /** @brief Input not parsed successfully: reached end of input */
    EndOfFile,
    /** @brief Input not parsed successfully: bad input */
    BadInput
};

/**
 * @brief Parse the next item from the stream into the specified type instance
 * @param sstream  `std::stringstream` to parse from
 * @param result Result to store into
 * @return The parse result
 */
template <typename T>
static InputParseResult parse(std::stringstream& sstream, T& result)
{
    if (sstream.eof()) {
        return InputParseResult::EndOfFile;
    }
    if constexpr (std::is_unsigned_v<T>) {
        std::string current;
        sstream >> current;
        if (current.size() >= 1 && *current.begin() == '-') {
            sstream.setstate(std::ios::failbit);
        }
        std::stringstream current_str(current);
        current_str >> result;
        if (current_str.fail()) {
            sstream.setstate(std::ios::failbit);
        }
    } else {
        sstream >> result;
    }
    if (sstream.fail()) {
        return InputParseResult::BadInput;
    }
    return InputParseResult::Success;
}

/**
 * @brief Parse the next item from the stream into the specified character instance
 * @discussion Explicit template specialization of `InputParseResult parse(std::stringstream, T& result)`
 *  in order to handle the case of multi-character words. This specialization
 *  adds a check to ensure the parsed word has only 1 character.
 * @param sstream  `std::stringstream` to parse from
 * @param result Result to store into
 * @return The parse result
 */
template <>
InputParseResult parse(std::stringstream& sstream, char& result)
{
    std::string resultStr;
    if (sstream.eof()) {
        return InputParseResult::EndOfFile;
    }
    sstream >> resultStr;
    if (sstream.fail()) {
        return InputParseResult::BadInput;
    }
    if (resultStr.size() != 1) {
        return InputParseResult::BadInput;
    }
    result = *resultStr.begin();
    return InputParseResult::Success;
}

/**
 * @brief Returns whether the stream has reached the end
 * @param sstream The stream
 * @return Whether the stream has reached the end
 */
static bool reachedEnd(std::stringstream& sstream)
{
    if (sstream.eof()) {
        return true;
    }
    /* Ignore whitespace characters */
    sstream >> std::noskipws;
    char c;
    while (sstream >> c) {
        if (!std::isspace(c)) {
            return false;
        }
    }
    return true;
}

constexpr size_t MAX_SYMBOL_SIZE = 8;
results_t SimpleCross::action(const std::string& line)
{
    using std::to_string;

    std::list<std::string> outputs;
    std::stringstream ss(line);
    if (line.size() == 0) {
        /* empty line */
        outputs.push_back("");
        return outputs;
    }
    char action;
    switch (parse(ss, action)) {
    case InputParseResult::Success:
        break;
    case InputParseResult::BadInput:
        outputs.push_back("E Action is malformed");
        return outputs;
    case InputParseResult::EndOfFile:
        outputs.push_back("E Expected action in input");
        return outputs;
    }

    if (action == 'O') {
        /* open order */
        OID oid;
        std::string symbol;
        char sideCh;
        OrderSide side = OrderSide::Buy;
        uint16_t quantity;
        Price price;

        /* parse order ID */
        switch (parse(ss, oid)) {
        case InputParseResult::Success:
            break;
        case InputParseResult::BadInput:
            outputs.push_back("E OID is malformed");
            return outputs;
        case InputParseResult::EndOfFile:
            outputs.push_back("E Expected OID in input");
            return outputs;
        }
        
        if (oid == 0) {
            outputs.push_back("E Expected positive OID");
            return outputs;
        }

        /* parse symbol */
        switch (parse(ss, symbol)) {
        case InputParseResult::Success:
            if (symbol.size() > MAX_SYMBOL_SIZE) {
                outputs.push_back("E Symbol size exceeds max symbol size");
                return outputs;
            }
            break;
        case InputParseResult::BadInput:
            outputs.push_back("E Symbol is malformed");
            return outputs;
        case InputParseResult::EndOfFile:
            outputs.push_back("E Expected symbol in input");
            return outputs;
        }

        /* check if alphanumeric */
        if (std::find_if(symbol.begin(), symbol.end(), [](char c) { return !isalnum(c); }) != symbol.end()) {
            outputs.push_back("E Symbol is not alphanumeric");
            return outputs;
        }

        /* parse side */
        switch (parse(ss, sideCh)) {
        case InputParseResult::Success:
            if (sideCh != 'B' && sideCh != 'S') {
                outputs.push_back("E Side must be either 'B' or 'S'");
                return outputs;
            }
            side = sideCh == 'B' ? OrderSide::Buy : OrderSide::Sell;
            break;
        case InputParseResult::BadInput:
            outputs.push_back("E Side is malformed");
            return outputs;
        case InputParseResult::EndOfFile:
            outputs.push_back("E Expected side in input");
            return outputs;
        }

        /* parse quantity */
        switch (parse(ss, quantity)) {
        case InputParseResult::Success:
            break;
        case InputParseResult::BadInput:
            outputs.push_back("E Quantity is malformed");
            return outputs;
        case InputParseResult::EndOfFile:
            outputs.push_back("E Expected quantity in input");
            return outputs;
        }

        if (quantity == 0) {
            outputs.push_back("E Expected positive quantity in input");
            return outputs;
        }

        /* parse price */
        switch (parse(ss, price)) {
        case InputParseResult::Success:
            break;
        case InputParseResult::BadInput:
            outputs.push_back("E Price is malformed");
            return outputs;
        case InputParseResult::EndOfFile:
            outputs.push_back("E Expected price in input");
            return outputs;
        }

        /* expect end of input */
        if (!reachedEnd(ss)) {
            outputs.push_back("E Expected end of input");
            return outputs;
        }

        /* create the order and insert it into `activeOrders` */
        auto [orderIt, inserted] = activeOrders.emplace(std::make_pair(oid, Order(oid, symbol, side, quantity, price)));
        if (!inserted) {
            /* order with the same ID already exists */
            outputs.push_back("E " + to_string(oid) + " Duplicate order id");
            return outputs;
        }
        /* get the order we just inserted */
        auto& [_, order] = *orderIt;

        /* get the OrderBook for this symbol */
        auto& bookForSymbol = books[order.symbol];
        /* get the opposite side of the orders */
        auto& oppositeOrders = order.side == OrderSide::Buy ? bookForSymbol.sells : bookForSymbol.buys;

        /* while we still have shares in the current order and orders to match against */
        while (order.quantity > 0 && !oppositeOrders.empty()) {
            /* get the first match */
            auto match = oppositeOrders.begin();

            /* check if trade can be executed */
            if ((order.side == OrderSide::Buy && order.price >= (*match)->price) || (order.side == OrderSide::Sell && order.price <= (*match)->price)) {

                uint16_t filledQty = std::min(order.quantity, (*match)->quantity);
                /* report fill */
                outputs.push_back("F " + to_string(order.oid) + " " + order.symbol + " " + to_string(filledQty) + " " + to_string((*match)->price));
                outputs.push_back("F " + to_string((*match)->oid) + " " + (*match)->symbol + " " + to_string(filledQty) + " " + to_string((*match)->price));

                /* subtract filled quantity */
                order.quantity -= filledQty;

                /* check if match still has shares */
                if ((*match)->quantity == filledQty) {
                    /* if not, delete the match */
                    auto book_iter = *(*match)->book_iter;

                    (*match)->quantity = 0;
                    (*match)->book_iter = std::nullopt;
                    oppositeOrders.erase(book_iter);
                } else {
                    /* subtract the filled quantity */
                    (*match)->quantity -= filledQty;
                }
            } else {
                break;
            }
        }

        /* check if there are any shares left in the order that were not filled */
        if (order.quantity > 0) {
            /* add remaining to order book */
            if (order.side == OrderSide::Buy) {
                const auto [book_iter, __] = bookForSymbol.buys.insert(&order);
                order.book_iter = book_iter;
            } else {
                const auto [book_iter, __] = bookForSymbol.sells.insert(&order);
                order.book_iter = book_iter;
            }
        }

    } else if (action == 'X') {
        OID oid;
        switch (parse(ss, oid)) {
        case InputParseResult::Success:
            break;
        case InputParseResult::BadInput:
            outputs.push_back("E OID is malformed");
            return outputs;
        case InputParseResult::EndOfFile:
            outputs.push_back("E Expected OID in input");
            return outputs;
        }
        
        if (oid == 0) {
            outputs.push_back("E Expected positive OID");
            return outputs;
        }

        if (!reachedEnd(ss)) {
            outputs.push_back("E Expected end of input");
            return outputs;
        }

        auto found = activeOrders.find(oid);
        if (found != activeOrders.end()) {
            auto& order = found->second;
            if (order.quantity == 0) {
                /* already filled */
                outputs.push_back("E Already filled order " + to_string(oid));
                return outputs;
            }
            if (order.book_iter != std::nullopt) {
                auto& bookForSymbol = books[order.symbol];
                if (order.side == OrderSide::Buy) {
                    bookForSymbol.buys.erase(*order.book_iter);
                } else if (order.side == OrderSide::Sell) {
                    bookForSymbol.sells.erase(*order.book_iter);
                }
                order.book_iter = std::nullopt;
                outputs.push_back("X " + to_string(oid));
            } else {
                /* already canceled */
                outputs.push_back("E Already canceled order " + to_string(oid));
                return outputs;
            }
        }

    } else if (action == 'P') {
        if (!reachedEnd(ss)) {
            outputs.push_back("E Expected end of input");
            return outputs;
        }
        for (const auto& [symbol, book] : books) {
            /* sells in reverse order */
            for (auto sellsIt = book.sells.rbegin(); sellsIt != book.sells.rend(); ++sellsIt) {
                const auto& order = **sellsIt;
                outputs.push_back("P " + to_string(order.oid) + " " + order.symbol + " S " + to_string(order.quantity) + " " + to_string(order.price));
            }
            for (const auto& orderPtr : book.buys) {
                const auto& order = *orderPtr;
                outputs.push_back("P " + to_string(order.oid) + " " + order.symbol + " B " + to_string(order.quantity) + " " + to_string(order.price));
            }
        }
    } else {
        outputs.push_back("E Unknown action " + std::string(1, action));
    }
    return outputs;
}

static int readActions(std::istream& actions)
{
    SimpleCross scross;
    std::string line;
    while (std::getline(actions, line)) {
        results_t results = scross.action(line);
        for (results_t::const_iterator it = results.begin(); it != results.end(); ++it) {
            std::cout << *it << std::endl;
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    if (argc == 2) {
        if (strncmp(argv[1], "-", strlen("-")) == 0) {
            /* read from stdin */
            return readActions(std::cin);
        } else {
            auto actions = std::ifstream(argv[1], std::ios::in);
            if (actions.fail()) {
                std::cerr << "Failed to read " << argv[1] << std::endl;
                return 1;
            }
            return readActions(actions);
        }
    } else {
        /* look for actions.txt in the current directory */
        auto actions = std::ifstream("actions.txt", std::ios::in);
        if (actions.fail()) {
            std::cerr << "Failed to read actions.txt" << std::endl;
            return 1;
        }
        return readActions(actions);
    }
}
