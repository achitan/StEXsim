#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

class Plotter {
private:
    size_t m_width;
    size_t m_height;
    std::vector<std::string> m_canvas;

    struct Series {
        std::vector<double> data;
        char marker;
    };
    std::vector<Series> m_series_list;

    void clear_canvas();
    void plot_char(size_t x, size_t y, char c);

public:
    explicit Plotter(size_t width = 80, size_t height = 20);
    void plot_line(const std::vector<double>& data, char marker = '*');
    void plot_histo(const std::vector<double>& data, size_t noBins = 10);
    void render() const;
};