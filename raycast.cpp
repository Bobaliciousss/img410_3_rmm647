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

    float width;
    float height;

};

// Base class
struct shape {

    float *position;
    float *cDiff;

    // float *rayOrigin, float *rayDirection
    virtual void intersect() {
        std::cerr << "Error: Cannot call intersect of base class \'Shape.\'\n";
    }
    virtual void setRadius(  float radius ) {
        std::cerr << "Error: Cannot assign radius property to base class \'Shape.\'\n";
    }
    virtual void setNormal(  float *normal ) {
        std::cerr << "Error: Cannot assign normal property to base class \'Shape.\'\n";
    }

};

struct sphere : shape {

    float radius;

    void intersect(  ) {
        std::cout << "Placeholder: Calculate intersection for sphere.\n";
    }
    void setRadius(  float rds ) {
        this->radius = rds;
        std::cerr << "Notice: Assigned radius property to derived class \'Sphere.\'\n";
    }

};

struct plane : shape {

    float *normal;

    void intersect() {
        std::cout << "Placeholder: Calculate intersection for plane.\n";
    }
    void setNormal(  float *nml ) {
        this->normal = nml;
        std::cerr << "Notice: Assigned normal property to derived class \'Plane.\'\n";
    }

};

int readScene( char *sceneFileName, shape **objects, camera *camera, int *numberOfShapes ) {

    printf("\nReading %s\n\n", sceneFileName);

    FILE *stream = fopen( sceneFileName, "r" );
    assert( stream != NULL );

    char magicChars[12];
    assert( fscanf( stream, "%s ", magicChars ) == 1 );
    std::string sceneHeader = magicChars; // For comparison
    std::string identityHeader = "img410scene";
    if ( sceneHeader != identityHeader ) {
        std::cerr << "Error: Invalid file format.\n";
        return 1;
    }

    char tempObject[10];
    std::string tempObjString = tempObject;
    std::string endOfScene = "end";

    assert( fscanf( stream, "%s ", tempObject ) == 1 ); // Primer
    tempObjString = tempObject;
    int objectsTableIndex = 0;
    // std::cout << tempObjString << "\n";

    while ( tempObjString != endOfScene ) {

        if ( tempObjString == "sphere" ) {
            objects[ objectsTableIndex ] = new sphere;
        }
        else if ( tempObjString == "plane" ) {
            objects[ objectsTableIndex ] = new plane;
        }

        int c = fgetc( stream ); // Primer
        char tempProperty[10];
        std::string tempPropString = tempProperty;

        while ( c != '\n' )
        {
            
            ungetc( c, stream );
            
            int tempInnerResult = fscanf( stream, "%s", tempProperty );
            assert( tempInnerResult > 0 && tempInnerResult < 5 );
            tempPropString = tempProperty;
            // std::cout << tempPropString << "\n";

            if ( tempPropString == "height:" ) {

                assert( fscanf( stream, "%f", &( camera->height ) ) == 1 );
                // std::cout << "SCANNED HEIGHT\n";

            }
            else if ( tempPropString == "width:" ) {

                assert( fscanf( stream, "%f", &( camera->width ) ) == 1 );
                // std::cout << "SCANNED WIDTH\n";

            }
            else if ( tempPropString == "position:" ) {

                float position[3] = { 0, 0, 0 };
                fscanf( stream, "%f %f %f", &( position[0] ), &( position[1] ), &( position[2] ) );
                // std::cout << "SCANNED POSITION\n";
                objects[ objectsTableIndex ]->position = position;

            }
            else if ( tempPropString == "radius:" ) {

                float radius;
                fscanf( stream, "%f", &radius );
                // std::cout << "SCANNED RADIUS\n";
                objects[ objectsTableIndex ]->setRadius( radius );
                
            }
            else if ( tempPropString == "normal:" ) {

                float normal[3] = { 0, 0, 0 };
                fscanf( stream, "%f %f %f", &( normal[0] ), &( normal[1] ), &( normal[2] ) );
                // std::cout << "SCANNED NORMAL\n";
                objects[ objectsTableIndex ]->setNormal( normal );
                

            }
            else if ( tempPropString == "c_diff:" ) {

                float cDiff[3] = { 0, 0, 0 };
                fscanf( stream, "%f %f %f", &( cDiff[0] ), &( cDiff[1] ), &( cDiff[2] ) );
                // std::cout << "SCANNED C_DIFF\n";
                objects[ objectsTableIndex ]->cDiff = cDiff;

            }

            c = fgetc( stream );

        }

        // std::cout << "END OF LINE\n\n";

        assert( fscanf( stream, "%s ", tempObject ) == 1 );
        tempObjString = tempObject;
        // std::cout << tempObjString << "\n";

        objectsTableIndex ++;
        
    }

    fclose( stream );

    *numberOfShapes = ( objectsTableIndex + 1 );

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
        int numberOfShapes = 0;
        camera camera;

        if ( readScene( argv[3], &objects, &camera, &numberOfShapes ) == 1 ) {
            return 1; // Invalid file format
        }
        else {

            // TEMPORARY PRINT OBJECTS
            std::cout << numberOfShapes << " shapes in array.\n\n";
            //for ( int index=0; index< )

        }

    }

    return 0;
}
