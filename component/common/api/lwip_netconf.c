/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "ethernetif.h"
#include "lwip_netconf.h"
#include "main.h"
#include "tcpip.h"
#include "osdep_service.h"
#if CONFIG_WLAN
#include "wifi_ind.h"
#endif

#include <platform/platform_stdlib.h>

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
#include "wlan_fast_connect/example_wlan_fast_connect.h"
#endif

#if LWIP_IPV6
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
#include "lwip/dhcp6.h"
#include "lwip/prot/dhcp6.h"
#endif
#endif

/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80
#endif

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif

/*Static IP ADDRESS*/
#ifndef AP_IP_ADDR0
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
#endif
   
/*NETMASK*/
#ifndef AP_NETMASK_ADDR0
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef AP_GW_ADDR0
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  
#endif

/*Static IP ADDRESS FOR ETHERNET*/
#ifndef ETH_IP_ADDR0
#define ETH_IP_ADDR0 192
#define ETH_IP_ADDR1 168
#define ETH_IP_ADDR2 0
#define ETH_IP_ADDR3 80
#endif

/*NETMASK FOR ETHERNET*/
#ifndef ETH_NETMASK_ADDR0
#define ETH_NETMASK_ADDR0 255
#define ETH_NETMASK_ADDR1 255
#define ETH_NETMASK_ADDR2 255
#define ETH_NETMASK_ADDR3 0
#endif

/*Gateway address for ethernet*/
#ifndef ETH_GW_ADDR0
#define ETH_GW_ADDR0 192
#define ETH_GW_ADDR1 168
#define ETH_GW_ADDR2 0
#define ETH_GW_ADDR3 1
#endif

/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 5
#if LWIP_IPV6
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
#define MAX_DHCP6_TRIES 5
#endif
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct netif xnetif[NET_IF_NUM]; /* network interface structure */
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
#if CONFIG_WLAN
extern  int error_flag;
extern rtw_mode_t wifi_mode;
#endif

int lwip_init_done = 0;

void LwIP_Init(void)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	int8_t idx = 0;

	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL );	

	/* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
	        struct ip_addr *netmask, struct ip_addr *gw,
	        void *state, err_t (* init)(struct netif *netif),
	        err_t (* input)(struct pbuf *p, struct netif *netif))

	Adds your network interface to the netif_list. Allocate a struct
	netif and pass a pointer to this structure as the first argument.
	Give pointers to cleared ip_addr structures when using DHCP,
	or fill them with sane numbers otherwise. The state pointer may be NULL.

	The init function pointer must point to a initialization function for
	your ethernet netif interface. The following code illustrates it's use.*/
	//printf("NET_IF_NUM:%d\n\r",NET_IF_NUM);
	for(idx=0;idx<NET_IF_NUM;idx++){
#if LWIP_VERSION_MAJOR >= 2
		if(idx==0){
			IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		}
		else{
			IP4_ADDR(ip_2_ip4(&ipaddr), AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
		}
#else
		if(idx==0){
			IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
			IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		}
		else{
			IP4_ADDR(&ipaddr, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(&gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
		}
#endif
#if CONFIG_ETHERNET
    if(idx == NET_IF_NUM - 1)
    {
#if LWIP_VERSION_MAJOR >= 2
			IP4_ADDR(ip_2_ip4(&ipaddr), ETH_IP_ADDR0, ETH_IP_ADDR1, ETH_IP_ADDR2, ETH_IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), ETH_NETMASK_ADDR0, ETH_NETMASK_ADDR1 , ETH_NETMASK_ADDR2, ETH_NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), ETH_GW_ADDR0, ETH_GW_ADDR1, ETH_GW_ADDR2, ETH_GW_ADDR3);
#else
			IP4_ADDR(&ipaddr, ETH_IP_ADDR0, ETH_IP_ADDR1, ETH_IP_ADDR2, ETH_IP_ADDR3);
			IP4_ADDR(&netmask, ETH_NETMASK_ADDR0, ETH_NETMASK_ADDR1 , ETH_NETMASK_ADDR2, ETH_NETMASK_ADDR3);
			IP4_ADDR(&gw, ETH_GW_ADDR0, ETH_GW_ADDR1, ETH_GW_ADDR2, ETH_GW_ADDR3);    	
#endif
    }
#endif
		xnetif[idx].name[0] = 'r';
		xnetif[idx].name[1] = '0'+idx;
#if LWIP_VERSION_MAJOR >= 2
#if CONFIG_ETHERNET
		if(idx == NET_IF_NUM - 1)
			netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw), NULL, &ethernetif_mii_init, &tcpip_input);
		else
			netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw), NULL, &ethernetif_init, &tcpip_input);
