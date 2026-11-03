#include "Player.h"
#include <iostream>
#include <random>
#include <algorithm>

Player::Player(std::string n) {
	this->m_name = n;
	this->m_capital = 0.0;
	this->m_behaviour = 7;
}

Player::Player(std::string n, double c) {
	this->m_name = n;
	this->m_capital = c;
	this->m_behaviour = 7;
}

void Player::setCapital(double c) {
	this->m_capital = c;
}

double Player::getCapital() const {
	return this->m_capital;
}

void Player::addBuyOrder(std::string t, double c, unsigned int d) {
	this->m_buymap[t].first += c;
	this->m_buymap[t].second = d;
	this->m_capital -= c;
}

void Player::addSellOrder(const std::string& tickerName, double quantity, unsigned int targetDay) {
	double sharesOwned = 0.0;
	auto it = m_portofolio.find(tickerName);
	if (it != m_portofolio.end()) {
		sharesOwned = it->second;
	}

	// Short-selling validation (if selling more than you own)
	if (quantity > sharesOwned) {
		double shortShares = quantity - sharesOwned;
		double estimatedCollateralNeeded = shortShares * 10.0; // Adjust collateral rule if needed
		if (m_capital < estimatedCollateralNeeded) {
			return; // Reject order if not enough capital to back the short
		}
	}

	this->m_sellmap[tickerName].first += quantity;
	this->m_sellmap[tickerName].second = targetDay;
}

void Player::deleteSellOrder(std::string t) {
	if (t.compare("*") == 0)
		this->m_sellmap.clear();
	else
		this->m_sellmap.erase(t);
}

void Player::deleteBuyOrder(std::string t) {
	if (t.compare("*") == 0)
		this->m_buymap.clear();
	else
		this->m_buymap.erase(t);
}

void Player::showOrders() {
	std::cout << "=====BUY ORDERS FOR " << this->m_name << "=====" << std::endl;
	for (auto& ord : this->m_buymap) {
		std::string capitalstr = std::to_string(ord.second.first);
		size_t idx = capitalstr.length();
		if (idx > 3) {
			idx -= 3;
			while (idx > 0 && idx <= capitalstr.length()) {
				capitalstr.insert(idx, "\'");
				idx -= 3;
			}
		}
		std::cout << ord.first << " for " << capitalstr << " dollars." << std::endl;
	}
	std::cout << "====================" << std::endl;
	std::cout << "=====SELL ORDERS FOR " << this->m_name << "=====" << std::endl;
	for (auto& ord : this->m_sellmap) {
		std::string capitalstr = std::to_string(ord.second.first);
		size_t idx = capitalstr.length();
		if (idx > 3) {
			idx -= 3;
			while (idx > 0 && idx <= capitalstr.length()) {
				capitalstr.insert(idx, "\'");
				idx -= 3;
			}
		}
		std::cout << ord.first << " for " << capitalstr << " stocks." << std::endl;
	}
	std::cout << "=====================" << std::endl;
}

std::map<std::string, std::pair<double, int>> Player::getBuyOrders() {
	return this->m_buymap;
}

std::map<std::string, std::pair<double, int>> Player::getSellOrders() {
	return this->m_sellmap;
}

const std::map<std::string, double>& Player::getPortfolio() const {
	return this->m_portofolio;
}

void Player::addPortofolio(std::string t, double v) {
	this->m_portofolio[t] += v;
}

void Player::setBehaviour(int b) {
	this->m_behaviour = b;
}

int Player::getBehaviour() const {
	return this->m_behaviour;
}

std::string Player::getName() const {
	return this->m_name;
}

double Player::calculateNetWorth(const std::vector<Ticker>& tickers) const {
	double totalNetWorth = this->m_capital;

	for (const auto& holding : this->m_portofolio) {
		std::string tickerName = holding.first;
		double quantity = holding.second; // Can be negative if short!

		// Find current market price of the ticker
		auto it = std::find_if(tickers.begin(), tickers.end(), [&](const Ticker& t) {
			return t.getName() == tickerName;
			});

		if (it != tickers.end()) {
			totalNetWorth += quantity * it->getPrice();
			// If quantity is negative (short position), holding quantity * price 
			// correctly subtracts the liability from their net worth!
		}
	}

	return totalNetWorth;
}

