#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"

typedef struct {
	double x, y;
	int size;
} STAR;

const int NUM_STARS = 100;
const double PI = 3.14156;
const int MAX_WINDOWS_IN_FLOOR = 40;
const int MAX_FLOORS = 70;
bool windows[MAX_FLOORS][MAX_WINDOWS_IN_FLOOR] = { false };
const double WATER_LEVEL = -0.3;


STAR stars[NUM_STARS];

void init()
{
	int i, j;
	glClearColor(0.0, 0.0, 0.3, 0); // color of window background
	glOrtho(-1, 1, -1, 1, -1, 1);  // set the coordinates system

	srand(time(0));

	for (i = 0; i < NUM_STARS; i++) {
		stars[i].x = ( - 1000 + rand() % 2001) / 1000.0; // random value in range [-1, 1]
		stars[i].y = - 0.3 + ( rand() % 1300 / 1000.0); // random value in range [-0.3, 1] // the sky stars at -0.3
		stars[i].size = 1 + rand() % 3;
	}

	// set random windows lit
	for (i = 0; i < MAX_FLOORS; i++) {
		for (j = 0; j < MAX_WINDOWS_IN_FLOOR; j++)
			windows[i][j] = rand() % 100 < 70;  // 60%lit windows
	}
}

void DrawWindows(double x, double y, double width, double height) {
	int i, j;
	double wWidth = 0.01 ;
	double wHeight = 0.02;
	double spacing = 0.015;

	int num_floors = (height- 2*spacing) / (wHeight + spacing);
	int num_windows_in_floor = (width - 2*spacing) / (wWidth  + spacing);

	glPointSize(2);
	glBegin(GL_POINTS);
	for (i = 0; i < num_floors; i++) {
		for (j = 0; j < num_windows_in_floor; j++) {
			if (windows[i][j])
				glColor3d(0.8, 0.8, 0);
			else
				glColor3d(0, 0, 0);

			// Calculate window position, considering spacing and offset from edges
			double wx = x + spacing + j * (wWidth + spacing);
			double wy = y - spacing - i * (wHeight + spacing);

			glVertex2d(wx, wy);  // Draw window point at calculated position
		}
	}
	glEnd();

}
void DrawWindowsReflection(double x, double y, double width, double height) {
	int i, j;
	double wWidth = 0.01;
	double wHeight = 0.02;
	double spacing = 0.015;
	double verticalOffset = 0.03; // Introduce a gap before starting the reflection

	// Calculate the number of floors and windows per floor
	int num_floors = (height - 2 * spacing) / (wHeight + spacing);
	int num_windows_in_floor = (width - 2 * spacing) / (wWidth + spacing);

	// Start drawing the reflection of windows
	glPointSize(2);
	glBegin(GL_POINTS);
	for (i = 0; i < num_floors; i++) {
		for (j = 0; j < num_windows_in_floor; j++) {
			if (windows[num_floors - 1 - i][j]) // since the first row of windows in the reflection matches the last row of windows in the regular building
				glColor4f(0.5, 0.5, 0, 0.7);  
			else
				glColor4f(0, 0, 0, 0.5); 

			// Calculate window reflection positions, starting at the waterline with a vertical offset
			double wx = x + spacing + j * (wWidth + spacing);
			double wy = y - height - spacing - i * (wHeight + spacing) - verticalOffset;

			glVertex2d(wx, wy);  // Draw window reflection point at calculated position
		}
	}
	glEnd();
}

