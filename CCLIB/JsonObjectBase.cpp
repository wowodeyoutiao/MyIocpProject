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
	Json::FastWriter jsonWrite;
	string retString = jsonWrite.write(AsJson());
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
		LoadFrom(root);
		return true;
	}
	return false;
}

void CCJsonObjectBase::LoadFrom(Json::Value &jsonObj)
{
	PJsonObjectInfo pInfo;
	Json::Value::Members members = jsonObj.getMemberNames();
	Json::Value::Members::iterator iter;
	for (iter = members.begin(); iter != members.end(); ++iter)
	{
		pInfo = GetInfoByName(*iter);
		if (pInfo != nullptr)
		{
			switch (pInfo->PropType)
			{
			case asInt:
				(*(CC_INT*)pInfo->PropAddr) = jsonObj.get(*iter, 0).asInt();
				break;
			case asUInt:
				(*(CC_UINT*)pInfo->PropAddr) = jsonObj.get(*iter, 0).asUInt();
				break;
			case asInt64:
				(*(CC_INT64*)pInfo->PropAddr) = jsonObj.get(*iter, 0).asInt64();
				break;
			case asUInt64:
				(*(CC_UINT64*)pInfo->PropAddr) = jsonObj.get(*iter, 0).asUInt64();
				break;
			case asString:
				(*(string*)pInfo->PropAddr) = jsonObj.get(*iter, "").asString();
				break;
			case asDouble:
				(*(CC_DOUBLE*)pInfo->PropAddr) = jsonObj.get(*iter, 0.0).asDouble();
				break;
			case asFloat:
				(*(CC_FLOAT*)pInfo->PropAddr) = jsonObj.get(*iter, 0.0f).asFloat();
				break;
			case asBool:
				(*(CC_BOOL*)pInfo->PropAddr) = jsonObj.get(*iter, false).asBool();
				break;
			case asArray:
			case asObject:
				LoadComplexPropFromJson(pInfo, jsonObj);
				break;
			default:
				//我暂时只支持这几种类型，需要的可以自行添加 
				break;
			}
		}
	}
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


/************************Start Of CCJsonObjectTestEx******************************************/

CCJsonObjectTestEx::CCJsonObjectTestEx(void)
{
	memset(&saveDataEx, 0, sizeof(saveDataEx));
	SetPropertys();
}

void CCJsonObjectTestEx::SetPropertys()
{
	SetProperty("iNum1", asInt, &(saveDataEx.iNum1));
	SetProperty("iNum2", asInt, &(saveDataEx.iNum2));
	SetProperty("IntArrayData", asArray, &(saveDataEx.IntArrayData[0]));
	SetProperty("dataEx", asObject, &(saveDataEx.dataEx));	
	SetProperty("dataExArray", asArray, &(saveDataEx.dataExArray));
}

void CCJsonObjectTestEx::AddComplexPropToJson(PJsonObjectInfo pInfo, Json::Value &jsonObj)
{
	if (pInfo != nullptr)
	{
		if (pInfo->PropName.compare("dataEx") == 0)
		{
			CCJsonObjectTest testdata;
			testdata.saveData = *((PSaveTestData)pInfo->PropAddr);
			jsonObj[pInfo->PropName] = testdata.AsJson();
		}
		else if (pInfo->PropName.compare("IntArrayData") == 0)
		{
			Json::Value intArray;
			int iArrayLen = sizeof(saveDataEx.IntArrayData) / sizeof(saveDataEx.IntArrayData[0]);
			for (int i = 0; i < iArrayLen; i++)
				intArray.append((*((CC_INT*)pInfo->PropAddr + i)));
			jsonObj[pInfo->PropName] = intArray;
		}
		else if (pInfo->PropName.compare("dataExArray") == 0)
		{
			Json::Value dataArray;
			int iArrayLen = sizeof(saveDataEx.dataExArray) / sizeof(saveDataEx.dataExArray[0]);
			CCJsonObjectTest itemdata;
			for (int i = 0; i < iArrayLen; i++)
			{
				itemdata.saveData = *((PSaveTestData)pInfo->PropAddr + i);
				dataArray.append(itemdata.AsJson());
			}
			jsonObj[pInfo->PropName] = dataArray;
		}
	}
}

void CCJsonObjectTestEx::LoadComplexPropFromJson(PJsonObjectInfo pInfo, Json::Value &jsonObj)
{
	if (pInfo != nullptr)
	{
		if (pInfo->PropName.compare("dataEx") == 0)
		{		
			CCJsonObjectTest testdata;
			testdata.LoadFrom(jsonObj.get(pInfo->PropName, 0));
			(*(PSaveTestData)pInfo->PropAddr) = testdata.saveData;
		}
		else if (pInfo->PropName.compare("IntArrayData") == 0)
		{
			Json::Value intArray;
			intArray = jsonObj.get(pInfo->PropName, 0);
			for (int i = 0; i < intArray.size(); i++)
				(*((CC_INT*)pInfo->PropAddr + i)) = intArray.get(i, 0).asInt();
		}
		else if (pInfo->PropName.compare("dataExArray") == 0)
		{
			Json::Value dataArray;
			CCJsonObjectTest testdata;
			dataArray = jsonObj.get(pInfo->PropName, 0);
			for (int i = 0; i < dataArray.size(); i++)
			{
				testdata.LoadFrom(dataArray.get(i, 0));
				(*((PSaveTestData)pInfo->PropAddr + i)) = testdata.saveData;
			}
		}
	}
}

/************************End Of CCJsonObjectTestEx******************************************/

