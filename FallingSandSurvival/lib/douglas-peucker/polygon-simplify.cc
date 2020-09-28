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

#include "polygon-simplify.hh"

/* std::vector<b2Vec2> is just a vector<Point>, methods do exactly what you think. */

/* Polyline Simplification Algorithm */

void DouglasPeucker::simplify_section(const std::vector<b2Vec2>& pts,
	float tolerance,
	size_t i, size_t j,
	std::vector<bool>* mark_map,
	size_t omitted){
	/* make sure we always return 2 points. */
	if (pts.size() - omitted <= 2)
		return;

	assert(mark_map && mark_map->size() == pts.size());

	if ((i + 1) == j) {
		return;
	}

	float max_distance = -1.0f;
	size_t max_index = i;

	for (size_t k = i + 1; k < j; k++) {
		float distance = pDistance(pts[k].x, pts[k].y, pts[i].x, pts[i].y, pts[j].x, pts[j].y);

		if (distance > max_distance) {
			max_distance = distance;
			max_index = k;
		}
	}

	if (max_distance <= tolerance) {
		for (size_t k = i + 1; k < j; k++) {
			(*mark_map)[k] = false;
			++omitted;
		}
	}
	else {
		simplify_section(pts, tolerance, i, max_index, mark_map, omitted);
		simplify_section(pts, tolerance, max_index, j, mark_map, omitted);
	}
}


std::vector<b2Vec2> DouglasPeucker::simplify(const std::vector<b2Vec2>& vertices, float tolerance)
{
	std::vector<bool> mark_map(vertices.size(), true);

	simplify_section(vertices, tolerance, 0, vertices.size() - 1, &mark_map);

	std::vector<b2Vec2> result;
	for (size_t i = 0; i != vertices.size(); ++i) {
		if (mark_map[i]) {
			result.push_back(vertices[i]);
		}
	}

	return result;
}

float DouglasPeucker::pDistance(float x, float y, float x1, float y1, float x2, float y2) {

	float A = x - x1;
	float B = y - y1;
	float C = x2 - x1;
	float D = y2 - y1;

	float dot = A * C + B * D;
	float len_sq = C * C + D * D;
	float param = -1;
	if (len_sq != 0) //in case of 0 length line
		param = dot / len_sq;

	float xx, yy;

	if (param < 0) {
		xx = x1;
		yy = y1;
	}
	else if (param > 1) {
		xx = x2;
		yy = y2;
	}
	else {
		xx = x1 + param * C;
		yy = y1 + param * D;
	}

	float dx = x - xx;
	float dy = y - yy;
	return sqrt(dx * dx + dy * dy);
}
