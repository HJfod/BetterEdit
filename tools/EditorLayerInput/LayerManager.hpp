#pragma once

#include "../../BetterEdit.hpp"
#include <array>

class LayerManager {
    public:
        static constexpr const unsigned int default_gd_opacity = 50;
        static constexpr const unsigned int use_default_opacity = -1;
        int default_opacity = default_gd_opacity;

        struct Layer {
            int m_nLayerNumber;
            bool m_bLocked = false;
            bool m_bVisible = true;
            int m_nOpacity = use_default_opacity;
            std::string m_sName = "";

            Layer(int n) : m_nLayerNumber(n) {}
        };

        static constexpr const unsigned int max_layer_num = 499;

        struct Level {
            std::array<Layer*, max_layer_num> m_mLayers;
            bool m_bMultipleVisible = false;
            std::set<int> m_vVisibleLayers;

            inline void addVisible(int index) {
                if (!m_bMultipleVisible)
                    m_vVisibleLayers.clear();

                m_vVisibleLayers.insert(index);
            }

            inline bool isMultiple() {
                return m_vVisibleLayers.size();
            }

            inline void clearVisible() {
                m_vVisibleLayers.clear();
            }
        };

        using LevelName = std::string;

    protected:
        std::map<LevelName, Level*> m_mLevels {};

        bool init();

    public:
        void encodeDataTo(DS_Dictionary* data);
        void dataLoaded(DS_Dictionary* data);

        LevelName getLevelName();
        Layer * getLayer(int number);
        Level* getLevel();

        static bool initGlobal();
        static LayerManager* get();
};
