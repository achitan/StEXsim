#include <iostream>
#include <string>
#include <random>
#include <algorithm>

#include "Player.h"
#include "Ticker.h"
#include "Market.h"
#include "Plotter.h"

unsigned int Market::date = 1;

int main() {
    Market tst(1000);
    tst.placeOrders();
    tst.updateMarket();
    
    std::vector<double> speculative_run;// = { 0.1,0.1,0.1,0.2,0.3,0.3,0.5,0.5 };
    double current_price = 10.0;
    for (int day = 0; day < 100; ++day) {
        speculative_run.push_back(current_price);
        current_price *= 1.05; // 5% daily geometric explosion
    }
    Plotter plotter(60, 20);

    // Plot and view the ASCII matrix output
    plotter.plot_line(speculative_run);
    //plotter.plot_histo(speculative_run);
    plotter.render();

    std::cout << tst.date << std::endl;
    
    return EXIT_SUCCESS;
}