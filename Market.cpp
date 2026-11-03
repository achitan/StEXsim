#include "Market.h"
#include "MarketEvent.h"

Market::Market(long long n, long m) : m_mechanics(500) {
    this->m_playerV.clear();
    this->m_tickerV.clear();
    this->createTickers(n);
    this->createPlayers(m);
    this->recordMarket();
    this->updateIndeces();
}

Market::Market(std::vector<Ticker> ts, std::vector<Player> ps) : m_mechanics(500) {
    this->m_playerV.clear();
    this->m_tickerV.clear();
    this->m_playerV = ps;
    this->m_tickerV = ts;
    this->recordMarket();
    this->updateIndeces();
}

Market::Market(std::vector<Ticker> ts) : m_mechanics(500) {
    this->m_tickerV.clear();
    this->m_playerV.clear();
    this->m_tickerV = ts;
}

void Market::createTickers(long long n) {
    std::vector<std::string> ticks(n);
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_int_distribution<short> r_char(65, 90);
    for (auto& t : ticks) {
        t = r_char(re);
        t += r_char(re);
        t += r_char(re);
    }
    std::sort(ticks.begin(), ticks.end());
    auto iterTicks = ticks.begin() + 1;
    for (; iterTicks != ticks.end(); ++iterTicks) {
        if ((*iterTicks).compare(*(iterTicks - 1)) == 0) {
            ticks.erase(iterTicks);
            --iterTicks;
        }
    }
    std::vector<long long> volumes(ticks.size());
    std::vector<double> prices(ticks.size());
    std::uniform_int_distribution<long long> r_longlong(100'000, 1'000'000);
    std::uniform_real_distribution<double> r_double(1., 50.);
    for (auto& v : volumes) {
        v = r_longlong(re);
    }
    for (auto& p : prices) {
        p = r_double(re);
    }
    auto iterVs = volumes.begin();
    auto iterPs = prices.begin();
    for (auto& t : ticks) {
        this->m_tickerV.push_back(Ticker(t, *iterVs, *iterPs));

        ++iterVs; ++iterPs;
    }
}

void Market::createPlayers(long m) {
    std::vector<std::string> players(m);
    std::vector<long long> capitals(m);
    std::string buff("Player");
    long i = 1;
    for (auto& p : players) {
        p = buff + std::to_string(i);
        i++;
    }

    std::random_device rd;
    std::default_random_engine re(rd());

    // Standard retail capital range
    std::uniform_int_distribution<long long> r_longlong(1'000, 1'000'000);
    // Massive capital range for Investment Banks (e.g., 50M to 200M)
    std::uniform_int_distribution<long long> r_ib_capital(50'000'000, 200'000'000);

    // Uniform random integer generator for retail typologies: 1 (Noise), 2 (Momentum), 3 (Value)
    std::uniform_int_distribution<int> r_behavior(1, 3);

    std::uniform_int_distribution<size_t> distTick(0, this->m_tickerV.size() - 1);
    std::uniform_real_distribution<double> distShares(10.0, 100.0);
    std::uniform_real_distribution<double> distIB_Shares(1000.0, 5000.0); // Heavier starting inventory for IBs

    for (auto& c : capitals) {
        c = r_longlong(re);
    }

    auto iterCs = capitals.begin();
    long playerIndex = 0;

    for (auto& p : players) {
        // Designate the first few players (e.g., playerIndex < 3) as Investment Banks (Typology 4)
        if (playerIndex < 3) {
            long long massiveCapital = r_ib_capital(re);
            Player ibPlayer(p, massiveCapital);
            ibPlayer.setBehaviour(4); // Investment Bank behavior

            // Give them heavy starting inventory
            std::string randomTicker = this->m_tickerV[distTick(re)].getName();
            ibPlayer.addPortofolio(randomTicker, distIB_Shares(re));

            this->m_playerV.push_back(ibPlayer);
        }
        else {
            // Standard retail/algorithmic players (1, 2, or 3)
            Player newPlayer(p, *iterCs);
            newPlayer.setBehaviour(r_behavior(re));

            std::string randomTicker = this->m_tickerV[distTick(re)].getName();
            newPlayer.addPortofolio(randomTicker, distShares(re));

            this->m_playerV.push_back(newPlayer);
        }

        ++iterCs;
        ++playerIndex;
    }
}

void Market::updateMarket() {
    this->placeOrders();

    this->m_mechanics.processBook(this->m_tickerV, this->m_playerV);

    MarketEvent::processEvents(this->m_tickerV, date);

    ++date;
    this->recordMarket();
    this->updateIndeces();
}

void Market::fulfillOrders() {
    for (auto& p : this->m_playerV) {
        for (auto& o : p.getBuyOrders()) {
            for (auto& t : this->m_tickerV) {
                if (t.getName().compare(o.first) == 0) {
                    long long STB = static_cast<long long>(std::floor(static_cast<double>(o.second.first) / t.getPrice()));
                    bool buyingMaxStock = (STB >= (t.getVolume() / 5)) ? true : false;
                    if (buyingMaxStock) {
                        STB = t.getVolume() / 3;
                        p.setCapital(p.getCapital() + (o.second.first - static_cast<long long>(STB * t.getPrice())));
                        o.second.first = static_cast<long long>(STB * t.getPrice());
                    }
                    p.addPortofolio(o.first, STB);
                    long long diff = o.second.first - static_cast<long long>(STB * t.getPrice());
                    p.setCapital(p.getCapital() + diff);
                    p.deleteBuyOrder(o.first);
                    t.setPrice((static_cast<double>(t.getCapitalization()) + static_cast<double>(STB) * t.getPrice()) / static_cast<double>(t.getVolume()));
                }
            }
        }
    }
}

void Market::placeOrders() {
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution<double> priceVariance(-0.03, 0.03);

    for (auto& p : this->m_playerV) {
        p.makeDecision(this->m_tickerV, date);
    }

    for (auto& p : this->m_playerV) {
        auto buyOrders = p.getBuyOrders();
        for (const auto& ord : buyOrders) {
            std::string tickerName = ord.first;
            double spendAmount = ord.second.first;

            auto it = std::find_if(m_tickerV.begin(), m_tickerV.end(), [&](const Ticker& t) {
                return t.getName() == tickerName;
                });

            if (it != m_tickerV.end() && it->getPrice() > 0.0) {
                double basePrice = it->getPrice();
                double targetPrice = basePrice * (1.0 + priceVariance(re));
                double quantity = spendAmount / targetPrice;

                if (quantity > 0.0) {
                    this->m_mechanics.addOrder(tickerName, targetPrice, quantity, true, p.getName());
                }
            }
        }

        auto sellOrders = p.getSellOrders();
        for (const auto& ord : sellOrders) {
            std::string tickerName = ord.first;
            double sharesToSell = ord.second.first;

            auto it = std::find_if(m_tickerV.begin(), m_tickerV.end(), [&](const Ticker& t) {
                return t.getName() == tickerName;
                });

            if (it != m_tickerV.end()) {
                double basePrice = it->getPrice();
                double targetPrice = basePrice * (1.0 + priceVariance(re));

                if (sharesToSell > 0.0) {
                    this->m_mechanics.addOrder(tickerName, targetPrice, sharesToSell, false, p.getName());
                }
            }
        }

        p.deleteBuyOrder("*");
        p.deleteSellOrder("*");
    }
}

void Market::recordMarket() {
    for (auto& t : this->m_tickerV) {
        this->m_priceHistory[t.getName()].push_back(t.getPrice());
    }
}

unsigned int Market::getDate() {
    return this->date;
}

struct {
    bool operator() (Ticker a, Ticker b) const {
        return (a.getCapitalization() > b.getCapitalization());
    }
} orderCapital;

struct {
    bool operator() (Ticker a, Ticker b) const {
        return (a.getPrice() > b.getPrice());
    }
} orderPrice;

double getStdDev(std::vector<double> in) {
    double n = 0., sumV = 0.;
    for (auto& val : in) {
        sumV += val;
        n++;
    }
    double miu = sumV / n;
    sumV = 0.;
    for (auto& val : in) {
        sumV += pow(val - miu, 2.);
    }
    double result = sqrt(sumV / n);
    return result;
}

void Market::updateIndeces() {
    int perc = static_cast<int>(std::ceil(static_cast<double>(this->m_tickerV.size()) / 100.));
    double sumPrices = 0.;

    std::sort(this->m_tickerV.begin(), this->m_tickerV.end(), orderCapital);
    for (int i = 0; i < perc; i++) {
        sumPrices += this->m_tickerV.at(i).getPrice();
    }
    this->m_percCap.push_back(sumPrices / static_cast<double>(perc));

    std::sort(this->m_tickerV.begin(), this->m_tickerV.end(), orderPrice);
    sumPrices = 0.;
    for (int i = 0; i < perc; i++) {
        sumPrices += this->m_tickerV.at(i).getPrice();
    }
    this->m_percPrice.push_back(sumPrices / static_cast<double>(perc));

    sumPrices = 0.;
    for (auto& p : this->m_priceHistory) {
        sumPrices += getStdDev(p.second);
    }
    this->m_volatility.push_back(sumPrices / static_cast<double>(this->m_priceHistory.size()));

    sumPrices = 0.;
    std::vector<double> caps;
    for (auto& t : this->m_tickerV) {
        caps.push_back(t.getCapitalization());
    }
    this->m_coherence.push_back(getStdDev(caps));
}

const std::vector<Player>& Market::getPlayers() const {
    return this->m_playerV;
}

const std::vector<Ticker>& Market::getTickers() const {
    return this->m_tickerV;
}

std::vector<double> Market::getVolatility() {
    return this->m_volatility;
}

std::vector<double> Market::getCoherence() {
    return this->m_coherence;
}

std::vector<double> Market::getpercPrice() {
    return this->m_percPrice;
}

std::vector<double> Market::getpercCap() {
    return this->m_percCap;
}

void Market::readMarket(Player p) {

}

std::vector<double> Market::getTickerPriceHisto(std::string t) {
    return this->m_priceHistory[t];
}