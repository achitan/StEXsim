#include "Player.h"

Player::Player(std::string n) {
	this->m_name = n;
	this->m_capital = 0.;
	this->m_behaviour = 7;
}

Player::Player(std::string n, long long c) {
	this->m_name = n;
	this->m_capital = c;
	this->m_behaviour = 7;
}

void Player::setCapital(long long c) {
	this->m_capital = c;
}

long long Player::getCapital() {
	return this->m_capital;
}

void Player::addBuyOrder(std::string t, long long c, unsigned int d) {
	this->m_buymap[t].first += c;
	this->m_buymap[t].second = d;
	this->m_capital -= c;
}

void Player::addSellOrder(std::string t, long long v, unsigned int d) {
	this->m_sellmap[t].first += v;
	this->m_sellmap[t].second = d;
}

void Player::deleteSellOrder(std::string t) {
	if (t.compare("*") == 0)
		this->m_sellmap.clear();
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
		idx -= 3;
		while (idx > 0 && idx <= capitalstr.length()) {
			capitalstr.insert(idx, "\'");
			idx -= 3;
		}
		std::cout << ord.first << " for " << capitalstr << " dollars." << std::endl;
	}
	std::cout << "====================" << std::endl;
	std::cout << "=====SELL ORDERS FOR " << this->m_name << "=====" << std::endl;
	for (auto& ord : this->m_sellmap) {
		std::string capitalstr = std::to_string(ord.second.first);
		size_t idx = capitalstr.length();
		idx -= 3;
		while (idx > 0 && idx <= capitalstr.length()) {
			capitalstr.insert(idx, "\'");
			idx -= 3;
		}
		std::cout << ord.first << " for " << capitalstr << " stocks." << std::endl;
	}
	std::cout << "=====================" << std::endl;
}

std::map<std::string, std::pair<long long, int>> Player::getBuyOrders() {
	return this->m_buymap;
}
std::map<std::string, std::pair<long long, int>> Player::getSellOrders() {
	return this->m_sellmap;
}

void Player::addPortofolio(std::string t, long long v) {
	this->m_portofolio[t] += v;
}

void Player::setBehaviour(int b) {
	this->m_behaviour = b;
}

int Player::getBehaviour() {
	return this->m_behaviour;
}