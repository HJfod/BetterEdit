#pragma once

#include "../../BetterEdit.hpp"
#include <array>

class LayerManager {
    public:
        struct Layer {
            int m_nLayerNumber;
            bool m_bLocked = false;
            bool m_bVisible = true;
            float m_fOpacity = .3f;
            const char* m_sName = nullptr;

            Layer(int n) : m_nLayerNumber(n) {}
        };

        static constexpr const unsigned int max_layer_num = 999;

        using Layers = std::array<Layer*, max_layer_num>;
        using LevelName = std::string;

    protected:
        std::map<LevelName, Layers> m_mLayers;

        bool init();

    public:
        void encodeDataTo(DS_Dictionary* data);
        void dataLoaded(DS_Dictionary* data);

        LevelName getLevel();
        Layer * getLayer(int number);

        static bool initGlobal();
        static LayerManager* get();
};
