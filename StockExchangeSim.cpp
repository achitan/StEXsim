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
    long no_tickers = 1000;
    long no_players = 500;
    Market market(no_tickers, no_players);

    std::cout << "Starting Stock Exchange Simulation with " << no_tickers << " Tickers and " << no_players << " Players...\n" << std::endl;

    // 2. Run simulation loop for 30 trading days
    const int tradingDays = 365;
    for (int day = 1; day <= tradingDays; ++day) {
        market.updateMarket(); // Drives LOB matching engine and player strategies
        //std::cout << "Completed Trading Day: " << market.getDate() << std::endl;
    }

    // 3. Find an active ticker using your getTickerPriceHisto method
    std::vector<double> targetTickerPrices;
    std::string selectedTickerName = "None";

    for (const auto& ticker : market.getTickers()) {
        std::string tName = ticker.getName();
        std::vector<double> history = market.getTickerPriceHisto(tName);

        if (history.size() > 1) {
            double minP = *std::min_element(history.begin(), history.end());
            double maxP = *std::max_element(history.begin(), history.end());

            // Select the first ticker that experienced price movement
            if (maxP != minP) {
                targetTickerPrices = history;
                selectedTickerName = tName;
                break;
            }
        }
    }

    // Fallback if no volatile ticker found yet
    if (targetTickerPrices.size() < 5 && !market.getTickers().empty()) {
        selectedTickerName = market.getTickers()[0].getName();
        targetTickerPrices = market.getTickerPriceHisto(selectedTickerName);
    }

    // Ultimate fallback dummy data if history is empty
    if (targetTickerPrices.size() < 5) {
        targetTickerPrices = { 50.0, 51.5, 50.2, 53.0, 52.1, 55.4, 54.0, 57.2, 56.5, 60.0 };
        selectedTickerName = "DemoTicker";
    }

    // Print out the actual historical prices for inspection
    //std::cout << "\n--- Price History for Ticker: " << selectedTickerName << " ---" << std::endl;
    //for (size_t i = 0; i < targetTickerPrices.size(); ++i) {
    //    std::cout << "Day " << (i + 1) << ": $" << targetTickerPrices[i] << std::endl;
    //}

    // 4. Calculate Bollinger Bands (using standard BollingerBands struct)
    BollingerBands bb = TechnicalAnalyzer::calculateBollingerBands(targetTickerPrices, 5, 2.0);

    // 5. Render Actual Prices & Technical Overlay using ASCII Plotter
    std::cout << "\n--- ASCII Chart: Prices & Bollinger Bands (" << selectedTickerName << ") ---\n" << std::endl;
    Plotter plotter(60, 20);

    // Plot actual prices and indicator lines
    plotter.plot_line(bb.upperBand,'+');
    plotter.plot_line(bb.middleBand,'=');
    plotter.plot_line(bb.lowerBand,'+');
    plotter.plot_line(targetTickerPrices);
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