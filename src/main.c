/*
 * Joypad - Modular controller firmware for RP2040-based devices
 * MODIFIED FOR WAVESHARE RP2350 FORCE PIN 28
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/flash.h"

// --- AJOUT POUR FORCER L'USB ---
#include "pio_usb.h" 
// Si le build échoue à cause de cette ligne, retire-la et déclare la fonction manuellement :
// void pio_usb_host_add_port(uint8_t pin, uint8_t pin_dm);
// -------------------------------

#include "core/input_interface.h"
#include "core/output_interface.h"
#include "core/services/players/manager.h"
#include "core/services/leds/leds.h"
#include "core/services/storage/storage.h"

// App layer (linked per-product)
extern void app_init(void);
extern void app_task(void);
extern const OutputInterface** app_get_output_interfaces(uint8_t* count);
extern const InputInterface** app_get_input_interfaces(uint8_t* count);

// Cached interfaces (set once at startup)
static const OutputInterface** outputs = NULL;
static uint8_t output_count = 0;
static const InputInterface** inputs = NULL;
static uint8_t input_count = 0;

// Active/primary output interface (accessible from other modules)
const OutputInterface* active_output = NULL;

// Store core1 task for wrapper - can be set after Core 1 launch
static volatile void (*core1_actual_task)(void) = NULL;
static volatile bool core1_task_ready = false;

// Core 1 wrapper - initializes flash safety, then waits for and runs actual task
static void core1_wrapper(void) {
  flash_safe_execute_core_init();
  while (!core1_task_ready) {
    __wfe(); 
  }
  if (core1_actual_task) {
    core1_actual_task();
  } else {
    while (1) {
      __wfi(); 
    }
  }
}

// Core 0 main loop
static void __not_in_flash_func(core0_main)(void)
{
  printf("[joypad] Entering main loop\n");
  static bool first_loop = true;
  while (1)
  {
    if (first_loop) printf("[joypad] Loop: leds\n");
    leds_task();
    if (first_loop) printf("[joypad] Loop: players\n");
    players_task();
    if (first_loop) printf("[joypad] Loop: storage\n");
    storage_task();
    
    for (uint8_t i = 0; i < output_count; i++) {
      if (outputs[i] && outputs[i]->task) {
        if (first_loop) printf("[joypad] Loop: output %s\n", outputs[i]->name);
        outputs[i]->task();
      }
    }

    if (first_loop) printf("[joypad] Loop: app\n");
    app_task();

    for (uint8_t i = 0; i < input_count; i++) {
      if (inputs[i] && inputs[i]->task) {
        if (first_loop) printf("[joypad] Loop: input %s\n", inputs[i]->name);
        inputs[i]->task();
      }
    }
    first_loop = false;
  }
}

int main(void)
{
  stdio_init_all();

  printf("\n[joypad] Starting...\n");

  // --- MODIFICATION FORCE BRUTE ---
  // On tente d'activer le port USB manuellement sur 28/29
  // C'est la méthode de la dernière chance.
  printf("[joypad] FORCING PIO USB ON PIN 28\n");
  // Le driver PIO USB utilise généralement ppc_usb_config, mais on tente l'init directe
  // Si cette fonction n'est pas trouvée au build, c'est que la librairie n'est pas linkée ici.
  // Note: PIO_USB_PINOUT_DPDM est souvent implicite.
  pio_usb_host_add_port(28, 29); 
  // --------------------------------

  sleep_ms(250); 

  printf("[joypad] Launching core1 for flash safety...\n");
  multicore_launch_core1(core1_wrapper);
  sleep_ms(10); 

  leds_init();
  storage_init();
  players_init();
  app_init();

  inputs = app_get_input_interfaces(&input_count);
  for (uint8_t i = 0; i < input_count; i++) {
    if (inputs[i] && inputs[i]->init) {
      printf("[joypad] Initializing input: %s\n", inputs[i]->name);
      inputs[i]->init();
    }
  }

  outputs = app_get_output_interfaces(&output_count);
  if (output_count > 0 && outputs[0]) {
    active_output = outputs[0]; 
  }
  for (uint8_t i = 0; i < output_count; i++) {
    if (outputs[i] && outputs[i]->init) {
      printf("[joypad] Initializing output: %s\n", outputs[i]->name);
      outputs[i]->init();
    }
  }

  for (uint8_t i = 0; i < output_count; i++) {
    if (outputs[i] && outputs[i]->core1_task) {
      printf("[joypad] Core1 task from: %s\n", outputs[i]->name);
      core1_actual_task = outputs[i]->core1_task;
      break; 
    }
  }

  printf("[joypad] Signaling core1 (task: %s)\n",
         core1_actual_task ? "yes" : "idle");
  core1_task_ready = true;
  __sev(); 

  core0_main();

  return 0;
}
