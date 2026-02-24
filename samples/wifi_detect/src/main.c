#include <zephyr/kernel.h>
#include <zephyr/device.h>

int main(void) {
    k_sleep(K_MSEC(100));
    
    printk("\n\n=== Pico 2 WiFi Detection ===\n");
    printk("Board: %s\n", CONFIG_BOARD);
    
#ifdef CONFIG_WIFI
    printk("WiFi config: ENABLED\n");
#else
    printk("WiFi config: DISABLED\n");
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(infineon_airoc_wifi)
    printk("AIROC WiFi DT node: FOUND\n");
    printk("\n*** RESULT: Raspberry Pi Pico 2 W ***\n\n");
#else
    printk("AIROC WiFi DT node: NOT FOUND\n");
    printk("\n*** RESULT: Raspberry Pi Pico 2 (standard) ***\n\n");
#endif
    
    while (1) {
        k_sleep(K_SECONDS(5));
        printk(".\n");
    }
    
    return 0;
}