void DrawWater() {
	// Water - Use a gradient from dark purple to lighter purple and red
	glBegin(GL_POLYGON);
	glColor3d(0.1, 0.0, 0.3); // Dark purple at the bottom of the water
	glVertex2d(-1, -1); // Bottom left
	glVertex2d(1, -1); // Bottom right

	// Lighter purple-red towards the top of the water
	glColor3d(0.5, 0.2, 0.4); // Lighter purple-red, reflecting the sky colors
	glVertex2d(1, -0.3 + 0.02 * sin(10 * PI * 1)); // Top right with ripple effect
	glVertex2d(-1, -0.3 + 0.02 * sin(10 * PI * -1)); // Top left with ripple effect
	glEnd();

	// Adding reflections of the buildings and windows
	glBegin(GL_LINES);
	for (double x = -1; x <= 1; x += 0.1) {
		// Set a color that simulates building reflections (a darker orange)
		glColor3d(0.4, 0.2, 0.1); // Reflected building color
		glVertex2d(x, -0.3 + 0.01 * sin(20 * PI * x)); // Wavy lines for building reflection
		glVertex2d(x + 0.05, -0.3 + 0.01 * sin(20 * PI * (x + 0.05)));

		// Reflections of the window lights (yellow dots)
		if (int(x * 10) % 2 == 0) { // Create some pattern for window reflection
			glColor3d(0.8, 0.8, 0.1); // Light yellow for window reflection
			glVertex2d(x, -0.3 + 0.01 * sin(20 * PI * x) + 0.01); // Position window reflection slightly higher
			glVertex2d(x + 0.05, -0.3 + 0.01 * sin(20 * PI * (x + 0.05)) + 0.01);
		}
	}
	glEnd();
}
void DrawSky() {
	// Sky - blend of blue, light purple, and light red
	glBegin(GL_POLYGON);
	glColor3d(0.0, 0.0, 0.5);
	glVertex2d(-1, 1); // Top left
	glVertex2d(1, 1); // Top right

	glColor3d(0.5, 0.3, 0.7);
	glVertex2d(1, 0.5); // Mid-right
	glVertex2d(-1, 0.5); // Mid-left
	glEnd();

	glBegin(GL_POLYGON);
	glColor3d(0.5, 0.3, 0.7); // Continuing light purple
	glVertex2d(-1, 0.5); // Mid-left
	glVertex2d(1, 0.5); // Mid-right

	glColor3d(0.8, 0.3, 0.3); 
	glVertex2d(1, -0.3); // Bottom right
	glVertex2d(-1, -0.3); // Bottom left
	glEnd();

	DrawWater();

	// Stars 
	int i;
	glColor3d(1.0, 1.0, 0.0); // Yellow
	for (i = 0; i < NUM_STARS; i++) {
		glPointSize(stars[i].size);
		glBegin(GL_POINTS);
		glVertex2d(stars[i].x, stars[i].y);
		glEnd();
	}
}
void DrawMoon() {
	glColor3d(0.9, 0.9, 1.0);
	double centerX = -0.58;
	double centerY = 0.88;
	double radius = 0.08;

	glBegin(GL_POLYGON); // Drawing the moon as a circle
	for (int i = 0; i < 360; i++) {
		double angle = i * PI / 180.0;
		glVertex2d(centerX + cos(angle) * radius, centerY + sin(angle) * radius);
	}
	glEnd();

	// Moon reflection
	glColor4f(0.75, 0.75, 1.0, 0.05);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		double angle = i * PI / 180.0;
		glVertex2d(centerX + cos(angle) * radius, -centerY + sin(angle) * radius);
	}
	glEnd();
}

