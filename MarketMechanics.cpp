#include "MarketMechanics.h"
#include <iostream>
#include <algorithm>

MarketMechanics::MarketMechanics(long long marketPace) {
    this->m_marketPace = marketPace;
    this->m_nextOrderId = 1;
}

void MarketMechanics::setMarketPace(long long pace) {
    this->m_marketPace = pace;
}

long long MarketMechanics::getMarketPace() const {
    return this->m_marketPace;
}

void MarketMechanics::addOrder(const std::string& ticker, double price, long long quantity, bool isBuy, const std::string& playerName) {
    std::lock_guard<std::mutex> lock(m_orderBookMutex); // Thread-safe order book push
    Order ord{ m_nextOrderId++, playerName, price, quantity, isBuy };
    if (isBuy) {
        m_bidBooks[ticker].push(ord);
    }
    else {
        m_askBooks[ticker].push(ord);
    }
}

void MarketMechanics::processBook(std::vector<Ticker>& tickers, std::vector<Player>& players) {
    long long processedTransactions = 0;

    // Helper lambda to find players easily by name
    auto findPlayer = [&](const std::string& name) -> Player* {
        for (auto& p : players) {
            if (p.getName() == name) {
                return &p;
            }
        }
        return nullptr;
        };

    for (auto& ticker : tickers) {
        std::string tName = ticker.getName();

        while (processedTransactions < m_marketPace &&
            !m_bidBooks[tName].empty() &&
            !m_askBooks[tName].empty()) {

            Order bestBid = m_bidBooks[tName].top();
            Order bestAsk = m_askBooks[tName].top();

            if (bestBid.price >= bestAsk.price) {
                m_bidBooks[tName].pop();
                m_askBooks[tName].pop();

                long long tradeQty = std::min(bestBid.quantity, bestAsk.quantity);
                float executionPrice = bestAsk.price;

                // 1. Update Ticker Price & Volume
                ticker.emitVolume(tradeQty, executionPrice);

                // 2. Locate Buyer and Seller
                Player* buyer = findPlayer(bestBid.playerName);
                Player* seller = findPlayer(bestAsk.playerName);

                double totalCost = tradeQty * executionPrice;

                // 3. Settlement for Buyer
                if (buyer) {
                    buyer->setCapital(buyer->getCapital() - totalCost);
                    buyer->addPortofolio(tName, static_cast<double>(tradeQty));
                }

                // 4. Settlement for Seller (Handles short-selling automatically if portfolio goes negative)
                if (seller) {
                    seller->setCapital(seller->getCapital() + totalCost);
                    seller->addPortofolio(tName, -static_cast<double>(tradeQty));
                }

                processedTransactions++;

                // Handle partial fills
                if (bestBid.quantity > tradeQty) {
                    bestBid.quantity -= tradeQty;
                    m_bidBooks[tName].push(bestBid);
                }
                if (bestAsk.quantity > tradeQty) {
                    bestAsk.quantity -= tradeQty;
                    m_askBooks[tName].push(bestAsk);
                }
            }
            else {
                break;
            }
        }

        if (processedTransactions >= m_marketPace) {
            break;
        }
    }
}