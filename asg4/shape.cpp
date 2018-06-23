// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}


text::text (void* glut_bitmap_font, const string& textdata):
   glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
         dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): 
    ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
           polygon({{width, height}, {width, 0}, {0,0}, 
                    {0, height}}){

   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (const GLfloat width, const GLfloat height): 
    polygon({{0,0}, {width/2, height/2}, 
                    {width, 0}, {width/2, 0 - height/2}}) {
    DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto text = reinterpret_cast<const GLubyte*> (textdata.c_str());
   glColor3ubv (color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, text);
  // glutSwapBuffers();
}

void text::showNum(const vertex& center, const rgbcolor& color,
                   const size_t& num) const {
    if(num < 10) {
        auto text = reinterpret_cast<const GLubyte*> 
                    (to_string(num).c_str());
        glColor3ubv (color.ubvec);
        glRasterPos2f (center.xpos, center.ypos);
        glutBitmapString (GLUT_BITMAP_8_BY_13, text);
        //glutSwapBuffers();
    }
}

void text::draw_border (const vertex& center, const rgbcolor& color,
                        const GLfloat& width, 
                        const size_t& number) const {
   auto text = reinterpret_cast<const GLubyte*> (textdata.c_str());
   size_t length = glutBitmapLength (glut_bitmap_font, text);
   size_t height = glutBitmapHeight (glut_bitmap_font);
   glColor3ubv (color.ubvec);
   glLineWidth(width);
   glBegin (GL_LINE_LOOP);
   glVertex2f (center.xpos - width, 
               center.ypos - width);
   glVertex2f (center.xpos - width, 
               center.ypos + height + width);
   glVertex2f (center.xpos + length + width, 
               center.ypos + height + width);
   glVertex2f (center.xpos + length + width, 
               center.ypos - width);
   glEnd();
   vertex numVert = {(center.xpos + (length /2)),
                     (center.ypos + (height /2))};
   text::showNum(numVert,color,number);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   int points = 60;
   const GLfloat theta = 2.0 * M_PI / points;
   glBegin(GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   for (GLfloat point = 0; point < 2.0 * M_PI; point += theta) {
      glVertex2f (dimension.xpos/2 * cos(point) + center.xpos,
                  dimension.ypos/2 * sin(point) + center.ypos);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::showNum(const vertex& center, const rgbcolor& color,
                      const size_t& num) const {
    if(num < 10) {
        auto text = reinterpret_cast<const GLubyte*> 
                    (to_string(num).c_str());
        glColor3ubv (color.ubvec);
        glRasterPos2f (center.xpos, center.ypos);
        glutBitmapString (GLUT_BITMAP_8_BY_13, text);
        glEnd();
        //glutSwapBuffers();
    }
}

void ellipse::draw_border (const vertex& center, const rgbcolor& color,
                           const GLfloat& width, 
                           const size_t& number) const {
   int points = 64;
   const GLfloat theta = 2.0 * M_PI / points;
   showNum(center,color,number);
   glLineWidth(width);
   glEnable (GL_LINE_SMOOTH);
   glBegin (GL_LINE_LOOP);
   glColor3ubv(color.ubvec);
   for (GLfloat point = 0; point < 2.0 * M_PI; point += theta) {
      glVertex2f (((dimension.xpos + width)/2 * 
                  (cos(point) + center.xpos)),
                  ((dimension.ypos + width)/2 * 
                  (sin(point) + center.ypos)));
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   for(auto vert: vertices) {
       glVertex2f (vert.xpos + center.xpos,
                   vert.ypos + center.ypos);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::showNum(const vertex& center, const rgbcolor& color,
                      const size_t& num) const {
    if(num < 10) {
        auto text = reinterpret_cast<const GLubyte*> 
                    (to_string(num).c_str());
        glColor3ubv (color.ubvec);
        glRasterPos2f (center.xpos, center.ypos);
        glutBitmapString (GLUT_BITMAP_8_BY_13, text);
        //glutSwapBuffers();
    }
}

void polygon::draw_border (const vertex& center, const rgbcolor& color,
                           const GLfloat& width,
                           const size_t& number) const {
   glLineWidth(width);
   glBegin (GL_LINE_LOOP);
   glColor3ubv (color.ubvec);
   float xAvg = 0;
   float yAvg = 0;
   int numVerts = 0;
   for(auto vert: vertices) {
       glVertex2f (vert.xpos + center.xpos,
                   vert.ypos + center.ypos);
       xAvg += vert.xpos + center.xpos;
       yAvg += vert.ypos + center.ypos;
       numVerts += 1;
   }
   if(numVerts > 1) {
       xAvg = xAvg / numVerts;
       yAvg = yAvg / numVerts;
   }
   glEnd();
   vertex numVert = {xAvg, yAvg};
   showNum(numVert,color,number);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

triangle::triangle (const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);
}

equilateral::equilateral(const GLfloat width): triangle(
    {{0,0}, {width / 2, width}, {width,0}}) {
    DEBUGF ('c', this);
}

right_triangle::right_triangle(const GLfloat width): triangle(
    {{0,0}, {width, 0}, {0,width}}) {
    DEBUGF ('c', this);
}

isosceles::isosceles(const GLfloat width): triangle(
    {{0,0}, {width / 2, width}, {width /2,0}}) {
    DEBUGF ('c', this);
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

