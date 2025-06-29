# Script to pre-compute bit-packed array of non-leap years

SUPPORTED_FIRST_YEAR = 1178
SUPPORTED_LAST_YEAR = 3000

# All these years are not leap, while they are considered leap by the 33-year
# rule. The year following each of them is leap, but it's considered non-leap
# by the 33-year rule. This table has been tested to match the modified
# astronomical algorithm based on the 52.5 degrees east meridian from 1178 AP
# (an arbitrary date before the Persian calendar was adopted in 1304 AP) to
# 3000 AP (an arbitrary date far into the future).

NON_LEAP_CORRECTION = [
    1502, 1601, 1634, 1667,
    1700, 1733, 1766, 1799,
    1832, 1865, 1898, 1931,
    1964, 1997, 2030, 2059, 2063,
    2096, 2129, 2158, 2162, 2191,
    2195, 2224, 2228, 2257, 2261,
    2290, 2294, 2323, 2327, 2356,
    2360, 2389, 2393, 2422, 2426,
    2455, 2459, 2488, 2492, 2521,
    2525, 2554, 2558, 2587, 2591,
    2620, 2624, 2653, 2657, 2686,
    2690, 2719, 2723, 2748, 2752,
    2756, 2781, 2785, 2789, 2818,
    2822, 2847, 2851, 2855, 2880,
    2884, 2888, 2913, 2917, 2921,
    2946, 2950, 2954, 2979, 2983, 2987
]

BIT_PACKED = [0] * ((SUPPORTED_LAST_YEAR - SUPPORTED_FIRST_YEAR + 1) // 8 + 1)

for year in NON_LEAP_CORRECTION:
    if SUPPORTED_FIRST_YEAR <= year <= SUPPORTED_LAST_YEAR:
        byte_index = (year - SUPPORTED_FIRST_YEAR) // 8
        bit_index = (year - SUPPORTED_FIRST_YEAR) % 8
        BIT_PACKED[byte_index] |= (1 << bit_index)

print("static const unsigned char NON_LEAP_LOOKUP[] = {")
for i, byte in enumerate(BIT_PACKED):
    if i % 8 == 0:
        print("    ", end="")
    print(f"0x{byte:02X}, ", end="")
    if i % 8 == 7:
        print()
print("\n};")