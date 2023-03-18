import torch
import cv2
import numpy as np
from yolov5.models.experimental import attempt_load
from yolov5.utils.torch_utils import select_device

# 加载yolov5模型
device = select_device('cpu')
model = attempt_load('/home/fo_pluto/demo_opencv/best.pt', map_location=device)
model.eval()

# 对象检测函数
def detect(img):
    # 将图片转换为yolov5所需的格式
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = img.transpose(2, 0, 1)
    img = np.expand_dims(img, axis=0)
    img = torch.from_numpy(img).float()

    # 进行对象检测
    with torch.no_grad():
        pred = model(img)[0]
        pred = pred[pred[:, 4] > 0.5]

    # 处理检测结果并返回
    results = []
    for i in range(len(pred)):
        x1, y1, x2, y2, score, cls = pred[i].tolist()
        results.append([x1, y1, x2, y2, score, cls])
    return results
