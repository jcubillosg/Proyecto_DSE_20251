#Video loop is broken by pressing 'q'

import numpy as np
import matplotlib.pyplot as plt
import cv2
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
from mediapipe import solutions
from mediapipe.framework.formats import landmark_pb2

def draw_landmarks_on_image(rgb_image, detection_result):
  face_landmarks_list = detection_result.face_landmarks
  annotated_image = np.copy(rgb_image)

  # Loop through the detected faces to visualize.
  for idx in range(len(face_landmarks_list)):
    face_landmarks = face_landmarks_list[idx]

    # Draw the face landmarks.
    face_landmarks_proto = landmark_pb2.NormalizedLandmarkList()
    face_landmarks_proto.landmark.extend([
      landmark_pb2.NormalizedLandmark(x=landmark.x, y=landmark.y, z=landmark.z) for landmark in face_landmarks
    ])

    solutions.drawing_utils.draw_landmarks(
        image=annotated_image,
        landmark_list=face_landmarks_proto,
        connections=mp.solutions.face_mesh.FACEMESH_TESSELATION,
        landmark_drawing_spec=None,
        connection_drawing_spec=mp.solutions.drawing_styles
        .get_default_face_mesh_tesselation_style())
    solutions.drawing_utils.draw_landmarks(
        image=annotated_image,
        landmark_list=face_landmarks_proto,
        connections=mp.solutions.face_mesh.FACEMESH_CONTOURS,
        landmark_drawing_spec=None,
        connection_drawing_spec=mp.solutions.drawing_styles
        .get_default_face_mesh_contours_style())
    solutions.drawing_utils.draw_landmarks(
        image=annotated_image,
        landmark_list=face_landmarks_proto,
        connections=mp.solutions.face_mesh.FACEMESH_IRISES,
        landmark_drawing_spec=None,
        connection_drawing_spec=mp.solutions.drawing_styles
        .get_default_face_mesh_iris_connections_style())

  return annotated_image

#################################################################################################

base_options = python.BaseOptions(model_asset_path='face_landmarker_v2_with_blendshapes.task')
options = vision.FaceLandmarkerOptions(base_options=base_options,
                                       output_face_blendshapes=True,
                                       output_facial_transformation_matrixes=True,
                                       num_faces=1)
detector = vision.FaceLandmarker.create_from_options(options)

cam = cv2.VideoCapture(0)
frame_width = int(cam.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cam.get(cv2.CAP_PROP_FRAME_HEIGHT))
# Define the codec and create VideoWriter object
#fourcc = cv2.VideoWriter_fourcc(*'mp4v')
#out = cv2.VideoWriter('output.mp4', fourcc, 20.0, (frame_width, frame_height))

while True:
    ret, frame = cam.read()
    if ret: mp_frame = mp.Image(image_format=mp.ImageFormat.SRGB, data=frame)
    detection_result = detector.detect(mp_frame)
    annotated_image = draw_landmarks_on_image(mp_frame.numpy_view(), detection_result)
    landmark_list = detection_result.face_landmarks
    left_eye = 0
    right_eye = 0
    if len(landmark_list) >= 1:
        left_eye = np.sqrt((landmark_list[0][159].x - landmark_list[0][145].x)**2 +(landmark_list[0][159].y - landmark_list[0][145].y)**2 + (landmark_list[0][159].z - landmark_list[0][145].z)**2)
        right_eye = np.sqrt((landmark_list[0][386].x - landmark_list[0][374].x)**2 +(landmark_list[0][386].y - landmark_list[0][374].y)**2 +(landmark_list[0][386].z - landmark_list[0][374].z)**2)
        #print(f'Left eye: {left_eye}; Right eye: {right_eye}')
    #annotated_image = cv2.cvtColor(annotated_image, cv2.COLOR_RGB2BGR)
    avg = np.mean(np.array([left_eye, right_eye])) 
    if avg <= 0.03 and avg > 0.01:
        print("Eyes closed")
    cv2.imshow('Press \'q\' to exit', annotated_image)
    if cv2.waitKey(1) == ord('q'):
        break

cam.release()
#out.release()
cv2.destroyAllWindows()
