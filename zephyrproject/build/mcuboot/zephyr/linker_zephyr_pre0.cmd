       
MEMORY
{
  iram_loader_seg (RWX) : org = 0x40078000,
                          len = 0x4000
  iram_seg (RWX) : org = 0x400a0000,
                          len = 0xa000
  dram_seg (RW) : org = 0x3ffe8000,
                          len = 0x7a00
  IDT_LIST(RW): org = 0x3ebfe010, len = 0x2000
}
ENTRY("__start")
SECTIONS
{
  .iram0.loader_text :
  {
    _loader_text_start = ABSOLUTE(.);
    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *libarch__xtensa__core.a:xtensa_asm2_util.*(.literal .text .literal.* .text.*)
    *libarch__common.a:dynamic_isr.*(.literal .text .literal.* .text.*)
    *libarch__common.a:sw_isr_common.*(.literal .text .literal.* .text.*)
    *libapp.a:flash_map_extended.*(.literal .text .literal.* .text.*)
    *libzephyr.a:cbprintf_nano.*(.literal .text .literal.* .text.*)
    *libzephyr.a:cpu.*(.literal .text .literal.* .text.*)
    *libzephyr.a:mmu_hal.*(.literal .text .literal.* .text.*)
    *libzephyr.a:flash_map.*(.literal .text .literal.* .text.*)
    *libzephyr.a:esp_rom_spiflash.*(.literal .text .literal.* .text.*)
    *libkernel.a:device.*(.literal .text .literal.* .text.*)
    *libzephyr.a:esp_loader.*(.literal .text .literal.* .text.*)
    *(.literal.esp_intr_disable .literal.esp_intr_disable.* .text.esp_intr_disable .text.esp_intr_disable.*)
    *(.literal.default_intr_handler .text.default_intr_handler .iram1.*.default_intr_handler)
    *(.literal.esp_log_timestamp .text.esp_log_timestamp)
    *(.literal.esp_log_early_timestamp .text.esp_log_early_timestamp)
    *(.literal.esp_system_abort .text.esp_system_abort)
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    . += 16;
    _text_end = ABSOLUTE(.);
    _etext = .;
    . = ALIGN(4) + 16;
    _loader_text_end = ABSOLUTE(.);
    _iram_text_end = ABSOLUTE(.);
    _iram_end = ABSOLUTE(.);
  } > iram_loader_seg
  .iram0.vectors : ALIGN(4)
  {
    _init_start = ABSOLUTE(.);
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    _invalid_pc_placeholder = ABSOLUTE(.);
    *(.*Vector.literal)
    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    . = ALIGN (4);
    _init_end = ABSOLUTE(.);
    _iram_start = ABSOLUTE(.);
  } > iram_seg
  .iram0.text :
  {
    . = ALIGN(4);
    *(.iram1 .iram1.*)
    *(.iram0.literal .iram.literal .iram.text.literal .iram0.text .iram.text)
    *(.literal .text .literal.* .text.*)
    . = ALIGN(4);
    *(.irom0.text)
    . = ALIGN(4) + 16;
  } > iram_seg
  .dram0.data : ALIGN(16)
  {
    . = ALIGN(4);
    __data_start = ABSOLUTE(.);
    . = ALIGN(4);
    . = ALIGN(4);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *libzephyr.a:mmu_hal.*(.rodata .rodata.*)
    *libzephyr.a:rtc_clk.*(.rodata .rodata.*)
    KEEP(*(.jcr))
    *(.dram1 .dram1.*)
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
    *(.irom1.text)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    . = ALIGN(4);
    __rodata_region_end = ABSOLUTE(.);
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    *(.rodata_wlog)
    *(.rodata_wlog*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > dram_seg
 initlevel :
 {
  __init_start = .;
  __init_EARLY_start = .; KEEP(*(SORT(.z_init_EARLY_P_?_*))); KEEP(*(SORT(.z_init_EARLY_P_??_*))); KEEP(*(SORT(.z_init_EARLY_P_???_*)));
  __init_PRE_KERNEL_1_start = .; KEEP(*(SORT(.z_init_PRE_KERNEL_1_P_?_*))); KEEP(*(SORT(.z_init_PRE_KERNEL_1_P_??_*))); KEEP(*(SORT(.z_init_PRE_KERNEL_1_P_???_*)));
  __init_PRE_KERNEL_2_start = .; KEEP(*(SORT(.z_init_PRE_KERNEL_2_P_?_*))); KEEP(*(SORT(.z_init_PRE_KERNEL_2_P_??_*))); KEEP(*(SORT(.z_init_PRE_KERNEL_2_P_???_*)));
  __init_POST_KERNEL_start = .; KEEP(*(SORT(.z_init_POST_KERNEL_P_?_*))); KEEP(*(SORT(.z_init_POST_KERNEL_P_??_*))); KEEP(*(SORT(.z_init_POST_KERNEL_P_???_*)));
  __init_APPLICATION_start = .; KEEP(*(SORT(.z_init_APPLICATION_P_?_*))); KEEP(*(SORT(.z_init_APPLICATION_P_??_*))); KEEP(*(SORT(.z_init_APPLICATION_P_???_*)));
  __init_SMP_start = .; KEEP(*(SORT(.z_init_SMP_P_?_*))); KEEP(*(SORT(.z_init_SMP_P_??_*))); KEEP(*(SORT(.z_init_SMP_P_???_*)));
  __init_end = .;
 } > dram_seg
 device_area : { _device_list_start = .; KEEP(*(SORT(._device.static.*_?_*))); KEEP(*(SORT(._device.static.*_??_*))); KEEP(*(SORT(._device.static.*_???_*))); KEEP(*(SORT(._device.static.*_????_*))); KEEP(*(SORT(._device.static.*_?????_*))); _device_list_end = .;; } > dram_seg
 initlevel_error :
 {
  KEEP(*(SORT(.z_init_*)))
 }
 ASSERT(SIZEOF(initlevel_error) == 0, "Undefined initialization levels used.")
 app_shmem_regions :
 {
  __app_shmem_regions_start = .;
  KEEP(*(SORT(.app_regions.*)));
  __app_shmem_regions_end = .;
 } > dram_seg
 k_p4wq_initparam_area : { _k_p4wq_initparam_list_start = .; KEEP(*(SORT_BY_NAME(._k_p4wq_initparam.static.*))); _k_p4wq_initparam_list_end = .;; } > dram_seg
 _static_thread_data_area : { __static_thread_data_list_start = .; KEEP(*(SORT_BY_NAME(.__static_thread_data.static.*))); __static_thread_data_list_end = .;; } > dram_seg
 device_deps :
 {
__device_deps_start = .;
KEEP(*(SORT(.__device_deps_pass2*)));
__device_deps_end = .;
 } > dram_seg
flash_driver_api_area : { _flash_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._flash_driver_api.static.*))); _flash_driver_api_list_end = .;; } > dram_seg
gpio_driver_api_area : { _gpio_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._gpio_driver_api.static.*))); _gpio_driver_api_list_end = .;; } > dram_seg
shared_irq_driver_api_area : { _shared_irq_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._shared_irq_driver_api.static.*))); _shared_irq_driver_api_list_end = .;; } > dram_seg
crypto_driver_api_area : { _crypto_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._crypto_driver_api.static.*))); _crypto_driver_api_list_end = .;; } > dram_seg
adc_driver_api_area : { _adc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._adc_driver_api.static.*))); _adc_driver_api_list_end = .;; } > dram_seg
auxdisplay_driver_api_area : { _auxdisplay_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._auxdisplay_driver_api.static.*))); _auxdisplay_driver_api_list_end = .;; } > dram_seg
bbram_driver_api_area : { _bbram_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._bbram_driver_api.static.*))); _bbram_driver_api_list_end = .;; } > dram_seg
bt_hci_driver_api_area : { _bt_hci_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._bt_hci_driver_api.static.*))); _bt_hci_driver_api_list_end = .;; } > dram_seg
can_driver_api_area : { _can_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._can_driver_api.static.*))); _can_driver_api_list_end = .;; } > dram_seg
cellular_driver_api_area : { _cellular_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._cellular_driver_api.static.*))); _cellular_driver_api_list_end = .;; } > dram_seg
charger_driver_api_area : { _charger_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._charger_driver_api.static.*))); _charger_driver_api_list_end = .;; } > dram_seg
clock_control_driver_api_area : { _clock_control_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._clock_control_driver_api.static.*))); _clock_control_driver_api_list_end = .;; } > dram_seg
comparator_driver_api_area : { _comparator_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._comparator_driver_api.static.*))); _comparator_driver_api_list_end = .;; } > dram_seg
coredump_driver_api_area : { _coredump_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._coredump_driver_api.static.*))); _coredump_driver_api_list_end = .;; } > dram_seg
counter_driver_api_area : { _counter_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._counter_driver_api.static.*))); _counter_driver_api_list_end = .;; } > dram_seg
crc_driver_api_area : { _crc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._crc_driver_api.static.*))); _crc_driver_api_list_end = .;; } > dram_seg
dac_driver_api_area : { _dac_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._dac_driver_api.static.*))); _dac_driver_api_list_end = .;; } > dram_seg
dai_driver_api_area : { _dai_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._dai_driver_api.static.*))); _dai_driver_api_list_end = .;; } > dram_seg
display_driver_api_area : { _display_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._display_driver_api.static.*))); _display_driver_api_list_end = .;; } > dram_seg
dma_driver_api_area : { _dma_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._dma_driver_api.static.*))); _dma_driver_api_list_end = .;; } > dram_seg
edac_driver_api_area : { _edac_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._edac_driver_api.static.*))); _edac_driver_api_list_end = .;; } > dram_seg
eeprom_driver_api_area : { _eeprom_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._eeprom_driver_api.static.*))); _eeprom_driver_api_list_end = .;; } > dram_seg
emul_bbram_driver_api_area : { _emul_bbram_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._emul_bbram_driver_api.static.*))); _emul_bbram_driver_api_list_end = .;; } > dram_seg
fuel_gauge_emul_driver_api_area : { _fuel_gauge_emul_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._fuel_gauge_emul_driver_api.static.*))); _fuel_gauge_emul_driver_api_list_end = .;; } > dram_seg
emul_sensor_driver_api_area : { _emul_sensor_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._emul_sensor_driver_api.static.*))); _emul_sensor_driver_api_list_end = .;; } > dram_seg
entropy_driver_api_area : { _entropy_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._entropy_driver_api.static.*))); _entropy_driver_api_list_end = .;; } > dram_seg
espi_driver_api_area : { _espi_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._espi_driver_api.static.*))); _espi_driver_api_list_end = .;; } > dram_seg
espi_saf_driver_api_area : { _espi_saf_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._espi_saf_driver_api.static.*))); _espi_saf_driver_api_list_end = .;; } > dram_seg
fpga_driver_api_area : { _fpga_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._fpga_driver_api.static.*))); _fpga_driver_api_list_end = .;; } > dram_seg
fuel_gauge_driver_api_area : { _fuel_gauge_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._fuel_gauge_driver_api.static.*))); _fuel_gauge_driver_api_list_end = .;; } > dram_seg
gnss_driver_api_area : { _gnss_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._gnss_driver_api.static.*))); _gnss_driver_api_list_end = .;; } > dram_seg
haptics_driver_api_area : { _haptics_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._haptics_driver_api.static.*))); _haptics_driver_api_list_end = .;; } > dram_seg
hwspinlock_driver_api_area : { _hwspinlock_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._hwspinlock_driver_api.static.*))); _hwspinlock_driver_api_list_end = .;; } > dram_seg
i2c_driver_api_area : { _i2c_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._i2c_driver_api.static.*))); _i2c_driver_api_list_end = .;; } > dram_seg
i2c_target_driver_api_area : { _i2c_target_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._i2c_target_driver_api.static.*))); _i2c_target_driver_api_list_end = .;; } > dram_seg
i2s_driver_api_area : { _i2s_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._i2s_driver_api.static.*))); _i2s_driver_api_list_end = .;; } > dram_seg
i3c_driver_api_area : { _i3c_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._i3c_driver_api.static.*))); _i3c_driver_api_list_end = .;; } > dram_seg
ipm_driver_api_area : { _ipm_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._ipm_driver_api.static.*))); _ipm_driver_api_list_end = .;; } > dram_seg
led_driver_api_area : { _led_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._led_driver_api.static.*))); _led_driver_api_list_end = .;; } > dram_seg
led_strip_driver_api_area : { _led_strip_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._led_strip_driver_api.static.*))); _led_strip_driver_api_list_end = .;; } > dram_seg
lora_driver_api_area : { _lora_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._lora_driver_api.static.*))); _lora_driver_api_list_end = .;; } > dram_seg
mbox_driver_api_area : { _mbox_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._mbox_driver_api.static.*))); _mbox_driver_api_list_end = .;; } > dram_seg
mdio_driver_api_area : { _mdio_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._mdio_driver_api.static.*))); _mdio_driver_api_list_end = .;; } > dram_seg
mipi_dbi_driver_api_area : { _mipi_dbi_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._mipi_dbi_driver_api.static.*))); _mipi_dbi_driver_api_list_end = .;; } > dram_seg
mipi_dsi_driver_api_area : { _mipi_dsi_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._mipi_dsi_driver_api.static.*))); _mipi_dsi_driver_api_list_end = .;; } > dram_seg
mspi_driver_api_area : { _mspi_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._mspi_driver_api.static.*))); _mspi_driver_api_list_end = .;; } > dram_seg
opamp_driver_api_area : { _opamp_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._opamp_driver_api.static.*))); _opamp_driver_api_list_end = .;; } > dram_seg
peci_driver_api_area : { _peci_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._peci_driver_api.static.*))); _peci_driver_api_list_end = .;; } > dram_seg
ps2_driver_api_area : { _ps2_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._ps2_driver_api.static.*))); _ps2_driver_api_list_end = .;; } > dram_seg
ptp_clock_driver_api_area : { _ptp_clock_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._ptp_clock_driver_api.static.*))); _ptp_clock_driver_api_list_end = .;; } > dram_seg
pwm_driver_api_area : { _pwm_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._pwm_driver_api.static.*))); _pwm_driver_api_list_end = .;; } > dram_seg
regulator_parent_driver_api_area : { _regulator_parent_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._regulator_parent_driver_api.static.*))); _regulator_parent_driver_api_list_end = .;; } > dram_seg
regulator_driver_api_area : { _regulator_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._regulator_driver_api.static.*))); _regulator_driver_api_list_end = .;; } > dram_seg
reset_driver_api_area : { _reset_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._reset_driver_api.static.*))); _reset_driver_api_list_end = .;; } > dram_seg
retained_mem_driver_api_area : { _retained_mem_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._retained_mem_driver_api.static.*))); _retained_mem_driver_api_list_end = .;; } > dram_seg
rtc_driver_api_area : { _rtc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._rtc_driver_api.static.*))); _rtc_driver_api_list_end = .;; } > dram_seg
sdhc_driver_api_area : { _sdhc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._sdhc_driver_api.static.*))); _sdhc_driver_api_list_end = .;; } > dram_seg
sensor_driver_api_area : { _sensor_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._sensor_driver_api.static.*))); _sensor_driver_api_list_end = .;; } > dram_seg
smbus_driver_api_area : { _smbus_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._smbus_driver_api.static.*))); _smbus_driver_api_list_end = .;; } > dram_seg
spi_driver_api_area : { _spi_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._spi_driver_api.static.*))); _spi_driver_api_list_end = .;; } > dram_seg
stepper_driver_api_area : { _stepper_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._stepper_driver_api.static.*))); _stepper_driver_api_list_end = .;; } > dram_seg
syscon_driver_api_area : { _syscon_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._syscon_driver_api.static.*))); _syscon_driver_api_list_end = .;; } > dram_seg
tee_driver_api_area : { _tee_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._tee_driver_api.static.*))); _tee_driver_api_list_end = .;; } > dram_seg
video_driver_api_area : { _video_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._video_driver_api.static.*))); _video_driver_api_list_end = .;; } > dram_seg
virtio_driver_api_area : { _virtio_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._virtio_driver_api.static.*))); _virtio_driver_api_list_end = .;; } > dram_seg
w1_driver_api_area : { _w1_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._w1_driver_api.static.*))); _w1_driver_api_list_end = .;; } > dram_seg
wdt_driver_api_area : { _wdt_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._wdt_driver_api.static.*))); _wdt_driver_api_list_end = .;; } > dram_seg
can_transceiver_driver_api_area : { _can_transceiver_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._can_transceiver_driver_api.static.*))); _can_transceiver_driver_api_list_end = .;; } > dram_seg
nrf_clock_control_driver_api_area : { _nrf_clock_control_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._nrf_clock_control_driver_api.static.*))); _nrf_clock_control_driver_api_list_end = .;; } > dram_seg
i3c_target_driver_api_area : { _i3c_target_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._i3c_target_driver_api.static.*))); _i3c_target_driver_api_list_end = .;; } > dram_seg
its_driver_api_area : { _its_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._its_driver_api.static.*))); _its_driver_api_list_end = .;; } > dram_seg
vtd_driver_api_area : { _vtd_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._vtd_driver_api.static.*))); _vtd_driver_api_list_end = .;; } > dram_seg
renesas_elc_driver_api_area : { _renesas_elc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._renesas_elc_driver_api.static.*))); _renesas_elc_driver_api_list_end = .;; } > dram_seg
tgpio_driver_api_area : { _tgpio_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._tgpio_driver_api.static.*))); _tgpio_driver_api_list_end = .;; } > dram_seg
pcie_ctrl_driver_api_area : { _pcie_ctrl_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._pcie_ctrl_driver_api.static.*))); _pcie_ctrl_driver_api_list_end = .;; } > dram_seg
pcie_ep_driver_api_area : { _pcie_ep_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._pcie_ep_driver_api.static.*))); _pcie_ep_driver_api_list_end = .;; } > dram_seg
psi5_driver_api_area : { _psi5_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._psi5_driver_api.static.*))); _psi5_driver_api_list_end = .;; } > dram_seg
sent_driver_api_area : { _sent_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._sent_driver_api.static.*))); _sent_driver_api_list_end = .;; } > dram_seg
svc_driver_api_area : { _svc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._svc_driver_api.static.*))); _svc_driver_api_list_end = .;; } > dram_seg
uart_driver_api_area : { _uart_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._uart_driver_api.static.*))); _uart_driver_api_list_end = .;; } > dram_seg
bc12_emul_driver_api_area : { _bc12_emul_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._bc12_emul_driver_api.static.*))); _bc12_emul_driver_api_list_end = .;; } > dram_seg
bc12_driver_api_area : { _bc12_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._bc12_driver_api.static.*))); _bc12_driver_api_list_end = .;; } > dram_seg
usbc_ppc_driver_api_area : { _usbc_ppc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._usbc_ppc_driver_api.static.*))); _usbc_ppc_driver_api_list_end = .;; } > dram_seg
tcpc_driver_api_area : { _tcpc_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._tcpc_driver_api.static.*))); _tcpc_driver_api_list_end = .;; } > dram_seg
usbc_vbus_driver_api_area : { _usbc_vbus_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._usbc_vbus_driver_api.static.*))); _usbc_vbus_driver_api_list_end = .;; } > dram_seg
ivshmem_driver_api_area : { _ivshmem_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._ivshmem_driver_api.static.*))); _ivshmem_driver_api_list_end = .;; } > dram_seg
ethphy_driver_api_area : { _ethphy_driver_api_list_start = .; KEEP(*(SORT_BY_NAME(._ethphy_driver_api.static.*))); _ethphy_driver_api_list_end = .;; } > dram_seg
 tracing_backend_area : { _tracing_backend_list_start = .; KEEP(*(SORT_BY_NAME(._tracing_backend.static.*))); _tracing_backend_list_end = .;; } > dram_seg
 zephyr_dbg_info :
 {
  KEEP(*(".dbg_thread_info"));
 } > dram_seg
 symbol_to_keep :
 {
  __symbol_to_keep_start = .;
  KEEP(*(SORT(.symbol_to_keep*)));
  __symbol_to_keep_end = .;
 } > dram_seg
 shell_area : { _shell_list_start = .; KEEP(*(SORT_BY_NAME(._shell.static.*))); _shell_list_end = .;; } > dram_seg
 shell_root_cmds_area : { _shell_root_cmds_list_start = .; KEEP(*(SORT_BY_NAME(._shell_root_cmds.static.*))); _shell_root_cmds_list_end = .;; } > dram_seg
 shell_subcmds_area : { _shell_subcmds_list_start = .; KEEP(*(SORT_BY_NAME(._shell_subcmds.static.*))); _shell_subcmds_list_end = .;; } > dram_seg
 shell_dynamic_subcmds_area : { _shell_dynamic_subcmds_list_start = .; KEEP(*(SORT_BY_NAME(._shell_dynamic_subcmds.static.*))); _shell_dynamic_subcmds_list_end = .;; } > dram_seg
 cfb_font_area : { _cfb_font_list_start = .; KEEP(*(SORT_BY_NAME(._cfb_font.static.*))); _cfb_font_list_end = .;; } > dram_seg
