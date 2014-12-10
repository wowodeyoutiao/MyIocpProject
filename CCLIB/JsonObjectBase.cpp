/**************************************************************************************
@author: 
@content: 将c++对象进行序列化
**************************************************************************************/
#include "JsonObjectBase.h"

/************************Start Of CCJsonObjectBase******************************************/

void CCJsonObjectBase::SetProperty(string name, CEnumJsonTypeMap type, void* addr)
{
	PJsonObjectInfo pInfo = new TJsonObjectInfo;
	pInfo->PropName.assign(name);
	pInfo->PropType = type;
	pInfo->PropAddr = addr;
	m_PropList.push_back(pInfo);
}

string CCJsonObjectBase::AsString()
{
	Json::Value jsonObj(AsJson());
	Json::FastWriter jsonWrite;
	string retString = jsonWrite.write(jsonObj);
	return retString;
}

PJsonObjectInfo CCJsonObjectBase::GetInfoByName(const string &sName)
{
	PJsonObjectInfo pInfo = nullptr;
	vector<PJsonObjectInfo>::iterator iter;
	for (iter = m_PropList.begin(); iter != m_PropList.end(); ++iter)
	{
		pInfo = *iter;
		if ((pInfo != nullptr) && (sName.compare(pInfo->PropName) == 0))
			break;		
	}
	return pInfo;
}

Json::Value CCJsonObjectBase::AsJson()
{
	Json::Value jsonObj;
	PJsonObjectInfo pInfo;
	vector<PJsonObjectInfo>::iterator iter;
	for (iter = m_PropList.begin(); iter != m_PropList.end(); ++iter)
	{
		pInfo = *iter;
		if (nullptr == pInfo)
			continue;

		switch (pInfo->PropType)
		{
		case asInt:
			jsonObj[pInfo->PropName] = (*(CC_INT*)pInfo->PropAddr);
			break;
		case asUInt:
			jsonObj[pInfo->PropName] = (*(CC_UINT*)pInfo->PropAddr);
			break;
		case asInt64:
			jsonObj[pInfo->PropName] = (*(CC_INT64*)pInfo->PropAddr);
			break;
		case asUInt64:
			jsonObj[pInfo->PropName] = (*(CC_UINT64*)pInfo->PropAddr);
			break;
		case asString:
			jsonObj[pInfo->PropName] = (*(string*)pInfo->PropAddr);
			break;
		case asDouble:
			jsonObj[pInfo->PropName] = (*(CC_DOUBLE*)pInfo->PropAddr);
			break;
		case asFloat:
			jsonObj[pInfo->PropName] = (*(CC_FLOAT*)pInfo->PropAddr);
			break;
		case asBool:
			jsonObj[pInfo->PropName] = (*(CC_BOOL*)pInfo->PropAddr);
			break;
		case asArray:
		case asObject:	
			AddComplexPropToJson(pInfo, jsonObj);
			break;
		default:
			break;
		}		
	}
	return jsonObj;
}

bool CCJsonObjectBase::LoadFrom(const string &str)
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(str, root))
	{
		PJsonObjectInfo pInfo;
		Json::Value::Members members = root.getMemberNames();
		Json::Value::Members::iterator iter;
		for (iter = members.begin(); iter != members.end(); ++iter)
		{
			pInfo = GetInfoByName(*iter);
			if (pInfo != nullptr)
			{
				switch (pInfo->PropType)
				{
				case asInt:
					(*(CC_INT*)pInfo->PropAddr) = root.get(*iter, 0).asInt();
					break;
				case asUInt:
					(*(CC_UINT*)pInfo->PropAddr) = root.get(*iter, 0).asUInt();
					break;
				case asInt64:
					(*(CC_INT64*)pInfo->PropAddr) = root.get(*iter, 0).asInt64();
					break;
				case asUInt64:
					(*(CC_UINT64*)pInfo->PropAddr) = root.get(*iter, 0).asUInt64();
					break;
				case asString:
					(*(string*)pInfo->PropAddr) = root.get(*iter, "").asString();
					break;
				case asDouble:
					(*(CC_DOUBLE*)pInfo->PropAddr) = root.get(*iter, 0.0).asDouble();
					break;
				case asFloat:
					(*(CC_FLOAT*)pInfo->PropAddr) = root.get(*iter, 0.0f).asFloat();
					break;
				case asBool:
					(*(CC_BOOL*)pInfo->PropAddr) = root.get(*iter, false).asBool();
					break;
				case asArray:
				case asObject:
					LoadComplexPropFromJson(pInfo, root);
					break;
				default:
					//我暂时只支持这几种类型，需要的可以自行添加 
					break;
				}
			}
		}
		return true;
	}
	return false;
}

/************************End Of CCJsonObjectBase******************************************/



/************************Start Of CCJsonObjectTest******************************************/

CCJsonObjectTest::CCJsonObjectTest(void)
{
	memset(&saveData, 0, sizeof(saveData));
	SetPropertys();
}

void CCJsonObjectTest::SetPropertys()
{
	SetProperty("iNum1", asInt, &(saveData.iNum1));
	SetProperty("iNum2", asInt, &(saveData.iNum2));
	SetProperty("bFlag1", asBool, &(saveData.bFlag1));
	SetProperty("bFlag2", asBool, &(saveData.bFlag2));
	SetProperty("sName1", asString, &(saveData.sName1));
	SetProperty("sName2", asString, &(saveData.sName2));
}

/************************End Of CCJsonObjectTest******************************************/
