#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include "Ticker.h"
#include "Player.h"

struct Order {
    int orderId;
    std::string playerName;
    double price;
    double quantity;
    bool isBuy;
};

// Comparator for Bids: Highest price first
struct CompareBids {
    bool operator()(const Order& a, const Order& b) {
        return a.price < b.price;
    }
};

// Comparator for Asks: Lowest price first
struct CompareAsks {
    bool operator()(const Order& a, const Order& b) {
        return a.price > b.price;
    }
};

class MarketMechanics {
private:
    long long m_marketPace; // Max number of order matches processed per turn
    std::map<std::string, std::priority_queue<Order, std::vector<Order>, CompareBids>> m_bidBooks;
    std::map<std::string, std::priority_queue<Order, std::vector<Order>, CompareAsks>> m_askBooks;
    int m_nextOrderId;

public:
    MarketMechanics(long long marketPace);

    void setMarketPace(long long pace);
    long long getMarketPace() const;

    void addOrder(const std::string& ticker, double price, long long quantity, bool isBuy, const std::string& playerName);

    // Processes orders up to m_marketPace limit, updates tickers and player portfolios/capital
    void processBook(std::vector<Ticker>& tickers, std::vector<Player>& players);
};