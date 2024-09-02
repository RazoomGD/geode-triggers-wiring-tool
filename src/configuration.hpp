#include <map>
#include <utility>
#include <vector>
#include <string>
#include <set>

using objId = short;

enum srcObjType {
    any,   // anything (objects)
    trig,  // triggers
    anim,  // animated objects
    keyFrame, // keyframe
};

enum sourceFuncType {
    addGr,    // just add to the group
    addGrSM,  // add group + set spawn trigger & multi trigger
    addGrAnim, // add group + set animateOnTrigger
    color,    // does nothing to source objects
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

// Animated objects (objects that can be animated with animate trigger)
const std::set<objId> animatedIDs = {
    2065, 3000, 1583, 1697, 1841, 2026, 2038, 2050, 2871, 2871, 2883, 3121, 918, 1327, 1053, 3002, 3001, 1591, 1593, 
    1849, 1850, 1698, 1699, 1857, 1860, 1842, 1936, 1937, 1938, 2030, 2029, 2028, 2027, 2039, 2040, 2041, 2042, 2054, 
    2053, 2052, 2051, 2872, 2873, 2874, 2875, 2884, 2885, 2886, 2887, 2888, 2886, 2223, 2246, 2605, 3219, 3303, 3304, 
    3492, 3493, 4211, 1329, 1328, 1584, 2012, 920, 1592, 1054, 1851, 1852, 1853, 1516, 1518, 1517, 1939, 2020, 2021, 
    2033, 2032, 2031, 2043, 2044, 2045, 2865, 2864, 2055, 2876, 2877, 2878, 2889, 2890, 2891, 2629, 2630, 3482, 3483, 
    4300, 919, 1050, 1051, 921, 924, 923, 1854, 1855, 1858, 1519, 1618, 1839, 2022, 2023, 2024, 2036, 2035, 2034, 2046, 
    2047, 2048, 2869, 2868, 2867, 2879, 2880, 2881, 2892, 2893, 2694, 3119, 3484, 1052, 1582, 1856, 1840, 2025, 2037, 
    2049, 2870, 2882, 2894, 3120 
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



/*

1,4300,2,765,3,-456,57,32,155,10,156,12,128,2.5,129,2.5,      107,1;
1,4300,2,765,3,-456,57,32,155,10,156,12,128,2.5,129,2.5,123,1,107,1;
1,2065,2,885,3,-405,57,32,155,2,156,13,145,;
1,2065,2,885,3,-405,57,32,155,2,156,13,123,1,145,30a-1a1a0.;

*/
// Common objects are all the remaining ids

const struct Condition {
    std::pair<std::string, std::string> m_condition; // key, value (value "" == any)
    std::string m_yes;
    std::string m_no;
};

const struct Variant {
    std::string m_name;
    std::string m_triggerConfigString;
    std::vector<Condition> m_triggerConditionalConfigString;
};

const struct TargetOption {
    sourceFuncType m_srcFuncType;
    std::vector<Variant> m_variants;
};

const std::map<objId, std::map<srcObjType, TargetOption>> CONFIGURATION = {
    {899, { // color trigger
        {srcObjType::any, {sourceFuncType::color, {{"Color", "23,g"}}}},
    }},
    {901, { // move trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Target", "51,g"}, 
                                                    {"TargetPos", "100,1,394,0,71,g", {{{"394", "1"}, "100,0,394,1"}}}, 
                                                    {"Center", "100,1,394,0,395,g", {{{"394", "1"}, "100,0,394,1"}}},
                                                    }}},
    }},
    {1616, { // stop trigger
        {srcObjType::trig, {sourceFuncType::addGr, {{"Target", "51,g"}}}},
    }},
    {1006, { // pulse trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Group", "52,1,51,g"}}}},
    }},
    {1007, { // alpha trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Group", "51,g"}}}},
    }},
    {1049, { // toggle trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Group", "51,g"}}}},
    }}, 
    {1268, { // spawn trigger
        {srcObjType::trig, {sourceFuncType::addGrSM, {{"Group", "51,g"}}}},
    }},
    {1346, { // rotate trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Target", "51,g"}, 
                                                    {"Center", "71,g"}, 
                                                    {"Rot-Target", "100,1,394,0,401,g", {{{"394", "1"}, "100,0,394,1"}}},
                                                    }}},
    }},
    {2067, { // scale trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Target", "51,g"}, {"Center", "71,g"}}}},
    }},
    {1347, { // follow trigger
        {srcObjType::any, {sourceFuncType::addGr, {{"Target", "51,g"}, {"Follow", "71,g"}}}},
    }},
    {1585, { // animate trigger
        {srcObjType::anim, {sourceFuncType::addGrAnim, {{"Group", "51,g"}}}},
    }},
    {1585, { // keyframe trigger
        {srcObjType::keyFrame, {sourceFuncType::addGr, {{"Animation", "76,g"}}}},
    }},

};


/*

1,3033,2,225,3,15,36,1,         520,1,521,1,545,1,522,1,523,1,546,1;
1,3033,2,225,3,15,36,1,76,22222,520,1,521,1,545,1,522,1,523,1,546,1;

*/