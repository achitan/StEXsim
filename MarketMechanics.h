#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex> // Required for thread synchronization
#include "Ticker.h"
#include "Player.h"

struct Order {
    int orderId;
    std::string playerName;
    double price;
    double quantity;
    bool isBuy;
};

struct CompareBids {
    bool operator()(const Order& a, const Order& b) {
        return a.price < b.price;
    }
};

struct CompareAsks {
    bool operator()(const Order& a, const Order& b) {
        return a.price > b.price;
    }
};

class MarketMechanics {
private:
    long long m_marketPace;
    std::map<std::string, std::priority_queue<Order, std::vector<Order>, CompareBids>> m_bidBooks;
    std::map<std::string, std::priority_queue<Order, std::vector<Order>, CompareAsks>> m_askBooks;
    int m_nextOrderId;

    std::mutex m_orderBookMutex; // Protects order book queues during concurrent additions

public:
    MarketMechanics(long long marketPace);

    void setMarketPace(long long pace);
    long long getMarketPace() const;

    void addOrder(const std::string& ticker, double price, long long quantity, bool isBuy, const std::string& playerName);
    void processBook(std::vector<Ticker>& tickers, std::vector<Player>& players);
};