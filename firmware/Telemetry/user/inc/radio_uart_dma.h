#ifndef FLAREFIRMWARE_RADIO_UART_DMA_H
#define FLAREFIRMWARE_RADIO_UART_DMA_H

namespace radio_uart_dma
{
void start();
void processRxHalf();
void processRxFull();

void onRxHalfCompleteFromISR();
void onRxCompleteFromISR();
}  // namespace radio_uart_dma

#endif  //FLAREFIRMWARE_RADIO_UART_DMA_H
