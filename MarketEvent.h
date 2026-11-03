#ifndef MARKET_EVENT_H
#define MARKET_EVENT_H

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include "Ticker.h"

class MarketEvent {
private:
    static std::unordered_map<std::string, unsigned int> m_lastEventDay;

    static std::map<unsigned int, std::vector<std::pair<std::string, double>>> m_dailyEvents;

public:
    static void processEvents(std::vector<Ticker>& tickers, unsigned int currentDay);

    static const std::map<unsigned int, std::vector<std::pair<std::string, double>>>& getMarketEvents();
};

#endif // MARKET_EVENT_H