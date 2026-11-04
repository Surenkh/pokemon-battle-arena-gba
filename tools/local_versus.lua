-- One-window keyboard input for Pokemon Battle Arena Local Versus.
-- Requires mGBA 0.11 development scripting (the global `input` object).
-- P1: arrows, J melee/ready, K fire/back, U block, I super, Enter pause.
-- P2: W/A/S/D, F melee/ready, G fire/back, Q block, E super, T pause.
-- P1 U/I changes location on the shared selection screen.

assert(emu, "Load Pokemon Battle Arena before running local_versus.lua")
assert(input and input.isKeyActive,
    "Local Versus needs an mGBA 0.11 development build with raw keyboard scripting")

if pkmn_local_versus_adapter then
    callbacks:remove(pkmn_local_versus_adapter.poll)
    callbacks:remove(pkmn_local_versus_adapter.reset)
end

local marker = "PKMNVS01"
local rom_base = 0x08000000
local mailbox = nil
local sequence = 0
local live_magic = 0x56534931

local function find_mailbox()
    local size = emu:romSize()
    assert(size and size >= 192 and size <= 32 * 1024 * 1024, "Unsupported ROM size")
    local bytes = emu:readRange(rom_base, size)
    local cursor = 1
    local found = nil
    while true do
        local offset = bytes:find(marker, cursor, true)
        if not offset then break end
        local address = rom_base + offset - 1
        local protocol = emu:read32(address + 8)
        local target = emu:read32(address + 12)
        local length = emu:read32(address + 16)
        local in_ewram = target >= 0x02000000 and target + length <= 0x02040000
        local in_iwram = target >= 0x03000000 and target + length <= 0x03008000
        if protocol == 1 and length == 16 and target % 4 == 0 and (in_ewram or in_iwram) then
            assert(not found, "Ambiguous Local Versus input descriptor")
            found = target
        end
        cursor = offset + #marker
    end
    assert(found, "This ROM does not contain a supported Local Versus input adapter")
    return found
end

local function letter(key)
    return input:isKeyActive(key) or input:isKeyActive(key:upper())
end

local function add(mask, active, button)
    if active then return mask | button end
    return mask
end

local function poll()
    -- Resolve again after a ROM reload/reset so addresses always belong to this ROM.
    if mailbox == false then return end
    if not mailbox then
        local ok, address = pcall(find_mailbox)
        if not ok then
            console:error(address)
            mailbox = false
            return
        end
        mailbox = address
    end
    local p1, p2 = 0, 0
    p1 = add(p1, input:isKeyActive(C.KEY.RIGHT), 16)
    p1 = add(p1, input:isKeyActive(C.KEY.LEFT), 32)
    p1 = add(p1, input:isKeyActive(C.KEY.UP), 64)
    p1 = add(p1, input:isKeyActive(C.KEY.DOWN), 128)
    p1 = add(p1, letter("j"), 1)
    p1 = add(p1, letter("k"), 2)
    p1 = add(p1, letter("u"), 512)
    p1 = add(p1, letter("i"), 256)
    p1 = add(p1, input:isKeyActive(C.KEY.ENTER), 8)

    p2 = add(p2, letter("d"), 16)
    p2 = add(p2, letter("a"), 32)
    p2 = add(p2, letter("w"), 64)
    p2 = add(p2, letter("s"), 128)
    p2 = add(p2, letter("f"), 1)
    p2 = add(p2, letter("g"), 2)
    p2 = add(p2, letter("q"), 512)
    p2 = add(p2, letter("e"), 256)
    p2 = add(p2, letter("t"), 8)

    sequence = (sequence + 2) & 0x7ffffffe
    if sequence == 0 then sequence = 2 end
    emu:write32(mailbox + 4, sequence | 1)
    emu:write32(mailbox, live_magic)
    emu:write32(mailbox + 8, p1)
    emu:write32(mailbox + 12, p2)
    emu:write32(mailbox + 4, sequence)
end

pkmn_local_versus_adapter = {
    reset = callbacks:add("reset", function() mailbox = nil end),
    poll = callbacks:add("keysRead", poll)
}
console:log("Local Versus keyboard adapter loaded. P1 arrows J/K U/I; P2 WASD F/G Q/E.")
