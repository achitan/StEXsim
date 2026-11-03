#ifndef MARKET_EXCEPTION_H
#define MARKET_EXCEPTION_H

#include <exception>
#include <string>

// Base class for all simulator exceptions
class MarketException : public std::exception {
protected:
    std::string m_message;
public:
    explicit MarketException(const std::string& message) : m_message(message) {}
    virtual ~MarketException() noexcept {}

    virtual const char* what() const noexcept override {
        return m_message.c_str();
    }
};

// Thrown when a player tries to buy or short sell without enough capital/assets
class InsufficientFundsException : public MarketException {
public:
    explicit InsufficientFundsException(const std::string& playerName, double required, double available)
        : MarketException("[Insufficient Funds] Player '" + playerName +
            "' required " + std::to_string(required) +
            " but only had " + std::to_string(available)) {}
};

// Thrown when an order has negative or zero quantity/price
class InvalidOrderException : public MarketException {
public:
    explicit InvalidOrderException(const std::string& details)
        : MarketException("[Invalid Order] " + details) {}
};

// Thrown when a requested ticker cannot be located in the market
class TickerNotFoundException : public MarketException {
public:
    explicit TickerNotFoundException(const std::string& tickerName)
        : MarketException("[Ticker Not Found] Ticker '" + tickerName + "' does not exist in the market.") {}
};

#endif // MARKET_EXCEPTION_H