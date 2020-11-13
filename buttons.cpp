#include "D:\Txlib\TX\TXlib.h"
#include <stdlib.h>
#include <string>
#include <cmath>

RGBQUAD* Video_memory = NULL;

struct plot {
	double x, y, z;
};


class CoordSys {
	plot coords0_, coords1_;
	double scaleX_, scaleY_;
	const char* signature_;
public:
	plot to_pixels(plot coords);
	void draw_pixel(plot coords, plot color_of_point, double size_of_window);
	void draw_point(plot coords, COLORREF color_of_point, double R);
	void draw_window();
	void draw_grid();
	void draw_axis();
	void set_color_back();
	void write_signature();
	CoordSys(plot coords0, plot coords1, double scaleX, double scaleY, const char* signature) {
		coords0_ = coords0;
		coords1_ = coords1;
		signature_ = signature;
		scaleX_ = scaleX;
		scaleY_ = scaleY;
	}
};


class Vector {
public:
	plot coords_;
	double length();
	void turn_vector_counter_clock_wise(double degree);
	void turn_vector_clock_wise(double degree);
	void normalization();
	Vector(plot coords) {
		coords_.x = coords.x;
		coords_.y = coords.y;
	}
};

typedef double (*func_t) (double x);

class Button {
	plot coords0_, coords1_;
public:
	func_t function_;
	const char* name_;
	void draw_button();
	bool if_button_pressed();
	Button(plot coords0, plot coords1, const char* name, func_t function) {
		coords0_ = coords0;
		coords1_ = coords1;
		name_ = name;
		function_ = function;
	}
};

void create_working_space(CoordSys vector_space, Button buttons[]);
void draw_graphic(CoordSys vector_space, func_t function, COLORREF color);
Vector operator + (Vector a, Vector b);
Vector operator * (double koef_of_length, Vector a);
double operator * (Vector a, Vector b);


int main() {
	txCreateWindow(1300, 700);
	txBegin();
	CoordSys vector_space({ 0, 0 }, { 1300, 500 }, 25, 35, "vector space");
	Button buttons[] = { 	Button({ 200,  550 }, { 300, 600 }, "sin", sin),
				Button({ 350,  550 }, { 450, 600 }, "cos", cos),
				Button({ 500,  550 }, { 600, 600 }, "tg", tan),
				Button({ 650,  550 }, { 750, 600 }, "abs", abs),
				Button({ 800,  550 }, { 900, 600 }, "log", log),
				Button({ 950,  550 }, { 1050, 600 }, "log10", log10),
				Button({ 0,  0 }, { 0, 0 }, NULL, NULL) };
	
	create_working_space(vector_space, buttons);
	while (txMouseButtons() != 3) {
		for (int i = 0; buttons[i].name_ != NULL; i++) {
			if (buttons[i].if_button_pressed()) {
				draw_graphic(vector_space, buttons[i].function_, RGB(0, 191, 255));
			}
		}
		txSleep();
	}
}

void create_working_space(CoordSys vector_space, Button buttons[]) {
	vector_space.draw_window();
	vector_space.draw_grid();
	vector_space.draw_axis();
	vector_space.set_color_back();
	//vector_space.write_signature();
	
	for (int i = 0; buttons[i].name_ != NULL; i++) {
		buttons[i].draw_button();
	}
}


void draw_graphic(CoordSys vector_space, func_t function, COLORREF color)
{
	for (double x = 0.1; x <= 600; x = x + 0.1)
	{
		vector_space.draw_point({ x,function(x) }, color, 2);
	}
}

double Vector::length() {
	return sqrt(coords_.x * coords_.x + coords_.y * coords_.y);
}


void Vector::turn_vector_counter_clock_wise(double degree) {
	plot new_coords = { 0, 0 };
	new_coords.x = coords_.x * cos(degree) - coords_.y * sin(degree);
	new_coords.y = coords_.x * sin(degree) + coords_.y * cos(degree);
	coords_ = new_coords;
}


