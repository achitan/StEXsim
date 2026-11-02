#include "Ticker.h"

Ticker::Ticker() {
	this->m_name.clear();
	this->m_price = 1.;
	this->m_volume = 1;
}

Ticker::Ticker(std::string n, long long v, float p) {
	this->m_name = n;
	this->m_volume = v;
	this->m_price = p;
}

Ticker::Ticker(std::string n, long long c) {
	this->m_name = n;
	this->m_price = 1.;
	this->m_volume = c;
}

void Ticker::setName(std::string n) {
	this->m_name = n;
}

void Ticker::setPrice(float p) {
	this->m_price = p;
}

void Ticker::setVolume(long long v) {
	this->m_volume = v;
}

void Ticker::emitVolume(long long v, float p) {
	this->m_volume += v;
	this->m_price = p;
}

long long Ticker::getCapitalization() {
	float result = static_cast<long long>(this->m_volume * this->m_price);
	return result;
}

std::string Ticker::getName() {
	return this->m_name;
}

float Ticker::getPrice() {
	return this->m_price;
}

long long Ticker::getVolume() {
	return this->m_volume;
}