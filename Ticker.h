#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

class Ticker {
private:
	std::string m_name;
	long long m_volume;
	float m_price;

public:
	Ticker();
	Ticker(std::string n, long long v, float p);
	Ticker(std::string n, long long c);
	void setName(std::string n);
	std::string getName();
	void setPrice(float p);
	float getPrice();
	void setVolume(long long v);
	void emitVolume(long long v, float p);
	long long getCapitalization();
	long long getVolume();
};