import ctypes
from pathlib import Path
import persiancalendar

from persiancalendar import fixed_from_gregorian as fixed_from_gregorian_py

lib = ctypes.CDLL(str(Path(__file__).parent / "persiancalendar_fast.so"))

class GregorianDate(ctypes.Structure):
    _fields_ = [("year", ctypes.c_int),
                ("month", ctypes.c_int),
                ("day", ctypes.c_int)]

lib.fixed_from_gregorian.argtypes = [GregorianDate]
lib.fixed_from_gregorian.restype  = ctypes.c_int

# --- Tests ---
def test_known():
    assert lib.fixed_from_gregorian(GregorianDate(1,1,1)) == 1
    assert lib.fixed_from_gregorian(GregorianDate(2000,2,29)) == fixed_from_gregorian_py((2000,2,29))
    assert lib.fixed_from_gregorian(GregorianDate(1900,3,1))  == fixed_from_gregorian_py((1900,3,1))
    print("known dates OK")

def test_random(n=10000):
    import random
    for _ in range(n):
        y = random.randint(1,3000)
        m = random.randint(1,12)
        if m==2:
            dmax = 29 if persiancalendar.gregorian_leap_year(y) else 28
        elif m in (4,6,9,11):
            dmax = 30
        else:
            dmax = 31
        d = random.randint(1,dmax)
        c_val  = lib.fixed_from_gregorian(GregorianDate(y,m,d))
        py_val = fixed_from_gregorian_py((y,m,d))
        assert c_val == py_val, f"Mismatch on {y}-{m}-{d}: C={c_val}, PY={py_val}"
    print(f"{n} random dates OK")

if __name__ == "__main__":
    test_known()
    test_random()
    print("all tests passed")