void Player::makeDecision(const std::vector<Ticker>& tickers, unsigned int currentDay) {
	if (tickers.empty() || this->m_capital <= 100.0) return;

	std::random_device rd;
	std::default_random_engine re(rd());

	// 1. LIQUIDITY INJECTION: If the player owns stocks, give them a 30% chance to sell some!
	if (!this->m_portofolio.empty() && std::uniform_int_distribution<int>(0, 3)(re) == 0) {
		auto holdingIt = this->m_portofolio.begin();
		std::string tickerName = holdingIt->first;
		double sharesOwned = holdingIt->second;

		if (sharesOwned > 1.0) {
			// Sell a random portion of their holdings
			double sellQty = std::uniform_real_distribution<double>(1.0, sharesOwned / 2.0)(re);

			auto it = std::find_if(tickers.begin(), tickers.end(), [&](const Ticker& t) {
				return t.getName() == tickerName;
				});

			if (it != tickers.end()) {
				// Submit sell order
				this->addSellOrder(tickerName, sellQty, currentDay + 3);
				return; // Handled this turn's action
			}
		}
	}

	// 2. Standard Buying Behaviors (Noise, Momentum, Value)
	switch (this->m_behaviour) {
	case 1: { // NOISE / RETAIL TRADER
		std::uniform_int_distribution<size_t> distTick(0, tickers.size() - 1);
		std::uniform_real_distribution<double> distCap(50.0, this->m_capital / 5.0);

		const auto& chosenTicker = tickers[distTick(re)];
		double spend = distCap(re);

		double targetPrice = chosenTicker.getPrice() * (1.0 + (std::uniform_real_distribution<double>(-0.05, 0.05)(re)));
		double quantity = spend / targetPrice;

		if (quantity > 0.0 && this->m_capital >= spend) {
			this->addBuyOrder(chosenTicker.getName(), spend, currentDay + 3);
		}
		break;
	}
	case 2: { // MOMENTUM TRADER
		auto bestTickerIt = std::max_element(tickers.begin(), tickers.end(), [](const Ticker& a, const Ticker& b) {
			return a.getPrice() < b.getPrice();
			});

		if (bestTickerIt != tickers.end() && this->m_capital > 500.0) {
			double spend = this->m_capital / 4.0;
			this->addBuyOrder(bestTickerIt->getName(), spend, currentDay + 2);
		}
		break;
	}
	case 3: { // VALUE INVESTOR
		auto cheapestTickerIt = std::min_element(tickers.begin(), tickers.end(), [](const Ticker& a, const Ticker& b) {
			return a.getPrice() < b.getPrice();
			});

		if (cheapestTickerIt != tickers.end() && this->m_capital > 200.0) {
			double spend = this->m_capital / 3.0;
			this->addBuyOrder(cheapestTickerIt->getName(), spend, currentDay + 5);
		}
		break;
	}
	case 4: { // INVESTMENT BANK: Massive capital, heavy weight, executes block orders
		if (this->m_capital < 1000000) break; // Needs heavy capital to operate

		// Select a high-value or random target ticker
		std::uniform_int_distribution<size_t> distTick(0, tickers.size() - 1);
		const auto& chosenTicker = tickers[distTick(re)];

		// Large block order capital allocation (e.g., 10% to 25% of massive capital)
		std::uniform_real_distribution<double> distBlockAlloc(0.10, 0.25);
		long long heavySpend = static_cast<long long>(this->m_capital * distBlockAlloc(re));

		double targetPrice = chosenTicker.getPrice();
		long long heavyQty = static_cast<long long>(heavySpend / targetPrice);

		if (heavyQty > 0 && this->m_capital >= heavySpend) {
			// Places a heavy buy order that shifts market depth
			this->addBuyOrder(chosenTicker.getName(), heavySpend, currentDay + 1);
		}
		break;
	}
	default: {
		break;
	}
	}
}