#pragma once
#include <atlbase.h>
#define REG_INET_SETTING _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings")
//#include "base\string_number_conversions.h"

class CProxy
{
public:
  typedef enum _PROXY_TYPE
  {
    PROXY_NONE      = 0,
    PROXY_TYPE_IE,
    PROXY_TYPE_HTTP,
    PROXY_TYPE_SOCK5,        
    PROXY_TYPE_SOCK4, 
  } PROXY_TYPE;

  CProxy(){};
  ~CProxy(){};
  
  int GetIEProxy( int& nProxyType, int& nPort, std::wstring& strAddress)
  {
    int nResult = 0;
    LONG lRet = 0;

    DWORD dwLen = MAX_PATH;
    CRegKey reg;
    TCHAR szAddr[MAX_PATH] = {0};
    TCHAR szProxy[MAX_PATH] = {0};
    TCHAR* pSearch = NULL;

    if (!_IsIEProxyEnable())
      goto Exit0;
    
    lRet = reg.Open(HKEY_CURRENT_USER, REG_INET_SETTING, KEY_QUERY_VALUE);
    if (ERROR_SUCCESS != lRet)
      goto Exit0;

    lRet = reg.QueryStringValue(_T("ProxyServer"), szAddr, &dwLen);
    if (ERROR_SUCCESS != lRet)
      goto Exit0;

    if (0 == *szAddr)
      goto Exit0;

    _tcslwr(szAddr);

    nProxyType = PROXY_TYPE_HTTP;
    if (pSearch = _tcsstr(szAddr, _T("socks=")))
    {
      nProxyType = PROXY_TYPE_SOCK5;
      pSearch += _tcslen(_T("socks="));
      _tcscpy(szProxy, pSearch);
    }
    else if (pSearch = _tcsstr(szAddr, _T("http=")))
    {
      nProxyType = PROXY_TYPE_HTTP;
      pSearch += _tcslen(_T("http="));
      _tcscpy(szProxy, pSearch);
    }
    else 
    {// 使用的全局代理
      nProxyType = PROXY_TYPE_HTTP;
      _tcscpy(szProxy, szAddr);
    }

    // pSearch现在是IP(加端口)的格式
    _GetProxyAddrAndPort(szProxy, strAddress, nPort);        
    if (strAddress.empty())
      goto Exit0;

    nResult = 1;

Exit0:
    reg.Close();
    return nResult;
  }

private:
  DWORD _IsIEProxyEnable()
  {
    LONG lRetCode   = 0;
    DWORD dwValue   = 0;
    CRegKey reg;

    lRetCode = reg.Open(HKEY_CURRENT_USER, REG_INET_SETTING, KEY_QUERY_VALUE);
    if (ERROR_SUCCESS == lRetCode)
    {
      lRetCode = reg.QueryDWORDValue(
        _T("ProxyEnable"),
        dwValue
        );
    }

    reg.Close();
    return dwValue;
  }

  int _GetProxyAddrAndPort(
    const std::wstring& strProxyServer,
    std::wstring& strAddress,
    int& nProxyPort
    )
  {
    int nPos    = 0;
    int nResult = 0;

    if (strProxyServer.empty())
      goto Exit0;

    nPos = strProxyServer.find(_T(":"));
    if (nPos == -1)
      strAddress = strProxyServer;
    else
    {
      strAddress = strProxyServer.substr(0,nPos);
      //base::StringToInt(strProxyServer.substr(nPos + 1),&nProxyPort);
      nProxyPort = _wtoi(strProxyServer.substr(nPos + 1).c_str());
    }

    if (0 == nProxyPort)
      nProxyPort = 80;

    nResult = 1;
Exit0:
    return nResult;
  }
};