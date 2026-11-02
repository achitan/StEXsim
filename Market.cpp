#include "Market.h"

Market::Market(long long n, long m) {
    this->m_playerV.clear();
    this->m_tickerV.clear();
    this->createTickers(n);
    this->createPlayers(m);
    this->recordMarket();
    this->updateIndeces();
}

Market::Market(std::vector<Ticker> ts, std::vector<Player> ps) {
    this->m_playerV.clear();
    this->m_tickerV.clear();
    this->m_playerV = ps;
    this->m_tickerV = ts;
    this->recordMarket();
    this->updateIndeces();
}

Market::Market(std::vector<Ticker> ts) {
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
    std::vector<float> prices(ticks.size());
    std::uniform_int_distribution<long long> r_longlong(100'000, 1'000'000);
    std::uniform_real_distribution<float> r_float(1., 50.);
    for (auto& v : volumes) {
        v = r_longlong(re);
    }
    for (auto& p : prices) {
        p = r_float(re);
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
    std::uniform_int_distribution<long long> r_longlong(1'000, 1'000'000);
    for (auto& c : capitals) {
        c = r_longlong(re);
    }
    auto iterCs = capitals.begin();
    for (auto& p : players) {
        this->m_playerV.push_back(Player(p, *iterCs));
        ++iterCs;
    }
}

void Market::updateMarket() {
    this->fulfillOrders();
    ++date;
    this->recordMarket();
    this->updateIndeces();
}

void Market::fulfillOrders() {
    for (auto& p : this->m_playerV) {
        for (auto& o : p.getBuyOrders()) {
            for (auto& t : this->m_tickerV) {
                if (t.getName().compare(o.first) == 0) {
                    //std::cout << t.getName() << ": " << t.getPrice();
                    long long STB = static_cast<long long>(std::floor(static_cast<float>(o.second.first) / t.getPrice()));
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
                    t.setPrice((static_cast<float>(t.getCapitalization()) + static_cast<float>(STB) * t.getPrice()) / static_cast<float>(t.getVolume()));
                    //std::cout << t.getVolume() / STB << std::endl;
                    //std::cout << (static_cast<float>(t.getCapitalization()) + static_cast<float>(STB) * t.getPrice()) / static_cast<float>(t.getVolume()) << std::endl;
                    //std::cout << " --> " << t.getPrice() << std::endl;
                }
            }
        }
    }
}

void Market::placeOrders() {
    std::random_device rd;
    std::default_random_engine re(rd());
    int sz = this->m_tickerV.size();
    std::uniform_int_distribution<int> r_tick(0, sz - 1);
    for (auto& p : this->m_playerV) {
        std::uniform_int_distribution<int> r_capital(p.getCapital() / 6, p.getCapital() / 4);
        p.addBuyOrder(this->m_tickerV.at(r_tick(re)).getName(), r_capital(re), date + 5);
        //p.showOrders();
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

float getStdDev(std::vector<float> in) {
    float n = 0., sumV = 0.;
    for (auto& val : in) {
        sumV += val;
        n++;
    }
    float miu = sumV / n;
    sumV = 0.;
    for (auto& val : in) {
        sumV += pow(val - miu, 2.);
    }
    float result = sqrt(sumV / n);
    return result;
}

void Market::updateIndeces() {
    int perc = static_cast<int>(std::ceil(static_cast<float>(this->m_tickerV.size()) / 100.));
    float sumPrices = 0.;

    //MEAN PRICE FOR 1% OF PRICEs
    std::sort(this->m_tickerV.begin(), this->m_tickerV.end(), orderCapital);
    for (int i = 0; i < perc; i++) {
        sumPrices += this->m_tickerV.at(i).getPrice();
    }
    this->m_percCap.push_back(sumPrices / static_cast<float>(perc));

    //MEAN PRICE FOR 1% OF CAP
    std::sort(this->m_tickerV.begin(), this->m_tickerV.end(), orderPrice);
    sumPrices = 0.;
    for (int i = 0; i < perc; i++) {
        sumPrices += this->m_tickerV.at(i).getPrice();
    }
    this->m_percPrice.push_back(sumPrices / static_cast<float>(perc));

    //VOLATILITY
    sumPrices = 0.;
    for (auto& p : this->m_priceHistory) {
        sumPrices += getStdDev(p.second);
    }
    this->m_volatility.push_back(sumPrices / static_cast<float>(this->m_priceHistory.size()));

    //COHERENCE
    sumPrices = 0.;
    std::vector<float> caps;
    for (auto& t : this->m_tickerV) {
        caps.push_back(t.getCapitalization());
    }
    this->m_coherence.push_back(getStdDev(caps));
}

std::vector<float> Market::getVolatility() {
    return this->m_volatility;
}

std::vector<float> Market::getCoherence() {
    return this->m_coherence;
}

std::vector<float> Market::getpercPrice() {
    return this->m_percPrice;
}

std::vector<float> Market::getpercCap() {
    return this->m_percCap;
}

void Market::readMarket(Player p) {

}