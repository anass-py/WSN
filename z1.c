#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "contiki-lib.h"
#include "dev/serial-line.h"
#include "dev/leds.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY 1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

static struct simple_udp_connection udp_conn;
static uip_ipaddr_t ipaddr;
PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void udp_rx_callback(struct simple_udp_connection *c,
        const uip_ipaddr_t *sender_addr,
        uint16_t sender_port,
        const uip_ipaddr_t *receiver_addr,
        uint16_t receiver_port,
        const uint8_t *data,
        uint16_t datalen)
{
  LOG_INFO("Received response '%.*s' from the raspberry sonde\n", datalen, (char *) data);
  //LOG_INFO_6ADDR(sender_addr);
  memcpy(&ipaddr,sender_addr,sizeof(uip_ipaddr_t));
  //ipaddr = *sender_addr;
#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  //LOG_INFO("Sending response.\n");
  /*printf("ipaddr 1 = ");
  LOG_INFO_6ADDR(&ipaddr);*/
  //simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data) {
  PROCESS_BEGIN();
  serial_line_init();
  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();
  /* Initialize UDP connection */
  static char buf[256];
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  while (1) {
    PROCESS_YIELD();
    if (ev==serial_line_event_message && data != NULL) {
      printf("Envoie de %s à l'adresse : ",(char *) data);
      LOG_INFO_6ADDR(&ipaddr);
      snprintf(buf, sizeof(buf), "%s",(char *) data);
      simple_udp_sendto(&udp_conn, buf, strlen(buf)+1,&ipaddr);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
