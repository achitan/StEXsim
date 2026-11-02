#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

class Player {
private:
	std::string m_name;
	std::map<std::string, long long> m_portofolio;
	std::map<std::string, std::pair<long long, int>> m_sellmap;
	std::map<std::string, std::pair<long long, int>> m_buymap;
	long long m_capital;
	int m_behaviour;

public:
	Player(std::string n);
	Player(std::string n, long long c);
	void setBehaviour(int b);
	void setCapital(long long c);
	long long getCapital();
	void addBuyOrder(std::string t, long long c, unsigned int d);
	void addSellOrder(std::string t, long long v, unsigned int d);
	void deleteSellOrder(std::string t);
	void deleteBuyOrder(std::string t);
	void showOrders();
	std::map<std::string, std::pair<long long, int>> getBuyOrders();
	std::map<std::string, std::pair<long long, int>> getSellOrders();
	void addPortofolio(std::string t, long long c);
	int getBehaviour();
};