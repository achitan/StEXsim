#include <iostream>
#include <string>
#include <random>
#include <algorithm>

#include "Player.h"
#include "Ticker.h"
#include "Market.h"
#include "Plotter.h"
#include "TechnicalAnalyzer.h"

unsigned int Market::date = 0;

int main() {
    // 1. Initialize Market with 100 Tickers and 50 Players
    Market market(10000, 100);

    std::cout << "Starting Stock Exchange Simulation with 100 Tickers and 50 Players...\n" << std::endl;

    // 2. Run simulation loop for 30 trading days
    const int tradingDays = 365;
    for (int day = 1; day <= tradingDays; ++day) {
        market.updateMarket(); // Drives LOB matching engine and player strategies
        std::cout << "Completed Trading Day: " << market.getDate() << std::endl;
    }

    // 3. Find an active ticker using your new getTickerPriceHisto method
    // Find the ticker with the most price volatility or activity
    std::string activeTicker = "None";
    std::vector<double> targetTickerPrices;
    double maxPriceVariance = -1.0;

    for (const auto& ticker : market.getTickers()) {
        std::string tName = ticker.getName();
        std::vector<double> history = market.getTickerPriceHisto(tName);

        if (!history.empty()) {
            double minP = *std::min_element(history.begin(), history.end());
            double maxP = *std::max_element(history.begin(), history.end());
            //std::cout << minP << "\t" << maxP << std::endl;
            double variance = maxP - minP;

            if (variance > maxPriceVariance) {
                maxPriceVariance = variance;
                targetTickerPrices = history;
                activeTicker = tName;
            }
        }
    }

    // Fallback if none found
    if (targetTickerPrices.size() < 5) {
        targetTickerPrices = { 50.0, 51.5, 50.2, 53.0, 52.1, 55.4 };
        activeTicker = "DefaultFallback";
    }

    // 4. Calculate Bollinger Bands (using standard BollingerBands struct)
    BollingerBands bb = TechnicalAnalyzer::calculateBollingerBands(targetTickerPrices, 5, 2.0);

    // 5. Render Actual Prices & Technical Overlay using ASCII Plotter
    std::cout << "\n--- ASCII Chart: Prices & Bollinger Bands (" << activeTicker << ") ---\n" << std::endl;
    Plotter plotter(60, 20);

    plotter.plot_line(targetTickerPrices);
    plotter.plot_line(bb.upperBand);
    plotter.plot_line(bb.middleBand);
    plotter.plot_line(bb.lowerBand);
    plotter.render();

    std::cout << "\nSimulation completed successfully!" << std::endl;

    // --- LEADERBOARD ---
    std::cout << "\n=============================================" << std::endl;
    std::cout << "         FINAL LEADERBOARD (TOP 3)           " << std::endl;
    std::cout << "=============================================" << std::endl;

    std::vector<Player> allPlayers = market.getPlayers();

    std::sort(allPlayers.begin(), allPlayers.end(), [&market](const Player& a, const Player& b) {
        return a.calculateNetWorth(market.getTickers()) > b.calculateNetWorth(market.getTickers());
        });

    for (size_t i = 0; i < std::min<size_t>(3, allPlayers.size()); ++i) {
        const auto& p = allPlayers[i];
        double netWorth = p.calculateNetWorth(market.getTickers());

        std::cout << i + 1 << ". Player Name: " << p.getName()
            << " | Typology: " << p.getBehaviour()
            << " | Net Worth: $" << netWorth
            << " | Cash Capital: $" << p.getCapital() << std::endl;

        std::cout << "   Portfolio Holdings:" << std::endl;
        if (p.getPortfolio().empty()) {
            std::cout << "     - (None)" << std::endl;
        }
        else {
            for (const auto& holding : p.getPortfolio()) {
                std::cout << "     - " << holding.first << ": " << holding.second << " shares" << std::endl;
            }
        }
        std::cout << "---------------------------------------------" << std::endl;
    }

    return EXIT_SUCCESS;
}