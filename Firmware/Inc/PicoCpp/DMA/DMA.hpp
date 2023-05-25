//
// Created by TS on 5/24/2023
//

#ifndef EE490_FIRMWARE_DMA_HPP
#define EE490_FIRMWARE_DMA_HPP

#include <utility>

#include "hardware/dma.h"

namespace rpp::dma {
  /**
   * @brief Constants for all the DMA triggers available.
   */
  enum class Triggers : uint8_t {
    pio_0_tx_0 = DREQ_PIO0_TX0,
    pio_0_tx_1 = DREQ_PIO0_TX1,
    pio_0_tx_2 = DREQ_PIO0_TX2,
    pio_0_tx_3 = DREQ_PIO0_TX3,
    pio_0_rx_0 = DREQ_PIO0_RX0,
    pio_0_rx_1 = DREQ_PIO0_RX1,
    pio_0_rx_2 = DREQ_PIO0_RX2,
    pio_0_rx_3 = DREQ_PIO0_RX3,
    pio_1_tx_0 = DREQ_PIO1_TX0,
    pio_1_tx_1 = DREQ_PIO1_TX1,
    pio_1_tx_2 = DREQ_PIO1_TX2,
    pio_1_tx_3 = DREQ_PIO1_TX3,
    pio_1_rx_0 = DREQ_PIO1_RX0,
    pio_1_rx_1 = DREQ_PIO1_RX1,
    pio_1_rx_2 = DREQ_PIO1_RX2,
    pio_1_rx_3 = DREQ_PIO1_RX3,
    spi_0_tx = DREQ_SPI0_TX,
    spi_0_rx = DREQ_SPI0_RX,
    spi_1_tx = DREQ_SPI1_TX,
    spi_1_rx = DREQ_SPI1_RX,
    uart_0_tx = DREQ_UART0_TX,
    uart_0_rx = DREQ_UART0_RX,
    uart_1_tx = DREQ_UART1_TX,
    uart_1_rx = DREQ_UART1_RX,
    pwm_wrap_0 = DREQ_PWM_WRAP0,
    pwm_wrap_1 = DREQ_PWM_WRAP1,
    pwm_wrap_2 = DREQ_PWM_WRAP2,
    pwm_wrap_3 = DREQ_PWM_WRAP3,
    pwm_wrap_4 = DREQ_PWM_WRAP4,
    pwm_wrap_5 = DREQ_PWM_WRAP5,
    pwm_wrap_6 = DREQ_PWM_WRAP6,
    pwm_wrap_7 = DREQ_PWM_WRAP7,
    i2c_0_tx = DREQ_I2C0_TX,
    i2c_0_rx = DREQ_I2C0_RX,
    i2c_1_tx = DREQ_I2C1_TX,
    i2c_1_rx = DREQ_I2C1_RX,
    acd = DREQ_ADC,
    xip_stream = DREQ_XIP_STREAM,
    xip_ssitx = DREQ_XIP_SSITX,
    xip_ssirx = DREQ_XIP_SSIRX,
    dma_timer_0 = DREQ_DMA_TIMER0,
    dma_timer_1 = DREQ_DMA_TIMER1,
    dma_timer_2 = DREQ_DMA_TIMER2,
    dma_timer_3 = DREQ_DMA_TIMER3,
    force = DREQ_FORCE
  };

  /**
   * @brief Constants for the number of bits that will be transferred at a time via DMA.
   */
  enum class TransferSizes {
    s_8_bit = DMA_SIZE_8,    ///< Byte transfer (8 bits)
    s_16_bit = DMA_SIZE_16,  ///< Half word transfer (16 bits)
    s_32_bit = DMA_SIZE_32   ///< Word transfer (32 bits)
  };

  enum class States : bool {
    disabled = false,
    enabled = true
  };

  /**
   * @brief Describes a DMA channel and allows interacting with it.
   */
  struct DMA {
    enum class DMAError : uint8_t {
      success = 0,
      no_valid_channel,
    };

    int32_t channel_ = -1;
    dma_channel_config config_{}; /**< The actual DMA pin being modified by the methods */

    auto Claim(int32_t channel = -1) -> bool {
      if (channel == -1) [[likely]] {  // Default parameter, let the hardware pick a channel
        this->channel_ = dma_claim_unused_channel(false);
      } else [[unlikely]] {            // A specific channel is requested
        if (!dma_channel_is_claimed(channel)) [[likely]] {
          dma_channel_claim(channel);
          this->channel_ = channel;
        }
      }
      return this->channel_ != -1;
    }

    auto UnClaim() const -> void {
      dma_channel_unclaim(this->channel_);
    }

    auto Init() -> DMA& {
      dma_channel_get_default_config(this->channel_);
      return *this;
    }

    auto Reset() -> DMA& {
      dma_channel_get_default_config(this->channel_);
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto WriteIncrement(States state) -> DMA& {
      channel_config_set_write_increment(&this->config_, std::to_underlying(state));
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto ReadIncrement(States state) -> DMA& {
      channel_config_set_read_increment(&this->config_, std::to_underlying(state));
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto Trigger(Triggers trigger) -> DMA& {
      channel_config_set_dreq(&this->config_, std::to_underlying(trigger));
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto ForceTrigger(Triggers trigger) -> DMA& {
      dma_channel_start(this->channel_);
      return *this;
    }

    auto TransferSize(TransferSizes size) -> DMA& {
      channel_config_set_transfer_data_size(&this->config_, static_cast<enum dma_channel_transfer_size>(size));
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto HighPriority(States state) -> DMA& {
      channel_config_set_high_priority(&this->config_, std::to_underlying(state));
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto Enable(States state) -> DMA& {
      channel_config_set_enable(&this->config_, std::to_underlying(state));
      dma_channel_set_config(this->channel_, &this->config_, false);
      return *this;
    }

    auto WriteAddress(volatile void* address) -> DMA& {
      dma_channel_set_write_addr(this->channel_, address, false);
      return *this;
    }

    auto ReadAddress(volatile void* address) -> DMA& {
      dma_channel_set_read_addr(this->channel_, address, false);
      return *this;
    }

    auto CancelTransfer() -> DMA& {
      dma_channel_abort(this->channel_);
      return *this;
    }

    /**
     * @brief Defines how many transfers will take place each time the DMA is triggered.
     * @param count The number of transfers, NOT BYTES, which will be performed.
     * @returns Reference to itself for chaining.
     */
    auto TransferCount(uint32_t count) -> DMA& {
      dma_channel_set_trans_count(this->channel_, count, false);
      return *this;
    }

    [[nodiscard]] auto IsBusy() const -> bool {
      return dma_channel_is_busy(this->channel_);
    }

    auto BlockUntilFinished() const -> void {
      dma_channel_wait_for_finish_blocking(this->channel_);
    }
  };
}  // namespace rpp::dma

#endif  //EE490_FIRMWARE_DMA_HPP
