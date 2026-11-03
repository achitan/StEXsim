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

    for (auto& ticker : tickers) {
        std::string tName = ticker.getName();

        // Match orders as long as we haven't hit the market pace limit and orders exist
        while (processedTransactions < m_marketPace &&
            !m_bidBooks[tName].empty() &&
            !m_askBooks[tName].empty()) {

            Order bestBid = m_bidBooks[tName].top();
            Order bestAsk = m_askBooks[tName].top();

            // Check if a trade can occur (Bid price >= Ask price)
            if (bestBid.price >= bestAsk.price) {
                m_bidBooks[tName].pop();
                m_askBooks[tName].pop();

                long long tradeQty = std::min(bestBid.quantity, bestAsk.quantity);
                double executionPrice = bestAsk.price;

                // Update Ticker Price & Volume (This moves the price!)
                ticker.emitVolume(tradeQty, executionPrice);

                ticker.setPrice(executionPrice);

                // Find buyer and seller objects
                Player* buyer = nullptr;
                Player* seller = nullptr;
                for (auto& p : players) {
                    if (p.getName() == bestBid.playerName) buyer = &p;
                    if (p.getName() == bestAsk.playerName) seller = &p;
                }

                // Transfer cash and shares between buyer and seller
                double totalCost = tradeQty * executionPrice;
                if (buyer) {
                    buyer->addPortofolio(tName, tradeQty);
                    buyer->setCapital(buyer->getCapital() - totalCost); // Deduct cash from buyer
                }
                if (seller) {
                    seller->setCapital(seller->getCapital() + totalCost); // Add cash to seller via arrow operator ->
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
                // No cross between highest bid and lowest ask for this ticker
                break;
            }
        }

        if (processedTransactions >= m_marketPace) {
            break; // Market throughput ceiling reached for this turn
        }
    }
}