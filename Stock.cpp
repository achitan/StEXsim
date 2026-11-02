#include "Stock.h"

Stock::Stock(std::string t, float p) {
	this->m_tick = t;
	this->m_price = p;
}

Stock::Stock() {
	this->m_tick.clear();
	this->m_price = 0.;
}

void Stock::SetOwner(std::string o) {
	this->m_owner.SetName(o);
}

void Stock::SetTick(std::string t) {
	this->m_tick = t;
}

void Stock::SetPrice(float p) {
	this->m_price = p;
}