void DrawBuildingReflection(double x, double y, double width, double height, bool isFar) {

	/// Calculate the starting point and height for the reflection
	double reflectionStartY = -0.3; // Fixed waterline reflection start
	double reflectionHeight = height; // Same height for the reflection

	// Begin drawing the reflection polygon
	glBegin(GL_POLYGON);
	if (isFar) {
		glColor4f(0.15, 0.15, 0.25, 0.3); // Darker color with low opacity for far reflections
	}
	else {
		glColor4f(0.25, 0.25, 0.35, 0.45); // Lighter color with low opacity for near reflections
	}
	glVertex2d(x, reflectionStartY); // Top-left corner of the reflection
	glVertex2d(x + width, reflectionStartY); // Top-right corner of the reflection

	// Apply a gradient effect towards the bottom
	if (isFar) {
		glColor4f(0.15, 0.1, 0.1, 0.3); // Fading effect for far reflections
	}
	else {
		glColor4f(0.25, 0.15, 0.2, 0.45); // Fading effect for near reflections
	}
	glVertex2d(x + width, reflectionStartY - reflectionHeight); // Bottom-right corner of the reflection
	glVertex2d(x, reflectionStartY - reflectionHeight); // Bottom-left corner of the reflection
	glEnd();

	// Draw the reflection of the windows 
	DrawWindowsReflection(x, y, width, height);
}
void DrawOneBuilding(double x, double y, double width, double height, bool isFar) {
	// Base building color with a gradient for depth
	glBegin(GL_POLYGON);

	if (isFar) 
		glColor3d(0.2, 0.2, 0.3); 
	else 
		glColor3d(0.3, 0.3, 0.45); 
	
	glVertex2d(x, y); // Left top corner
	glVertex2d(x + width, y); // Right top corner

	// Gradient effect towards the bottom
	if (isFar) {
		glColor3d(0.2, 0.1, 0.1); 
	}
	else {
		glColor3d(0.3, 0.15, 0.25); 
	}

	glVertex2d(x + width, y - height); // Right bottom corner
	glVertex2d(x, y - height); // Left bottom corner
	glEnd();

	DrawWindows(x, y, width, height); // Draw windows on the building


	glBegin(GL_POLYGON);
	// Start with brighter orange-peach at the top
	if (isFar) {
		glColor3d(1.0, 0.7, 0.4); // Brighter orange-peach for far buildings
	}
	else {
		glColor3d(1.0, 0.7, 0.4); // Same for near buildings to keep consistency
	}

	glVertex2d(x + width - 0.03, y); // Near the right top corner
	glVertex2d(x + width, y); // Right top corner

	// Transition to a much darker orange-red at the bottom
	if (isFar) {
		glColor3d(0.6, 0.3, 0.15); // Darker reddish-orange for far buildings
	}
	else {
		glColor3d(0.7, 0.35, 0.2); // Darker orange-red for near buildings
	}

	glVertex2d(x + width, y - height); // Right bottom corner
	glVertex2d(x + width - 0.03, y - height); // Near the right bottom corner
	glEnd();

}

