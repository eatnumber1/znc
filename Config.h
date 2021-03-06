/*
 * Copyright (C) 2004-2011  See the AUTHORS file for details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "zncconfig.h"
#include "ZNCString.h"

class CFile;

class CConfig {
public:
	struct CConfigEntry {
		CConfigEntry();
		CConfigEntry(const CConfig& Config);
		CConfigEntry(const CConfigEntry& other);
		~CConfigEntry();
		CConfigEntry& operator=(const CConfigEntry& other);

		CConfig* m_pSubConfig;
	};

	typedef map<CString, VCString> EntryMap;
	typedef map<CString, CConfigEntry> SubConfig;
	typedef map<CString, SubConfig> SubConfigMap;

	typedef EntryMap::const_iterator EntryMapIterator;
	typedef SubConfigMap::const_iterator SubConfigMapIterator;

	EntryMapIterator BeginEntries() const {
		return m_ConfigEntries.begin();
	}
	EntryMapIterator EndEntries() const {
		return m_ConfigEntries.end();
	}

	SubConfigMapIterator BeginSubConfigs() const {
		return m_SubConfigs.begin();
	}
	SubConfigMapIterator EndSubConfigs() const {
		return m_SubConfigs.end();
	}

	void AddKeyValuePair(const CString& sName, const CString& sValue) {
		if (sName.empty() || sValue.empty()) {
			return;
		}

		m_ConfigEntries[sName].push_back(sValue);
	}

	bool AddSubConfig(const CString& sTag, const CString& sName, CConfig Config) {
		SubConfig &conf = m_SubConfigs[sTag];
		SubConfig::const_iterator it = conf.find(sName);

		if (it != conf.end()) {
			return false;
		}

		conf[sName] = Config;
		return true;
	}

	bool FindStringVector(const CString& sName, VCString& vsList) {
		EntryMap::iterator it = m_ConfigEntries.find(sName);
		vsList.clear();
		if (it == m_ConfigEntries.end())
			return false;
		vsList = it->second;
		m_ConfigEntries.erase(it);
		return true;
	}

	bool FindStringEntry(const CString& sName, CString& sRes, const CString& sDefault = "") {
		EntryMap::iterator it = m_ConfigEntries.find(sName);
		sRes = sDefault;
		if (it == m_ConfigEntries.end() || it->second.empty())
			return false;
		sRes = it->second.front();
		it->second.erase(it->second.begin());
		if (it->second.empty())
			m_ConfigEntries.erase(it);
		return true;
	}

	bool FindBoolEntry(const CString& sName, bool& bRes, bool bDefault = false) {
		CString s;
		if (FindStringEntry(sName, s)) {
			bRes = s.ToBool();
			return true;
		}
		bRes = bDefault;
		return false;
	}

	bool FindUIntEntry(const CString& sName, unsigned int& uRes, unsigned int uDefault = 0) {
		CString s;
		if (FindStringEntry(sName, s)) {
			uRes = s.ToUInt();
			return true;
		}
		uRes = uDefault;
		return false;
	}

	bool FindSubConfig(const CString& sName, SubConfig& Config) {
		SubConfigMap::iterator it = m_SubConfigs.find(sName);
		if (it == m_SubConfigs.end()) {
			Config.clear();
			return false;
		}
		Config = it->second;
		m_SubConfigs.erase(it);
		return true;
	}

	bool empty() const {
		return m_ConfigEntries.empty() && m_SubConfigs.empty();
	}

	bool Parse(CFile& file, CString& sErrorMsg);
	void Write(CFile *pFile, unsigned int iIndentation = 0);

private:
	EntryMap m_ConfigEntries;
	SubConfigMap m_SubConfigs;
};

#endif // !CONFIG_H
