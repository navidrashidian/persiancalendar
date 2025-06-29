import ctypes
from pathlib import Path
import persiancalendar

# PARAMETERS
ROUNDTRIP_START_YEAR = 1178
ROUNDTRIP_END_YEAR   = 3000

# — load your C fast‑lib —
lib = ctypes.CDLL(str(Path(__file__).parent / "persiancalendar_fast.so"))

class PersianDate(ctypes.Structure):
    _fields_ = [("year", ctypes.c_int),
                ("month", ctypes.c_int),
                ("day", ctypes.c_int)]

lib.fixed_from_persian_fast.argtypes    = [PersianDate]
lib.fixed_from_persian_fast.restype     = ctypes.c_int
lib.persian_fast_from_fixed.argtypes    = [ctypes.c_int]
lib.persian_fast_from_fixed.restype     = PersianDate

# — roundtrip test —
def test_roundtrip_fast():
    start = lib.fixed_from_persian_fast(PersianDate(ROUNDTRIP_START_YEAR, 1, 1))
    end   = lib.fixed_from_persian_fast(PersianDate(ROUNDTRIP_END_YEAR,   1, 1))
    for d in range(start, end):
        pd   = lib.persian_fast_from_fixed(d)
        back = lib.fixed_from_persian_fast(pd)
        assert d == back, f"roundtrip failed at fixed={d}"

# — compare to astronomical Python impl —
def test_fast():
    sf = ctypes.c_int.in_dll(lib, "SUPPORTED_FIRST_YEAR").value
    sl = ctypes.c_int.in_dll(lib, "SUPPORTED_LAST_YEAR").value
    start = persiancalendar.fixed_from_persian((sf, 1, 1))
    end   = persiancalendar.fixed_from_persian((sl+1, 1, 1))

    for d in range(start, end):
        fast_pd  = lib.persian_fast_from_fixed(d)
        astro_pd = persiancalendar.persian_from_fixed(d)
        tup_fast = (fast_pd.year, fast_pd.month, fast_pd.day)
        assert tup_fast == astro_pd, f"mismatch at {d}: fast={tup_fast}, astro={astro_pd}"

if __name__ == "__main__":
    test_roundtrip_fast()
    test_fast()
    print("all fast‑tests passed")