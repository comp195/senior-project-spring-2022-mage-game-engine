#!/bin/sh

/usr/bin/glslc src/shaders/shader.vert -o src/shaders/bytecode/vert.spv
/usr/bin/glslc src/shaders/shader.frag -o src/shaders/bytecode/frag.spv
