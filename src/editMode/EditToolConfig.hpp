#pragma once

#include "../ToolConfig.hpp"


enum srcObjType {
    any,          // anything (objects)
    trig,         // triggers
    anim,         // animated objects
    keyFrame,     // keyframe
    areaEffect,   // area triggers
    item,         // item
    particle,     // particle generator
    collectable,  // also resetable
    sfxTrig,      // sfx trigger
    collision,    // collision block
    gradientTrig, // gradient trigger
};

// hey: don't forget to add a new type to Variant::getLowerMenuType()
enum sourceFuncType {
    addGr,        // just add to the group
    addGrSM,      // add group + set spawn trigger & multi trigger
    addGrAnim,    // add group + set animateOnTrigger
    addGrCol,     // exclusively for pulse trigger preview mode (does the same as addGr)
    color,        // does nothing to source objects
    setItem,      // sets item id
    setCollision, // sets collision block id
    setGradient,  // sets id in gradient trigger
    honestAddGr,  // add group using honest strategy
    copyGroup,    // copy groups from target obj to source objects
};

enum lowerMenuType {
    selectGroup,
    selectColor,
    selectItem,
    selectCollision,
    selectGradient,
    selectCopyGroup,
};

struct TWTObjCopy : public cocos2d::CCObject {
    TWTObjCopy() {
        this->autorelease();
        this->m_groups = new std::array<short, 10>;
    }
    ~TWTObjCopy() {
        delete this->m_groups;
    }
    std::array<short, 10> * m_groups;
    short m_groupCount;
    std::optional<bool> m_isSpawnTrigger;
    std::optional<bool> m_isMultiTrigger;
    std::optional<bool> m_isAnimOnTrigger;
    std::optional<int> m_itemID;
    std::optional<bool> m_isCollisionDyn;
    std::optional<int> m_collisionID;
    std::optional<int> m_gradientID;
};

struct Condition {
    std::pair<std::string, std::string> m_condition; // key, value (value "" == any)
    std::string m_yes;
    std::string m_no;
};

struct Variant {
    std::string m_name; // shown in menu
    std::string m_triggerConfigString;
    std::vector<Condition> m_triggerConditionalConfigString;
    sourceFuncType m_srcFuncType; // is used to create lower menu and apply the final config
    srcObjType m_srcObjType;
    bool m_appendNotOverride = false;

    std::pair<lowerMenuType, srcObjType> getLowerMenuType() {
        static const std::map<sourceFuncType, lowerMenuType> sourceFuncToMenuType = {
            {sourceFuncType::addGr, lowerMenuType::selectGroup},
            {sourceFuncType::addGrSM, lowerMenuType::selectGroup},
            {sourceFuncType::addGrAnim, lowerMenuType::selectGroup},
            {sourceFuncType::addGrCol, lowerMenuType::selectGroup},
            {sourceFuncType::color, lowerMenuType::selectColor},
            {sourceFuncType::setItem, lowerMenuType::selectItem},
            {sourceFuncType::setCollision, lowerMenuType::selectCollision},
            {sourceFuncType::setGradient, lowerMenuType::selectGradient},
            {sourceFuncType::honestAddGr, lowerMenuType::selectGroup},
            {sourceFuncType::copyGroup, lowerMenuType::selectCopyGroup},
        };
        return {sourceFuncToMenuType.at(this->m_srcFuncType), this->m_srcObjType};
    }
};

const Variant copyGroupDefaultVariant = {"Copy_group", "", {}, sourceFuncType::copyGroup, srcObjType::any};

