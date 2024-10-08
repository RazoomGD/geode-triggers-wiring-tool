#pragma once

#include "EditLogic.hpp"

using namespace geode::prelude;

std::set<srcObjType> getTypesById(short objId);

std::map<std::string, std::string> objectToKeyVal(std::string objSaveString);

CCArray * filterObjectsByType(srcObjType filteringType, CCArray * objects, bool color, EditorUI * editorInstance);
std::vector<short> getObjectsCommonGroups(CCArray * objects);

bool isNewGroupPossible(CCArray * objects);
bool isGroupPossible(CCArray * objects, int group);
void addToGroup(int group, CCArray * objects);
void addToGroupSM(int group, CCArray * objects);
void addToGroupAnim(int group, CCArray * objects);
void setItemId(int id, CCArray * objects);
void setCollisionId(int id, CCArray * objects);

void myCopyObjectProps(GameObject * from, TWTObjCopy * to);
void myPasteObjectProps(TWTObjCopy * from, GameObject * to);

std::vector<int> getObjectsAllColors(CCArray * objects);
std::vector<int> getItemsAllIds(CCArray * objects);
std::vector<int> getCollisionsAllIds(CCArray * objects);
std::vector<int> getGradientsAllIds(CCArray * objects);
std::optional<int> getCommonBaseColor(CCArray * objects);
std::optional<int> getCommonDetailColor(CCArray * objects);

int getNextFreeItemFixed();
int getNextFreeBlock();

std::string applyDifference(std::string before, std::string after, std::string obj);
