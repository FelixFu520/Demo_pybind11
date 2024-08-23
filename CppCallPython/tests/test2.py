import numpy as np
import cv2

def inferByPython(image) -> list:
    print(type(image))
    return [np.array([10, 20, 30, 40, 0.9, 2]), np.array([50, 60, 70, 80, 0.8, 1])]