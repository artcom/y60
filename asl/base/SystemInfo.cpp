// SystemInfo.cpp: implementation of the SystemInfo class.
//////////////////////////////////////////////////////////////////////

#include "SystemInfo.h"

#include <tchar.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define BUFSIZE 80

#ifndef VER_SUITE_WH_SERVER
#define VER_SUITE_WH_SERVER 0x8000
#endif

typedef void (WINAPI *PGetNativeSystemInfo)(LPSYSTEM_INFO);

typedef BOOL (WINAPI *PGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

namespace asl {

    SystemInfo::SystemInfo()
    {
       BOOL canDetect = TRUE;
       PGetNativeSystemInfo pGNSI = NULL;

       m_bOsVersionInfoEx = FALSE;
       m_nWinVersion = Windows;
       m_nWinEdition = EditionUnknown;
       memset(m_szServicePack, 0, sizeof(m_szServicePack));

       // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
       ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
       m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

       if( !(m_bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &m_osvi)) )
       {
          // If that fails, try using the OSVERSIONINFO structure.
          m_osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
          if (! GetVersionEx ( (OSVERSIONINFO *) &m_osvi) ) 
             canDetect = FALSE;
       }

       pGNSI = (PGetNativeSystemInfo) GetProcAddress(
          GetModuleHandle(_T("kernel32.dll")), 
          "GetNativeSystemInfo");

       if(NULL != pGNSI) pGNSI(&m_SysInfo);
       else GetSystemInfo(&m_SysInfo);

       if(canDetect)
       {
	       DetectWindowsVersion();
          DetectWindowsEdition();
          DetectWindowsServicePack();
       }
    }

    SystemInfo::~SystemInfo()
    {

    }

    void SystemInfo::DetectWindowsVersion()
    {
       if(m_bOsVersionInfoEx)
       {
          switch (m_osvi.dwPlatformId)
          {
          case VER_PLATFORM_WIN32s:
             {
                m_nWinVersion = Windows32s;
             }
             break;

             // Test for the Windows 95 product family.
          case VER_PLATFORM_WIN32_WINDOWS:
             {
                switch(m_osvi.dwMajorVersion)
                {
                case 4:
                   {
                      switch(m_osvi.dwMinorVersion)
                      {
                      case 0:
                         if (m_osvi.szCSDVersion[0] == 'B' || m_osvi.szCSDVersion[0] == 'C')
                            m_nWinVersion = Windows95OSR2;
                         else
                            m_nWinVersion = Windows95;
                         break;
                      case 10:
                         if (m_osvi.szCSDVersion[0] == 'A')
                            m_nWinVersion = Windows98SE;
                         else
                            m_nWinVersion = Windows98;
                         break;
                      case 90:
                         m_nWinVersion = WindowsMillennium;
                         break;       
                      }
                   }
                   break;
                }
             }
             break;

             // Test for the Windows NT product family.
          case VER_PLATFORM_WIN32_NT:
             {
                switch (m_osvi.dwMajorVersion)
                {
                case 3:
                   m_nWinVersion = WindowsNT351;
                   break;

                case 4:
                   switch (m_osvi.wProductType)
                   {
                   case 1:
                      m_nWinVersion = WindowsNT40;
                      break;
                   case 3:
                      m_nWinVersion = WindowsNT40Server;
                      break;
                   }
                   break;

                case 5:
                   {
                      switch (m_osvi.dwMinorVersion)
                      {
                      case 0:
                         m_nWinVersion = Windows2000;
                         break;
                      case 1:
                         m_nWinVersion = WindowsXP;
                         break;
                      case 2:
                         {
                            if (m_osvi.wSuiteMask == VER_SUITE_WH_SERVER)
                            {
                               m_nWinVersion = WindowsHomeServer;
                            }
                            else if (m_osvi.wProductType == VER_NT_WORKSTATION &&
                               m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                            {
                               m_nWinVersion = WindowsXPProfessionalx64;
                            }
                            else
                            {
                               m_nWinVersion = ::GetSystemMetrics(SM_SERVERR2) == 0 ?
                               WindowsServer2003 :
                               WindowsServer2003R2;
                            }
                         }
                         break;
                      }

                   }
                   break;

                case 6:
                   {
                      switch (m_osvi.dwMinorVersion)
                      {
                      case 0:
                         {
                            m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ?
                            WindowsVista :
                            WindowsServer2008;
                         }
                         break;

                      case 1:
                         {
                            m_nWinVersion = m_osvi.wProductType == VER_NT_WORKSTATION ?
                            Windows7 :
                            WindowsServer2008R2;
                         }
                         break;
                      }
                   }
                   break;
                }
             }
             break;
          }
       }
       else // Test for specific product on Windows NT 4.0 SP5 and earlier
       {
          HKEY hKey;
          char szProductType[BUFSIZE];
          DWORD dwBufLen=BUFSIZE;
          LONG lRet;

          lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
             "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
             0, KEY_QUERY_VALUE, &hKey );
          if( lRet != ERROR_SUCCESS )
             return;

          lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
             (LPBYTE) szProductType, &dwBufLen);
          if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
             return;

          RegCloseKey( hKey );

          if ( lstrcmpi( "WINNT", szProductType) == 0 )
          {
             if ( m_osvi.dwMajorVersion <= 4 )
             {
                m_nWinVersion = WindowsNT40;
                m_nWinEdition = Workstation;
             }
          }
          if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
          {
             if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
             {
                m_nWinVersion = WindowsServer2003;
             }

             if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
             {
                m_nWinVersion = Windows2000;
                m_nWinEdition = Server;
             }

             if ( m_osvi.dwMajorVersion <= 4 )
             {
                m_nWinVersion = WindowsNT40;
                m_nWinEdition = Server;
             }
          }
          if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
          {
             if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
             {
                m_nWinVersion = WindowsServer2003;
                m_nWinEdition = EnterpriseServer;
             }

             if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
             {
                m_nWinVersion = Windows2000;
                m_nWinEdition = AdvancedServer;
             }

             if ( m_osvi.dwMajorVersion <= 4 )
             {
                m_nWinVersion = WindowsNT40;
                m_nWinEdition = EnterpriseServer;
             }
          }
       }
    }

    void SystemInfo::DetectWindowsEdition()
    {
       if(m_bOsVersionInfoEx)
       {
          switch(m_osvi.dwMajorVersion)
          {
          case 4:
             {
                switch(m_osvi.wProductType)
                {
                case VER_NT_WORKSTATION:
                   m_nWinEdition = Workstation;
                   break;

                case VER_NT_SERVER:
                   m_nWinEdition = (m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0 ? 
                      EnterpriseServer :
                      StandardServer;
                   break;
                }
             }
             break;

          case 5:
             {
                switch (m_osvi.wProductType)
                {
                case VER_NT_WORKSTATION:
                   {
                      m_nWinEdition = (m_osvi.wSuiteMask & VER_SUITE_PERSONAL) != 0 ?
                         Home :
                         Professional;
                   }
                   break;

                case VER_NT_SERVER:
                   {
                      switch(m_osvi.dwMinorVersion)
                      {
                      case 0:
                         {
                            if ((m_osvi.wSuiteMask & VER_SUITE_DATACENTER) != 0)
                            {
                               m_nWinEdition = DatacenterServer;
                            }
                            else if ((m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                            {
                               m_nWinEdition = AdvancedServer;
                            }
                            else
                            {
                               m_nWinEdition = Server;
                            }
                         }
                         break;

                      default:
                         {
                            if ((m_osvi.wSuiteMask & VER_SUITE_DATACENTER) != 0)
                            {
                               m_nWinEdition = DatacenterServer;
                            }
                            else if ((m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                            {
                               m_nWinEdition = EnterpriseServer;
                            }
                            else if ((m_osvi.wSuiteMask & VER_SUITE_BLADE) != 0)
                            {
                               m_nWinEdition = WebServer;
                            }
                            else
                            {
                               m_nWinEdition = StandardServer;
                            }
                         }
                         break;
                      }
                   }
                   break;
                }
             }
             break;

          case 6:
             {
                DWORD dwReturnedProductType = DetectProductInfo();
                switch (dwReturnedProductType)
                {
                case PRODUCT_UNDEFINED:
                   m_nWinEdition = EditionUnknown;
                   break;

                case PRODUCT_ULTIMATE:
                   m_nWinEdition = Ultimate;
                   break;
                case PRODUCT_HOME_BASIC:
                   m_nWinEdition = HomeBasic;
                   break;
                case PRODUCT_HOME_PREMIUM:
                   m_nWinEdition = HomePremium;
                   break;
                case PRODUCT_ENTERPRISE:
                   m_nWinEdition = Enterprise;
                   break;
                case PRODUCT_HOME_BASIC_N:
                   m_nWinEdition = HomeBasic_N;
                   break;
                case PRODUCT_BUSINESS:
                   m_nWinEdition = Business;
                   break;
                case PRODUCT_STANDARD_SERVER:
                   m_nWinEdition = StandardServer;
                   break;
                case PRODUCT_DATACENTER_SERVER:
                   m_nWinEdition = DatacenterServer;
                   break;
                case PRODUCT_SMALLBUSINESS_SERVER:
                   m_nWinEdition = SmallBusinessServer;
                   break;
                case PRODUCT_ENTERPRISE_SERVER:
                   m_nWinEdition = EnterpriseServer;
                   break;
                case PRODUCT_STARTER:
                   m_nWinEdition = Starter;
                   break;
                case PRODUCT_DATACENTER_SERVER_CORE:
                   m_nWinEdition = DatacenterServerCore;
                   break;
                case PRODUCT_STANDARD_SERVER_CORE:
                   m_nWinEdition = StandardServerCore;
                   break;
                case PRODUCT_ENTERPRISE_SERVER_CORE:
                   m_nWinEdition = EnterpriseServerCore;
                   break;
                case PRODUCT_ENTERPRISE_SERVER_IA64:
                   m_nWinEdition = EnterpriseServerIA64;
                   break;
                case PRODUCT_BUSINESS_N:
                   m_nWinEdition = Business_N;
                   break;
                case PRODUCT_WEB_SERVER:
                   m_nWinEdition = WebServer;
                   break;
                case PRODUCT_CLUSTER_SERVER:
                   m_nWinEdition = ClusterServer;
                   break;
                case PRODUCT_HOME_SERVER:
                   m_nWinEdition = HomeServer;
                   break;
                case PRODUCT_STORAGE_EXPRESS_SERVER:
                   m_nWinEdition = StorageExpressServer;
                   break;
                case PRODUCT_STORAGE_STANDARD_SERVER:
                   m_nWinEdition = StorageStandardServer;
                   break;
                case PRODUCT_STORAGE_WORKGROUP_SERVER:
                   m_nWinEdition = StorageWorkgroupServer;
                   break;
                case PRODUCT_STORAGE_ENTERPRISE_SERVER:
                   m_nWinEdition = StorageEnterpriseServer;
                   break;
                case PRODUCT_SERVER_FOR_SMALLBUSINESS:
                   m_nWinEdition = ServerForSmallBusiness;
                   break;
                case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                   m_nWinEdition = SmallBusinessServerPremium;
                   break;

    #if _WIN32_WINNT >= 0x0601 // windows 7
                case PRODUCT_HOME_PREMIUM_N:
                   m_nWinEdition = HomePremium_N;
                   break;
                case PRODUCT_ENTERPRISE_N:
                   m_nWinEdition = Enterprise_N;
                   break;
                case PRODUCT_ULTIMATE_N:
                   m_nWinEdition = Ultimate_N;
                   break;
                case PRODUCT_WEB_SERVER_CORE:
                   m_nWinEdition = WebServerCore;
                   break;
                case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
                   m_nWinEdition = MediumBusinessServerManagement;
                   break;
                case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
                   m_nWinEdition = MediumBusinessServerSecurity;
                   break;
                case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
                   m_nWinEdition = MediumBusinessServerMessaging;
                   break;
                case PRODUCT_SERVER_FOUNDATION:
                   m_nWinEdition = ServerFoundation;
                   break;
                case PRODUCT_HOME_PREMIUM_SERVER:
                   m_nWinEdition = HomePremiumServer;
                   break;
                case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
                   m_nWinEdition = ServerForSmallBusiness_V;
                   break;
                case PRODUCT_STANDARD_SERVER_V:
                   m_nWinEdition = StandardServer_V;
                   break;
                case PRODUCT_DATACENTER_SERVER_V:
                   m_nWinEdition = DatacenterServer_V;
                   break;
                case PRODUCT_ENTERPRISE_SERVER_V:
                   m_nWinEdition = EnterpriseServer_V;
                   break;
                case PRODUCT_DATACENTER_SERVER_CORE_V:
                   m_nWinEdition = DatacenterServerCore_V;
                   break;
                case PRODUCT_STANDARD_SERVER_CORE_V:
                   m_nWinEdition = StandardServerCore_V;
                   break;
                case PRODUCT_ENTERPRISE_SERVER_CORE_V:
                   m_nWinEdition = EnterpriseServerCore_V;
                   break;
                case PRODUCT_HYPERV:
                   m_nWinEdition = HyperV;
                   break;
                case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
                   m_nWinEdition = StorageExpressServerCore;
                   break;
                case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
                   m_nWinEdition = StorageStandardServerCore;
                   break;
                case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
                   m_nWinEdition = StorageWorkgroupServerCore;
                   break;
                case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
                   m_nWinEdition = StorageEnterpriseServerCore;
                   break;
                case PRODUCT_STARTER_N:
                   m_nWinEdition = Starter_N;
                   break;
                case PRODUCT_PROFESSIONAL:
                   m_nWinEdition = Professional;
                   break;
                case PRODUCT_PROFESSIONAL_N:
                   m_nWinEdition = Professional_N;
                   break;
                case PRODUCT_SB_SOLUTION_SERVER:
                   m_nWinEdition = SBSolutionServer;
                   break;
                case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
                   m_nWinEdition = ServerForSBSolution;
                   break;
                case PRODUCT_STANDARD_SERVER_SOLUTIONS:
                   m_nWinEdition = StandardServerSolutions;
                   break;
                case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
                   m_nWinEdition = StandardServerSolutionsCore;
                   break;
                case PRODUCT_SB_SOLUTION_SERVER_EM:
                   m_nWinEdition = SBSolutionServer_EM;
                   break;
                case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
                   m_nWinEdition = ServerForSBSolution_EM;
                   break;
                case PRODUCT_SOLUTION_EMBEDDEDSERVER:
                   m_nWinEdition = SolutionEmbeddedServer;
                   break;
                case PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE:
                   m_nWinEdition = SolutionEmbeddedServerCore;
                   break;
                case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
                   m_nWinEdition = SmallBusinessServerPremiumCore;
                   break;
                case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
                   m_nWinEdition = EssentialBusinessServerMGMT;
                   break;
                case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
                   m_nWinEdition = EssentialBusinessServerADDL;
                   break;
                case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
                   m_nWinEdition = EssentialBusinessServerMGMTSVC;
                   break;
                case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
                   m_nWinEdition = EssentialBusinessServerADDLSVC;
                   break;
                case PRODUCT_CLUSTER_SERVER_V:
                   m_nWinEdition = ClusterServer_V;
                   break;
                case PRODUCT_EMBEDDED:
                   m_nWinEdition = Embedded;
                   break;
                case PRODUCT_STARTER_E:
                   m_nWinEdition = Starter_E;
                   break;
                case PRODUCT_HOME_BASIC_E:
                   m_nWinEdition = HomeBasic_E;
                   break;
                case PRODUCT_HOME_PREMIUM_E:
                   m_nWinEdition = HomePremium_E;
                   break;
                case PRODUCT_PROFESSIONAL_E:
                   m_nWinEdition = Professional_E;
                   break;
                case PRODUCT_ENTERPRISE_E:
                   m_nWinEdition = Enterprise_E;
                   break;
                case PRODUCT_ULTIMATE_E:
                   m_nWinEdition = Ultimate_E;
                   break;
    #endif
                }
             }
             break;
          }
       }
    }

    void SystemInfo::DetectWindowsServicePack()
    {
       // Display service pack (if any) and build number.

       if( m_osvi.dwMajorVersion == 4 && 
          lstrcmpi( m_osvi.szCSDVersion, "Service Pack 6" ) == 0 )
       {
          HKEY hKey;
          LONG lRet;

          // Test for SP6 versus SP6a.
          lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
             "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
             0, KEY_QUERY_VALUE, &hKey );
          if( lRet == ERROR_SUCCESS )
             sprintf(m_szServicePack, "Service Pack 6a (Build %d)\n", m_osvi.dwBuildNumber & 0xFFFF );         
          else // Windows NT 4.0 prior to SP6a
          {
             sprintf(m_szServicePack, "%s (Build %d)\n",
                m_osvi.szCSDVersion,
                m_osvi.dwBuildNumber & 0xFFFF);
          }

          RegCloseKey( hKey );
       }
       else // Windows NT 3.51 and earlier or Windows 2000 and later
       {
          sprintf(m_szServicePack, "%s (Build %d)\n",
             m_osvi.szCSDVersion,
             m_osvi.dwBuildNumber & 0xFFFF);
       }
    }

    DWORD SystemInfo::DetectProductInfo()
    {
       DWORD dwProductInfo = PRODUCT_UNDEFINED;

    #if _WIN32_WINNT >= 0x0600 
	    if(m_osvi.dwMajorVersion >= 6)
	    {
		    PGetProductInfo lpProducInfo = (PGetProductInfo)GetProcAddress(
			    GetModuleHandle(_T("kernel32.dll")), "GetProductInfo");

		    if(NULL != lpProducInfo)
		    {
			    lpProducInfo(m_osvi.dwMajorVersion, 
							    m_osvi.dwMinorVersion, 
							    m_osvi.wServicePackMajor, 
							    m_osvi.wServicePackMinor, 
							    &dwProductInfo);
		    }
	    }
    #endif

       return dwProductInfo;
    }

    WindowsVersion SystemInfo::GetWindowsVersion() const
    {
	    return m_nWinVersion;
    }

    WindowsEdition SystemInfo::GetWindowsEdition() const
    {
       return m_nWinEdition;
    }

    bool SystemInfo::IsNTPlatform() const
    {
	    return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
    }

    bool SystemInfo::IsWindowsPlatform() const
    {
	    return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
    }

    bool SystemInfo::IsWin32sPlatform() const
    {
	    return m_osvi.dwPlatformId == VER_PLATFORM_WIN32s;
    }
    	
    DWORD SystemInfo::GetMajorVersion() const
    {
	    return m_osvi.dwMajorVersion;
    }

    DWORD SystemInfo::GetMinorVersion() const
    {
	    return m_osvi.dwMinorVersion;
    }

    DWORD SystemInfo::GetBuildNumber() const
    {
	    return m_osvi.dwBuildNumber;
    }

    DWORD SystemInfo::GetPlatformID() const
    {
	    return m_osvi.dwPlatformId;	
    }

    // PARAMETER szServicePack must not be NULL
    void SystemInfo::GetServicePackInfo(TCHAR* szServicePack) const
    {
	    if(szServicePack == NULL) return;
    	
	    _tcscpy(szServicePack, m_szServicePack);
    }

    bool SystemInfo::Is32bitPlatform() const
    {
	    return !Is64bitPlatform();
    }

    bool SystemInfo::Is64bitPlatform() const
    {
	    return (
		    m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || 
		    m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		    m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64);
    }
}
