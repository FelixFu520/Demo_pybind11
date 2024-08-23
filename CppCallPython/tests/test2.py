import numpy as np
import cv2
import example

def inferByPython() -> int:

    print(f"Python: image shape: {image.shape}")
    cv2.imwrite('test2.jpg', image)
    return 0