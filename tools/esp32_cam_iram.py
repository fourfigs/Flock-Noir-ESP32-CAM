"""Move task-context libc time routines to flash for the ESP32-CAM build."""

import re
from pathlib import Path

Import("env")

# Equivalent to disabling CONFIG_SPIRAM_CACHE_LIBTIME_IN_IRAM in ESP-IDF.
# https://docs.espressif.com/projects/esp-idf/en/v4.4.6/esp32/api-guides/performance/ram-usage.html
# Keep the PSRAM workaround libraries and all explicit IRAM/ISR sections intact.
TIME_OBJECTS = (
    "asctime", "asctime_r", "ctime", "ctime_r", "lcltime", "lcltime_r",
    "gmtime", "gmtime_r", "strftime", "mktime", "tzset_r", "tzset",
    "time", "gettzinfo", "systimes", "month_lengths", "timelocal",
    "tzvars", "tzlock", "tzcalc_limits", "strptime",
)

if env.BoardConfig().get("build.mcu") != "esp32":
    raise RuntimeError("ESP32-CAM IRAM placement requires the classic ESP32")

framework = Path(env.PioPlatform().get_package_dir("framework-arduinoespressif32-libs"))
source = framework / "esp32" / "ld" / "sections.ld"
sections = source.read_text()
for name in TIME_OBJECTS:
    selector = f"*libc.a:libc_a-{name}.*"
    # The pinned SDK selects code/data for RAM and excludes them from flash.
    # Fail if the SDK layout changes instead of silently keeping an overflow.
    if sections.count(selector) != 12:
        raise RuntimeError(f"Unexpected linker placement for {name} in {source}")
    pattern = rf"^\s*{re.escape(selector)}\(\.literal \.literal\.\* \.text \.text\.\*\)\n"
    sections, count = re.subn(pattern, "", sections, flags=re.MULTILINE)
    if count != 1:
        raise RuntimeError(f"Unexpected IRAM selector for {name} in {source}")
    pattern = rf"^\s*{re.escape(selector)}\(\.rodata \.rodata\.\* \.sdata2 \.sdata2\.\* \.srodata \.srodata\.\*\)\n"
    sections, count = re.subn(pattern, "", sections, flags=re.MULTILINE)
    if count != 1:
        raise RuntimeError(f"Unexpected DRAM selector for {name} in {source}")
    sections = sections.replace(selector, "")

destination = Path(env.subst("$BUILD_DIR")) / "esp32_cam_sections.ld"
destination.parent.mkdir(parents=True, exist_ok=True)
destination.write_text(sections)

flags = list(env["LINKFLAGS"])
if flags.count("sections.ld") != 1:
    raise RuntimeError("Expected exactly one sections.ld linker argument")
flags[flags.index("sections.ld")] = str(destination)
env.Replace(LINKFLAGS=flags)
