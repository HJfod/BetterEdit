#pragma once

#include "../../../BetterEdit.hpp"

void goToImportLayer(CCArray*);
void addLevelsToImportLayer(CCArray*);
void addLevelsToImportLayerFromVector(std::vector<std::string> const&);
void notifyErrorsWhileImporting(std::vector<std::string> const&);
