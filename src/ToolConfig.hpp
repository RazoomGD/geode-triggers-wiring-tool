#pragma once

#include <map>
#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <cstring>
#include <optional>
#include <set>

using objId = short;

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
};

enum lowerMenuType {
    selectGroup,
    selectColor,
    selectItem,
    selectCollision,
    selectGradient,
};

// Triggers (basically every object you are able to set as spawn trigger & multi trigger)
const std::set<objId> triggerIDs = {
    105, 744, 900, 29, 30, 915, 2063, 899, 1585, 3033, 3011, 3012, 1817, 3614, 1914, 1913, 3602, 3603, 3018, 
    3017, 23, 2899, 2913, 2914, 24, 901, 3021, 1006, 1814, 3016, 3660, 3013, 3014, 3015, 3615, 3617, 3619, 2015, 
    2901, 1916, 3604, 1935, 2999, 1616, 1812, 33, 3022, 2066, 2903, 2915, 2916, 2917, 25, 26, 27, 3019, 3020, 
    1007, 1049, 1268, 3661, 55, 3006, 3024, 3029, 3030, 3620, 3641, 1912, 3023, 2925, 2062, 3606, 3612, 56, 32, 
    1613, 1612, 2907, 2905, 2904, 2919, 2920, 2921, 28, 1346, 2067, 1347, 1520, 3007, 3008, 3009, 3010, 3031, 
    1595, 1611, 1811, 2068, 3607, 3608, 3618, 3605, 1934, 2900, 1917, 59, 57, 1815, 3609, 1818, 1819, 3600, 1932, 
    2912, 2911, 2910, 2909, 2922, 2923, 2924, 22, 1915, 58
};

// Animated objects (objects that can be animated on animate trigger)
const std::set<objId> animatedIDs = {
    2065, 3000, 3001, 3002, 1583, 1591, 1593, 1697, 1698, 1699, 1841, 1842, 1936, 2026, 2027, 2028, 2038, 2039, 
    2040, 2050, 2051, 2052, 2871, 2872, 2873, 2883, 2884, 2885, 2223, 3121, 1329, 1053, 1592, 1054, 1849, 1850, 
    1851, 1852, 1857, 1860, 1516, 1518, 1937, 1938, 1939, 2020, 2029, 2030, 2031, 2032, 2041, 2042, 2043, 2044, 
    2053, 2054, 2055, 2864, 2874, 2875, 2876, 2877, 2886, 2887, 2888, 2889, 2246, 2605, 2629, 2630, 3303, 3304, 
    3482, 4211, 3492, 3493, 1050, 920, 923, 924, 1853, 1854, 1855, 1519, 1618, 2021, 2022, 2023, 2033, 2034, 2035, 
    2045, 2046, 2047, 2865, 2867, 2868, 2878, 2879, 2880, 2890, 2891, 2892, 2694, 3119, 3120, 3483, 3484, 4300, 
    1051, 1052, 921, 2894, 1858, 1856, 1839, 1840, 2024, 2025, 2036, 2037, 2048, 2049, 2869, 2870, 2881, 2882, 
    2893, 918, 1584, 2012
};

// objects (not triggers) that contain item id 
const std::set<objId> itemIdContainingObjects = {
    4402, 4414, 4426, 4438, 4450, 4462, 4474, 4486, 4497, 4508, 4520, 4532, 4532, 4538, 4523, 4537, 1587, 4403, 
    4533, 4417, 4416, 4415, 4427, 4511, 4510, 4441, 4440, 4439, 4451, 4509, 4453, 4463, 4521, 4465, 4475, 4535, 
    4477, 4487, 1275, 4404, 4405, 4428, 4429, 4452, 4464, 4476, 4488, 4498, 4499, 4522, 4534, 4500, 4491, 1589, 
    4406, 4536, 4539, 4420, 4419, 4418, 4430, 4514, 4513, 4444, 4443, 4442, 4454, 4512, 4456, 4466, 4524, 4468, 
    4478, 4480, 4489, 4526, 4407, 4408, 4431, 4432, 4455, 4467, 4479, 4490, 4501, 4502, 4525, 4524, 4539, 4503, 
    4495, 1598, 4504, 3601, 4409, 4410, 4530, 4424, 4423, 4422, 4421, 4433, 4434, 4518, 4517, 4448, 4447, 4446, 
    4445, 4457, 4459, 4516, 4460, 4469, 4470, 4515, 4472, 4481, 4482, 4527, 4484, 4492, 4493, 4528, 1614, 4411, 
    4412, 4435, 4436, 4458, 4471, 4483, 4494, 4505, 4506, 4529, 4519, 4507, 4401, 4413, 4425, 4437, 4449, 4461, 
    4473, 4485, 4496, 4531
};

