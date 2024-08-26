#include <map>
#include <utility>
#include <vector>
#include <string>
#include <set>

// types of source objects
struct objTypes {
    static const short any = 1; // anything
    static const short obj = 2; // objects
    static const short trig = 3; // triggers
};

// types of source functions
struct funcTypes {
    static const short addGr = 1; // just add to the group
    static const short addGrSM = 2; // add group + set spawn trigger & multi trigger
};

// Triggers (basically every object you are able to set as spawn trigger & multi trigger)
std::set<short> triggersIDs = {
    105, 744, 900, 29, 30, 915, 2063, 899, 1585, 3033, 3011, 3012, 1817, 3614, 1914, 1913, 3602, 3603, 3018, 
    3017, 23, 2899, 2913, 2914, 24, 901, 3021, 1006, 1814, 3016, 3660, 3013, 3014, 3015, 3615, 3617, 3619, 2015, 
    2901, 1916, 3604, 1935, 2999, 1616, 1812, 33, 3022, 2066, 2903, 2915, 2916, 2917, 25, 26, 27, 3019, 3020, 
    1007, 1049, 1268, 3661, 55, 3006, 3024, 3029, 3030, 3620, 3641, 1912, 3023, 2925, 2062, 3606, 3612, 56, 32, 
    1613, 1612, 2907, 2905, 2904, 2919, 2920, 2921, 28, 1346, 2067, 1347, 1520, 3007, 3008, 3009, 3010, 3031, 
    1595, 1611, 1811, 2068, 3607, 3608, 3618, 3605, 1934, 2900, 1917, 59, 57, 1815, 3609, 1818, 1819, 3600, 1932, 
    2912, 2911, 2910, 2909, 2922, 2923, 2924, 22, 1915, 58
};

// Special objects (objects we process differently from triggers)


// Common objects are all the remaining ids


std::map<short, std::map<short, std::vector<std::pair<std::pair<std::string, std::string>, short>>>> CONFIGURATION = {
    {901, { // move trigger
        {objTypes::any,  {{{{"Target", "51,g"}, funcTypes::addGr}, {{"TargetPos", "100,1,394,0,71,g"}, funcTypes::addGr}, {{"Center", "100,1,394,0,395,g"}, funcTypes::addGr}}}},
    }},
};

/*





*/