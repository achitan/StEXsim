#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

struct BollingerBands {
    std::vector<double> upperBand;
    std::vector<double> middleBand; // Simple Moving Average (SMA)
    std::vector<double> lowerBand;
};

class TechnicalAnalyzer {
public:
    // Simple Moving Average (SMA)
    static std::vector<double> calculateSMA(const std::vector<double>& prices, int period);

    // Exponential Moving Average (EMA)
    static std::vector<double> calculateEMA(const std::vector<double>& prices, int period);

    // Relative Strength Index (RSI)
    static std::vector<double> calculateRSI(const std::vector<double>& prices, int period = 14);

    // Moving Average Convergence Divergence (MACD)
    // Returns a pair of vectors: {MACD Line, Signal Line}
    static std::pair<std::vector<double>, std::vector<double>> calculateMACD(
        const std::vector<double>& prices,
        int fastPeriod = 12,
        int slowPeriod = 26,
        int signalPeriod = 9
    );

    // Bollinger Bands
    static BollingerBands calculateBollingerBands(const std::vector<double>& prices, int period = 20, double multiplier = 2.0f);
};