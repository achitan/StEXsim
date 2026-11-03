#include "MarketEvent.h"
#include <random>
#include <iostream>

std::unordered_map<std::string, unsigned int> MarketEvent::m_lastEventDay;
std::map<unsigned int, std::vector<std::pair<std::string, double>>> MarketEvent::m_dailyEvents;

void MarketEvent::processEvents(std::vector<Ticker>& tickers, unsigned int currentDay) {
    std::random_device rd;
    std::default_random_engine re(rd());

    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    std::uniform_real_distribution<double> impactDist(-0.15, 0.15);

    for (auto& ticker : tickers) {
        std::string tName = ticker.getName();

        bool pastCooldown = true;
        if (m_lastEventDay.find(tName) != m_lastEventDay.end()) {
            if (currentDay < m_lastEventDay[tName] + 30) {
                pastCooldown = false;
            }
        }

        if (pastCooldown && (probDist(re) < 0.01)) {
            double impactFactor = impactDist(re); // e.g., 0.02, -0.14, etc.
            double currentPrice = ticker.getPrice();
            double newPrice = currentPrice * (1.0 + impactFactor);

            if (newPrice < 0.01) {
                newPrice = 0.01;
            }

            ticker.setPrice(newPrice);
            m_lastEventDay[tName] = currentDay;

            m_dailyEvents[currentDay].push_back({ tName, impactFactor });

            //std::cout << "[MARKET EVENT] Day " << currentDay
            //    << " | Shock on Ticker [" << tName
            //    << "] -> Price changed by " << (impactFactor * 100.0)
            //    << "% (New Price: " << newPrice << ")" << std::endl;
        }
    }
}

const std::map<unsigned int, std::vector<std::pair<std::string, double>>>& MarketEvent::getMarketEvents() {
    return m_dailyEvents;
}