#include "Plotter.h"

Plotter::Plotter(size_t width, size_t height) : m_width(width), m_height(height), m_canvas(height, std::string(width, ' ')) {
    clear_canvas();
}

void Plotter::clear_canvas() {
    m_canvas.assign(m_height, std::string(m_width, ' '));

    if (m_height < 3 || m_width < 3) return;

    // Draw horizontal boundaries
    for (size_t x = 0; x < m_width; ++x) {
        m_canvas.front()[x] = '-';
        m_canvas.back()[x] = '-';
    }

    // Draw vertical boundaries
    for (size_t y = 0; y < m_height; ++y) {
        m_canvas[y][0] = '|';
        m_canvas[y][m_width - 1] = '|';
    }
}

void Plotter::plot_char(size_t x, size_t y, char c) {
    // Ensure coordinates fit securely inside the borders
    if (x >= 1 && x < m_width - 1 && y >= 1 && y < m_height - 1) {
        m_canvas[m_height - 1 - y][x] = c;
    }
}

void Plotter::plot_line(const std::vector<double>& data, char marker) {
    //clear_canvas();
    if (data.empty()) return;

    // 1. Core Bounds Checking
    auto [min_it, max_it] = std::minmax_element(data.begin(), data.end());
    double min_val = static_cast<double>(*min_it);
    double max_val = static_cast<double>(*max_it);
    double range = max_val - min_val;
    if (range == 0.0) range = 1.0;

    // Absolute printable bounds inside the canvas borders
    size_t plot_width = m_width - 2;   // e.g., 78 columns
    size_t plot_height = m_height - 2; // e.g., 18 rows

    // Map screen columns to data values
    for (size_t x_screen = 1; x_screen <= plot_width; ++x_screen) {

        // Convert current screen column step to a ratio (0.0 to 1.0)
        double x_ratio = static_cast<double>(x_screen - 1) / (plot_width - 1);

        // Map ratio to the closest index in the data array
        size_t data_idx = static_cast<size_t>(std::round(x_ratio * (data.size() - 1)));
        if (data_idx >= data.size()) data_idx = data.size() - 1;

        // Normalize the Y metric (0.0 to 1.0)
        double norm_y = (static_cast<double>(data[data_idx]) - min_val) / range;

        // Scale cleanly into row rows
        size_t y_screen = 1 + static_cast<size_t>(std::round(norm_y * (plot_height - 1)));

        plot_char(x_screen, y_screen, marker);
    }
}

void Plotter::plot_histo(const std::vector<double>& data, size_t noBins) {
    clear_canvas();
    if (data.empty()) return;

    // Absolute printable bonds
    size_t plot_width = m_width - 2;
    size_t plot_height = m_height - 2;

    // Histogram the data
    double min_data = *(std::min_element(data.begin(), data.end()));
    double max_data = *(std::max_element(data.begin(), data.end()));
    double range_data = max_data - min_data;
    double bin_width = range_data / static_cast<double>(noBins);
    std::vector<double> bin_limits;
    //bin_limits.push_back(min_data); first boundry is not necessary as there should be no data under 
    size_t j = 1;
    while (j < noBins) {
        bin_limits.push_back(min_data + static_cast<double>(j) * bin_width);
        j++;
    }
    bin_limits.push_back(max_data);
    std::vector<size_t> freqs(noBins, 0);
    for (auto& value : data) {
        auto it_freqs = freqs.begin();
        for (auto& bin : bin_limits) {
            if (value <= bin) {
                (*it_freqs)++;
                break;
            }
            it_freqs++;
        }
    }

    // Plot the histogram
    min_data = *(std::min_element(freqs.begin(), freqs.end()));
    max_data = *(std::max_element(freqs.begin(), freqs.end()));
    range_data = max_data - min_data;
    bin_width = range_data / static_cast<double>(noBins);
    for (size_t x_screen = 1; x_screen <= plot_width; ++x_screen) {

        // Convert current screen column step to a ratio (0.0 to 1.0)
        double x_ratio = static_cast<double>(x_screen - 1) / (plot_width - 1);

        // Map ratio to the closest index in the data array
        size_t data_idx = static_cast<size_t>(std::round(x_ratio * (freqs.size() - 1)));
        if (data_idx >= freqs.size()) data_idx = freqs.size() - 1;

        // Normalize the Y metric (0.0 to 1.0)
        double norm_y = (static_cast<double>(freqs[data_idx]) - min_data) / range_data;

        // Scale cleanly into row rows
        size_t y_screen = 1 + static_cast<size_t>(std::round(norm_y * (plot_height - 1)));

        size_t y_buff = y_screen;
        while (y_buff > 0) {
            plot_char(x_screen, y_buff, '_');
            y_buff--;
        }
    }
}

void Plotter::render() const {
    for (const auto& row : m_canvas) {
        std::cout << row << '\n';
    }
}