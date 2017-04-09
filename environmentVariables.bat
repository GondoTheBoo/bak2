@echo off
echo setting sfml environment variable:
SETX VCG_2016_SFML_DIR  "%~dp0externalLibs\sfml"
echo ---
echo setting glew environment variable:
SETX VCG_2016_GLEW_DIR  "%~dp0externalLibs\glew"
echo ---
echo setting glm environment variable:
SETX VCG_2016_GLM_DIR  "%~dp0externalLibs\glm"
echo ---
echo setting assimp environment variable:
SETX VCG_2016_ASSIMP_DIR  "%~dp0externalLibs\assimp"
echo ---
echo setting picojson environment variable:
SETX VCG_2016_PICOJSON_DIR  "%~dp0externalLibs\picojson"
echo ---
echo setting data environment variable:
SETX VCG_2016_DATA_DIR  "%~dp0data"
echo ---
echo all done
pause