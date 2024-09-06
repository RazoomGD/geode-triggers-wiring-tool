#pragma once

#include <map>
#include <utility>
#include <vector>
#include <string>
#include <set>

using objId = short;

enum srcObjType {
    any,      // anything (objects)
    trig,     // triggers
    anim,     // animated objects
    keyFrame, // keyframe
};

enum sourceFuncType {
    addGr,     // just add to the group
    addGrSM,   // add group + set spawn trigger & multi trigger
    addGrAnim, // add group + set animateOnTrigger
    color,     // does nothing to source objects
};

enum lowerMenuType {
    selectGroup,
    selectColor,
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
// Beast, bat and spike-ball
const std::set<objId> animatedSpecialIDs = {
    918, 1584, 2012
};

// Special objects (objects we process differently from triggers)
const objId keyFrameOjb = 3032;

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

struct TWTObjCopy : public CCObject {
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

    std::pair<lowerMenuType, srcObjType> getLowerMenuType() {
        static const std::map<sourceFuncType, lowerMenuType> sourceFuncToMenuType = {
            {sourceFuncType::addGr, lowerMenuType::selectGroup},
            {sourceFuncType::addGrSM, lowerMenuType::selectGroup},
            {sourceFuncType::addGrAnim, lowerMenuType::selectGroup},
            {sourceFuncType::color, lowerMenuType::selectColor},
        };
        return {sourceFuncToMenuType.at(this->m_srcFuncType), this->m_srcObjType};
    }
};

const std::map<objId, std::map<srcObjType, std::vector<Variant>>> CONFIGURATION = {
    {899, { // color trigger
        {srcObjType::any, {
            {"Color", "23,g", {}, sourceFuncType::color, srcObjType::any},
    }}}},
    {901, { // move trigger
        {srcObjType::any, {
            {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
            {"Target_pos", "100,1,394,0,71,g", {{{"394", "1"}, "100,0,394,1"}}, sourceFuncType::addGr, srcObjType::any},
            {"Center", "100,1,394,0,395,g", {{{"394", "1"}, "100,0,394,1"}}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {1616, { // stop trigger
        {srcObjType::trig, {
            {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::trig},
    }}}},
    {1006, { // pulse trigger
        {srcObjType::any, {
            {"Color", "52,0,51,g", {}, sourceFuncType::color, srcObjType::any},
            {"Group", "52,1,51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {1007, { // alpha trigger
        {srcObjType::any, {
            {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {1049, { // toggle trigger
        {srcObjType::any, {
            {"Group", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {1268, { // spawn trigger
        {srcObjType::trig, {
            {"Group", "51,g", {}, sourceFuncType::addGrSM, srcObjType::trig},
    }}}},
    {1346, { // rotate trigger
        {srcObjType::any, {
            {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
            {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
            {"Rot_target", "100,1,401,g", {{{"394", "1"}, "100,0,394,1"}}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {2067, { // scale trigger
        {srcObjType::any, {
            {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
            {"Center", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {1347, { // follow trigger
        {srcObjType::any, {
            {"Target", "51,g", {}, sourceFuncType::addGr, srcObjType::any},
            {"Follow", "71,g", {}, sourceFuncType::addGr, srcObjType::any},
    }}}},
    {1585, { // animate trigger
        {srcObjType::anim, {
            {"Group", "51,g", {}, sourceFuncType::addGrAnim, srcObjType::anim},
    }}}},


};

/*

1,1585,2,735,3,75,36,1,51,0,76,0;
1,1585,2,735,3,75,36,1,51,111,76,0;
1,1585,2,735,3,75,36,1,51,111,76,0;

1,918,2,885,3,105,57,5.7;




*/