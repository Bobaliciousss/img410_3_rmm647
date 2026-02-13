#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <cstring>
#include <string>
#include "ppm.h"
#include "v3math.h"

extern "C" {
    #include "v3math.h"
}

struct camera {

    int width;
    int height;

};

// Base class
struct shape {

    float *position;
    float *cDiff;

    // float *rayOrigin, float *rayDirection
    virtual void intersect() {
        std::cerr << "Error: Cannot call intersect of base class \'Shape.\'\n";
    }

};

struct sphere : shape {

    float radius;

    void intersect(  ) {
        std::cout << "Placeholder: Calculate intersection for sphere.\n";
    }

};

struct plane : shape {

    float *normal;

    void intersect() {
        std::cout << "Placeholder: Calculate intersection for plane.\n";
    }

};

int readScene( char *sceneFileName, shape **objects ) {

    printf("%s\n", sceneFileName);

    FILE *stream = fopen( sceneFileName, "r" );
    assert( stream != NULL );

    char magicChars[12]; // Our "magic number" for the file
    assert( fscanf( stream, "%s ", magicChars ) == 1 );
    std::string sceneHeader = magicChars; // So I can compare it
    std::string identityHeader = "img410scene";
    if ( sceneHeader != identityHeader ) {
        std::cerr << "Error: Invalid file format.\n";
        return 1;
    }

    char tempObject[10];
    std::string tempObjString = tempObject; // For comparison
    std::string endOfScene = "end";

    while ( tempObjString != endOfScene ) {

        int outerResult = fscanf( stream, "%s ", tempObject );
        assert( outerResult == 1 );
        tempObjString = tempObject;
        std::cout << tempObjString << "\n";

        int c = fgetc( stream ); // Primer
        char tempProperty[10];
        std::string tempPropString = tempProperty;
        while ( c != '\n' )
        {

            ungetc( c, stream );
            fscanf( stream, "%s:", tempProperty );
            std::cout << tempPropString;

        }
        
    }

    fclose( stream );

    return 0;

}

int main(int argc, char *argv[])
{
    
    if ( argc != 5 ) {

        printf( "Usage: raycast width height input.scene output.ppm\n" );
        return 1;

    }
    else {

        shape *objects;

        if ( readScene( argv[3], &objects ) == 1 ) {
            return 1; // Invalid file format
        }
        else {

        }

    }

    return 0;
}
