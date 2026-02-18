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

    virtual ~shape() {}

    // float *rayOrigin, float *rayDirection
    virtual void intersect() {
        std::cerr << "Error: Cannot call intersect of base class \'Shape.\'\n";
    }
    virtual std::string getShapeType() {
        std::string shapeType = "Base";
        return shapeType;
    }
    virtual void setRadius( float radius ) {
        std::cerr << "Error: Cannot assign radius to non-sphere class.\n";
    }
    virtual float getRadius() { 
        std::cerr << "Error: Cannot get radius from from non-sphere class.\n";
        assert( 1 != 1 );
    }
    virtual void setNormal( float *normal ) {
        std::cerr << "Error: Cannot assign normal to non-plane class.\n";
    }
    virtual void getNormal( float *normal ) { 
        std::cerr << "Error: Cannot get radius from from non-sphere class.\n";
    }

};

struct sphere : shape {

    float radius;

    void intersect(  ) {
        std::cout << "Placeholder: Calculate intersection for sphere.\n";
    }
    virtual std::string getShapeType() {
        std::string shapeType = "Sphere";
        return shapeType;
    }
    void setRadius(  float rds ) {
        this->radius = rds;
        std::cout << "Notice: Assigned radius property to sphere.\n";
    }
    virtual float getRadius() { 
        std::cout << "Notice: Got radius property from sphere.\n";
        return radius;
    };

};

struct plane : shape {

    
    float *normal;

    void intersect() {
        std::cout << "Placeholder: Calculate intersection for plane.\n";
    }
    virtual std::string getShapeType() {
        std::string shapeType = "Plane";
        return shapeType;
    }
    void setNormal(  float *nml ) {
        this->normal = nml;
        std::cout << "Notice: Assigned normal property to plane.\n";
    }
    virtual void getNormal( float *nml ) { 
        for ( int index=0; index<2; index++ ) {
            nml[ index ] = normal[ index ];
        }
        std::cout << "Notice: Got normal property from plane.\n";
    };

};

int readScene( char *sceneFileName, shape ***objects, camera *camera, int *numberOfShapes ) {

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
    int objectsTableIndex = -1; // Start at -1 since it increments when a new one is read
    // std::cout << tempObjString << "\n";

    while ( tempObjString != endOfScene ) {

        if ( tempObjString == "sphere" ) {
            objectsTableIndex ++;
            ( *objects )[ objectsTableIndex ] = new sphere;
        }
        else if ( tempObjString == "plane" ) {
            objectsTableIndex ++;
            ( *objects )[ objectsTableIndex ] = new plane;
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
                ( *objects )[ objectsTableIndex ]->position = position;

            }
            else if ( tempPropString == "radius:" ) {

                float radius;
                fscanf( stream, "%f", &radius );
                // std::cout << "SCANNED RADIUS\n";
                ( *objects )[ objectsTableIndex ]->setRadius( radius );
                
            }
            else if ( tempPropString == "normal:" ) {

                float normal[3] = { 0, 0, 0 };
                fscanf( stream, "%f %f %f", &( normal[0] ), &( normal[1] ), &( normal[2] ) );
                // std::cout << "SCANNED NORMAL\n";
                ( *objects )[ objectsTableIndex ]->setNormal( normal );
                

            }
            else if ( tempPropString == "c_diff:" ) {

                float cDiff[3] = { 0, 0, 0 };
                fscanf( stream, "%f %f %f", &( cDiff[0] ), &( cDiff[1] ), &( cDiff[2] ) );
                // std::cout << "SCANNED C_DIFF\n";
                ( *objects )[ objectsTableIndex ]->cDiff = cDiff;

            }

            c = fgetc( stream );

        }

        // std::cout << "END OF LINE\n\n";

        assert( fscanf( stream, "%s ", tempObject ) == 1 );
        tempObjString = tempObject;
        // std::cout << tempObjString << "\n";
        
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

        int maxShapes = 128;
        shape **objects = new shape*[ maxShapes ];
        int numberOfShapes;
        camera camera;

        if ( readScene( argv[3], &objects, &camera, &numberOfShapes ) == 1 ) {
            return 1; // Invalid file format
        }
        else {

            assert( numberOfShapes > 0 );
            
            // */ TEMPORARY PRINT OBJECTS
            std::cout << numberOfShapes << " shapes in array.\n\n";

            for ( int index=0; index<( numberOfShapes ); index++ ) {

                std::string shapeTypeString = objects[ index ]->getShapeType();
                if ( shapeTypeString == "Sphere" ) {
                    std::cout << "Object is a sphere. " << objects[ index ]->getRadius() << "\n";

                }
                else if ( shapeTypeString == "Plane" ) {
                    std::cout << "Object is a plane.\n";
                }
                else {
                    std::cerr << "Error: Undefined object in array.\n";
                    std::cout << shapeTypeString << "\n";
                    assert( 1 != 1 );
                }

            }
            // --- TEMP

            std::cout << "TEST\n";

            int imgWidth = std::stof(argv[1] );
            int imgHeight = std::stof( argv[2] );
            //float R_o[3] = { 0, 0, 0 };

            
            float testHeight = camera.height; // copy to a local variable
            std::cout << "Test height = " << testHeight << std::endl;


            for ( int imgY=0; imgY<imgHeight; imgY++ ) {
                for ( int imgX=0; imgX<imgWidth; imgX++ ) {

                    // std::cout << "Pixel " << imgY << imgX << "\n";
                    float rDistX = -0.5f * camera.width + imgX * ( camera.width / imgWidth ) + ( camera.width / imgWidth ) / 2.0f;
                    //float rDistY = -0.5f * camera.height + imgY * ( camera.height / imgHeight ) + ( camera.height / imgHeight ) / 2.0f;
                    //float rVector[3] = { rDistX, rDistY, 1 };

                    std::cout << rDistX << " " << "\n";
                    //std::cout << rDistY << " " << "\n";

                    //float R_d[3] = { 0, 0, 0 };
                    //v3_normalize( R_d, rVector );

                }
            }
            
        }

        // Deallocate
        
    }

    return 0;
}
