#include "D:\Txlib\TX\TXlib.h"
#include <stdlib.h>
#include <string>
#include <cmath>

RGBQUAD* Video_memory = NULL;
typedef void (*func_t) (void);
typedef double(*graph_t)(double x);

struct plot {
	double x, y, z;
};


class CoordSys {
	plot coords0_, coords1_;
	double scaleX_, scaleY_;
	const char* signature_;
public:
	void draw_graphic(graph_t function, COLORREF color);
	plot to_pixels(plot coords);
	void draw_pixel(plot coords, plot color_of_point, double size_of_window);
	void draw_point(plot coords, COLORREF color_of_point, double R);
	void draw_window();
	void draw_grid();
	void draw_axis();
	void set_color_back();
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


class BasicButton {
public:
	plot coords0_, coords1_;
	func_t function_;
	const char* name_;
	virtual void draw_button() {
		txSetColor(TX_LIGHTRED);
		txLine(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txLine(coords1_.x, coords0_.y, coords0_.x, coords1_.y);
		txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
	}

	virtual bool if_button_pressed() {
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

	BasicButton(plot coords0, plot coords1, const char* name, func_t function):
		coords0_ (coords0),
		coords1_ (coords1),
		name_ (name),
		function_ (function)
	{}
};

class RectButton : public BasicButton {
public:
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGREEN);
		txSetFillColor(TX_BLACK);
		txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
	}

	RectButton(plot coords0, plot coords1, const char* name, func_t function):
		BasicButton(coords0, coords1, name, function)
	{}
};


class CircButton : public BasicButton {
	double R_ = sqrt((coords1_.x - coords0_.x) * (coords1_.x - coords0_.x) + (coords1_.y - coords0_.y) * (coords1_.y - coords0_.y)) / 3;
	plot coord_of_centre_{ (coords0_.x + coords1_.x) / 2 , (coords0_.y + coords1_.y) / 2 };
public:
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGREEN);
		txSetFillColor(TX_BLACK);
		txCircle(coord_of_centre_.x, coord_of_centre_.y, R_);
		txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
	}

	virtual bool if_button_pressed() override {
		if (txMouseButtons() == 1 && sqrt((txMouseX() - coord_of_centre_.x)*(txMouseX() - coord_of_centre_.x) + 
										  (txMouseY() - coord_of_centre_.y)*(txMouseY() - coord_of_centre_.y)) < R_) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	CircButton(plot coords0, plot coords1, const char* name, func_t function) :
		BasicButton(coords0, coords1, name, function)
	{}
};


class EllipseButton : public BasicButton {
	plot coord_of_centre_{ (coords0_.x + coords1_.x) / 2 , (coords0_.y + coords1_.y) / 2 };
	double a_ = (coords1_.x - coords0_.x) / 2;
	double b_ = (coords1_.y - coords0_.y) / 2;
public:
	virtual void draw_button() override {
		txSetColor(TX_LIGHTGREEN);
		txSetFillColor(TX_BLACK);
		txEllipse(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
		txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
	}
	virtual bool if_button_pressed() override {
		if (txMouseButtons() == 1 && ((txMouseX() - coord_of_centre_.x) * (txMouseX() - coord_of_centre_.x)) / (a_*a_) +
									 ((txMouseY() - coord_of_centre_.y) * (txMouseY() - coord_of_centre_.y)) / (b_*b_) < 1) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	EllipseButton(plot coords0, plot coords1, const char* name, func_t function) :
		BasicButton(coords0, coords1, name, function)
	{}
};

const int Number_of_buttons = 50;


class ManagerButton {
public:
	BasicButton* buttons_[Number_of_buttons];

	void add(BasicButton* button) {
		int i;
		for (i = 0; buttons_[i] != NULL; ++i) {}
		buttons_[i] = button;
	}

	void draw_buttons() {
		for (int i = 0; buttons_[i] != NULL; i++) {
			(*buttons_[i]).draw_button();
		}
	}

	void run() {
		while (txMouseButtons() != 3) {
			for (int i = 0; buttons_[i] != NULL; i++) {
				if ((*buttons_[i]).if_button_pressed()) {
					(*buttons_[i]).function_();
				}
			}
			txSleep();
		}
	}

	ManagerButton() {
		for (int i = 0; i < Number_of_buttons; i++) {
			buttons_[i] = NULL;
		}
	}
	~ManagerButton() {
		for (int i = 0; buttons_[i] != NULL; i++) {
			delete buttons_[i];
		}
	}
};


void draw_sin();
void draw_cos();
void draw_tan();
void draw_abs();
void draw_log();
void draw_log10();
void clear();
Vector operator + (Vector a, Vector b);
Vector operator * (double koef_of_length, Vector a);
double operator * (Vector a, Vector b);

CoordSys vector_space({ 0, 0 }, { 1300, 500 }, 25, 35, "vector space");

int main() {
	txCreateWindow(1300, 700);
	txBegin();

	vector_space.draw_window();
	vector_space.draw_grid();
	vector_space.draw_axis();
	vector_space.set_color_back();

	ManagerButton manager;

	/*RectButton    sin_button(  { 350,  550 }, { 450, 600 }, "sin", draw_sin); manager.add(&sin_button);
	CircButton    cos_button(  { 500,  550 }, { 600, 600 }, "cos", draw_cos); manager.add(&cos_button);
	EllipseButton tan_button(  { 650, 550 },  { 750, 600 }, "tan", draw_tan); manager.add(&tan_button);
	BasicButton   clear_button({ 800, 550 },  { 900, 600 }, "clear", clear);  manager.add(&clear_button);
	EllipseButton abs_button({ 950, 550 }, { 1050, 600 }, "abs", draw_abs);  manager.add(&abs_button);*/

	manager.add(new RectButton(   { 350, 550 }, { 450, 600 }, "sin", draw_sin));
	manager.add(new CircButton(   { 500, 550 }, { 600, 600 }, "cos", draw_cos));
	manager.add(new EllipseButton({ 650, 550 }, { 750, 600 }, "tan", draw_tan));
	manager.add(new BasicButton(  { 800, 550 }, { 900, 600 }, "clear", clear));
	manager.add(new EllipseButton({ 950, 550 }, { 1050, 600 }, "abs", draw_abs));

	manager.draw_buttons();

	manager.run();
}


void CoordSys::draw_graphic(graph_t function, COLORREF color)
{
	for (double x = -600; x <= 600; x = x + 0.1)
	{
		(*this).draw_point({ x,function(x) }, color, 2);
	}
}


void draw_sin() {
	vector_space.draw_graphic(sin, TX_LIGHTRED);
}

void draw_cos() {
	vector_space.draw_graphic(cos, TX_LIGHTGREEN);
}

void draw_tan() {
	vector_space.draw_graphic(tan, RGB(0, 191, 255));
}

void draw_abs() {
	vector_space.draw_graphic(abs, RGB(0, 191, 255));
}

void draw_log() {
	vector_space.draw_graphic(log, RGB(0, 191, 255));
}

void draw_log10() {
	vector_space.draw_graphic(log10, RGB(0, 191, 255));
}

void clear() {
	vector_space.draw_window();
	vector_space.draw_grid();
	vector_space.draw_axis();
	vector_space.set_color_back();
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
	if (rec_coord.x > vector_space.coords0_.x &&
		rec_coord.x < vector_space.coords1_.x &&
		rec_coord.y > vector_space.coords0_.y &&
		rec_coord.y < vector_space.coords1_.y) {
		txCircle(rec_coord.x, rec_coord.y, R);
	}
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