void Vector::turn_vector_clock_wise(double degree) {
	plot new_coords = { 0, 0 };
	new_coords.x = coords_.x * cos(degree) + coords_.y * sin(degree);
	new_coords.y = -coords_.x * sin(degree) + coords_.y * cos(degree);
	coords_ = new_coords;
}


void Vector::normalization() {
	Vector temp_vector = *this;
	double length = temp_vector.length();
	temp_vector = 20 / length * temp_vector;
	coords_.x = temp_vector.coords_.x;
	coords_.y = temp_vector.coords_.y;
}

plot CoordSys::to_pixels(plot coords) {
	plot start_of_coord = { coords1_.x / 2, coords1_.y / 2 };
	plot rec_coord = { coords.x * scaleX_ + start_of_coord.x, start_of_coord.y - coords.y * scaleY_ };
	return rec_coord;
}


void CoordSys::draw_point(plot coords, COLORREF color_of_point, double R) {
	txSetFillColor(color_of_point);
	txSetColor(color_of_point);
	plot rec_coord = to_pixels(coords);
	txCircle(rec_coord.x, rec_coord.y, R);
}


void CoordSys::draw_pixel(plot coords, plot color_of_point, double size_of_window) {
	plot rec_coord = to_pixels(coords);
	RGBQUAD* pixel = &Video_memory[(int)rec_coord.x + ((int)size_of_window - (int)rec_coord.y) * (int)size_of_window];
	pixel->rgbRed = color_of_point.x;
	pixel->rgbGreen = color_of_point.y;
	pixel->rgbBlue = color_of_point.z;
}


void CoordSys::draw_window() {
	txSetColor(TX_LIGHTGREEN);

	txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
}

void CoordSys::draw_grid() {
	for (int line = coords0_.x; line < coords1_.x; line += 10) {
		txLine(line, coords0_.y, line, coords1_.y);
	}
	for (int line = coords0_.y; line < coords1_.y; line += 10) {
		txLine(coords0_.x, line, coords1_.x, line);
	}
}


void CoordSys::draw_axis() {
	txSetColor(TX_BLACK);

	txLine(coords1_.x / 2, coords0_.y, coords1_.x / 2, coords1_.y);
	txLine(coords0_.x, coords1_.y / 2, coords1_.x, coords1_.y / 2);

	txLine(coords1_.x / 2, coords0_.y, coords1_.x / 2 + 5, coords0_.y + 5);
	txLine(coords1_.x / 2, coords0_.y, coords1_.x / 2 - 5, coords0_.y + 5);

	txLine(coords1_.x - 5, coords1_.y / 2 + 5, coords1_.x, coords1_.y / 2);
	txLine(coords1_.x - 5, coords1_.y / 2 - 5, coords1_.x, coords1_.y / 2);
}


void CoordSys::set_color_back() {
	txSetFillColor(TX_WHITE);
}


void CoordSys::write_signature() {
	txDrawText(coords0_.x - 30, coords1_.y + 15, coords1_.x, coords1_.y + 30, signature_);
}


void Button::draw_button() {
	txSetColor(TX_LIGHTGREEN);
	txSetFillColor(TX_BLACK);
	txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
}


bool Button::if_button_pressed() {
	if (txMouseButtons() == 1 &&
		txMouseX() >= coords0_.x &&
		txMouseX() <= coords1_.x &&
		txMouseY() >= coords0_.y &&
		txMouseY() <= coords1_.y) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


Vector operator + (Vector a, Vector b) {
	plot coords_of_new_vector = { a.coords_.x + b.coords_.x, a.coords_.y + b.coords_.y };
	return Vector(coords_of_new_vector);
}


Vector operator * (double koef_of_length, Vector a) {
	plot coords_of_new_vector = { koef_of_length * a.coords_.x, koef_of_length * a.coords_.y };
	return Vector(coords_of_new_vector);
}


double operator * (Vector a, Vector b) {
	return (a.coords_.x * b.coords_.x + a.coords_.y * b.coords_.y);
}

