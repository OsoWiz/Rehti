import os
import sys

def embedShaders(shaderDir, outputFile):
    supportedFileExtensions = [".vert", ".frag", ".comp", ".geom", ".tesc", ".tese", ".spv"]
    compiledShaders = {}
    for root, _, files in os.walk(shaderDir): # Walk through all files in the directory
        for file in files:
            if os.path.splitext(file)[1] in supportedFileExtensions: # Check if file is a shader file
                with open(os.path.join(root, file), "r") as f:
                    compiledShaders[os.path.basename(file)] = f.read() # read content of the file and store it in the dictionary
    
    with open(outputFile, 'w+') as f:
        f.write("#pragma once\n")
        f.write("#include <unordered_map>\n")
        f.write("#include <string>\n")
        f.write("namespace ShaderEmbedder {\n")
        f.write("std::unordered_map<std::string, std::string> shaders = {\n")
        for shaderName, shaderContent in compiledShaders.items():
            f.write("\t{\"" + shaderName + "\", R\"(" + shaderContent + ")\"},\n")
        f.write("};\n")
        f.write("}\n")



if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: ShaderEmbedder.py <shaderDir> <outputFile>")
    else:
        embedShaders(sys.argv[1], sys.argv[2])