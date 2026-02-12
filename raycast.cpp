#include <stdio.h>
#include <iostream>
#include <assert.h>
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

int readScene( char *sceneFileName ) {

    FILE *stream = fopen( sceneFileName, "r" );
    assert( stream != NULL );

    return 0

}

int main(int argc, char *argv[])
{
    
    if ( argc != 5 ) {

        printf( "Usage: Blur width height input.scene output.ppm\n" );
        return 1;

    }
    else {

        

        if ( readScene( argv[4],  ) == 1 ) {

        }
        else {

        }

    }

    return 0;
}
