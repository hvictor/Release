#!/bin/sh
nvcc -arch=sm_32 -Xcompiler '-fPIC' -dc gpu_contrastu8.cu gpu_flow_recogu8_binary.cu gpu_flow_recogu8.cu gpu_physical_model.cu
nvcc -arch=sm_32 -Xcompiler '-fPIC' -dlink gpu_flow_recogu8_binary.o gpu_contrastu8.o gpu_flow_recogu8.o gpu_physical_model.o -o gpu_objs.o
g++ -shared -o libgpu.so gpu_flow_recogu8_binary.o gpu_contrastu8.o gpu_flow_recogu8.o gpu_physical_model.o gpu_objs.o -L/usr/local/cuda/lib -lcudart
