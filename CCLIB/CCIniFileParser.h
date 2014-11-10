/**************************************************************************************
@author: Steven.zdwang@gmail.com
@content: INI 文件解析类 StdC++实现
**************************************************************************************/
#pragma  once

#ifndef __CC_INIFILE_PARSER_H__
#define __CC_INIFILE_PARSER_H__
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <codecvt>
template < class STR_T, class READ_FILE_T, class WRITE_FILE_T >
class CWgtCustomIniFile
{
public:
	CWgtCustomIniFile()
	{
	}
	virtual ~CWgtCustomIniFile()
	{
	}
	//-----------------------------------------------------------
	//    Load and save ini file
	//-----------------------------------------------------------
	bool loadFromFile(const STR_T& aPathName)
	{
#ifdef UNICODE
		READ_FILE_T fin(aPathName.c_str(), std::ios::binary);
		fin.imbue(std::locale(fin.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
#else
		READ_FILE_T fin(aPathName.c_str());
#endif
		if (!fin.is_open())return false;
		m_pathName = aPathName;
		STR_T inbuf;
		while (!fin.eof())
		{
			std::getline(fin, inbuf);
			_trim_string(inbuf, TEXT(" "), true);
			_parser_line(inbuf);
		}
		fin.close();
		return true;
	}
	void saveToFile(const STR_T& aPathName)
	{
		WRITE_FILE_T fout(aPathName.c_str());
		if (!fout.is_open()) return;
		STR_T outbuf;
		typename _mem_ini_type::iterator ps;
		typename _key_value_map_type::iterator pk;
		for (ps = m_memIni.begin(); ps != m_memIni.end(); ++ps){
			// write section name
			fout << std::endl;
			fout << TEXT("[") << ps->first << TEXT("]") << std::endl;
			// write section values
			_key_value_map_type & key_values = ps->second;
			for (pk = key_values.begin(); pk != key_values.end(); ++pk){
				fout << pk->first << TEXT(" = ") << pk->second << std::endl;
			}
		}
		fout.close();
	}
	void clear()
	{
		m_memIni.clear();
	}
	STR_T getFileName() const { return m_pathName; }
	//------------------------------------------------------------
	//    String Access
	//------------------------------------------------------------
	STR_T getString(const STR_T& aSectionName, const STR_T& aKeyName, const STR_T& aDefaultValue)
	{
		if (!aSectionName.empty() && !aKeyName.empty()){
			typename _mem_ini_type::iterator ps = m_memIni.find(aSectionName);
			if (ps != m_memIni.end()){
				typename _key_value_map_type::iterator pk = ps->second.find(aKeyName);
				if (pk != ps->second.end()){
					return pk->second;
				}
			}
		}
		return aDefaultValue;
	}
	void setString(const STR_T& aSectionName, const STR_T& aKeyName, const STR_T& aValue)
	{
		if (!aSectionName.empty() && !aKeyName.empty()){
			m_memIni[aSectionName][aKeyName] = aValue;
		}
	}
	//------------------------------------------------------------
	//    Primitive Data Type Access
	//------------------------------------------------------------
	int getInteger(const STR_T& aSectionName, const STR_T& aKeyName, int aDefaultValue)
	{
		STR_T temp = getString(aSectionName, aKeyName, TEXT(""));
		std::basic_stringstream< typename STR_T::value_type > sstream;
		sstream << temp;
		sstream >> aDefaultValue;
		return aDefaultValue;
	}
	void setInteger(const STR_T& aSectionName, const STR_T& aKeyName, int aValue)
	{
		std::basic_stringstream< typename STR_T::value_type > sstream;
		sstream << aValue;
		setString(aSectionName, aKeyName, sstream.str());
	}
	bool getBoolean(const STR_T& aSectionName, const STR_T& aKeyName, bool aDefaultValue)
	{
		STR_T temp = getString(aSectionName, aKeyName, TEXT(""));
		if (!temp.empty()){
			return _string_to_boolean(temp, aDefaultValue);
		}
		return aDefaultValue;
	}
	void setBoolean(const STR_T& aSectionName, const STR_T& aKeyName, bool aValue)
	{
		setInteger(aSectionName, aKeyName, aValue ? 1 : 0);
	}
	double getDouble(const STR_T& aSectionName, const STR_T& aKeyName, double aDefaultValue)
	{
		STR_T temp = getString(aSectionName, aKeyName, TEXT(""));
		std::basic_stringstream< typename STR_T::value_type > sstream;
		if (!temp.empty()){
			sstream << temp;
			sstream >> aDefaultValue;
		}
		return aDefaultValue;
	}
	void setDouble(const STR_T& aSectionName, const STR_T& aKeyName, double aValue, int aPrecision)
	{
		std::basic_stringstream< typename STR_T::value_type > sstream;
		if (aPrecision)
			sstream.precision(aPrecision);
		sstream << aValue;
		setString(aSectionName, aKeyName, sstream.str());
	}
	//------------------------------------------------------------
	//    Section Operations
	//------------------------------------------------------------
	bool isSectionExist(const STR_T& aSectionName) const
	{
		return (m_memIni.find(aSectionName) != m_memIni.end());
	}
	void getSectionNames(std::vector<STR_T> & aSectionNames)
	{
		if (!aSectionNames.empty())
			aSectionNames.clear();
		typename _mem_ini_type::iterator ps = m_memIni.begin();
		for (; ps != m_memIni.end(); ++ps){
			aSectionNames.push_back(ps->first);
		}
	}
	bool deleteSection(const STR_T& aSectionName)
	{
		if (aSectionName.empty()) return false;
		typename _mem_ini_type::iterator ps = m_memIni.find(aSectionName);
		if (ps != m_memIni.end()){
			m_memIni.erase(ps);
			return true;
		}
		return false;
	}
	//------------------------------------------------------------
	//    Key Operations
	//------------------------------------------------------------
	bool isKeyExist(const STR_T& aSectionName, const STR_T& aKeyName) const
	{
		if (aSectionName.length() == 0 || aKeyName.empty()) return false;
		typename _mem_ini_type::const_iterator ps = m_memIni.find(aSectionName);
		if (ps != m_memIni.end()){
			return (ps->second.find(aKeyName) != ps->second.end());
		}
		return false;
	}
	void getKeyNames(const STR_T& aSectionName, std::vector<STR_T> & aKeyNames)
	{
		if (aSectionName.empty()) return;
		if (!aKeyNames.empty())
			aKeyNames.clear();
		typename _mem_ini_type::iterator ps = m_memIni.find(aSectionName);
		typename _key_value_map_type::iterator pk;
		if (ps != m_memIni.end()){
			for (pk = ps->second.begin(); pk != ps->second.end(); ++pk){
				aKeyNames.push_back(pk->first);
			}
		}
	}
	bool deleteKey(const STR_T& aSectionName, const STR_T& aKeyName)
	{
		if (aSectionName.empty() || aKeyName.empty()) return false;
		typename _mem_ini_type::iterator ps = m_memIni.find(aSectionName);
		typename _key_value_map_type::iterator pk;
		if (ps != m_memIni.end()){
			pk = ps->second.find(aKeyName);
			if (pk != ps->second.end()){
				ps->second.erase(pk);
				return true;
			}
		}
		return false;
	}
protected:
	//------------------------------------------------------------
	//    Helper Functions
	//------------------------------------------------------------
	void _trim_string(STR_T& val, const STR_T& chars, bool strip_all)
	{
		if (val.empty()) return;
		// delete from head
		while (val.find(chars) == 0){
			val.erase(0, chars.length());
			if (!strip_all) break;
		}
		// delete from tail
		while (!val.empty() && (val.rfind(chars) == (val.length() - chars.length()))){
			val.erase(val.length() - chars.length(), chars.length());
			if (!strip_all) break;
		}
	}
	STR_T _middle_string(const STR_T& value, const STR_T& first, const STR_T& last)
	{
		STR_T result;
		if (!value.empty()) {
			typename STR_T::size_type pos_begin = 0, pos_end = 0;
			if (!first.empty()){
				pos_begin = value.find(first);
				if (pos_begin == STR_T::npos) return result;
				pos_begin += first.size();
			}
			else{
				pos_begin = 0;
			}
			if (!last.empty()){
				pos_end = value.find(last, pos_begin);
				if (pos_end == STR_T::npos) return result;
				result = value.substr(pos_begin, pos_end - pos_begin);
			}
			else{
				result = value.substr(pos_begin, STR_T::npos);
			}
		}
		return result;
	}
	void _parser_line(const STR_T& val)
	{
		if (val.empty()) return;
		switch (val[0]){
		case TEXT('['):
		{
						  STR_T aSectionName = _middle_string(val, TEXT("["), TEXT("]"));
						  _trim_string(aSectionName, TEXT(" "), true);
						  if (!aSectionName.empty())
							  m_activeSectionName = aSectionName;
		}
			return;
		case TEXT('#'):
			return;
		default:
		{
				   if (val.find(TEXT("=")) != STR_T::npos && !m_activeSectionName.empty()){
					   STR_T aKeyName = _middle_string(val, TEXT(""), TEXT("="));
					   _trim_string(aKeyName, TEXT(" "), true);
					   STR_T aKeyValue = _middle_string(val, TEXT("="), TEXT(""));
					   _trim_string(aKeyValue, TEXT(" "), true);
					   if (!aKeyName.empty())
						   m_memIni[m_activeSectionName][aKeyName] = aKeyValue;
				   }
				   return;
		}
		}
	}
	bool _string_to_boolean(const STR_T& aStringValue, bool aDefaultValue)
	{
		// Default: empty string
		// TRUE: "true", "yes", non-zero decimal numner
		// FALSE: all other cases
		if (aStringValue.empty())
			return aDefaultValue;
		std::basic_stringstream< typename STR_T::value_type > sstream;
		sstream << aStringValue;
		int val = 0;
		sstream >> val;
		return (aStringValue.compare(TEXT("true")) == 0 || val > 0);
	}
	//------------------------------------------------------------
	//    Member Data
	//------------------------------------------------------------
	STR_T m_pathName; // Stores path of the associated ini file
	STR_T m_activeSectionName;
	typedef std::map< STR_T, STR_T > _key_value_map_type;
	typedef std::map< STR_T, _key_value_map_type > _mem_ini_type;
	_mem_ini_type m_memIni;
};
typedef CWgtCustomIniFile< std::wstring, std::wifstream, std::wofstream >   CWgtUnicodeIniFile;
typedef CWgtCustomIniFile< std::string, std::ifstream, std::ofstream >   CWgtIniFile;
#endif // __CC_INIFILE_PARSER_H__