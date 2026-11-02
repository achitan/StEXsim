#include "TechnicalAnalyzer.h"

std::vector<double> TechnicalAnalyzer::calculateSMA(const std::vector<double>& prices, int period) {
    std::vector<double> sma;
    if (prices.size() < period) return sma;

    for (size_t i = 0; i < prices.size(); ++i) {
        if (i < period - 1) {
            sma.push_back(0.0f); // Not enough data points yet
        }
        else {
            double sum = 0.0f;
            for (int j = 0; j < period; ++j) {
                sum += prices[i - j];
            }
            sma.push_back(sum / period);
        }
    }
    return sma;
}

std::vector<double> TechnicalAnalyzer::calculateEMA(const std::vector<double>& prices, int period) {
    std::vector<double> ema;
    if (prices.empty()) return ema;

    double multiplier = 2.0f / (period + 1);
    double prevEMA = prices[0];
    ema.push_back(prevEMA);

    for (size_t i = 1; i < prices.size(); ++i) {
        double currentEMA = (prices[i] - prevEMA) * multiplier + prevEMA;
        ema.push_back(currentEMA);
        prevEMA = currentEMA;
    }
    return ema;
}

std::vector<double> TechnicalAnalyzer::calculateRSI(const std::vector<double>& prices, int period) {
    std::vector<double> rsi;
    if (prices.size() <= period) return rsi;

    // Pad initial values
    for (int i = 0; i < period; ++i) {
        rsi.push_back(50.0f);
    }

    double avgGain = 0.0f;
    double avgLoss = 0.0f;

    // First average gain/loss
    for (int i = 1; i <= period; ++i) {
        double change = prices[i] - prices[i - 1];
        if (change > 0) avgGain += change;
        else avgLoss += std::abs(change);
    }
    avgGain /= period;
    avgLoss /= period;

    for (size_t i = period; i < prices.size(); ++i) {
        if (i > period) {
            double change = prices[i] - prices[i - 1];
            double gain = (change > 0) ? change : 0.0f;
            double loss = (change < 0) ? std::abs(change) : 0.0f;

            avgGain = (avgGain * (period - 1) + gain) / period;
            avgLoss = (avgLoss * (period - 1) + loss) / period;
        }

        if (avgLoss == 0.0f) {
            rsi.push_back(100.0f);
        }
        else {
            double rs = avgGain / avgLoss;
            rsi.push_back(100.0f - (100.0f / (1.0f + rs)));
        }
    }
    return rsi;
}

std::pair<std::vector<double>, std::vector<double>> TechnicalAnalyzer::calculateMACD(
    const std::vector<double>& prices, int fastPeriod, int slowPeriod, int signalPeriod) {

    std::vector<double> fastEMA = calculateEMA(prices, fastPeriod);
    std::vector<double> slowEMA = calculateEMA(prices, slowPeriod);
    std::vector<double> macdLine;

    for (size_t i = 0; i < prices.size(); ++i) {
        macdLine.push_back(fastEMA[i] - slowEMA[i]);
    }

    std::vector<double> signalLine = calculateEMA(macdLine, signalPeriod);
    return { macdLine, signalLine };
}

BollingerBands TechnicalAnalyzer::calculateBollingerBands(const std::vector<double>& prices, int period, double multiplier) {
    BollingerBands bands;
    std::vector<double> sma = calculateSMA(prices, period);
    bands.middleBand = sma;

    for (size_t i = 0; i < prices.size(); ++i) {
        if (i < period - 1 || prices.empty()) {
            // Use the first valid price instead of 0.0 to prevent breaking chart scaling
            double fallback = prices.empty() ? 0.0 : prices[0];
            bands.upperBand.push_back(fallback);
            bands.lowerBand.push_back(fallback);
        }
        else {
            double mean = sma[i];
            double varianceSum = 0.0;
            for (int j = 0; j < period; ++j) {
                varianceSum += std::pow(prices[i - j] - mean, 2);
            }
            double stDev = std::sqrt(varianceSum / period);
            bands.upperBand.push_back(mean + (multiplier * stDev));
            bands.lowerBand.push_back(mean - (multiplier * stDev));
        }
    }
    return bands;
}