.intList :
{
 KEEP(*(.irq_info*))
 KEEP(*(.intList*))
} > IDT_LIST
 /DISCARD/ : { *(.eh_frame) }
 sw_isr_table :
 {
  . = ALIGN(4);
  *(.gnu.linkonce.sw_isr_table*)
 } > dram_seg
        device_states :
        {
  . = ALIGN(4);
                __device_states_start = .;
  KEEP(*(".z_devstate"));
  KEEP(*(".z_devstate.*"));
                __device_states_end = .;
  . = ALIGN(4);
        } > dram_seg
 log_mpsc_pbuf_area : { _log_mpsc_pbuf_list_start = .; *(SORT_BY_NAME(._log_mpsc_pbuf.static.*)); _log_mpsc_pbuf_list_end = .;; } > dram_seg
 log_msg_ptr_area : { _log_msg_ptr_list_start = .; KEEP(*(SORT_BY_NAME(._log_msg_ptr.static.*))); _log_msg_ptr_list_end = .;; } > dram_seg
 log_dynamic_area : { _log_dynamic_list_start = .; KEEP(*(SORT_BY_NAME(._log_dynamic.static.*))); _log_dynamic_list_end = .;; } > dram_seg
 k_timer_area : { _k_timer_list_start = .; *(SORT_BY_NAME(._k_timer.static.*)); _k_timer_list_end = .;; } > dram_seg
 k_mem_slab_area : { _k_mem_slab_list_start = .; *(SORT_BY_NAME(._k_mem_slab.static.*)); _k_mem_slab_list_end = .;; } > dram_seg
 k_heap_area : { _k_heap_list_start = .; *(SORT_BY_NAME(._k_heap.static.*)); _k_heap_list_end = .;; } > dram_seg
 k_mutex_area : { _k_mutex_list_start = .; *(SORT_BY_NAME(._k_mutex.static.*)); _k_mutex_list_end = .;; } > dram_seg
 k_stack_area : { _k_stack_list_start = .; *(SORT_BY_NAME(._k_stack.static.*)); _k_stack_list_end = .;; } > dram_seg
 k_msgq_area : { _k_msgq_list_start = .; *(SORT_BY_NAME(._k_msgq.static.*)); _k_msgq_list_end = .;; } > dram_seg
 k_mbox_area : { _k_mbox_list_start = .; *(SORT_BY_NAME(._k_mbox.static.*)); _k_mbox_list_end = .;; } > dram_seg
 k_pipe_area : { _k_pipe_list_start = .; *(SORT_BY_NAME(._k_pipe.static.*)); _k_pipe_list_end = .;; } > dram_seg
 k_sem_area : { _k_sem_list_start = .; *(SORT_BY_NAME(._k_sem.static.*)); _k_sem_list_end = .;; } > dram_seg
 k_event_area : { _k_event_list_start = .; *(SORT_BY_NAME(._k_event.static.*)); _k_event_list_end = .;; } > dram_seg
 k_queue_area : { _k_queue_list_start = .; *(SORT_BY_NAME(._k_queue.static.*)); _k_queue_list_end = .;; } > dram_seg
 k_fifo_area : { _k_fifo_list_start = .; *(SORT_BY_NAME(._k_fifo.static.*)); _k_fifo_list_end = .;; } > dram_seg
 k_lifo_area : { _k_lifo_list_start = .; *(SORT_BY_NAME(._k_lifo.static.*)); _k_lifo_list_end = .;; } > dram_seg
 k_condvar_area : { _k_condvar_list_start = .; *(SORT_BY_NAME(._k_condvar.static.*)); _k_condvar_list_end = .;; } > dram_seg
 sys_mem_blocks_ptr_area : { _sys_mem_blocks_ptr_list_start = .; *(SORT_BY_NAME(._sys_mem_blocks_ptr.static.*)); _sys_mem_blocks_ptr_list_end = .;; } > dram_seg
 net_buf_pool_area : { _net_buf_pool_list_start = .; KEEP(*(SORT_BY_NAME(._net_buf_pool.static.*))); _net_buf_pool_list_end = .;; } > dram_seg
 log_strings_area : { _log_strings_list_start = .; KEEP(*(SORT_BY_NAME(._log_strings.static.*))); _log_strings_list_end = .;; } > dram_seg
 log_stmesp_ptr_area : { _log_stmesp_ptr_list_start = .; KEEP(*(SORT_BY_NAME(._log_stmesp_ptr.static.*))); _log_stmesp_ptr_list_end = .;; } > dram_seg
 log_stmesp_str_area : { _log_stmesp_str_list_start = .; KEEP(*(SORT_BY_NAME(._log_stmesp_str.static.*))); _log_stmesp_str_list_end = .;; } > dram_seg
 log_const_area : { _log_const_list_start = .; KEEP(*(SORT_BY_NAME(._log_const.static.*))); _log_const_list_end = .;; } > dram_seg
 log_backend_area : { _log_backend_list_start = .; KEEP(*(SORT_BY_NAME(._log_backend.static.*))); _log_backend_list_end = .;; } > dram_seg
 log_link_area : { _log_link_list_start = .; KEEP(*(SORT_BY_NAME(._log_link.static.*))); _log_link_list_end = .;; } > dram_seg
  .noinit (NOLOAD):
  {
    . = ALIGN(8);
    *(.noinit)
    *(.noinit.*)
    . = ALIGN(8) ;
  } > dram_seg
  .bss (NOLOAD):
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    __bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.share.mem)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    __bss_end = ABSOLUTE(.);
    _bss_end = ABSOLUTE(.);
    _end = ABSOLUTE(.);
  } > dram_seg
  ASSERT(((__bss_end - ORIGIN(dram_seg)) <= LENGTH(dram_seg)), "DRAM segment data does not fit.")
 .stab 0 : { *(.stab) }
 .stabstr 0 : { *(.stabstr) }
 .stab.excl 0 : { *(.stab.excl) }
 .stab.exclstr 0 : { *(.stab.exclstr) }
 .stab.index 0 : { *(.stab.index) }
 .stab.indexstr 0 : { *(.stab.indexstr) }
 .gnu.build.attributes 0 : { *(.gnu.build.attributes .gnu.build.attributes.*) }
 .comment 0 : { *(.comment) }
 .debug 0 : { *(.debug) }
 .line 0 : { *(.line) }
 .debug_srcinfo 0 : { *(.debug_srcinfo) }
 .debug_sfnames 0 : { *(.debug_sfnames) }
 .debug_aranges 0 : { *(.debug_aranges) }
 .debug_pubnames 0 : { *(.debug_pubnames) }
 .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
 .debug_abbrev 0 : { *(.debug_abbrev) }
 .debug_line 0 : { *(.debug_line .debug_line.* .debug_line_end ) }
 .debug_frame 0 : { *(.debug_frame) }
 .debug_str 0 : { *(.debug_str) }
 .debug_loc 0 : { *(.debug_loc) }
 .debug_macinfo 0 : { *(.debug_macinfo) }
 .debug_weaknames 0 : { *(.debug_weaknames) }
 .debug_funcnames 0 : { *(.debug_funcnames) }
 .debug_typenames 0 : { *(.debug_typenames) }
 .debug_varnames 0 : { *(.debug_varnames) }
 .debug_pubtypes 0 : { *(.debug_pubtypes) }
 .debug_ranges 0 : { *(.debug_ranges) }
 .debug_addr 0 : { *(.debug_addr) }
 .debug_line_str 0 : { *(.debug_line_str) }
 .debug_loclists 0 : { *(.debug_loclists) }
 .debug_macro 0 : { *(.debug_macro) }
 .debug_names 0 : { *(.debug_names) }
 .debug_rnglists 0 : { *(.debug_rnglists) }
 .debug_str_offsets 0 : { *(.debug_str_offsets) }
 .debug_sup 0 : { *(.debug_sup) }
  .xtensa.info 0 : { *(.xtensa.info) }
  .xt.insn 0 :
  {
    KEEP (*(.xt.insn))
    KEEP (*(.gnu.linkonce.x.*))
  }
  .xt.prop 0 :
  {
    KEEP (*(.xt.prop))
    KEEP (*(.xt.prop.*))
    KEEP (*(.gnu.linkonce.prop.*))
  }
  .xt.lit 0 :
  {
    KEEP (*(.xt.lit))
    KEEP (*(.xt.lit.*))
    KEEP (*(.gnu.linkonce.p.*))
  }
  .xt.profile_range 0 :
  {
    KEEP (*(.xt.profile_range))
    KEEP (*(.gnu.linkonce.profile_range.*))
  }
  .xt.profile_ranges 0 :
  {
    KEEP (*(.xt.profile_ranges))
    KEEP (*(.gnu.linkonce.xt.profile_ranges.*))
  }
  .xt.profile_files 0 :
  {
    KEEP (*(.xt.profile_files))
    KEEP (*(.gnu.linkonce.xt.profile_files.*))
  }
.intList :
{
 KEEP(*(.irq_info*))
 KEEP(*(.intList*))
} > IDT_LIST
}
