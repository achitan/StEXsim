#include "Ticker.h"

Ticker::Ticker() {
	this->m_name.clear();
	this->m_price = 1.;
	this->m_volume = 1;
}

Ticker::Ticker(std::string n, double v, double p) {
	this->m_name = n;
	this->m_volume = v;
	this->m_price = p;
}

Ticker::Ticker(std::string n, double c) {
	this->m_name = n;
	this->m_price = 1.;
	this->m_volume = c;
}

void Ticker::setName(std::string n) {
	this->m_name = n;
}

void Ticker::setPrice(double p) {
	this->m_price = p;
}

void Ticker::setVolume(double v) {
	this->m_volume = v;
}

void Ticker::emitVolume(double v, double p) {
	this->m_volume += v;
	this->m_price = p;
}

long long Ticker::getCapitalization() const { // Added const here
	double result = static_cast<double>(this->m_volume) * static_cast<double>(this->m_price);
	return static_cast<long long>(result);
}

std::string Ticker::getName() const {         // Added const here
	return this->m_name;
}

float Ticker::getPrice() const {              // Added const here
	return this->m_price;
}

long long Ticker::getVolume() const {         // Added const here
	return this->m_volume;
}