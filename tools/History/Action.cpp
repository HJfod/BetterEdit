#include "Action.hpp"

CCNode* ActionObject::createActionCellItem(float width, float height) {
    auto label = CCLabelBMFont::create(this->describe().c_str(), "goldFont.fnt");
    label->setScale(.3f);
    label->setAnchorPoint({ .0f, .5f });
    label->setPosition(20.0f, height / 2);
    return label;
}

CCNode* ObjectAction::createActionCellItem(float width, float height) {
    auto layer = CCNode::create();

    auto label = CCLabelBMFont::create(this->describe().c_str(), "goldFont.fnt");
    label->setScale(.3f);
    label->setAnchorPoint({ .0f, .5f });
    label->setPositionY(5.0f);
    layer->addChild(label);

    std::string uuidStr = "";

    if (this->m_vObjectUUIDs.size()) {
        uuidStr = "Obj UUIDs: ";
        for (auto const& uuid : this->m_vObjectUUIDs)
            uuidStr += std::to_string(uuid);
    } else {
        uuidStr = "Obj UUID: " + std::to_string(this->m_nObjectUUID);
    }

    auto uuidLabel = CCLabelBMFont::create(
        uuidStr.c_str(), "chatFont.fnt"
    );
    uuidLabel->setScale(.3f);
    uuidLabel->setAnchorPoint({ .0f, .5f });
    uuidLabel->setPositionY(-5.0f);
    layer->addChild(uuidLabel);
    
    layer->setPosition(20.0f, height / 2);
    
    return layer;
}
