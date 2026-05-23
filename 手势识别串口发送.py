import cv2
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python.vision import HandLandmarker, HandLandmarkerOptions
from mediapipe.tasks.python.vision import drawing_utils
import serial

# ==================== 手指检测（更准确的方法） ====================
def is_finger_open(landmarks, tip_idx, pip_idx):
    """指尖高于关节 = 手指伸直"""
    return landmarks[tip_idx].y < landmarks[pip_idx].y

def count_fingers(landmarks):
    count = 0
    # 四指：指尖y < 第二关节y
    if is_finger_open(landmarks, 8, 6):    # 食指
        count += 1
    if is_finger_open(landmarks, 12, 10):  # 中指
        count += 1
    if is_finger_open(landmarks, 16, 14):  # 无名指
        count += 1
    if is_finger_open(landmarks, 20, 18):  # 小指
        count += 1
    # 拇指：用x坐标比较（拇指横向运动）
    if abs(landmarks[4].x - landmarks[3].x) > 0.04:
        count += 1
    return count

# ==================== 串口初始化 ====================
ser = serial.Serial('COM11', 115200, timeout=1)

# ==================== MediaPipe 初始化 ====================
options = HandLandmarkerOptions(
    base_options=python.BaseOptions(model_asset_path='hand_landmarker.task'),
    running_mode=python.vision.RunningMode.IMAGE,
    num_hands=1,
    min_hand_detection_confidence=0.5
)
detector = HandLandmarker.create_from_options(options)

# ==================== 摄像头初始化 ====================
cap = cv2.VideoCapture(1)
if not cap.isOpened():
    print("ERROR: Cannot open webcam")
    exit(1)

cv2.namedWindow('Hand', cv2.WINDOW_NORMAL)

# ==================== 变量 ====================
mode = "cruise"
no_hand_count = 0
last_count = -1

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)
    h, w, _ = frame.shape

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb)
    results = detector.detect(mp_image)

    if results.hand_landmarks:
        no_hand_count = 0

        for hand_landmarks in results.hand_landmarks:
            drawing_utils.draw_landmarks(
                frame, hand_landmarks,
                python.vision.HandLandmarksConnections.HAND_CONNECTIONS,
                drawing_utils.DrawingSpec(color=(0, 255, 0), thickness=2, circle_radius=3)
            )

            if mode == "cruise":
                mode = "follow"
                ser.write(b'f\n')
                print("检测到手掌！")

            finger_count = count_fingers(hand_landmarks)

            # 只在变化时发送
            if finger_count != last_count:
                if finger_count == 5:
                    ser.write(b'o\n')
                    print("手掌张开5指 → LED全亮")
                elif finger_count == 0:
                    ser.write(b'x\n')
                    print("握拳0指 → LED全灭")
                last_count = finger_count

            # 显示
            cv2.putText(frame, f"Fingers: {finger_count}", (50, 100),
                        cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 0, 0), 3)

            if finger_count == 5:
                cv2.putText(frame, "ALL ON", (50, 150),
                            cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 3)
            elif finger_count == 0:
                cv2.putText(frame, "ALL OFF", (50, 150),
                            cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 255), 3)

    else:
        no_hand_count += 1
        if mode == "follow" and no_hand_count > 30:
            mode = "cruise"
            ser.write(b'c\n')
            print("手掌丢失！")
            no_hand_count = 0
        cv2.putText(frame, "No Hand", (50, 100),
                    cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 255), 3)

    cv2.imshow('Hand', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
ser.close()
cv2.destroyAllWindows()