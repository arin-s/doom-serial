// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <set>
#include <string>

// doom
#include "doomdef.h"
#include "stubs.h"
#include "buds.h"
#include "common_serial.h"
#include "doomgeneric.h"

// jpegenc
#include "JPEGENC.h"

// os
#include "hal_uart.h"
#include "hal_iomux.h"
#include "heap_api.h"
#include "hal_timer.h"
#include "cmsis_os.h"


#define HEAP_SIZE 1000*50
static char heap[HEAP_SIZE];
osSemaphoreId binary_semaphore;
osSemaphoreDef(binary_semaphore);

// DMA stuff
void txdma(uint32_t xfer_size, int dma_error);
uint8_t buffer_writing_arr[JPEG_BUFFER_SIZE];
uint8_t buffer_dma_arr[JPEG_BUFFER_SIZE];
uint8_t* buffer_writing = buffer_writing_arr;
uint8_t* buffer_dma = buffer_dma_arr;
int buffer_writing_size;
int buffer_dma_size = JPEG_BUFFER_SIZE;
const uint32_t DESCRIPTOR_COUNT = JPEG_BUFFER_SIZE/HAL_DMA_MAX_DESC_XFER_SIZE;
HAL_DMA_DESC_T descriptors[DESCRIPTOR_COUNT];
uint32_t desc_cnt;
static const struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE, // HAL_UART_FLOW_CONTROL_RTSCTS,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .baud = 3000000,
    .dma_rx = false,
    .dma_tx = true,
    .dma_rx_stop_on_err = false,
};

// called from noapp_main
void doom_main()
{
    binary_semaphore = osSemaphoreCreate(osSemaphore(binary_semaphore), 1);
    uint32_t start_ms, end_ms, doom_delta, jpeg_delta, encode_delta;
    // Initialise UART peripheral
    hal_uart_open(HAL_UART_ID_0, &uart_cfg);
    // Set DMA tx handler
    hal_uart_irq_set_dma_handler(HAL_UART_ID_0, NULL, txdma);
    // Heap init
    med_heap_init(heap, HEAP_SIZE);
    // Initialize doom
    doomgeneric_Create(0, (char**){NULL});
    // Main loop
    while (true)
    {
        // Update game loop
        start_ms = DG_GetTicksMs();
        doomgeneric_Tick();
        end_ms = DG_GetTicksMs();
        doom_delta = end_ms - start_ms;
        // Generate JPEG
        start_ms = DG_GetTicksMs();
        getJPEG(buffer_writing, &buffer_writing_size);
        end_ms = DG_GetTicksMs();
        jpeg_delta = end_ms - start_ms;
        start_ms = DG_GetTicksMs();
        cobsEncode(buffer_writing, buffer_writing_size, PACKET_VIDEO);
        end_ms = DG_GetTicksMs();
        encode_delta = end_ms - start_ms;
        //doom_log("DOOM: %u\nJPEG: %u\nCOBS: %u\n", doom_delta, jpeg_delta, encode_delta);
        osSemaphoreWait(binary_semaphore, osWaitForever);
        if(log_offset > 0) {
            memcpy(buffer_dma + COBS_OFFSET, log_buf, log_offset);
            cobsEncode(buffer_dma, log_offset, PACKET_LOG);
            desc_cnt = DESCRIPTOR_COUNT;
            hal_uart_dma_send(HAL_UART_ID_0, buffer_dma, log_offset, descriptors, &desc_cnt);
            osSemaphoreWait(binary_semaphore, osWaitForever);
            log_offset = 0;
        }
        uint8_t *tmp = buffer_writing;
        buffer_writing = buffer_dma;
        buffer_dma = tmp;
        buffer_dma_size = buffer_writing_size;
        desc_cnt = DESCRIPTOR_COUNT;
        int result = hal_uart_dma_send(HAL_UART_ID_0, buffer_dma, buffer_dma_size, descriptors, &desc_cnt);
        if(!result)
        {
            uint8_t buf[32] = "value=";
            sprintf((char*)buf + strlen((char*)buf), "%d", result);
            hal_uart_dma_send(HAL_UART_ID_0, buf, strlen((char*)buf), descriptors, NULL);
            //while(true){}
        }
    }
    return;
}

void txdma(uint32_t xfer_size, int dma_error) {
    osSemaphoreRelease(binary_semaphore);
}