#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Ticker.h"

class Player {
private:
	std::string m_name;
	std::map<std::string, double> m_portofolio;
	std::map<std::string, std::pair<double, int>> m_sellmap;
	std::map<std::string, std::pair<double, int>> m_buymap;
	double m_capital;
	int m_behaviour;

public:
	Player(std::string n);
	Player(std::string n, double c);

	void setBehaviour(int b);
	int getBehaviour() const;

	void setCapital(double c);
	double getCapital() const;
	std::string getName() const;

	void addBuyOrder(std::string t, double c, unsigned int d);
	void addSellOrder(std::string t, double v, unsigned int d);

	void deleteSellOrder(std::string t);
	void deleteBuyOrder(std::string t);
	void showOrders();

	double calculateNetWorth(const std::vector<Ticker>& tickers) const;

	std::map<std::string, std::pair<double, int>> getBuyOrders();
	std::map<std::string, std::pair<double, int>> getSellOrders();
	const std::map<std::string, double>& getPortfolio() const;

	void addPortofolio(std::string t, double v);
	void makeDecision(const std::vector<Ticker>& tickers, unsigned int currentDay);
};