void DrawBridge() {

	double startX = -1.0, endX = 1.0;  // Start and end positions for the full-city bridge
	double archHeight = 0.3;
	double roadHeight = -0.11, roadThickness = 0.02;

	glColor3d(0.7, 0.7, 0.7);

	// Draw the bridge deck
	glBegin(GL_POLYGON);
	glVertex2d(startX, roadHeight); // left-bottom
	glVertex2d(endX, roadHeight); // right-bottom
	glVertex2d(endX, roadHeight + roadThickness); // right-top
	glVertex2d(startX, roadHeight + roadThickness); // left-top 
	glEnd();


	// Arch of the bridge
	double x, y;
	glLineWidth(5.0); // Set the line width to 5.0 
	glBegin(GL_LINE_STRIP);
	for (x = startX; x < endX; x += 0.01) {
		y = -fabs(cos(x * 5) / 5) + archHeight;
		glVertex2d(x, y);

	}
	glEnd();

	// Draw the vertical support cables
	glLineWidth(1.0); // Reset the line width to 1.0
	glColor3d(0.2, 0.2, 0.2); // Dark gray color for the vertical cables
	glBegin(GL_LINES);
	for (double x = startX; x <= endX; x += 0.04) {
		double y = -fabs(cos(x * 5) / 5) + archHeight;
		glVertex2d(x, y);
		glVertex2d(x, roadHeight);
	}
	glEnd();

	// Draw Poles of the bridge
	glLineWidth(5.0); // Reset the line width to 1.0
	glColor3d(0.2, 0.2, 0.2); // Dark gray color for the vertical cables
	glBegin(GL_LINES);
	for (double x = ((-3 * PI) / 10); x <= endX; x += (2 * PI) / 10) {
		double y = -fabs(cos(x * 5) / 5) + archHeight;
		glVertex2d(x, y);
		glVertex2d(x, WATER_LEVEL);
	}
	glEnd();


}
void DrawBridgeReflection() {

	glEnable(GL_BLEND);

	double startX = -1.0, endX = 1.0;  // Start and end positions for the full-city bridge
	double archHeight = -0.9;
	double roadHeight = -0.49, roadThickness = 0.02;
	double reflectionValue = 0.45;

	glColor4f(0.5, 0.5, 0.5, reflectionValue);

	// Draw the bridge deck
	glBegin(GL_POLYGON);
	glVertex2d(startX, roadHeight); // left-bottom
	glVertex2d(endX, roadHeight); // right-bottom
	glVertex2d(endX, roadHeight + roadThickness); // right-top
	glVertex2d(startX, roadHeight + roadThickness); // left-top 
	glEnd();


	// Arch of the bridge
	double x, y;
	glLineWidth(5.0); // Set the line width to 5.0
	glBegin(GL_LINE_STRIP);
	for (x = startX; x < endX; x += 0.01) {
		y = fabs(cos(x * 5) / 5) + archHeight;
		glVertex2d(x, y);

	}
	glEnd();

	// Draw the vertical support cables
	glLineWidth(1.0); // Reset the line width to 1.0
	glColor4f(0.1, 0.1, 0.1, reflectionValue); // Dark gray color for the vertical cables
	glBegin(GL_LINES);
	for (double x = startX; x <= endX; x += 0.04) {
		double y = fabs(cos(x * 5) / 5) + archHeight;
		glVertex2d(x, y);
		glVertex2d(x, roadHeight);
	}
	glEnd();

	// Draw Poles of the bridge
	glLineWidth(5.0); // Reset the line width to 1.0
	glColor4f(0.1, 0.1, 0.1, reflectionValue); // Dark gray color for the vertical cables
	glBegin(GL_LINES);
	for (double x = ((-3 * PI) / 10); x <= endX; x += (2 * PI) / 10) {
		double y = fabs(cos(x * 5) / 5) + archHeight;
		glVertex2d(x, y);
		glVertex2d(x, WATER_LEVEL);
	}
	glEnd();

	glDisable(GL_BLEND);

}

void DrawBuildings() {
	double x, y;

	// Far buildings - draw first
	for (x = -1; x <= 1; x += 0.18) {
		y = 0.6 * cos(0.4 * PI * x) + 0.25 * sin(7 * PI * x);
		DrawOneBuilding(x, y, 0.13, y + 0.3, true);  // Far buildings
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		DrawBuildingReflection(x, y, 0.13, y + 0.3, true);
		glDisable(GL_BLEND);
	}
	DrawBridge();
	DrawBridgeReflection();
	// Near buildings - draw after far buildings
	for (x = -1; x <= 1; x += 0.21) {
		y = 0.35 * cos(0.4 * PI * x) + 0.12 * sin(7 * PI * x);
		DrawOneBuilding(x, y - 0.1, 0.16, y + 0.2, false);  // Near buildings
		DrawBuildingReflection(x, y - 0.1, 0.16, y + 0.2, false);
	
	}
}

