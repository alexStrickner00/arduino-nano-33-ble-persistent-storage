
#include "FlashIAPBlockDevice.h"
#include "inttypes.h"

// --------------------------------
// persistent storage flash reservation and setup
// see:
// https://github.com/petewarden/arduino_nano_ble_write_flash/blob/main/arduino_nano_ble_write_flash.ino
// --------------------------------

// the BLEDevice address() method returns a String in the format "XX:XX:XX:XX:XX:XX"
#define BT_ADDRESS_LENGTH 17

/**
 * defines a struct that is stored in the flash
 */
typedef struct persistent_storage {
	/**
	 * the address of the access point that the device is paired with
	 * Length of address + 1 for the null terminator
	 */
	char accesspoint_address[BT_ADDRESS_LENGTH + 1];
} persistent_storage_t;

// the flash block size is 4096 bytes, according to git repo above
constexpr int kFlashBlockSize = 4096;

// round up to the next multiple of block_size
#define ROUND_UP(val, block_size) ((((val) + ((block_size)-1)) / (block_size)) * (block_size))

// the size of the buffer that is used to store the persistent storage struct
constexpr int kFlashBufferSize = ROUND_UP(sizeof(persistent_storage_t), kFlashBlockSize);

// define a const data block that reserves the flash memory
alignas(kFlashBlockSize) const uint8_t flash_buffer[kFlashBufferSize] = {};

// get the address of the flash buffer as an uint32_t for the FlashIAPBlockDevice
const uint32_t flash_buffer_address = reinterpret_cast<uint32_t>(flash_buffer);

// define a ram buffer that is used load the data from the flash
uint8_t ram_buffer[kFlashBufferSize];

// define a pointer to the ram buffer casted to a persistent_storage_t for use in code
persistent_storage_t* storage = reinterpret_cast<persistent_storage_t*>(ram_buffer);

// define a FlashIAPBlockDevice that is used to read to the ram buffer and write that back to the
// flash
static FlashIAPBlockDevice bd(flash_buffer_address, kFlashBufferSize);

/**
 * reads the persistent storage from the flash to the ram buffer
 */
void read_persistent_storage() {
	bd.init();
	bd.read((void*)(&ram_buffer), 0, kFlashBufferSize);
	bd.deinit();
}

/**
 * writes the persistent storage from the ram buffer to the flash
 */
void write_persistent_storage() {
	bd.init();
	// flash block(s) must be erased before writing to them
	bd.erase(0, kFlashBufferSize);
	bd.program((void*)(&ram_buffer), 0, kFlashBufferSize);
	bd.deinit();
}

// ------------------------------
// end of persistent storage code
// ------------------------------
