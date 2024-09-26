#include "../ToolConfig.hpp"

using namespace geode::prelude;

enum fieldType {
    groupIdField, 
    groupMapField,
    // colorId, - for future support
    itemIdField,
    keyframeIdField,
    collisionBlockIdField,
    areaEffectIdField,
    gradientIdField,

};

enum searchType {
    groupSearch,
    itemSearch,
    collisionBlockSearch,
    areaEffectSearch,
    gradientSearch,
};

const struct ConditionPreview {
    std::string m_conditionKey;
    std::string m_conditionValue; // "" - any, "0" - no such key
};

const struct ObjField {
    std::string m_name;
    std::string m_key;
    fieldType m_type;
    std::vector<ConditionPreview> m_conditions; // all other required keys
};

const struct SearchResult {
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
        {"Follow", "71", fieldType::groupIdField, {}},
    }},
    {3660, { // edit advanced follow trigger
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {3661, { // re-target advanced follow trigger
        {"Target", "51", fieldType::groupIdField, {}},
        {"Follow", "71", fieldType::groupIdField, {}},
    }},
    {3032, { // keyframe object
        {"Spawn", "71", fieldType::groupIdField, {}},
        {"Target", "51", fieldType::groupIdField, {}},
    }},
    {3006, { // area move
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
    }},
    {3007, { // area rotate
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
    }},
    {3008, { // area scale
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
    }},
    {3009, { // area fade
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
    }},
    {3010, { // area tint (no color)
        {"Target", "51", fieldType::groupIdField, {}},
        {"Center", "71", fieldType::groupIdField, {}},
    }},
    

};