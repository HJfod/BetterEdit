#pragma once

#include "../../../BetterEdit.hpp"
#include "LevelBackupManager.hpp"
#include "BackupViewLayer.hpp"

static constexpr const BoomListType kBoomListType_Backup
    = static_cast<BoomListType>(0x420);

class BackupCell : public TableViewCell {
    protected:
        BackupViewLayer* m_pBackupLayer;
        LevelBackup* m_pBackup;

		BackupCell(const char* name, CCSize size);

        void draw() override;

        void onView(CCObject*);
	
	public:
        void loadFromBackup(LevelBackup* backup);

		static BackupCell* create(BackupViewLayer*, const char* key, CCSize size);
};

class BackupListView : public CustomListView {
    protected:
        BackupViewLayer* m_pBackupLayer;

        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;
    
    public:
        static BackupListView* create(CCArray* backups, BackupViewLayer*, float width, float height);
};