#else
		netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw), NULL, &ethernetif_init, &tcpip_input);
#endif
#else	
		
#if CONFIG_ETHERNET
    if(idx == NET_IF_NUM - 1)
      netif_add(&xnetif[idx], &ipaddr, &netmask, &gw, NULL, &ethernetif_mii_init, &tcpip_input);
    else
      netif_add(&xnetif[idx], &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
#else
    netif_add(&xnetif[idx], &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
#endif
#endif
    printf("interface %d is initialized\n", idx);

	}
	
	/*  Registers the default network interface. */
	netif_set_default(&xnetif[0]);

	/*move these operations to wifi_on/wifi_off*/
	#if 0
	/*  When the netif is fully configured this function must be called.*/
	for(idx = 0;idx < NET_IF_NUM;idx++)
		netif_set_up(&xnetif[idx]); 
	#endif

	lwip_init_done = 1;	 
}

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
extern write_reconnect_ptr p_write_reconnect_ptr;

extern struct wlan_fast_reconnect wifi_data_to_flash;
extern uint32_t offer_ip;
extern uint32_t server_ip;
extern u8 is_the_same_ap;
extern void restore_wifi_info_to_flash(uint32_t offer_ip, uint32_t server_ip);
#endif

/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
uint8_t LwIP_DHCP(uint8_t idx, uint8_t dhcp_state)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	uint32_t IPaddress;
	uint8_t iptab[4];
	uint8_t DHCP_state;
	struct netif *pnetif = NULL;
	struct dhcp *dhcp = NULL;

	DHCP_state = dhcp_state;
	
#if !CONFIG_ETHERNET
	if(idx > 1)
		idx = 1;
#endif

	pnetif = &xnetif[idx];
	if(DHCP_state == 0){
#if LWIP_VERSION_MAJOR >= 2
		ip_addr_set_zero(&pnetif->ip_addr);
		ip_addr_set_zero(&pnetif->netmask);
		ip_addr_set_zero(&pnetif->gw);
#else
		pnetif->ip_addr.addr = 0;
		pnetif->netmask.addr = 0;
		pnetif->gw.addr = 0;
#endif
	}

