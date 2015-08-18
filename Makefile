all: SphericalHarmonicsTest.exe

SphericalHarmonicsTest.exe: display.o Model.o Cubemap.o SoftwareTextureSFML.o SphericalFunction.o
	g++ -LC:/resources/SFML-2.1/lib -LC:/resources/lib3ds-20080909/src -o $@ $^ -lmingw32 -lopengl32 -lglu32 -lwinmm -lgdi32 -lsfml-graphics -lsfml-window -lsfml-system -l3ds

display.o: display.cpp
	g++ -IC:/resources/SFML-2.1/include -c $<

Model.o: Model.cpp
	g++ -IC:/resources/lib3ds-20080909/src -c $<

Cubemap.o: Cubemap.cpp
	g++ -IC:/resources/SFML-2.1/include -c $<

SoftwareTextureSFML.o: SoftwareTextureSFML.cpp
	g++ -IC:/resources/SFML-2.1/include -c $<

SphericalFunction.o: SphericalFunction.cpp
	g++ -IC:/resources/SFML-2.1/include -c $<
