import numpy as np
import scipy as sp
import matplotlib.pyplot as plt
import scipy.fftpack as fp

im = np.random.random((100,100))

def makeFiltered(im, minimum, maximum):
    F1 = fp.fft2((im).astype(float))
    F2 = fp.fftshift(F1)

    (w, h) = im.shape
    half_w, half_h = int(w/2), int(h/2)
    
    # high pass filter
    F2[half_w - minimum : half_w + minimum + 1, half_h - minimum : half_h + minimum + 1] = 0 # select all but the first 50x50 (low) frequencies
    F2[:half_w - maximum, :] = 0
    F2[half_w + maximum:, :] = 0
    F2[:, :half_h - maximum] = 0
    F2[:, half_h + maximum:] = 0
    im1 = fp.ifft2(fp.ifftshift(F2)).real
    return im1

for minimum in range(26):
    for maximum in range(26):
        with open(f"../assets/filterbackgrounds/bg_{minimum}_{maximum}.bin", 'wb') as f:
            print(minimum, maximum)
            filtered = makeFiltered(im, minimum, maximum)
            f.write((filtered.flatten() * 256).astype(np.uint8).tobytes())
    

with open("FilterData.h", "w") as f:
    f.write("#pragma once\n")
    f.write('#include "BinaryData.h"\n')
    f.write('#include <array>\n')
    f.write("const std::array<const std::array<const char*, 26>, 26> filterData = {\n")
    for minimum in range(26):
        f.write("{")
        for maximum in range(26):
            f.write(f"BinaryData::bg_{minimum}_{maximum}_bin, ")
        f.write("},\n")
    f.write("};")