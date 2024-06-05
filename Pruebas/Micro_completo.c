/**
 * @file main.c
 * @brief Ejemplo de medición de ruido usando DMA y ADC en Raspberry Pi Pico W
 * 
 * Este programa mide el nivel de ruido usando un micrófono analógico conectado a la Raspberry Pi Pico W.
 * Utiliza DMA para transferir los datos del ADC y PWM para generar interrupciones periódicas.
 * Los valores de ruido se imprimen en la consola.
 * 
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"
#include "hardware/resets.h"

#define ADC_DMA_CH 5    ///< Canal DMA utilizado para ADC
#define ADC_CH     0    ///< Canal ADC utilizado para el micrófono (GPIO26)
#define ADC_CONVERT (3.3 / ((1<<12)-1)) ///< Conversión de ADC a voltaje

int16_t buffer[100];  ///< Buffer para almacenar los datos del ADC

volatile bool gFlagDMA = false; ///< Indicador de finalización del DMA
float noise = 0; ///< Variable para almacenar el nivel de ruido

/**
 * @brief Interrupción del DMA
 * 
 * Se llama cuando el DMA completa la transferencia de datos del ADC.
 * Calcula el voltaje promedio del micrófono y lo convierte a decibelios.
 */
void isrDMA_IRQ0(void) {
    dma_irqn_acknowledge_channel(0, ADC_DMA_CH);
    adc_run(false);
    uint16_t cnt = 0;
    noise = 0;
    for (int i = 0; i < 100; i++) {
        if (!(buffer[i] & (1 << 15))) {
            cnt++;
            noise += (float)buffer[i];
        }
    }
    noise /= cnt;
    noise *= ADC_CONVERT;
    float avg_noise_db = 20 * log10(noise / 0.00226);
    printf("AvgVolt: %f\tAvgNoisedB: %f\tErrors: %d\n", noise, avg_noise_db, 100 - cnt);
    gFlagDMA = true;
    pwm_set_enabled(0, false);
}

/**
 * @brief Interrupción del PWM
 * 
 * Se llama cuando el PWM genera una interrupción.
 * Reactiva el DMA para una nueva transferencia si no está ocupado.
 */
void pwmIRQ(void) {
    if (pwm_get_irq_status_mask() & 0x01UL) {
        pwm_clear_irq(0);
        if (!dma_channel_is_busy(ADC_DMA_CH)) {
            dma_hw->ch[ADC_DMA_CH].al2_transfer_count = 100;
            dma_hw->ch[ADC_DMA_CH].al2_write_addr_trig = (uint32_t)buffer;
            adc_fifo_drain();
            adc_run(true);
        } else {
            printf("Channel Busy %d\n", adc_fifo_get_level());
        }
    } else {
        printf("pwmIRQ multiple interruptions...\n");
    }
}

/**
 * @brief Inicialización del DMA para el ADC
 * 
 * Configura el canal DMA para transferir datos del ADC a un buffer.
 * 
 * @param channel Canal DMA a utilizar
 */
void initDMAxADC(uint8_t channel) {
    dma_channel_config myDMAADCCH;
    if (!dma_channel_is_claimed(channel)) {
        dma_channel_claim(channel);
        myDMAADCCH = dma_channel_get_default_config(channel);
        channel_config_set_read_increment(&myDMAADCCH, false);
        channel_config_set_write_increment(&myDMAADCCH, true);
        channel_config_set_dreq(&myDMAADCCH, DREQ_ADC);
        channel_config_set_transfer_data_size(&myDMAADCCH, DMA_SIZE_16);
        channel_config_set_ring(&myDMAADCCH, false, 0);
        channel_config_set_bswap(&myDMAADCCH, false);
        channel_config_set_irq_quiet(&myDMAADCCH, false);
        channel_config_set_high_priority(&myDMAADCCH, false);
        channel_config_set_enable(&myDMAADCCH, true);
        dma_channel_configure(channel, &myDMAADCCH, buffer, (const volatile void *)(ADC_BASE + ADC_FIFO_OFFSET), 100, false);
        dma_channel_set_irq0_enabled(channel, true);
        irq_set_exclusive_handler(DMA_IRQ_0, isrDMA_IRQ0);
        irq_set_enabled(DMA_IRQ_0, true);
        irq_set_priority(DMA_IRQ_0, 0);
        printf("Channel %d successfully initialized!\n", channel);
    } else {
        printf("Channel %d not available!\n", channel);
        while (1);
    }
}

/**
 * @brief Inicialización del ADC para el micrófono
 * 
 * Configura el ADC para leer del micrófono a la tasa de muestreo especificada.
 * 
 * @param fsample Frecuencia de muestreo en Hz
 */
void initADCxMIC_DMA(uint32_t fsample) {
    adc_init();
    adc_gpio_init(26 + ADC_CH);
    adc_select_input(ADC_CH);
    uint32_t divINT = 48000000UL / fsample;
    adc_set_clkdiv(divINT);
    adc_fifo_setup(true, true, 1, true, false);
    adc_fifo_drain();
}

/**
 * @brief Inicialización del PWM como temporizador
 * 
 * Configura el PWM para generar interrupciones periódicas.
 * 
 * @param slice Slice del PWM a utilizar
 * @param milis Periodo de la interrupción en milisegundos
 * @param enable Habilitar el PWM
 */
void initPWMasPIT(uint8_t slice, uint16_t milis, bool enable) {
    assert(milis <= 262);
    float prescaler = (float)SYS_CLK_KHZ / 500;
    assert(prescaler < 256);
    uint32_t wrap = 500000 * milis / 2000;
    assert(wrap < ((1UL << 16) - 1));
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_phase_correct(&cfg, true);
    pwm_config_set_clkdiv(&cfg, prescaler);
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_FREE_RUNNING);
    pwm_config_set_wrap(&cfg, wrap);
    pwm_set_irq_enabled(slice, true);
    irq_set_enabled(PWM_IRQ_WRAP, true);
    irq_set_priority(PWM_IRQ_WRAP, 255);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwmIRQ);
    pwm_init(slice, &cfg, enable);
}

/**
 * @brief Función principal
 * 
 * Inicializa el sistema, configura el DMA, el ADC y el PWM, y entra en un bucle infinito
 * para procesar los datos del micrófono.
 * 
 * @return int Código de retorno
 */
int main() {
    stdio_init_all();
    sleep_ms(5000);
    printf("Esto es una prueba del DMA\n\n");
    initDMAxADC(ADC_DMA_CH);
    initADCxMIC_DMA(100000);
    initPWMasPIT(0, 250, true);

    while (true) {
        while (gFlagDMA) {
            gFlagDMA = false;
        }
        __wfi();
    }
}