#if LWIP_VERSION_MAJOR >= 2
	dhcp = ((struct dhcp*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
	if(!netif_is_up(pnetif)) // netif should be set up before doing dhcp request (in lwip v2.0.0)
	{
		netif_set_up(pnetif);
	}
#else
	dhcp = pnetif->dhcp;
#endif

	for (;;)
	{
		//printf("\n\r ========DHCP_state:%d============\n\r",DHCP_state);
		switch (DHCP_state)
		{
			case DHCP_START:
			{
				/*acqurie wakelock to guarantee dhcp*/
				rtw_wakelock_timeout(4*1000);
#if CONFIG_WLAN
				wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
				if(is_the_same_ap){
					if( (offer_ip != 0 && offer_ip != 0xFFFFFFFF) || (dhcp != NULL) ){
						if(dhcp == NULL){
							dhcp = (struct dhcp *)mem_malloc(sizeof(struct dhcp));
							if (dhcp == NULL) {
							  printf("dhcp_start(): could not allocate dhcp\n");
							  return -1;
							}
						}
						memset(dhcp, 0, sizeof(struct dhcp));
						dhcp->offered_ip_addr.addr = (u32_t)offer_ip;
#if LWIP_VERSION_MAJOR >= 2
						ip_addr_set_ip4_u32(&dhcp->server_ip_addr, (u32_t)server_ip);
#else
						dhcp->server_ip_addr.addr = (u32_t)server_ip;
#endif
#if LWIP_VERSION_MAJOR >= 2
						netif_set_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP, dhcp);
#else
						pnetif->dhcp = dhcp;
#endif
					}
				}else{
					if(dhcp != NULL){
						memset(dhcp, 0, sizeof(struct dhcp));
					}
				}

#endif
				dhcp_start(pnetif);
#if LWIP_VERSION_MAJOR >= 2
				dhcp = ((struct dhcp*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
#else
				dhcp = pnetif->dhcp;
#endif
				IPaddress = 0;
				DHCP_state = DHCP_WAIT_ADDRESS;
			}
			break;

		case DHCP_WAIT_ADDRESS:
		{
			/* If DHCP stopped by wifi_disconn_hdl*/		
#if LWIP_VERSION_MAJOR >= 2
#include "lwip/prot/dhcp.h"
			if((dhcp_state_enum_t)dhcp->state == DHCP_STATE_OFF)
			{
				IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
				IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
				IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
				netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
				printf("\n\rLwIP_DHCP: dhcp stop.");
				return DHCP_STOP;
			}
#else
			if(dhcp->state == DHCP_OFF)
			{
                                IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
                                IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                                IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                                netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
				printf("\n\rLwIP_DHCP: dhcp stop.");
				return DHCP_STOP;
			}
#endif
			
			/* Read the new IP address */
#if LWIP_VERSION_MAJOR >= 2
			IPaddress = ip_addr_get_ip4_u32(netif_ip_addr4(pnetif));
#else
			IPaddress = pnetif->ip_addr.addr;
#endif

			if (IPaddress!=0) 
			{
#if LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS
                tcp_randomize_local_port();
                udp_randomize_local_port();
#endif
				DHCP_state = DHCP_ADDRESS_ASSIGNED;	
#if CONFIG_WLAN
				wifi_reg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl, NULL);
#endif
				
				/* Stop DHCP */
				// dhcp_stop(pnetif);  /* can not stop, need to renew, Robbie*/

				iptab[0] = (uint8_t)(IPaddress >> 24);
				iptab[1] = (uint8_t)(IPaddress >> 16);
				iptab[2] = (uint8_t)(IPaddress >> 8);
				iptab[3] = (uint8_t)(IPaddress);
				printf("\n\rInterface %d IP address : %d.%d.%d.%d", idx, iptab[3], iptab[2], iptab[1], iptab[0]);

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
#if LWIP_VERSION_MAJOR >= 2
				dhcp = ((struct dhcp*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
				restore_wifi_info_to_flash((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)(ip_2_ip4(&dhcp->server_ip_addr)));
#else
				dhcp = pnetif->dhcp;
				restore_wifi_info_to_flash((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)dhcp->server_ip_addr.addr);
#endif
#endif

#if CONFIG_WLAN
				error_flag = RTW_NO_ERROR;
#endif
				return DHCP_ADDRESS_ASSIGNED;
			}
			else
			{
				/* DHCP timeout */
				if (dhcp->tries > MAX_DHCP_TRIES)
				{
					DHCP_state = DHCP_TIMEOUT;
					/* Stop DHCP */
					dhcp_stop(pnetif);

					/* Static address used */

#if LWIP_VERSION_MAJOR >= 2
					IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
					IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
					IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
					netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
#else
					IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
					IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
					IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
					netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
#endif
					iptab[0] = IP_ADDR3;
					iptab[1] = IP_ADDR2;
					iptab[2] = IP_ADDR1;
					iptab[3] = IP_ADDR0;
					printf("\n\rInterface %d DHCP timeout",idx);
					printf("\n\rStatic IP address : %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
#if LWIP_VERSION_MAJOR >= 2
					restore_wifi_info_to_flash((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)(ip_2_ip4(&dhcp->server_ip_addr)));
#else
					restore_wifi_info_to_flash((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)dhcp->server_ip_addr.addr);
#endif
#endif

#if CONFIG_WLAN
					error_flag = RTW_DHCP_FAIL;
#endif

#if CONFIG_ETHERNET
                    if(idx == NET_IF_NUM -1) // This is the ethernet interface, set it up for static ip address
                       netif_set_up(pnetif);
#endif
					return DHCP_TIMEOUT;
					}
				}
			}
		break;
		case DHCP_RELEASE_IP:
#if CONFIG_WLAN
			wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
			printf("\n\rLwIP_DHCP: Release ip");
#if LWIP_VERSION_MAJOR >= 2
			dhcp_release(pnetif);
#else
			if(dhcp && dhcp->state != DHCP_OFF)
				dhcp_release_unicast(pnetif);
#endif
			return DHCP_RELEASE_IP;
		case DHCP_STOP:
#if CONFIG_WLAN
			wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
			printf("\n\rLwIP_DHCP: dhcp stop.");
			dhcp_stop(pnetif);
			return DHCP_STOP;
		default: 
			break;
	}
        /* wait 250 ms */
        vTaskDelay(10);
	}   
}

#if LWIP_IPV6
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
extern err_t dhcp6_enable(struct netif *netif);
uint8_t LwIP_DHCP6(uint8_t idx, uint8_t dhcp6_state)
{
	uint8_t *ipv6_global;
	uint8_t DHCP6_state;
	struct netif *pnetif = NULL;
	struct dhcp6 *dhcp6 = NULL;
#if defined(CONFIG_MATTER) && CONFIG_MATTER
	int dhcp6_handler_trigger = 0;
#endif
	err_t err;

	DHCP6_state = dhcp6_state;

#if !CONFIG_ETHERNET
	if(idx > 1)
	idx = 1;
#endif

	pnetif = &xnetif[idx];
	if(DHCP6_state == 0){
		for (int free_idx = 1; free_idx < LWIP_IPV6_NUM_ADDRESSES; free_idx++) {
			ip_addr_set_zero(&pnetif->ip6_addr[free_idx]);
			netif_ip6_addr_set_state(pnetif, free_idx, IP6_ADDR_INVALID);
		}
	}

	dhcp6 = ((struct dhcp6*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6));
	if(!netif_is_up(pnetif)) // netif should be set up before doing dhcp request (in lwip v2.0.0)
	{
		netif_set_up(pnetif);
	}

	for (;;)
	{
		//printf("\n\r ========DHCP6_state:%d============\n\r",DHCP6_state);
		switch (DHCP6_state)
		{

			case DHCP6_START:
			{
#if CONFIG_WLAN
				wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
				err = dhcp6_enable(pnetif);
				if (err != ERR_OK)
					printf("error in dhcp6_enable\r\n");

				dhcp6 = ((struct dhcp6*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6));

				ipv6_global = 0;
				DHCP6_state = DHCP6_WAIT_ADDRESS;
			}
			break;
			case DHCP6_WAIT_ADDRESS:
			{
				/* Read the new IPv6 address */
				ipv6_global = LwIP_GetIPv6_global(pnetif);
				ip6_addr_t zero_address;
				ip6_addr_set_any(&zero_address);

#if defined(CONFIG_MATTER) && CONFIG_MATTER
				if (ip6_addr_isvalid(netif_ip6_addr_state(pnetif, 0)) && (dhcp6_handler_trigger == 0))
				{
					wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
					dhcp6_handler_trigger = 1;
				}
#endif

				if(!ip6_addr_cmp(&zero_address, netif_ip6_addr(pnetif, 1))){

					DHCP6_state = DHCP6_ADDRESS_ASSIGNED;

#if CONFIG_WLAN
					wifi_reg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl, NULL);
#endif

					printf("\n\rInterface %d IPv6 address : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					idx, ipv6_global[0], ipv6_global[1],  ipv6_global[2],  ipv6_global[3],  ipv6_global[4],  ipv6_global[5],
					ipv6_global[6], ipv6_global[7], ipv6_global[8], ipv6_global[9], ipv6_global[10], ipv6_global[11],
					ipv6_global[12], ipv6_global[13], ipv6_global[14], ipv6_global[15]);

					/*Todo: error_flag for DHCPv6*/

#if defined(CONFIG_MATTER) && CONFIG_MATTER
					if (!dhcp6_handler_trigger)
						wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
#endif
					return DHCP6_ADDRESS_ASSIGNED;
				}

				else
				{
					/* DHCP timeout */
					if (dhcp6->tries > MAX_DHCP6_TRIES || pnetif->rs_timeout)
					{
						DHCP6_state = DHCP6_TIMEOUT;
						/* Stop DHCP */
						dhcp6_stop(pnetif);

						/*Todo: error_flag for DHCPv6*/

#if CONFIG_ETHERNET
						if(idx == NET_IF_NUM -1) // This is the ethernet interface, set it up for static ip address
							netif_set_up(pnetif);
#endif
#if defined(CONFIG_MATTER) && CONFIG_MATTER
						if (!dhcp6_handler_trigger)
							wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
#endif
						return DHCP6_TIMEOUT;
					}
				}
			}
			break;
			case DHCP6_RELEASE_IP:
#if CONFIG_WLAN
				wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
				printf("\n\rLwIP_DHCP6: Release ipv6");
				dhcp6_release(pnetif);
				return DHCP6_RELEASE_IP;
			case DHCP6_STOP:
#if CONFIG_WLAN
				wifi_unreg_event_handler(WIFI_EVENT_BEACON_AFTER_DHCP, wifi_rx_beacon_hdl);
#endif
				printf("\n\rLwIP_DHCP6: dhcp6 stop.");
				dhcp6_stop(pnetif);
				return DHCP6_STOP;
			default:
			break;
		}
		vTaskDelay(30);
	}
}
#endif
#endif

void LwIP_ReleaseIP(uint8_t idx)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct netif *pnetif = &xnetif[idx];
#if LWIP_VERSION_MAJOR >= 2
	IP4_ADDR(ip_2_ip4(&ipaddr), 0, 0, 0, 0);
	IP4_ADDR(ip_2_ip4(&netmask), 255, 255 , 255, 0);
	IP4_ADDR(ip_2_ip4(&gw), 0, 0, 0, 0);
	netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
#else
	IP4_ADDR(&ipaddr, 0, 0, 0, 0);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 0, 0, 0, 0);
	
	netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
#endif

#if LWIP_IPV6
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
	IP6_ADDR(ip_2_ip6(&ipaddr), 0,0,0,0);
	for (int idx = 1; idx < LWIP_IPV6_NUM_ADDRESSES; idx++) {
		netif_ip6_addr_set_state(pnetif, idx, IP6_ADDR_INVALID);
		netif_ip6_addr_set(pnetif, idx, ip_2_ip6(&ipaddr));
	}
#endif
#endif
}

uint8_t* LwIP_GetMAC(struct netif *pnetif)
{
	return (uint8_t *) (pnetif->hwaddr);
}

uint8_t* LwIP_GetIP(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->ip_addr);
}

