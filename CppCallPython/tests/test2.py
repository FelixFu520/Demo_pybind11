import numpy as np
import cv2

def inferByPython(image) -> list:
    print(type(image))
    print(image.shape)
    cv2.imwrite("./test2.jpg", image)
    image_raw = cv2.imread("./xz.jpg")
    print(image_raw.shape)
    image_sub = cv2.subtract(image_raw, image)
    print(image_sub.shape)
    num = np.sum(image_sub)
    print(num)
    return [np.array([10, 20, 30, 40, 0.9, 2], dtype=np.float32), np.array([50, 60, 70, 80, 0.8, 1], dtype=np.float32)]