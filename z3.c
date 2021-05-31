#include "contiki.h"
#include "contiki-lib.h"
#include "dev/serial-line.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "dev/leds.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

//#define SEND_INTERVAL           (20 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(raspberry, "Raspberry");
PROCESS(check_sonde, "Check Sonde");
AUTOSTART_PROCESSES(&raspberry, &check_sonde);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Received demande '%.*s' from the user\n", datalen, (char *) data);
  //LOG_INFO_6ADDR(sender_addr);
  printf("%s", (char *) data);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}
/*---------------------------------------------------------------------------*/
static void read_sonde(char* sonde_id) {
  printf("{T:1,S:%s}\n", sonde_id);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(raspberry, ev, data)
{
  //static struct etimer p_t;
  static char str[256];
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();
  serial_line_init();
  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  while(1) {
    PROCESS_YIELD();
    if (ev==serial_line_event_message && data != NULL) {
        //printf("Data = %s", (char *) data);
        if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
          /* Send to DAG root */
          LOG_INFO("Sending data to ");
          LOG_INFO_6ADDR(&dest_ipaddr);
          LOG_INFO_("\n");
          snprintf(str, sizeof(str), "%s", (char *) data);
          simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
        } else {
          LOG_INFO("Not reachable yet\n");
        }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(check_sonde, ev, data)
{
  static struct etimer p_t;
  PROCESS_BEGIN();
  etimer_set(&p_t, 5*CLOCK_SECOND);


  while(1) {
    PROCESS_WAIT_UNTIL(etimer_expired(&p_t));
    read_sonde("1");
    etimer_reset(&p_t);
    PROCESS_WAIT_UNTIL(etimer_expired(&p_t));
    read_sonde("2");
    etimer_reset(&p_t);
    PROCESS_WAIT_UNTIL(etimer_expired(&p_t));
    read_sonde("3");
    etimer_reset(&p_t);
  }

  PROCESS_END();
}
