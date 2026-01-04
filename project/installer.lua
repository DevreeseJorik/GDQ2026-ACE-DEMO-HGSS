local giftDataOffset = 0x9E4C
local boxDataOffset = 0xF710

local basePointer = mainmemory.read_u32_le(0x021D2228 - 0x2000000)
local giftDataPointer = basePointer + giftDataOffset
local boxDataPointer = basePointer + boxDataOffset
local partyDataPointer = basePointer + 0xA8

local config = {
    giftDataFile = "output/payloads/bins/initial_payload.bin",
    boxDataFile = "output/packages/bins/packed.bin",
    unpackedFile = "output/packages/bins/unpacked.bin",
    bootstrapMonFile = "bootstrap_pokemon.bin"
}

local function writeBinToMemory(filePath, memoryAddress)
    local file = io.open(filePath, "rb")
    if not file then
        error("Could not open file: " .. filePath)
    end

    local data = file:read("*all")
    file:close()

    mainmemory.write_bytes_as_array(memoryAddress - 0x2000000, {string.byte(data, 1, #data)})
    print(string.format("Wrote %d bytes to 0x%X from %s", #data, memoryAddress, filePath))
end

local function writeGiftHeader(memoryAddress)
    local giftHeader = {
        0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x9E, 0x10, 0x02,
        0x00, 0x00, 0x07, 0x00, 0x19, 0x82, 0x04, 0x02, 0x47, 0x03, 0x03, 0x00, 0x00
    }
    mainmemory.write_bytes_as_array(memoryAddress - 0x2000000, giftHeader)
end

local function forceBoxSave() 
    local boxSaveBitsPointer = 0x02028c8e
    mainmemory.write_u8(boxSaveBitsPointer - 0x2000000, 0xFF)
end

local function resetFlags()
    local momDialogue = basePointer + 0x1000;
    mainmemory.write_u8(momDialogue - 0x2000000, 0x3)
end

local function install()
    print(string.format("Base Pointer: 0x%X", basePointer))

    writeGiftHeader(giftDataPointer)
    writeBinToMemory(config.giftDataFile, giftDataPointer + 0x18)
    writeBinToMemory(config.boxDataFile, boxDataPointer)
    writeBinToMemory(config.unpackedFile, 0x23C4000)
    writeBinToMemory(config.bootstrapMonFile, partyDataPointer + 0x4 * 0xEC)
    resetFlags()
    forceBoxSave()
end

print("Starting installation...")
install()
print("Installation complete.")