//
// Created by marc on 13/08/2021.
//

// In this file, you can adjust how the engine works (Many of these will be turned into variables at some point)

#ifndef RAYTRACING_RAYMARCHING_HYBRID_CONFIG_H
#define RAYTRACING_RAYMARCHING_HYBRID_CONFIG_H

#define DO_TELEMETRY              // Whether the renderer should track telemetry

// Pathtracing
#define NUMBER_OF_THREADS 8       // The number of render threads to use
#define SAMPLES 32               // Number of samples per pixel. This will be rounded up to the closest multiple of the number of threads
#define MAX_BOUNCE_COUNT 18       // The number of bounces to which the pathtracer will be limited
#define CLEARANCE_DIST 0.001      // The distance that will be moved away from a surface tp clear it.
#define STOP_BOUNCE_ON_EMISSION   // Whether the engine should disregard bounce light bouncing off of emissive surfaces

// Raytracing
#define DO_BACKFACE_CULLING       // Whether the engine should cull faces that face away from the camera

// Raymarching
#define RAYMARCHING_MAX_STEPS 1000       // The max number of steps to be taken in the raymarching part of the engine
#define RAYMARCHING_MIN_DIST 0.0001     // The minimal distance to count as a hit
#define RAYMARCHING_NORMAL_OFFSET 0.01  // This distance will be taken from a point to compute it's normal

#endif //RAYTRACING_RAYMARCHING_HYBRID_CONFIG_H