const std::map<objId, std::vector<Variant>> CONFIGURATION = {
    {899, { // color trigger
        {"Color", "23,g", {}, sourceFuncType::color, srcObjType::any},
    }},
    {901, { // move trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Target_pos", "100,1,394,0,71,g", {{{"394", "1"}, "100,0,394,1"}}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "100,1,394,0,395,g", {{{"394", "1"}, "100,0,394,1"}}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1616, { // stop trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::trig},
    }},
    {1006, { // pulse trigger
        {"Color", "52,0,51,g", {}, sourceFuncType::color, srcObjType::any},
        {"Group", "52,1,51,g", {}, sourceFuncType::addGrCol, srcObjType::any}, 
    }},
    {1007, { // alpha trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1049, { // toggle trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1268, { // spawn trigger
        {"Group", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
    }},
    {1346, { // rotate trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Rot_target", "100,1,401,g", {{{"394", "1"}, "100,0,394,1"}}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2067, { // scale trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1347, { // follow trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Follow", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1585, { // animate trigger
        {"Group", "51,g", {}, sourceFuncType::addGrAnim, srcObjType::anim},
    }},
    {3033, { // keyframe trigger
        {"Animation", "76,g", {}, sourceFuncType::addGr, srcObjType::keyFrame},
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Parent", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1814, { // follow player Y trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3016, { // advanced follow trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Follow", "138,0,200,0,201,0,71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3660, { // edit advanced follow trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3661, { // re-target advanced follow trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Follow", "138,0,200,0,201,71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3032, { // keyframe object
        {"Spawn", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3006, { // area move
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "538,0,71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3007, { // area rotate
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "538,0,71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3008, { // area scale
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "538,0,71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3009, { // area fade
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "538,0,71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3010, { // area tint
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Color", "538,0,278,0,71,g", {}, sourceFuncType::color, srcObjType::any},
    }},
    {3011, { // edit area move todo: support for area effect id
        {"Group", "355,0,51,g", {}, sourceFuncType::addGr, srcObjType::areaEffect},
    }},
    {3012, { // edit area rotate
        {"Group", "355,0,51,g", {}, sourceFuncType::addGr, srcObjType::areaEffect},
    }},
    {3013, { // edit area scale
        {"Group", "355,0,51,g", {}, sourceFuncType::addGr, srcObjType::areaEffect},
    }},
    {3014, { // edit area fade
        {"Group", "355,0,51,g", {}, sourceFuncType::addGr, srcObjType::areaEffect},
    }},
    {3015, { // edit area tint
        {"Group", "355,0,51,g", {}, sourceFuncType::addGr, srcObjType::areaEffect},
    }},
    {1595, { // touch trigger
        {"Spawn", "82,1,51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Toggle", "82,0,51,g", {{{"82", "1"}, "82,1"}, {{"82", "2"}, "82,2"}}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1611, { // count trigger
        {"Spawn", "56,1,51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Toggle", "56,0,51,g", {{{"56", "1"}, "56,1"}}, sourceFuncType::addGr, srcObjType::any}, 
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {1811, { // instant count trigger
        {"Spawn", "56,1,51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Toggle", "56,0,51,g", {{{"56", "1"}, "56,1"}}, sourceFuncType::addGr, srcObjType::any},
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {1817, { // pickup trigger
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {3614, { // time trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {3615, { // time event trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {3617, { // time control trigger
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {3619, { // item edit trigger
        {"Target", "478,1,51,g", {}, sourceFuncType::setItem, srcObjType::item},
        {"Item_1", "476,1,80,g", {}, sourceFuncType::setItem, srcObjType::item},
        {"Item_2", "477,1,95,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {3620, { // item compare trigger
        {"Item_1", "476,1,80,g", {}, sourceFuncType::setItem, srcObjType::item},
        {"Item_2", "477,1,95,g", {}, sourceFuncType::setItem, srcObjType::item},
        {"On_false", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"On_true", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
    }},
    {3641, { // item persist trigger
        {"Item", "80,g", {}, sourceFuncType::setItem, srcObjType::item},
    }},
    {1912, { // random trigger
        {"Group_1", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Group_2", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
    }},
    {3607, { // sequence trigger
        {"Append", "435,g.1", {}, sourceFuncType::addGrSM, srcObjType::trig, true},
    }},
    {3608, { // spawn particle trigger
        {"Particle", "51,g", {}, sourceFuncType::addGr, srcObjType::particle},
        {"Position", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3618, { // reset trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::collectable},
    }},
    {1914, { // static camera trigger
        {"Target", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2062, { // edge camera trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3605, { // edit song trigger
        {"Central_obj", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Controlling_obj", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3602, { // sfx trigger
        {"Central_obj", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Controlling_obj", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3603, { // edit sfx trigger
        {"Central_obj", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Controlling_obj", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"SFX_group", "457,g", {}, sourceFuncType::addGr, srcObjType::sfxTrig},
    }},
    {3604, { // event trigger
        {"Group", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
    }},
    {3613, { // UI trigger
        {"UI_objects", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Anchor", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3662, { // link visible trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1815, { // collision trigger
        {"Spawn", "56,1,51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Toggle", "56,0,51,g", {{{"56", "1"}, "56,1"}}, sourceFuncType::addGr, srcObjType::any},
        {"Block_A", "138,0,200,0,201,0,80,g", {}, sourceFuncType::setCollision, srcObjType::collision},
        {"Block_B", "201,0,95,g", {}, sourceFuncType::setCollision, srcObjType::collision},
    }},
    {3609, { // instant collision trigger
        {"On_true", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"On_false", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Block_A", "138,0,200,0,201,0,80,g", {}, sourceFuncType::setCollision, srcObjType::collision},
        {"Block_B", "201,0,95,g", {}, sourceFuncType::setCollision, srcObjType::collision},
    }},
    {3640, { // state block
        {"State_on", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"State_off", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
    }},
    {3643, { // toggle block
        {"Spawn", "56,1,504,1,51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Toggle", "56,0,504,0,51,g", {{{"56", "1"}, "56,1"}}, sourceFuncType::addGr, srcObjType::any},
    }},
    {1812, { // death trigger
        {"Spawn", "56,1,51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Toggle", "56,0,51,g", {{{"56", "1"}, "56,1"}}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3600, { // end trigger
        {"Spawn_id", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"End_pos", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    // {2903, { // gradient trigger
    //     {"Disable", "208,1,209,g", {}, sourceFuncType::setGradient, srcObjType::gradientTrig},
    //     {"Point_1", "203,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
    //     {"Point_2", "204,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
    //     {"Point_3", "205,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
    //     {"Point_4", "206,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
    // }},
    {3022, { // teleport trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2905, { // shock wave shader
        {"Group", "138,0,200,0,188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2907, { // shock line shader
        {"Group", "138,0,200,0,188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2913, { // lens circles shader
        {"Group", "138,0,200,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2915, { // motion blur shader
        {"Group", "138,0,200,0,201,0,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2916, { // bulge shader
        {"Group", "188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2917, { // pinch shader
        {"Group", "188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},

};
 
/*

1,1815,2,315,3,105,36,1,51,0,10,0.5;
1,1815,2,315,3,105,36,1,51,0,10,0.5,138,1;
1,1815,2,315,3,105,36,1,51,0,10,0.5,138,1,200,1;
1,1815,2,315,3,105,36,1,51,0,10,0.5,138,1,200,1,201,1;


*/