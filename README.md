# Perception GPU Obstacle Detector Module
## About 

This CUDA-based module performs real time obstacle detection for the rover 
and sends LCM messages containing information about obstacles. Requires an NVIDIA 
GPU and CUDA installation to run.

Code walkthrough: https://youtube.com/playlist?list=PLj5rOuXit19mCtakn21m11At3zjgEycnO

## Dev Enviornment Setup

Follow the install guide for your desired workflow. 
- Local: https://docs.google.com/document/d/1MELSAlFY7PWr0ZisBeJtXIB8x-YFIi4BrLiyycQkH5Y/edit
- GreatLakes: https://docs.google.com/document/d/1Xkk615z5T1gS0-P0z-_xr7CUilrxN_Vcua-Uol290bg/edit?usp=sharing
   
## Usage

**Great Lakes:** 
1. Connect to UMich VPN (if off campus) and log into: https://greatlakes-oncampus.arc-ts.umich.edu/pun/sys/dashboard
2. Go to My Interactive Sessions and launch a new session on GPU partition with 1 GPU 
3. Run `./launchme` to open the Singularity container for development 
4. Run `source sourceme` to add CUDA compiler to your shell  
5. From `mrover-workspace` do:
```
./jarvis build jetson/percep_obs_detect
./jarvis exec percep_obs_detect
```

**Local:**
1. From `mrover-workspace` do:
```
./jarvis build jetson/percep_obs_detect
./jarvis exec percep_obs_detect
```
## Overview
**obs-detector:** Driver program, operating mode selection

**common:** Lazy utility file

**pass-through:** Simple filter to remove points too close or too far in our view

**plane-ransac:** RANSAC Plane segmentation for ground detection and removal

**euclidean-cluster:** Detects obstacle bounding boxes after all filtering 

**path-finder:** Solves a clear path based on bounding box detections 

## Other
Deprecated documentation: https://docs.google.com/document/d/1WPW7yxcCp_EcDjPbGVOtrZl3A5krSXDas_pcxD3N2IM/edit


