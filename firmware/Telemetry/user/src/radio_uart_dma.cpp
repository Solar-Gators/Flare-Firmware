#include "radio_uart_dma.h"

#include <cstddef>
#include <cstdint>

#include "main.h"

extern UART_HandleTypeDef huart1;

namespace
{
constexpr size_t FRAME_SIZE = 12;
constexpr size_t RX_DMA_SIZE = FRAME_SIZE * 32;

alignas(4) uint8_t rx_dma_buffer[RX_DMA_SIZE];

constexpr uint32_t RX_HALF_FLAG = 1u << 0;
constexpr uint32_t RX_FULL_FLAG = 1u << 1;
}  // namespace

namespace radio_uart_dma
{
void start()
{
    HAL_UART_Receive_DMA(&huart1, rx_dma_buffer, RX_DMA_SIZE);
}

void onRxHalfCompleteFromISR() {}

void onRxCompleteFromISR() {}
}  // namespace radio_uart_dma

extern "C" void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &huart1)
    {
        radio_uart_dma::onRxHalfCompleteFromISR();
    }
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &huart1)
    {
        radio_uart_dma::onRxCompleteFromISR();
    }
}