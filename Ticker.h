#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

class Ticker {
private:
	std::string m_name;
	double m_volume;
	double m_price;

public:
	Ticker();
	Ticker(std::string n, double v, double p);
	Ticker(std::string n, double c);

	void setName(std::string n);
	std::string getName() const;

	void setPrice(double p);
	float getPrice() const;

	void setVolume(double v);
	long long getVolume() const;

	void emitVolume(double v, double p);
	long long getCapitalization() const;
};