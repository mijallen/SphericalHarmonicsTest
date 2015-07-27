all: SphericalHarmonicsTest.exe

SphericalHarmonicsTest.exe: display.o Model.o Cubemap.o SoftwareTextureSFML.o
	g++ -LC:/resources/SFML-2.1/lib -DSFML_STATIC -LC:/resources/lib3ds-20080909/src -o SphericalHarmonics.exe display.o Model.o Cubemap.o SoftwareTextureSFML.o -lmingw32 -lopengl32 -lglu32 -lwinmm -lgdi32 -lsfml-graphics -lsfml-window -lsfml-system -l3ds

display.o: display.cpp
	g++ -IC:/resources/SFML-2.1/include -DSFML_STATIC -c display.cpp

Model.o: Model.cpp
	g++ -IC:/resources/lib3ds-20080909/src -c Model.cpp

Cubemap.o: Cubemap.cpp
	g++ -IC:/resources/SFML-2.1/include -DSFML_STATIC -c Cubemap.cpp

SoftwareTextureSFML.o: SoftwareTextureSFML.cpp
	g++ -IC:/resources/SFML-2.1/include -DSFML_STATIC -c SoftwareTextureSFML.cpp
