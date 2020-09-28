/**
 * Private utility methods for a simple implementation of the marching squares
 * algorithm that can identify
 * 
 * @author Tom Gibara
 * Ported to C++ by Juha Reunanen
 * 
 */

#pragma once

namespace MarchingSquares {

    bool isSet(int x, int y, int width, int height, unsigned char* data) {
        return x <= 0 || x > width || y <= 0 || y > height
            ? false
            : data[(y - 1) * width + (x - 1)] != 0;
    }
	
    int value(int x, int y, int width, int height, unsigned char* data) {
        int sum = 0;
        if (isSet(x, y, width, height, data)) sum |= 1;
        if (isSet(x + 1, y, width, height, data)) sum |= 2;
        if (isSet(x, y + 1, width, height, data)) sum |= 4;
        if (isSet(x + 1, y + 1, width, height, data)) sum |= 8;
        return sum;
    }

}
