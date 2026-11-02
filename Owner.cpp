#include "Owner.h"

Owner::Owner() {
	this->m_name = "";
}

Owner::Owner(std::string n) {
	this->m_name = n;
}

void Owner::SetName(std::string n) {
	this->m_name = n;
}

void Owner::PlaceOrder(std::string t, unsigned long long s) {
	this->m_orders[t] = s;
}