void DrawRoadReflection() {
	// Draw the base of the road reflection
	glColor3d(0.5, 0.5, 0.5); // Dark gray for the base
	glBegin(GL_POLYGON);
	glVertex2d(-1, -0.34); // Left bottom
	glVertex2d(1, -0.34);  // Right bottom
	glVertex2d(1, -0.32);  // Right top
	glVertex2d(-1, -0.32); // Left top
	glEnd();

	// Reflect vertical posts for railings
	glColor4d(0.6, 0.6, 0.6, 0.3); // Light gray with transparency for posts reflection
	glLineWidth(2.0);
	glBegin(GL_LINES);
	for (double x = -1; x <= 1; x += 0.1) {
		glVertex2d(x, -0.34); // Bottom of the post reflection
		glVertex2d(x, -0.40); // Top of the post reflection
	}
	glEnd();

	// Reflect horizontal railings
	glColor4d(0.7, 0.7, 0.7, 0.5); // Light gray with transparency for railings reflection
	glBegin(GL_LINES);
	for (double y = -0.37; y >= -0.39; y -= 0.02) {
		glVertex2d(-1, y);  // Left end of the railing reflection
		glVertex2d(1, y);   // Right end of the railing reflection
	}
	glEnd();


	// Reflect the lights along the road
	glColor4d(1.0, 0.9, 0.5, 0.2); // Yellowish color for lights with low opacity
	glPointSize(3); // Size of the lights reflection
	glBegin(GL_POINTS);
	for (double x = -0.95; x <= 0.95; x += 0.1) {
		glVertex2d(x, -0.335); // Positioning the lights reflection slightly below the road
	}
	glEnd();
}
void DrawRoad() {
	// Draw the base of the bridge
	glColor3d(0.5, 0.5, 0.5); // Dark gray for the base
	glBegin(GL_POLYGON);
	glVertex2d(-1, -0.3); 
	glVertex2d(1, -0.3);  
	glVertex2d(1, -0.28); 
	glVertex2d(-1, -0.28); 
	glEnd();

	// Draw vertical posts for railings
	glColor3d(0.6, 0.6, 0.6); // Light gray for posts
	glLineWidth(2.0);
	glBegin(GL_LINES);
	for (double x = -1; x <= 1; x += 0.1) {
		glVertex2d(x, -0.28); // Bottom of the post
		glVertex2d(x, -0.22); // Top of the post
	}
	glEnd();

	// Draw horizontal railings
	glColor3d(0.7, 0.7, 0.7); // Light gray for railings
	glBegin(GL_LINES);
	for (double y = -0.25; y <= -0.23; y += 0.02) {
		glVertex2d(-1, y);  // Left end of the railing
		glVertex2d(1, y);   // Right end of the railing
	}
	glEnd();

	// Draw bridge deck
	glColor3d(0.3, 0.3, 0.4); // Darker gray for bridge deck
	glBegin(GL_POLYGON);
	glVertex2d(-1, -0.32); // Bottom left corner of the bridge deck
	glVertex2d(1, -0.32);  // Bottom right corner of the bridge deck
	glVertex2d(1, -0.3);   // Top right corner of the bridge deck
	glVertex2d(-1, -0.3);  // Top left corner of the bridge deck
	glEnd();

	// Add lights along the bridge
	glColor3d(1.0, 0.9, 0.5); // Yellowish color for lights
	glPointSize(3); // Size of the lights
	glBegin(GL_POINTS);
	for (double x = -0.95; x <= 0.95; x += 0.1) {
		glVertex2d(x, -0.29); // Positioning the lights along the bridge
	}
	glEnd();

	//  creating a ripple /mirroring effect
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawRoadReflection();
	glDisable(GL_BLEND);
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer (fills memory with background color)
	DrawSky();
	DrawBuildings();
	DrawRoad();
	DrawMoon();
	glutSwapBuffers(); // show all
}
void idle()
{
	int i, j;
	
	// slowly spark of the stars
	if (rand() % 10 <= 1) { // not all frames
		i = rand() % NUM_STARS;
		stars[i].size = 1 + rand() % 3;
	}

	// change random window
	if(rand() % 50 <= 1) { // not all frames
		i = rand() % MAX_FLOORS;
		j = rand() % MAX_WINDOWS_IN_FLOOR;
		windows[i][j] = !windows[i][j];
	}

	
	glutPostRedisplay(); // indirect call to display
}
void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display); // refresh function is "display"
	glutIdleFunc(idle); // any changes put in idle

	init();

	glutMainLoop();
}