#pragma once

#include <cmath>

class GridSizeDelegate {
    protected:
        float m_fGridSize = 30.0f;

        GridSizeDelegate() = default;
        virtual ~GridSizeDelegate() = default;

    public:
        static GridSizeDelegate* get() {
            static GridSizeDelegate* instance = nullptr;
            if (!instance) {
                instance = new GridSizeDelegate();
            }
            return instance;
        }

        bool useCustomGridSize() {
            return m_fGridSize != 30.0f;
        }

        std::string getGridSizeString() {
            std::stringstream stream;
            
            int precision = 4;
            if (m_fGridSize >= 1.0f) {
                precision = 4 - int(log10(int(m_fGridSize)) + 1); // 4 sig figs
            }

            stream << std::fixed << std::setprecision(precision) << m_fGridSize;
            return stream.str();
        }

        float getGridSize() {
            return m_fGridSize;
        }

        void setGridSize(float size) {
            m_fGridSize = geode::utils::clamp(size, 0.9375f, 120.0f);
        }
};