// collectable (resetable)
const std::set<objId> collectableIDs = {
    2063, 1275, 4402, 4403, 4415, 4414, 4426, 4427, 4439, 4438, 4450, 4451, 4462, 4463, 4474, 4475, 4486, 4487, 
    4497, 4537, 4509, 4508, 4520, 4521, 4532, 4533, 4532, 1587, 1589, 4404, 4405, 4406, 4407, 4419, 4418, 4417, 
    4416, 4428, 4429, 4430, 4431, 4443, 4442, 4441, 4440, 4452, 4453, 4454, 4455, 4464, 4465, 4466, 4467, 4476, 
    4477, 4478, 4479, 4488, 4538, 4489, 4490, 4498, 4499, 4500, 4501, 4513, 4512, 4511, 4510, 4522, 4523, 4524, 
    4525, 4534, 4535, 4536, 4539, 4524, 4539, 1598, 4408, 4409, 4410, 4422, 4421, 4420, 4432, 4433, 4434, 4446, 
    4445, 4444, 4456, 4457, 4458, 4468, 4469, 4470, 4480, 4481, 4482, 4491, 4492, 4493, 4502, 4503, 4504, 4516, 
    4515, 4514, 4526, 4527, 4528, 1614, 3601, 4401, 4411, 4412, 4413, 4425, 4424, 4423, 4435, 4436, 4437, 4449, 
    4448, 4447, 4459, 4460, 4461, 4471, 4472, 4473, 4483, 4484, 4485, 4494, 4495, 4496, 4505, 4506, 4507, 4519, 
    4518, 4517, 4529, 4530, 4531
};

// Beast, bat and spike-ball
const std::set<objId> animatedSpecialIDs = {
    918, 1584, 2012
};

// Area effect triggers (that contain areaEffectId)
const std::set<objId> areaEffectsIDs = { 
    3006, 3007, 3008, 3009, 3010
};

const objId keyFrameOjbID = 3032; // keyframe path object
const objId itemObjID = 1615;
const objId particleObjID = 2065;
const objId sfxTriggerObjID = 3602;
const objId collisionBlockID = 1816;
const objId gradientTriggerId = 2903;

const std::map<int, std::string> colorIdName = {
    {1005, "Player-1"},
    {1006, "Player-2"},
    {1000, "BG"},
    {1001, "G-1"},
    {1013, "MG-1"},
    {1007, "LBG"},
    {1009, "G-2"},
    {1014, "MG-2"},
    {1004, "Obj"},
    {1002, "Line"},
    {1003, "3d-line"},
    {1012, "Lighter"},
    {1010, "Black"},
    {1011, "White"},
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

const struct Condition {
    std::pair<std::string, std::string> m_condition; // key, value (value "" == any)
    std::string m_yes;
    std::string m_no;
};

const struct Variant {
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
        };
        return {sourceFuncToMenuType.at(this->m_srcFuncType), this->m_srcObjType};
    }
};

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
        {"Follow", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3660, { // edit advanced follow trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3661, { // re-target advanced follow trigger
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Follow", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3032, { // keyframe object
        {"Spawn", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3006, { // area move
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3007, { // area rotate
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3008, { // area scale
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3009, { // area fade
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {3010, { // area tint
        {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
        {"Color", "278,0,71,g", {}, sourceFuncType::color, srcObjType::any},
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
        {"Block_A", "80,g", {}, sourceFuncType::setCollision, srcObjType::collision},
        {"Block_B", "95,g", {}, sourceFuncType::setCollision, srcObjType::collision},
    }},
    {3609, { // instant collision trigger
        {"On_true", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"On_false", "71,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Block_A", "80,g", {}, sourceFuncType::setCollision, srcObjType::collision},
        {"Block_B", "95,g", {}, sourceFuncType::setCollision, srcObjType::collision},
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
        {"Group", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
        {"Target_pos", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2903, { // gradient trigger
        {"Disable", "208,1,209,g", {}, sourceFuncType::setGradient, srcObjType::gradientTrig},
        {"Point_1", "203,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
        {"Point_2", "204,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
        {"Point_3", "205,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
        {"Point_4", "206,g", {}, sourceFuncType::honestAddGr, srcObjType::any},
    }},
    {3022, { // teleport trigger
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2905, { // shock wave shader
        {"Group", "188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2907, { // shock line shader
        {"Group", "188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2913, { // lens circles shader
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2915, { // motion blur shader
        {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},
    {2916, { // bulge shader
        {"Group", "188,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }},

};
 
/*



*/
