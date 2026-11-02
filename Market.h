#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>

#include "Ticker.h"
#include "Player.h"

class Market {
private:
	std::vector<Ticker> m_tickerV;
	std::vector<Player> m_playerV;
	std::map<std::string, std::vector<float>> m_priceHistory;
	std::vector<float> m_percCap;
	std::vector<float> m_percPrice;
	std::vector<float> m_volatility;
	std::vector<float> m_coherence;
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
	std::vector<float> getVolatility();
	std::vector<float> getpercPrice();
	std::vector<float> getpercCap();
	std::vector<float> getCoherence();
	void readMarket(Player p);
};