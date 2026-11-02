#pragma once

#include <string>
#include <map>

class Owner {
private:
	std::string m_name;
	std::map<std::string, unsigned long long> m_orders;

public:
	Owner();
	Owner(std::string n);
	void SetName(std::string n);
	void PlaceOrder(std::string t, unsigned long long s);
};