#include"ÒÀÀµº¯Êý.h"


std::string charToString(char* charArray)
{
    std::string str(charArray);
    return str;
}

bool wstrToint(std::wstring str, int* intstr)
{
    wchar_t* end = nullptr;
    *intstr = std::wcstol(str.c_str(), &end, 10);
    if (end == str.c_str() + str.size())
    {
        return true;
    }
    return false;
}

std::wstring charTowstring(char* charstr)
{
    std::string str = charstr;
    std::wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    if (len < 0)return result;
    wchar_t* buffer = new wchar_t[len + 1];
    if (buffer == NULL)return result;
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

std::wstring stringTowstring(std::string str)
{
    std::wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    if (len < 0)return result;
    wchar_t* buffer = new wchar_t[len + 1];
    if (buffer == NULL)return result;
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

std::string wstringTostring(std::wstring wstr)
{
    std::string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    if (len <= 0)return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL)return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

std::string GetCurrentTimeStr()
{
    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    char time_str[30];
    sprintf_s(time_str, "[%04d/%02d/%02d_%02d:%02d:%02d]", sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
    std::string str_time(time_str);
    return (str_time);
}

std::string GetClientInfo(SOCKET sock)
{
    sockaddr_in client_addr{};
    int addr_len = sizeof(client_addr);
    getpeername(sock, (sockaddr*)&client_addr, &addr_len);
    char* ip_str = inet_ntoa(client_addr.sin_addr);
    int port = ntohs(client_addr.sin_port);
    std::string client_info = "[" + std::string(ip_str) + "_" + std::to_string(port) + "]";
    return client_info;
}

std::string getWirelessAdapterIPV4Address()
{
    std::string ipAddress = "";
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    DWORD dwRetVal = 0;

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
    {
        return ipAddress;
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            return ipAddress;
        }
    }
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (pAdapter->Type == MIB_IF_TYPE_IEEE80211)
            {
                IP_ADDR_STRING* pIpAddrString = &(pAdapter->IpAddressList);
                while (pIpAddrString) {
                    if (pIpAddrString->IpAddress.String[0] != '0')
                    {
                        ipAddress = pIpAddrString->IpAddress.String;
                        break;
                    }
                    pIpAddrString = pIpAddrString->Next;
                }
                break;
            }
            pAdapter = pAdapter->Next;
        }
    }

    if (pAdapterInfo) {
        free(pAdapterInfo);
    }

    return ipAddress;
}

std::string getLocalSocketAddrName(SOCKET socketFd)
{
    sockaddr_in SocketAddr;
    socklen_t nLen = sizeof(SocketAddr);
    memset(&SocketAddr, 0, sizeof(SocketAddr));
    int nRet = getsockname(socketFd, reinterpret_cast<sockaddr*>(&SocketAddr), &nLen);
    if (nRet == 0)
    {
        uint32_t ip = ntohl(SocketAddr.sin_addr.s_addr);
        uint16_t port = ntohs(SocketAddr.sin_port);
        char strIp[20];
        snprintf(strIp, sizeof(strIp), "%d.%d.%d.%d",
            static_cast<unsigned char>((ip >> 24) & 0xFF),
            static_cast<unsigned char>((ip >> 16) & 0xFF),
            static_cast<unsigned char>((ip >> 8) & 0xFF),
            static_cast<unsigned char>(ip & 0xFF));
        std::string result(strIp);
        result += "_";
        result += std::to_string(port);
        result = "[" + result + "]";
        return result;
    }
    else
    {
        return "";
    }
}

bool Is_Valid_IP(const std::wstring& ip)
{
    std::wregex pattern(L"^(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})$");
    std::wsmatch matches;
    if (regex_match(ip, matches, pattern))
    {
        for (int i = 1; i <= 4; i++)
        {
            int num = stoi(matches[i]);
            if (num < 0 || num > 255)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}