uint8_t* LwIP_GetGW(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->gw);
}

uint8_t* LwIP_GetMASK(struct netif *pnetif)
{
	return (uint8_t *) &(pnetif->netmask);
}

uint8_t* LwIP_GetBC(struct netif *pnetif)
{
#if LWIP_VERSION_MAJOR >= 2
	//struct dhcp *dhcp = ((struct dhcp*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
	return NULL;
#else
	return (uint8_t *) &(pnetif->dhcp->offered_bc_addr);
#endif
}

#if LWIP_DNS
void LwIP_GetDNS(struct ip_addr* dns)
{
#if LWIP_VERSION_MAJOR >= 2
	struct ip_addr *tmp = (struct ip_addr *)dns_getserver(0);
	*dns = *tmp;        
#else
	*dns = dns_getserver(0);
#endif
}

void LwIP_SetDNS(struct ip_addr* dns)
{
	dns_setserver(0, dns);
}
#endif
void LwIP_UseStaticIP(struct netif *pnetif)
{
	struct ip_addr ipaddr = {0};
	struct ip_addr netmask = {0};
	struct ip_addr gw = {0};

	/* Static address used */
	if(pnetif->name[1] == '0'){
#if CONFIG_WLAN
		if(wifi_mode == RTW_MODE_STA){
#if LWIP_VERSION_MAJOR >= 2
		IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
		IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#else
		IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
		IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif
		}
		else if(wifi_mode == RTW_MODE_AP){
#if LWIP_VERSION_MAJOR >= 2
			IP4_ADDR(ip_2_ip4(&ipaddr), AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
#else
			IP4_ADDR(&ipaddr, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(&gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
#endif
		}
#endif
	}else{
#if LWIP_VERSION_MAJOR >= 2
		IP4_ADDR(ip_2_ip4(&ipaddr), AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
		IP4_ADDR(ip_2_ip4(&netmask), AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
		IP4_ADDR(ip_2_ip4(&gw), AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
#else
		IP4_ADDR(&ipaddr, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
		IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1 , AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
		IP4_ADDR(&gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
#endif
	}
#if LWIP_VERSION_MAJOR >= 2
	netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
#else
	netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
#endif
}
#if LWIP_AUTOIP
#include <lwip/autoip.h>
#if LWIP_VERSION_MAJOR >= 2
#include <lwip/prot/autoip.h>
#endif

void LwIP_AUTOIP(struct netif *pnetif)
{
	uint8_t *ip = LwIP_GetIP(pnetif);
	struct autoip *autoip = NULL;

#if LWIP_VERSION_MAJOR >= 2
	autoip = ((struct autoip*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_AUTOIP));
#else
	autoip = pnetif->autoip;
#endif
	if(autoip && (autoip->tried_llipaddr >= MAX_CONFLICTS)) // before autoip_start(), autoip may be NULL
		autoip->tried_llipaddr = 0;

	autoip_start(pnetif);

#if LWIP_VERSION_MAJOR >= 2
	autoip = ((struct autoip*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_AUTOIP));
#else
	autoip = pnetif->autoip;
#endif

	while((autoip->state == AUTOIP_STATE_PROBING) || (autoip->state == AUTOIP_STATE_ANNOUNCING)) {
		vTaskDelay(1000);
	}

	if(*((uint32_t *) ip) == 0) {
		struct ip_addr ipaddr;
		struct ip_addr netmask;
		struct ip_addr gw;

		printf("AUTOIP timeout\n");

		/* Static address used */
#if LWIP_VERSION_MAJOR >= 2 		
		IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
		IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
#else
		IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
		IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		netif_set_addr(pnetif, &ipaddr , &netmask, &gw);
#endif
		printf("Static IP address : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	}
	else {
		printf("\nLink-local address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	}
}
#endif

#if LWIP_IPV6
uint8_t* LwIP_GetIPv6_linklocal(struct netif *pnetif)
{
	return (uint8_t *) netif_ip6_addr(pnetif, 0)->addr;
}
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
uint8_t* LwIP_GetIPv6_global(struct netif *pnetif)
{
	return (uint8_t *) netif_ip6_addr(pnetif, 1)->addr;
}
#endif
#endif

uint32_t LWIP_Get_Dynamic_Sleep_Interval()
{
	#ifdef DYNAMIC_TICKLESS_SLEEP_INTERVAL
		return DYNAMIC_TICKLESS_SLEEP_INTERVAL;
	#else
		return 0;
	#endif
}
