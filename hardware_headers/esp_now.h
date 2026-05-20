#pragma once
// Minimal stub for esp_now.h
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef struct {
  uint8_t peer_addr[6];
  int channel;
  bool encrypt;
} esp_now_peer_info_t;

inline int esp_now_init() { return 0; }
inline int esp_now_add_peer(const esp_now_peer_info_t*) { return 0; }
inline int esp_now_send(const uint8_t*, const uint8_t*, size_t) { return 0; }

typedef struct esp_now_recv_info {
  uint8_t src_addr[6];
  int rx_ctrl;
} esp_now_recv_info;

typedef void (*esp_now_recv_cb_t)(const esp_now_recv_info*, const uint8_t*, int);

inline int esp_now_register_recv_cb(esp_now_recv_cb_t cb) { (void)cb; return 0; }
