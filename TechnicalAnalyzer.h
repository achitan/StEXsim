#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

struct BollingerBands {
    std::vector<double> upperBand;
    std::vector<double> middleBand;
    std::vector<double> lowerBand;
};

class TechnicalAnalyzer {
public:
    static std::vector<double> calculateSMA(const std::vector<double>& prices, int period);

    static std::vector<double> calculateEMA(const std::vector<double>& prices, int period);

    static std::vector<double> calculateRSI(const std::vector<double>& prices, int period = 14);

    static std::pair<std::vector<double>, std::vector<double>> calculateMACD(
        const std::vector<double>& prices,
        int fastPeriod = 12,
        int slowPeriod = 26,
        int signalPeriod = 9
    );

    static BollingerBands calculateBollingerBands(const std::vector<double>& prices, int period = 20, double multiplier = 2.0f);
};