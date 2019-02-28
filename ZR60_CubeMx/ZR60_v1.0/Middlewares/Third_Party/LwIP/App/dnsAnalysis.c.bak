/******************************************************
文件名：	dnsAnalysis

描述：	
Data			Vasion			author
2018/04/12		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "dnsAnalysis.h"

/*******************************************************
description： global variable definitions
*******************************************************/

/*******************************************************
description： static variable definitions
*******************************************************/
static struct ip_addr SednsAnalysis_dw_DestIPaddr[domainNum];//目的ip
static uint8  SednsAnalysis_u_Validity[domainNum];//ip有效性
static uint8  SednsAnalysis_u_UpdateHost[domainNum];//待更新ip的主机
static uint16 SednsAnalysis_w_Timer;//计时器
static char Se_u_arg[domainNum];
static char short_hostname[]="api.zzwtec.com";//短连接主机域名
static char lng_hostname[]="push2.zzwtec.com";//长连接主机域名
static char ntp_one_hostname[]="ntp1.aliyun.com";//ntp主机域名
static char ntp_two_hostname[]="time1.aliyun.com";//ntp主机域名
static char ntp_three_hostname[]="time.syn029.com";//ntp主机域名
static char ntp_four_hostname[]="ntp.shu.edu.cn";//ntp主机域名
static char ntp_five_hostname[]="s2f.time.edu.cn";//ntp主机域名
static char ntp_six_hostname[]="1.cn.pool.ntp.org";//ntp主机域名
static char http_download_hostname[]="mobile.zzwtec.com";//http文件下载服务器
//static char ntp_eight_hostname[]="time3.aliyun.com";//ntp主机域名

dnsAnalysis_Struct  domain[domainNum] = 
{
	short_hostname,
	lng_hostname,
	ntp_one_hostname,
	ntp_two_hostname,
	ntp_three_hostname,
	ntp_four_hostname,
	ntp_five_hostname,
	ntp_six_hostname,
	http_download_hostname
	//ntp_eight_hostname
};
/*******************************************************
description： function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/
static void dnsAnalysis_DNSfound(const char *name, struct ip_addr *ipaddr, void *arg);


/******************************************************
description： function code
******************************************************/
/******************************************************
*函数名：InitdnsAnalysis_parameter

*形  参：void

*返回值：void

*描  述：初始化

*备  注：
******************************************************/
void InitdnsAnalysis_parameter(void)
{
	char Le_u_i;
	for(Le_u_i = 0U;Le_u_i < domainNum;Le_u_i++)
	{
		SednsAnalysis_u_Validity[Le_u_i] = 0U;
		Se_u_arg[Le_u_i] = Le_u_i;
		SednsAnalysis_u_UpdateHost[Le_u_i] = 1U;
	}
	SednsAnalysis_w_Timer = DNS_ANALYSIS_TASK_PERIOD - 1100U;
}

/******************************************************
*函数名：TskdnsAnalysis_MainFunction

*形  参：

*返回值：

*描  述：域名解析主任务,5ms周期调用

*备  注：
******************************************************/
void TskdnsAnalysis_MainFunction(void)
{
	struct ip_addr IPaddr[domainNum];
	static char Le_u_i = 0;
	static uint8 SednsAnalysis_u_TaktTimer = 0;

	if((0 == DNS_GET_PHY_LINK_STATUS()) && (DHCP_ADDRESS_ASSIGNED != DNS_DHCP_STATE))/* Get Ethernet link status*/
	{
		SednsAnalysis_w_Timer = 0;
		return;
	}
	
	if(SednsAnalysis_w_Timer >= DNS_ANALYSIS_TASK_PERIOD)
	{
		if(SednsAnalysis_u_TaktTimer >= (20/DNS_ANALYSIS_SCHEDULING_CYCLE))
		{
			SednsAnalysis_u_TaktTimer = 0;
			if(SednsAnalysis_u_UpdateHost[Le_u_i] == 1U)
			{
				DNS_PRINTF_D("DNS域名 %s 开始解析\n",domain[Le_u_i].name);
				(void)dns_gethostbyname(domain[Le_u_i].name,&IPaddr[Le_u_i], \
																(dns_found_callback)(&dnsAnalysis_DNSfound),&Se_u_arg[Le_u_i]);//域名解析
			}
			
			Le_u_i += 1;
			
			if(domainNum == Le_u_i)
			{
				Le_u_i = 0;
			}
		}
		else
		{
			SednsAnalysis_u_TaktTimer++;
		}
	}
	else
	{
		SednsAnalysis_w_Timer++;
	}
}

/******************************************************
*函数名：GetdnsAnalysis_ipAddr

*形  参：void

*返回值：void

*描  述：获取dns解析后的ip

*备  注：
******************************************************/
void GetdnsAnalysis_ipAddr(uint8 index, struct ip_addr *ipaddr)
{
	ipaddr->addr = SednsAnalysis_dw_DestIPaddr[index].addr;
}


/******************************************************
*函数名：dnsAnalysis_DNSfound

*形  参：

*返回值：void

*描  述：dns解析完成回调函数

*备  注：
******************************************************/
static void dnsAnalysis_DNSfound(const char *name, struct ip_addr *ipaddr, void *arg)
{
	char Le_u_obj;
	Le_u_obj = *((char*)arg);
	if(ipaddr != NULL)
	{
		if(ipaddr->addr == 0)
		{
			//SednsAnalysis_u_Validity[Le_u_obj] = 0U;
			SednsAnalysis_u_UpdateHost[Le_u_obj] = 1U;
			DNS_PRINTF_D("\r\nErrorLogging：DNS域名 %s 解析失败,ip地址:0.0.0.0 ",domain[Le_u_obj].name);
		}
		else
		{
			SednsAnalysis_dw_DestIPaddr[Le_u_obj].addr = ipaddr->addr;
			SednsAnalysis_u_Validity[Le_u_obj] = 1U;
			SednsAnalysis_u_UpdateHost[Le_u_obj] = 0U;
			DNS_PRINTF_D("DNS域名 %s 解析成功,ip地址: ",domain[Le_u_obj].name);
			DNS_PRINTF_IP("%u.%u.%u.%u\n",(uint8_t)SednsAnalysis_dw_DestIPaddr[Le_u_obj].addr, \
							(uint8_t)(SednsAnalysis_dw_DestIPaddr[Le_u_obj].addr >> 8),	\
							(uint8_t)(SednsAnalysis_dw_DestIPaddr[Le_u_obj].addr >> 16),	\
							(uint8_t)(SednsAnalysis_dw_DestIPaddr[Le_u_obj].addr >> 24));
		}
	}
	else
	{
		//SednsAnalysis_u_Validity[Le_u_obj] = 0U;
		SednsAnalysis_u_UpdateHost[Le_u_obj] = 1U;
		DNS_PRINTF_D("\r\nErrorLogging： DNS域名 %s 解析失败",domain[Le_u_obj].name);
	}
}


/******************************************************
*函数名：GetdnsAnalysis_u_ipValidity

*形  参：void

*返回值：void

*描  述：获取dns解析后的ip有效性

*备  注：
******************************************************/
uint8 GetdnsAnalysis_u_ipValidity(uint8 index)
{
	return SednsAnalysis_u_Validity[index];
}


/******************************************************
*函数名：SetdnsAnalysis_ipUpdate

*形  参：void

*返回值：void

*描  述：更新ip

*备  注：
******************************************************/
void SetdnsAnalysis_ipUpdate(uint8 index)
{
	SednsAnalysis_u_UpdateHost[index] = 1U;
}

