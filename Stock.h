#pragma once

#include <string>
#include "Owner.h"

class Stock {
private:
	float m_price;
	std::string m_tick;
	Owner m_owner;

public:
	Stock(std::string t, float p);
	Stock();
	void SetOwner(std::string o);
	void SetTick(std::string t);
	void SetPrice(float p);
};