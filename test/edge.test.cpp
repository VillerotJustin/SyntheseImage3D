#include "../Lib/Geometry/Edge.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace geometry;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

bool isEqual(const Vector3D& a, const Vector3D& b, double epsilon = 1e-9) {
    return isEqual(a.x(), b.x(), epsilon) && 
           isEqual(a.y(), b.y(), epsilon) && 
           isEqual(a.z(), b.z(), epsilon);
}

// Test function declarations
void testEdgeConstructors();
void testEdgeBasicProperties();
void testEdgeGeometricOperations();
void testEdgeValidation();
void testEdgeOperators();
void testEdgeTransformations();
void testEdgeStaticMethods();
void testEdgeIntersections();

int main() {
    std::cout << "=== Edge Test Suite ===" << std::endl;
    
    try {
        testEdgeConstructors();
        std::cout << "✓ Edge constructors test passed" << std::endl;
        
        testEdgeBasicProperties();
        std::cout << "✓ Edge basic properties test passed" << std::endl;
        
        testEdgeGeometricOperations();
        std::cout << "✓ Edge geometric operations test passed" << std::endl;
        
        testEdgeValidation();
        std::cout << "✓ Edge validation test passed" << std::endl;
        
        testEdgeOperators();
        std::cout << "✓ Edge operators test passed" << std::endl;
        
        testEdgeTransformations();
        std::cout << "✓ Edge transformations test passed" << std::endl;
        
        testEdgeStaticMethods();
        std::cout << "✓ Edge static methods test passed" << std::endl;
        
        testEdgeIntersections();
        std::cout << "✓ Edge intersections test passed" << std::endl;
        
        std::cout << "\n🎉 All Edge tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testEdgeConstructors() {
    // Test parameterized constructor
    Vector3D start(1, 2, 3);
    Vector3D end(4, 5, 6);
    Edge edge1(start, end);
    
    assert(isEqual(edge1.getStart(), start));
    assert(isEqual(edge1.getEnd(), end));
    
    // Test default constructor
    Edge edge2;
    assert(isEqual(edge2.getStart(), Vector3D::ZERO));
    assert(isEqual(edge2.getEnd(), Vector3D::ZERO));
}

void testEdgeBasicProperties() {
    Vector3D start(0, 0, 0);
    Vector3D end(3, 4, 0);
    Edge edge(start, end);
    
    // Test direction
    Vector3D expectedDirection(3, 4, 0);
    assert(isEqual(edge.getDirection(), expectedDirection));
    
    // Test length
    double expectedLength = 5.0; // 3-4-5 triangle
    assert(isEqual(edge.getLength(), expectedLength));
    assert(isEqual(edge.getLengthSquared(), 25.0));
    
    // Test midpoint
    Vector3D expectedMidpoint(1.5, 2, 0);
    assert(isEqual(edge.getMidpoint(), expectedMidpoint));
    
    // Test normalized direction
    Vector3D expectedNormalized(0.6, 0.8, 0);
    assert(isEqual(edge.getDirectionNormalized(), expectedNormalized));
}

void testEdgeGeometricOperations() {
    Vector3D start(0, 0, 0);
    Vector3D end(10, 0, 0);
    Edge edge(start, end);
    
    // Test parametric points
    assert(isEqual(edge.getPointAtParameter(0.0), start));
    assert(isEqual(edge.getPointAtParameter(1.0), end));
    assert(isEqual(edge.getPointAtParameter(0.5), Vector3D(5, 0, 0)));
    
    // Test closest point
    Vector3D testPoint(5, 3, 0);
    Vector3D closestPoint = edge.closestPointTo(testPoint);
    assert(isEqual(closestPoint, Vector3D(5, 0, 0)));
    
    // Test distance
    double distance = edge.distanceTo(testPoint);
    assert(isEqual(distance, 3.0));
    
    // Test point containment
    assert(edge.containsPoint(Vector3D(5, 0, 0)));
    assert(!edge.containsPoint(Vector3D(5, 1, 0)));
    assert(edge.containsPoint(Vector3D(5, 0.0001, 0), 0.001)); // Within tolerance
}

void testEdgeValidation() {
    // Valid edge
    Edge validEdge(Vector3D(0, 0, 0), Vector3D(1, 1, 1));
    assert(validEdge.isValid());
    assert(!validEdge.isDegenerate());
    
    // Degenerate edge
    Vector3D point(5, 5, 5);
    Edge degenerateEdge(point, point);
    assert(!degenerateEdge.isValid());
    assert(degenerateEdge.isDegenerate());
}

void testEdgeOperators() {
    Vector3D start(1, 2, 3);
    Vector3D end(4, 5, 6);
    Edge edge1(start, end);
    Edge edge2(start, end);
    Edge edge3(end, start);
    
    // Test equality
    assert(edge1 == edge2);
    assert(edge1 != edge3);
}

void testEdgeTransformations() {
    Vector3D start(0, 0, 0);
    Vector3D end(2, 0, 0);
    Edge edge(start, end);
    
    // Test translation
    Vector3D translation(1, 1, 1);
    Edge translated = edge.translate(translation);
    assert(isEqual(translated.getStart(), Vector3D(1, 1, 1)));
    assert(isEqual(translated.getEnd(), Vector3D(3, 1, 1)));
    
    // Test scaling
    Edge scaled = edge.scale(2.0);
    assert(isEqual(scaled.getStart(), Vector3D(-1, 0, 0)));
    assert(isEqual(scaled.getEnd(), Vector3D(3, 0, 0)));
    assert(isEqual(scaled.getLength(), 4.0));
    
    // Test reverse
    Edge reversed = edge.reverse();
    assert(isEqual(reversed.getStart(), end));
    assert(isEqual(reversed.getEnd(), start));
}

void testEdgeStaticMethods() {
    // Test X-axis edge
    Edge xEdge = Edge::createXAxis(5.0);
    assert(isEqual(xEdge.getStart(), Vector3D::ZERO));
    assert(isEqual(xEdge.getEnd(), Vector3D(5, 0, 0)));
    
    // Test Y-axis edge with origin
    Vector3D origin(1, 1, 1);
    Edge yEdge = Edge::createYAxis(3.0, origin);
    assert(isEqual(yEdge.getStart(), origin));
    assert(isEqual(yEdge.getEnd(), Vector3D(1, 4, 1)));
    
    // Test Z-axis edge
    Edge zEdge = Edge::createZAxis(2.0);
    assert(isEqual(zEdge.getStart(), Vector3D::ZERO));
    assert(isEqual(zEdge.getEnd(), Vector3D(0, 0, 2)));
}

void testEdgeIntersections() {
    // Test intersecting edges
    Edge edge1(Vector3D(0, 0, 0), Vector3D(2, 0, 0));
    Edge edge2(Vector3D(1, -1, 0), Vector3D(1, 1, 0));
    
    assert(edge1.intersects(edge2));
    auto intersection = edge1.intersectionPoint(edge2);
    assert(intersection.has_value());
    assert(isEqual(intersection.value(), Vector3D(1, 0, 0)));
    
    // Test non-intersecting edges
    Edge edge3(Vector3D(0, 2, 0), Vector3D(2, 2, 0));
    assert(!edge1.intersects(edge3));
    assert(!edge1.intersectionPoint(edge3).has_value());
    
    // Test parallel edges
    Edge edge4(Vector3D(0, 1, 0), Vector3D(2, 1, 0));
    assert(!edge1.intersects(edge4));
    
    // Test collinear overlapping edges
    Edge edge5(Vector3D(1, 0, 0), Vector3D(3, 0, 0));
    assert(edge1.intersects(edge5));
    auto overlapIntersection = edge1.intersectionPoint(edge5);
    assert(overlapIntersection.has_value());
}