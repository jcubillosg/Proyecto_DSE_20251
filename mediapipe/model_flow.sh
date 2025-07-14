#!/usr/bin/bash
# 1. Import AI model as .tflite file

# 2. Generate C header for the model
xxd -i model.tflite > model.h
