#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <cstring>
#include <string>
#include <limits>
#include <cmath>
#include "ppm.h"

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
    virtual float intersect( float *R_o, float *R_d ) {
        std::cerr << "Error: Cannot call intersect of base class \'Shape.\'\n";
        return 0.0f;
    }
    virtual std::string getShapeType() {
        std::string shapeType = "Base";
        return shapeType;
    }
    virtual void setRadius( float radius ) {
        std::cerr << "Error: Cannot assign radius to non-sphere class.\n";
    }
    virtual void setNormal( float *normal ) {
        std::cerr << "Error: Cannot assign normal to non-plane class.\n";
    }

};

struct sphere : shape {

    float radius;

    float intersect( float *R_o, float *R_d ) {

        //float A = ( R_d[0] * R_d[0] ) + ( R_d[1] * R_d[1] ) + ( R_d[2] * R_d[2] ); // Should always equal 1
        //float A = 1;
        float B = 2.0f * ( R_d[0] * ( R_o[0] - this->position[0] ) 
                         + R_d[1] * ( R_o[1] - this->position[1] ) 
                         + R_d[2] * ( R_o[2] - this->position[2] ) );
        float C = ( R_o[0] - this->position[0] ) * ( R_o[0] - this->position[0] )
                + ( R_o[1] - this->position[1] ) * ( R_o[1] - this->position[1] )
                + ( R_o[2] - this->position[2] ) * ( R_o[2] - this->position[2] )
                - ( this->radius * this->radius );

        float discriminant = ( B * B ) - ( 4 * C );
        
        if ( discriminant < 0 ) {
            return std::numeric_limits<float>::infinity();
        }

        float t0 = ( -1 * B - std::sqrt( discriminant ) ) / 2.0f;
        float t1 = ( -1 * B + std::sqrt( discriminant ) ) / 2.0f;

        //std::cout << "Sphere A: " <<  A << " B: " << B << " C: " << C << " Discriminant: " << discriminant << " t0: " << t0 << " t1: " << t1 << "\n";
        
        if ( t1 < t0 && t1 >= 0 ) 
            return t1;
        else if ( t0 >= 0 ) 
            return t0;
        
        return std::numeric_limits<float>::infinity();

    }
    virtual std::string getShapeType() {
        std::string shapeType = "Sphere";
        return shapeType;
    }
    void setRadius(  float rds ) {
        this->radius = rds;
        //std::cout << "Notice: Assigned radius property to sphere.\n";
    }

};

struct plane : shape {

    float *normal;

    float intersect( float *R_o, float *R_d ) {
        
        float magnitude = v3_length( this->position );
        float t = ( this->normal[0] * R_o[0] + this->normal[1] * R_o[1] + this->normal[2] * R_o[2] + magnitude ) 
                / ( this->normal[0] * R_d[0] + this->normal[1] * R_d[1] + this->normal[2] * R_d[2] );

        //std::cout << "Plane t: " << t << "\n";

        if ( t >= 0 ) 
            return t; 

        return std::numeric_limits<float>::infinity();
    }
    virtual std::string getShapeType() {
        std::string shapeType = "Plane";
        return shapeType;
    }
    void setNormal(  float *nml ) {
        this->normal = nml;
        //std::cout << "Notice: Assigned normal property to plane.\n";
    }

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

                float *position = new float[3];
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

                float *normal = new float[3];
                fscanf( stream, "%f %f %f", &( normal[0] ), &( normal[1] ), &( normal[2] ) );
                // std::cout << "SCANNED NORMAL\n";
                ( *objects )[ objectsTableIndex ]->setNormal( normal );
                

            }
            else if ( tempPropString == "c_diff:" ) {

                float *cDiff = new float[3];
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
            
            /*
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
                    std::cerr << "Error: Undefined object in array:" << shapeTypeString << "\n";
                    return 1;
                }

            }
            */

            int imgWidth = std::stof(argv[1] );
            int imgHeight = std::stof( argv[2] );
            float R_o[3] = { 0, 0, 0 };

            uint8_t *pixmap;

            for ( int imgY=0; imgY<imgHeight; imgY++ ) {

                float rDistY = -0.5f * camera.height + imgY * ( camera.height / imgHeight ) + ( camera.height / imgHeight ) / 2.0f;
                
                for ( int imgX=0; imgX<imgWidth; imgX++ ) {

                    float rDistX = -0.5f * camera.width + imgX * ( camera.width / imgWidth ) + ( camera.width / imgWidth ) / 2.0f;
                    
                    float rVector[3] = { rDistX, rDistY, -1 }; // DO I NEED -Z VECTOR?
                    float R_d[3] = { 0, 0, 0 };
                    v3_normalize( R_d, rVector );

                    // std::cout << "\n" << R_d[0] << " " << R_d[1] << " " << R_d[2] << "\n";

                    float closestT = std::numeric_limits<float>::infinity();
                    int closestObjectIndex = -1;

                    // std::cout << "\nLoop objects\n";

                    for ( int index=0; index<numberOfShapes; index++ ) {

                        std::string objectType = objects[ index ]->getShapeType();
                        float intersectedT;

                        if ( objectType == "Sphere" ) {
                            intersectedT = objects[ index ]->intersect( R_o, R_d );
                        }
                        else if ( objectType == "Plane" ) {
                            intersectedT = objects[ index ]->intersect( R_o, R_d );
                        }
                        else {
                            std::cerr << "Error: Intersection called for invalid object.";
                            return 1;
                        }

                        if ( intersectedT < closestT ) {
                            //std::cout << "Closest object updated: " << intersectedT << " < " << closestT << "\n";
                            closestT = intersectedT;
                            closestObjectIndex = index;
                            //std::cout << "Closest object index is now: " << closestObjectIndex << "\n";
                        }

                        
                    }

                    if ( closestObjectIndex > -1 ) {

                        std::cout << "Set pixel color to " << objects[ closestObjectIndex ]->cDiff[0] << ", " << objects[ closestObjectIndex ]->cDiff[1] << ", " << objects[ closestObjectIndex ]->cDiff[2] << "\n";
                    
                    }
                    else {

                        std::cout << "Set pixel color to black (0, 0, 0).\n";

                    }
                        

                }
            }
            
        }

        // Deallocate
        for ( int index=0; index<numberOfShapes; index++ )
            delete objects[ index ];
        delete[] objects;

    }

    return 0;
}
