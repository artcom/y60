//////////////////////////////////////////////////////////////////////
// SystemInfo.h: interface for the SystemInfo class.
//////////////////////////////////////////////////////////////////////
#ifndef _included_asl_SystemInfo_
#define _included_asl_SystemInfo_

#include <Windows.h>

#include "asl_base_settings.h"

namespace asl {

    typedef enum WindowsVersion
    {
       Windows,
       Windows32s,
       Windows95,
       Windows95OSR2,
       Windows98,
       Windows98SE,
       WindowsMillennium,
       WindowsNT351,
       WindowsNT40,
       WindowsNT40Server,
       Windows2000,
       WindowsXP,
       WindowsXPProfessionalx64,
       WindowsHomeServer,
       WindowsServer2003,
       WindowsServer2003R2,
       WindowsVista,
       WindowsServer2008,
       WindowsServer2008R2,
       Windows7,
    };

    typedef enum WindowsEdition
    {
       EditionUnknown,

       Workstation,
       Server,
       AdvancedServer,
       Home,

       Ultimate,
       HomeBasic,
       HomePremium,
       Enterprise,
       HomeBasic_N,
       Business,
       StandardServer,
       DatacenterServer,
       SmallBusinessServer,
       EnterpriseServer,
       Starter,
       DatacenterServerCore,
       StandardServerCore,
       EnterpriseServerCore,
       EnterpriseServerIA64,
       Business_N,
       WebServer,
       ClusterServer,
       HomeServer,
       StorageExpressServer,
       StorageStandardServer,
       StorageWorkgroupServer,
       StorageEnterpriseServer,
       ServerForSmallBusiness,
       SmallBusinessServerPremium,
       HomePremium_N,
       Enterprise_N,
       Ultimate_N,
       WebServerCore,
       MediumBusinessServerManagement,
       MediumBusinessServerSecurity,
       MediumBusinessServerMessaging,
       ServerFoundation,
       HomePremiumServer,
       ServerForSmallBusiness_V,
       StandardServer_V,
       DatacenterServer_V,
       EnterpriseServer_V,
       DatacenterServerCore_V,
       StandardServerCore_V,
       EnterpriseServerCore_V,
       HyperV,
       StorageExpressServerCore,
       StorageStandardServerCore,
       StorageWorkgroupServerCore,
       StorageEnterpriseServerCore,
       Starter_N,
       Professional,
       Professional_N,
       SBSolutionServer,
       ServerForSBSolution,
       StandardServerSolutions,
       StandardServerSolutionsCore,
       SBSolutionServer_EM,
       ServerForSBSolution_EM,
       SolutionEmbeddedServer,
       SolutionEmbeddedServerCore,
       SmallBusinessServerPremiumCore,
       EssentialBusinessServerMGMT,
       EssentialBusinessServerADDL,
       EssentialBusinessServerMGMTSVC,
       EssentialBusinessServerADDLSVC,
       ClusterServer_V,
       Embedded,
       Starter_E,
       HomeBasic_E,
       HomePremium_E,
       Professional_E,
       Enterprise_E,
       Ultimate_E
    };

    class ASL_BASE_DECL SystemInfo  
    {
       WindowsVersion	   m_nWinVersion;
       WindowsEdition	   m_nWinEdition;
       TCHAR			      m_szServicePack[128];
       OSVERSIONINFOEX   m_osvi;
       SYSTEM_INFO		   m_SysInfo;
       BOOL			      m_bOsVersionInfoEx;

    private:
       void DetectWindowsVersion();
       void DetectWindowsEdition();
       void DetectWindowsServicePack();
       DWORD DetectProductInfo();

    public:
       SystemInfo();
       virtual ~SystemInfo();

       WindowsVersion GetWindowsVersion() const;		// returns the windows version
       WindowsEdition GetWindowsEdition() const;		// returns the windows edition
       bool IsNTPlatform() const;						   // true if NT platform
       bool IsWindowsPlatform() const;					   // true is Windows platform
       bool IsWin32sPlatform() const;					   // true is Win32s platform
       DWORD GetMajorVersion() const;					   // returns major version
       DWORD GetMinorVersion() const;				   	// returns minor version
       DWORD GetBuildNumber() const;			   		// returns build number
       DWORD GetPlatformID() const;				      	// returns platform ID
       void GetServicePackInfo(TCHAR* szServicePack) const;// additional information about service pack
       bool Is32bitPlatform() const;					   // true if platform is 32-bit
       bool Is64bitPlatform() const;					   // true if platform is 64-bit
    };

} //Namespace asl

#endif

