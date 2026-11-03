#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <thread>

#include "Ticker.h"
#include "Player.h"
#include "MarketMechanics.h"

class Market {
private:
	MarketMechanics m_mechanics;
	std::vector<Ticker> m_tickerV;
	std::vector<Player> m_playerV;
	std::map<std::string, std::vector<double>> m_priceHistory;
	std::vector<double> m_percCap;
	std::vector<double> m_percPrice;
	std::vector<double> m_volatility;
	std::vector<double> m_coherence;
	void fulfillOrders();
	void recordMarket();
	void updateIndeces();

public:
	static unsigned int date;
	Market(long long n, long m = 100);
	Market(std::vector<Ticker> ts, std::vector<Player> ps);
	Market(std::vector<Ticker> ts);
	void createTickers(long long n);
	void createPlayers(long m);
	void updateMarket();
	void placeOrders();
	unsigned int getDate();
	std::vector<double> getVolatility();
	std::vector<double> getpercPrice();
	std::vector<double> getpercCap();
	std::vector<double> getCoherence();
	const std::vector<Player>& getPlayers() const;
	const std::vector<Ticker>& getTickers() const;
	void readMarket(Player p);
	std::vector<double> getTickerPriceHisto(std::string t);
};