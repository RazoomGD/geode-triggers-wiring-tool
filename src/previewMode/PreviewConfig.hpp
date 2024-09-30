#include "../ToolConfig.hpp"

using namespace geode::prelude;

enum fieldType {
    groupIdField, 
    groupMapField,
    // colorId, - for future support
    itemIdField,
    collisionBlockIdField,
    // gradientIdField, - for future support
};

enum searchType {
    groupSearch,
    itemSearch,
    collisionBlockSearch,
    // areaEffectSearch,
    // gradientSearch,
};

struct ConditionPreview {
    std::string m_conditionKey;
    std::string m_conditionValue; // "" - any, "0" - no such key
};

struct ObjField {
    std::string m_name;
    std::string m_key;
    fieldType m_type;
    std::vector<ConditionPreview> m_conditions; // all other required keys
};

struct SearchResult {
    SearchResult(searchType searchType, short number, GameObject * queryObj) : 
        m_searchType(searchType), m_number(number), m_queryObject(queryObj) {
        this->m_resultObjects = CCArray::create();
    }
    searchType m_searchType;
    short m_number; // group or id of something
    Ref<GameObject> m_queryObject;
    Ref<CCArray> m_resultObjects;
};


const std::map<objId, std::vector<ObjField>> PREVIEW_CONFIG = {
    {3027, { // teleport orb
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {1594, { // button orb
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {747, { // old teleport
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {2902, { // new teleport
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {2063, { // checkpoint
        {"Target", "51", fieldType::groupIdField, {}},
        {"Target_pos", "71", fieldType::groupIdField, {}},
        {"Respawn_group", "448", fieldType::groupIdField, {}},
    }},
    {901, { // move trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Target_pos", "71", fieldType::groupIdField, {{"100", "1"}}},
        {"Target_pos", "71", fieldType::groupIdField, {{"394", "1"}}},
        {"Center", "395", fieldType::groupIdField, {{"100", "1"}}},
        {"Center", "395", fieldType::groupIdField, {{"394", "1"}}},
    }},
    {1616, { // stop trigger
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {1006, { // pulse trigger (group mode)
        {"Group", "51", fieldType::groupIdField, {{"52", "1"}}},
    }},
    {1007, { // alpha trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1049, { // toggle trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1268, { // spawn trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1346, { // rotate trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
        {"Rot_target", "401", fieldType::groupIdField, {{"100", "1"}}},
        {"Rot_target", "401", fieldType::groupIdField, {{"394", "1"}}},
    }},
    {2067, { // scale trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
    }},
    {1347, { // follow trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Follow", "71", fieldType::groupIdField, {}},
    }},
    {1585, { // animate trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {3033, { // keyframe trigger
        {"Animation", "76", fieldType::groupIdField, {}},
        {"Target", "51", fieldType::groupIdField, {}},
        {"Parent", "71", fieldType::groupIdField, {}},
    }},
    {1814, { // follow player Y trigger
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {3016, { // advanced follow trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Follow", "71", fieldType::groupIdField, {{"138","0"}, {"200","0"}, {"201","0"}}},
    }},
    {3660, { // edit advanced follow trigger
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {3661, { // re-target advanced follow trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Follow", "71", fieldType::groupIdField, {{"138","0"}, {"200","0"}, {"201","0"}}},
    }},
    {3032, { // keyframe object
        {"Spawn", "71", fieldType::groupIdField, {}},
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {3006, { // area move
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {{"538","0"}}},
    }},
    {3007, { // area rotate
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {{"538","0"}}},
    }},
    {3008, { // area scale
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {{"538","0"}}},
    }},
    {3009, { // area fade
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {{"538","0"}}},
    }},
    {3010, { // area tint (no color)
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {{"538","0"}}},
    }},
    {3011, { // edit area move
        {"Group", "51", fieldType::groupIdField, {{"355", "0"}}},
    }},
    {3012, { // edit area rotate
        {"Group", "51", fieldType::groupIdField, {{"355", "0"}}},
    }},
    {3013, { // edit area scale
        {"Group", "51", fieldType::groupIdField, {{"355", "0"}}},
    }},
    {3014, { // edit area fade
        {"Group", "51", fieldType::groupIdField, {{"355", "0"}}},
    }},
    {3015, { // edit area tint (no color)
        {"Group", "51", fieldType::groupIdField, {{"355", "0"}}},
    }},
    {1595, { // touch trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1611, { // count trigger
        {"Group", "51", fieldType::groupIdField, {}},
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {1811, { // instant count trigger
        {"Group", "51", fieldType::groupIdField, {}},
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {1817, { // pickup trigger
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {3614, { // time trigger
        {"Group", "51", fieldType::groupIdField, {}},
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {3615, { // time event trigger
        {"Group", "51", fieldType::groupIdField, {}},
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {3617, { // time control trigger
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {3619, { // item edit trigger
        {"Target", "51", fieldType::groupIdField, {{"478", "1"}}},
        {"Item_1", "80", fieldType::itemIdField, {{"476", "1"}}},
        {"Item_2", "95", fieldType::itemIdField, {{"477", "1"}}},
    }},
    {3620, { // item compare trigger
        {"Item_1", "80", fieldType::itemIdField, {{"476", "1"}}},
        {"Item_2", "95", fieldType::itemIdField, {{"477", "1"}}},
        {"On_false", "71", fieldType::groupIdField, {}},
        {"On_true", "51", fieldType::groupIdField, {}},
    }},
    {3641, { // item persist trigger
        {"Item", "80", fieldType::itemIdField, {}},
    }},
    {1912, { // random trigger
        {"Group_1", "51", fieldType::groupIdField, {}},
        {"Group_2", "71", fieldType::groupIdField, {}},
    }},
    {3607, { // sequence trigger
        {"Group(s)", "435", fieldType::groupMapField, {}},
    }},
    {3608, { // spawn particle trigger
        {"Particle", "51", fieldType::groupIdField, {}},
        {"Position", "71", fieldType::groupIdField, {}},
    }},
    {3618, { // reset trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1914, { // static cam trigger
        {"Target", "71", fieldType::groupIdField, {}},
    }},
    {2062, { // edge cam trigger
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {3605, { // edit song trigger
        {"Central_obj", "51", fieldType::groupIdField, {}},
        {"Controlling_obj", "71", fieldType::groupIdField, {}},
    }},
    {3602, { // sfx trigger
        {"Central_obj", "51", fieldType::groupIdField, {}},
        {"Controlling_obj", "71", fieldType::groupIdField, {}},
    }},
    {3603, { // edit sfx trigger
        {"Central_obj", "51", fieldType::groupIdField, {}},
        {"Controlling_obj", "71", fieldType::groupIdField, {}},
        {"SFX_group", "457", fieldType::groupIdField, {}},
    }},
    {3604, { // event trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {3613, { // UI trigger
        {"UI_objects", "51", fieldType::groupIdField, {}},
        {"Anchor", "71", fieldType::groupIdField, {}},
    }},
    {3662, { // link visible trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1815, { // collision trigger
        {"Group", "51", fieldType::groupIdField, {}},
        {"Block_A", "80", fieldType::collisionBlockIdField, {{"138","0"}, {"200","0"}, {"201","0"}}},
        {"Block_B", "95", fieldType::collisionBlockIdField, {{"201","0"}}},
    }},
    {3609, { // instant collision trigger
        {"On_true", "51", fieldType::groupIdField, {}},
        {"On_false", "71", fieldType::groupIdField, {}},
        {"Block_A", "80", fieldType::collisionBlockIdField, {{"138","0"}, {"200","0"}, {"201","0"}}},
        {"Block_B", "95", fieldType::collisionBlockIdField, {{"201","0"}}},
    }},
    {3640, { // state block
        {"State_on", "51", fieldType::groupIdField, {}},
        {"State_off", "71", fieldType::groupIdField, {}},
    }},
    {3643, { // toggle block
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {1812, { // death trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {3600, { // end trigger
        {"Spawn_id", "51", fieldType::groupIdField, {}},
        {"End_pos", "71", fieldType::groupIdField, {}},
    }},
    {2903, { // gradient trigger
        {"Point_1", "203", fieldType::groupIdField, {}},
        {"Point_2", "204", fieldType::groupIdField, {}},
        {"Point_3", "205", fieldType::groupIdField, {}},
        {"Point_4", "206", fieldType::groupIdField, {}},
    }},
    {3022, { // teleport trigger
        {"Group", "51", fieldType::groupIdField, {}},
    }},
    {2905, { // shock wave shader
        {"Group", "51", fieldType::groupIdField, {{"188", "1"}, {"138","0"}, {"200","0"}}},
    }},
    {2907, { // shock line shader
        {"Group", "51", fieldType::groupIdField, {{"188", "1"}, {"138","0"}, {"200","0"}}},
    }},
    {2913, { // lens circles shader
        {"Group", "51", fieldType::groupIdField, {{"138","0"}, {"200","0"}}},
    }},
    {2915, { // motion blur shader
        {"Group", "51", fieldType::groupIdField, {{"138","0"}, {"200","0"}, {"201","0"}}},
    }},
    {2916, { // bulge shader
        {"Group", "51", fieldType::groupIdField, {{"188", "1"}, {"138","0"}, {"200","0"}}},
    }},
    {2917, { // pinch shader
        {"Group", "51", fieldType::groupIdField, {{"188", "1"}, {"138","0"}, {"200","0"}}},
    }},
    
    

};
