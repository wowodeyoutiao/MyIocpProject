/**************************************************************************************
@author: 
@content: 将c++对象进行序列化
**************************************************************************************/
#ifndef __CC_JSON_OBJECT_BASE_H__
#define __CC_JSON_OBJECT_BASE_H__
#include <string>
#include <vector>
#include "json/json.h"

using std::string;
using std::vector;

typedef int CC_INT;
typedef unsigned int CC_UINT;
typedef __int64 CC_INT64;
typedef unsigned __int64 CC_UINT64;
typedef double CC_DOUBLE;
typedef float CC_FLOAT;
typedef bool CC_BOOL;

const enum CEnumJsonTypeMap
{
	asInt = 1,
	asUInt,
	asInt64,
	asUInt64,
	asString,
	asDouble,
	asFloat,
	asBool,
	asArray,
	asObject
};

typedef struct _TJsonObjectProp
{
	string PropName;
	CEnumJsonTypeMap PropType;
	void* PropAddr;
}TJsonObjectInfo, *PJsonObjectInfo;

class CCJsonObjectBase
{
protected:
	vector<PJsonObjectInfo> m_PropList;
protected:
	void SetProperty(string name, CEnumJsonTypeMap type, void* addr);
	virtual void SetPropertys() = 0;
	PJsonObjectInfo GetInfoByName(const string &sName);
	virtual void AddComplexPropToJson(PJsonObjectInfo pInfo, Json::Value &jsonObj){};
	virtual void LoadComplexPropFromJson(PJsonObjectInfo pInfo, Json::Value &jsonObj){};
public:
	CCJsonObjectBase(void){}
	virtual ~CCJsonObjectBase(void){}
	string AsString();
	Json::Value AsJson();
	bool LoadFrom(const string &str);
	void LoadFrom(Json::Value &jsonObj);
};

typedef struct _TSaveTestData
{
	int iNum1;
	int iNum2;
	bool bFlag1;
	bool bFlag2;
	string sName1;
	string sName2;
}TSaveTestData, *PSaveTestData;

class CCJsonObjectTest : public CCJsonObjectBase
{
protected:
	virtual void SetPropertys();
public:
	CCJsonObjectTest(void);
public:
	TSaveTestData saveData;
};

typedef struct _TSaveTestDataEx
{
	int iNum1;
	int iNum2;
	int IntArrayData[100];
	TSaveTestData dataEx;
	TSaveTestData dataExArray[10];
}TSaveTestDataEx, *PSaveTestDataEx;

class CCJsonObjectTestEx : public CCJsonObjectBase
{
protected:
	virtual void SetPropertys();
	virtual void AddComplexPropToJson(PJsonObjectInfo pInfo, Json::Value &jsonObj);
	virtual void LoadComplexPropFromJson(PJsonObjectInfo pInfo, Json::Value &jsonObj);
public:
	CCJsonObjectTestEx(void);
public:
	TSaveTestDataEx saveDataEx;
};

#endif  //__CC_JSON_OBJECT_BASE_H__