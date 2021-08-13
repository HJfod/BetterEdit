#pragma once

#include "../../../BetterEdit.hpp"
#include "LevelBackupManager.hpp"

static constexpr const BoomListType kBoomListType_Backup
    = static_cast<BoomListType>(0x420);

class BackupCell : public TableViewCell {
    protected:
		BackupCell(const char* name, CCSize size);

        void draw() override;
	
	public:
        void loadFromBackup(LevelBackup* backup);

		static BackupCell* create(const char* key, CCSize size);
};

class BackupListView : public CustomListView {
    protected:
        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;
    
    public:
        static BackupListView* create(CCArray* backups, float width, float height);
};
