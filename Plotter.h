#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include <cmath>
#include <map>

class Plotter {
private:
    size_t m_width;
    size_t m_height;
    std::vector<std::string> m_canvas;

    void clear_canvas();
    void plot_char(size_t x, size_t y, char c);

public:
    explicit Plotter(size_t width = 80, size_t height = 20);
    void plot_line(const std::vector<double>& data, char marker = '*');
    void plot_histo(const std::vector<double>& data, size_t noBins = 10);
    void render() const;
};