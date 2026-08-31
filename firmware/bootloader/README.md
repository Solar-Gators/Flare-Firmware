# CAN bootloader POC (A/B slots)

Portable bootloader for STM32U575 boards in this repo. The bootloader stays at
`0x08000000`. Two application slots hold a current image and a newly downloaded
image. A bad or unconfirmed image falls back to the last confirmed slot.

## Flash map

| Region | Address | Size |
| --- | --- | --- |
| Bootloader | `0x08000000` | 56 KB |
| Slot metadata | `0x0800E000` | 8 KB |
| Slot A | `0x08010000` | 992 KB (vectors at `+0x200`) |
| Slot B | `0x08108000` | 992 KB (vectors at `+0x200`) |

Board IDs: Front-VCU=1, Rear-VCU=2, Steering-Wheel=3, Telemetry=4.

CAN pins are a compile-time table: Steering-Wheel uses PA11/PA12; the other
U575 boards use PB8/PB9. Classic CAN at 500 kbps (same bit timing as production
apps).

## Targets

Built with the repo CMake presets (`debug` / `release`):

- `bootloader-steering-wheel` (POC receiver) and `bootloader-{rear-vcu,front-vcu,telemetry}`
- `bl-test-app-slot-a` / `bl-test-app-slot-b` (confirm after boot)
- `bl-test-app-noconfirm-slot-b` (resets without confirm — used to test revert)
- `bl-host-rear-vcu` — flash onto a Rear-VCU (PB8/PB9). Embeds both slot images
  and sends the one matching the target's inactive slot.

## Two-board hardware bring-up

Bus: 500 kbps, 120 Ω termination, Rear-VCU host to Steering-Wheel target.

1. **Jump-only.** J-Link the steering-wheel bootloader, then the slot-A test app
   at `0x08010200` (or a combined flash of both). Reset. After ~1 s with no CAN
   `ENTER`, it should jump to the test app. The confirm app marks slot A healthy.
2. **CAN update.** Flash `bl-host-rear-vcu` on Rear-VCU. Reset both boards so the
   host covers the 1 s ENTER window. Host queries the inactive slot, streams the
   matching `.bin`, `VERIFY`, `ACTIVATE`, `JUMP`.
3. **Fallback**
   - Power-cycle the target during a download: slot A must still run.
   - Wrong `board_id` is ignored/NAK; slot A unchanged.
   - Flash `bl-test-app-noconfirm-slot-b` via the host (`-DBL_HOST_SEND_NOCONFIRM=1`
     rebuild of `bl-host-rear-vcu`) or J-Link into slot B and activate it. After
     three unconfirmed resets the bootloader reverts to slot A.

Native tests (no hardware): `firmware/bootloader/tests` (`test_bl_slots`,
`test_bl_download`). CI runs them with host `gcc`.
