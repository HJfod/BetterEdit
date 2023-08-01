#pragma once

#include "Macros.hpp"
#include <Geode/DefaultInclude.hpp>

namespace better_edit {
    BE_DLL bool isGridSizeLocked();
    BE_DLL void zoomGridSize(bool in);
    BE_DLL float getGridSize();
    BE_DLL void setGridSize(float size);
    BE_DLL void setGridSizeLocked(bool lock);
}
