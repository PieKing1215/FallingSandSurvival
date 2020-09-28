/*
Copyright 2016 Mike Owens

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <assert.h>
#include <vector>
#include <box2d/b2_math.h>

/* std::vector<b2Vec2> is just a vector<Point>, methods do exactly what you think. */

/* Polyline Simplification Algorithm */
class DouglasPeucker {
public:
	static void simplify_section(const std::vector<b2Vec2>& pts,
		float tolerance,
		size_t i, size_t j,
		std::vector<bool>* mark_map,
		size_t omitted = 0);
	static std::vector<b2Vec2> simplify(const std::vector<b2Vec2>& vertices, float tolerance);
	static float pDistance(float x, float y, float x1, float y1, float x2, float y2);
};
