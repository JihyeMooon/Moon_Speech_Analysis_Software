# Real-Time Speech Analysis and Pathological Speech Detection Software

## Introduction
![image](https://github.com/JihyeMooon/Speech-Analysis-Software/assets/112595759/c09f0f46-b6c1-4d15-9de5-77448457cca1)

In 2015, I developed **a real-time pathological speech detection software using C++ and the Microsoft Foundation Class Library (MFC)**. This program provides real-time speech data recording, voice activity detection (also called speech end-point detection), pathological speech detection, and the ability to import external audio data.
 
<img src="https://github.com/JihyeMooon/Speech-Analysis-Software/assets/112595759/911ba534-e683-456a-823f-71d0f329a0f3"  width="60%" height="30%">

This software **detects speech endpoints using energy/zero crossing rate**, computes various speech features, including Jitter, Shimmer, and high-order statistics, and detects pathological speeches based on a pre-trained decision tree model **(a simple machine learning model!)**. The accuracy in detecting patholocial speeches was obtained as **83.11%**.

The details for the software were published as a peer-reviwed paper at [a Korean Journal](https://www.researchgate.net/publication/292670555_Development_of_medicalelectrical_convergence_software_for_classification_between_normal_and_pathological_voices) in 2015.

For now, this GitHub repository releases C++ code (compatible with MFC) for **only the real-time speech data recording and speech end-point detection parts**.
If you have any questions, please contact me at **husky.jihye.moon@gmail.com**.

## Other Voice Activity Detection Codes
For additional voice activity detection algorithms, I also implemented three based on Autocorrelation Function (ACF), Average Magnitude Difference Function (AMDF), and Higher Order Differential Energy Operators (HODEO) respectively. Results for other voice activity detection methods are displayed below.

<img src="https://github.com/JihyeMooon/Moon_Voice_Activity_Detection/assets/112595759/1e817bba-94b9-4870-9005-9931b887fae2"   width="60%" height="30%">

**ACF, AMDF, and HODEO-based voice activity detection codes are avablible at [Link](https://github.com/JihyeMooon/Moon_Voice_Activity_Detection)!**
