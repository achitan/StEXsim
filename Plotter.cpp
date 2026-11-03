#include "Plotter.h"

Plotter::Plotter(size_t width, size_t height) : m_width(width), m_height(height), m_canvas(height, std::string(width, ' ')) {
    clear_canvas();
}

void Plotter::clear_canvas() {
    m_canvas.assign(m_height, std::string(m_width, ' '));
    if (m_height < 5 || m_width < 15) return;

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
    if (x >= 1 && x < m_width - 1 && y >= 1 && y < m_height - 1) {
        m_canvas[m_height - 1 - y][x] = c;
    }
}

void Plotter::plot_line(const std::vector<double>& data, char marker) {
    if (!data.empty()) {
        m_series_list.push_back({ data, marker });
    }
}

void Plotter::plot_histo(const std::vector<double>& data, size_t noBins) {
    plot_line(data, '_');
}

void Plotter::render() const {
    Plotter* mutable_this = const_cast<Plotter*>(this);
    mutable_this->clear_canvas();

    if (m_series_list.empty()) {
        for (const auto& row : m_canvas) std::cout << row << '\n';
        return;
    }

    // 1. Compute global min, max, and max data size across all registered series
    double global_min = std::numeric_limits<double>::max();
    double global_max = std::numeric_limits<double>::lowest();
    size_t max_data_size = 0;

    for (const auto& series : m_series_list) {
        auto [min_it, max_it] = std::minmax_element(series.data.begin(), series.data.end());
        if (*min_it < global_min) global_min = *min_it;
        if (*max_it > global_max) global_max = *max_it;
        if (series.data.size() > max_data_size) max_data_size = series.data.size();
    }

    double range = global_max - global_min;
    if (range == 0.0) range = 1.0;

    size_t plot_width = m_width - 2;
    size_t plot_height = m_height - 2;

    // 2. Plot all series onto the canvas using the unified global scale
    for (const auto& series : m_series_list) {
        const auto& data = series.data;
        char marker = series.marker;

        for (size_t x_screen = 1; x_screen <= plot_width; ++x_screen) {
            double x_ratio = static_cast<double>(x_screen - 1) / (plot_width - 1);
            size_t data_idx = static_cast<size_t>(std::round(x_ratio * (data.size() - 1)));
            if (data_idx >= data.size()) data_idx = data.size() - 1;

            double norm_y = (data[data_idx] - global_min) / range;
            if (norm_y < 0.0) norm_y = 0.0;
            if (norm_y > 1.0) norm_y = 1.0;

            size_t y_screen = 1 + static_cast<size_t>(std::round(norm_y * (plot_height - 1)));
            mutable_this->plot_char(x_screen, y_screen, marker);
        }
    }

    // 3. Draw 5 Ordinate (Y-Axis) Ticks & Values on the Left Border
    for (int i = 0; i < 5; ++i) {
        double ratio = static_cast<double>(i) / 4.0; // 0.0, 0.25, 0.5, 0.75, 1.0
        double val = global_min + ratio * range;

        size_t canvas_row;
        if (i == 0) {
            canvas_row = m_height - 1; // Bottom
        }
        else if (i == 4) {
            canvas_row = 0;            // Top
        }
        else {
            size_t y_screen = 1 + static_cast<size_t>(std::round(ratio * (plot_height - 1)));
            canvas_row = m_height - 1 - y_screen;
        }

        mutable_this->m_canvas[canvas_row][0] = '+';

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << val;
        std::string val_str = ss.str();

        for (size_t j = 0; j < val_str.length() && (j + 1) < plot_width; ++j) {
            mutable_this->m_canvas[canvas_row][j + 1] = val_str[j];
        }
    }

    // 4. Draw 5 Abscissa (X-Axis) Ticks & Indices along the Bottom Border
    for (int i = 0; i < 5; ++i) {
        double ratio = static_cast<double>(i) / 4.0; // 0.0, 0.25, 0.5, 0.75, 1.0
        size_t data_idx = static_cast<size_t>(std::round(ratio * (max_data_size - 1))) + 1;
        std::string idx_str = std::to_string(data_idx);

        if (i == 0) {
            for (size_t j = 0; j < idx_str.length() && (j + 1) < m_width; ++j) {
                mutable_this->m_canvas[m_height - 1][j + 1] = idx_str[j];
            }
        }
        else if (i == 4) {
            size_t col = m_width - 1 - idx_str.length();
            if (col > 5) {
                mutable_this->m_canvas[m_height - 1][col - 1] = '^';
                for (size_t j = 0; j < idx_str.length(); ++j) {
                    mutable_this->m_canvas[m_height - 1][col + j] = idx_str[j];
                }
            }
        }
        else {
            size_t col = 1 + static_cast<size_t>(std::round(ratio * (plot_width - 1)));
            if (col + idx_str.length() < m_width - 1) {
                mutable_this->m_canvas[m_height - 1][col - 1] = '^';
                for (size_t j = 0; j < idx_str.length(); ++j) {
                    mutable_this->m_canvas[m_height - 1][col + j] = idx_str[j];
                }
            }
        }
    }

    // 5. Output Final Rendered Chart
    for (const auto& row : mutable_this->m_canvas) {
        std::cout << row << '\n';
    }
}