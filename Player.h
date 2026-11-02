#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Ticker.h"

class Player {
private:
	std::string m_name;
	std::map<std::string, double> m_portofolio; // Changed to double
	std::map<std::string, std::pair<double, int>> m_sellmap; // Changed to double
	std::map<std::string, std::pair<double, int>> m_buymap;  // Changed to double
	double m_capital; // Changed to double
	int m_behaviour;

public:
	Player(std::string n);
	Player(std::string n, double c); // Changed parameter to double

	void setBehaviour(int b);
	int getBehaviour() const;

	void setCapital(double c);      // Changed parameter to double
	double getCapital() const;      // Changed return to double
	std::string getName() const;

	void addBuyOrder(std::string t, double c, unsigned int d);   // Changed to double
	void addSellOrder(std::string t, double v, unsigned int d);  // Changed to double

	void deleteSellOrder(std::string t);
	void deleteBuyOrder(std::string t);
	void showOrders();

	double calculateNetWorth(const std::vector<Ticker>& tickers) const;

	std::map<std::string, std::pair<double, int>> getBuyOrders();   // Changed to double
	std::map<std::string, std::pair<double, int>> getSellOrders();  // Changed to double
	const std::map<std::string, double>& getPortfolio() const;      // Changed to double

	void addPortofolio(std::string t, double v);                    // Changed to double
	void makeDecision(const std::vector<Ticker>& tickers, unsigned int currentDay);
};