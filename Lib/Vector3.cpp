// Create by Villerot Justin 23/09/25

#include "Vector3.h"
#include "math_common.h"

#include <complex>
#include <stdexcept>
#include <utility>

namespace math {

  // Static constant definitions
  const Vector3 Vector3::ZERO(0.0, 0.0, 0.0);
  const Vector3 Vector3::UNIT_X(1.0, 0.0, 0.0);
  const Vector3 Vector3::UNIT_Y(0.0, 1.0, 0.0);
  const Vector3 Vector3::UNIT_Z(0.0, 0.0, 1.0);

  // Constructors

  Vector3::Vector3() : Vector(3) {
      values[0] = 0.0;
      values[1] = 0.0;
      values[2] = 0.0;
  }

  Vector3::Vector3(double x, double y, double z) : Vector(3) {
      values[0] = x;
      values[1] = y;
      values[2] = z;
  }

  Vector3::Vector3(const Vector& v) : Vector(v) {
      if (v.size() != 3) {
          throw std::invalid_argument("Vector must be of size 3 to convert to Vector3");
      }
  }

  // Getter methods

  
  double Vector3::x() const {
      return values[0];
  }

  double Vector3::y() const {
      return values[1];
  }

  double Vector3::z() const {
      return values[2];
  }

  // Setter methods

  void Vector3::setX(double x) {
      values[0] = x;
  }

  void Vector3::setY(double y) {
      values[1] = y;
  }

  void Vector3::setZ(double z) {
      values[2] = z;
  }

  void Vector3::set(double x, double y, double z) {
      values[0] = x;
      values[1] = y;
      values[2] = z;
  }

  // Vector operators conversion
  Vector3 Vector3::operator+(const Vector3& v2) const {
      Vector result = Vector::operator+(v2);
      return Vector3(result);
  }

  Vector3 Vector3::operator-(const Vector3& v2) const {
      Vector result = Vector::operator-(v2);
      return Vector3(result);
  }

  Vector3 Vector3::operator*(double mult) const {
      Vector result = Vector::operator*(mult);
      return Vector3(result);
  }

  //
  // Vector function overrides
  //

  double Vector3::dot(const Vector3& other) const {
      return Vector::dot(other);
  }

  double Vector3::length() const {
      return Vector::length();
  }

  double Vector3::lengthSquared() const {
      return Vector::squared_length();
  }

  Vector3 Vector3::normal() const {
      return Vector3(Vector::normal());
  }

  void Vector3::normalize() {
      double len = this->length();
      if (len == 0) {
          throw std::invalid_argument("Cannot normalize a zero-length vector.");
      }
      for (int i = 0; i < 3; ++i) {
          values[i] /= len;
      }
  }

  //
  // Vector 3 only operations
  // 
  Vector3 Vector3::cross(const Vector3& other) const {
      double cx = y() * other.z() - z() * other.y();
      double cy = z() * other.x() - x() * other.z();
      double cz = x() * other.y() - y() * other.x();
      return Vector3(cx, cy, cz);
  }

  bool Vector3::parallel(const Vector3& other) const {
      Vector3 crossProd = this->cross(other);
      return crossProd.zero();
  }

  // Function to calculate the angle (in radians) between two 3D vectors
  double Vector3::angle(const Vector3& other) const {
      double dotProd = this->dot(other);
      double magnitudes = this->length() * other.length();
      if (magnitudes == 0) {
          throw std::invalid_argument("Cannot calculate angle with zero-length vector.");
      }
      return std::acos(dotProd / magnitudes);  // Angle formula
  }



}
