# Speech Analysis Software

## Software Details
![image](https://github.com/JihyeMooon/Speech-Analysis-Software/assets/112595759/cb08eed3-f159-4222-a1e8-1cab190556fd)

In 2015, I developed **real-time speech disorder detection software using C++ and the Microsoft Foundation Class Library (MFC)**. This program provides real-time speech data recording, voice activity detection (also called speech end-point detection), and pathological speech detection. It also works for importing audio data. 

This program **detects speech endpoints using energy/zero crossing rate**, computes various speech features, including Jitter, Shimmer, and high-order statistics, and detects pathological speeches based on a pre-trained decision tree model **(a simple machine learning model!)**.

For now, I share C++ codes (compatible with MFC) for **only the real-time speech data recording and speech end-point detection parts**. 
If you have any questions, please contact me at **husky.jihye.moon@gmail.com**.

## Reference
![image](https://github.com/JihyeMooon/Speech-Analysis-Software/assets/112595759/c09f0f46-b6c1-4d15-9de5-77448457cca1)

My paper published in 2015 is at https://www.researchgate.net/publication/292670555_Development_of_medicalelectrical_convergence_software_for_classification_between_normal_and_pathological_voices (Korean Language)
