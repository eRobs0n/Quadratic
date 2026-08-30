#include "plot.h"
#include "common.h"
#include "raylib.h"
#include "raymath.h"
#include "solver.h"
#include <cassert>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define DEBUG if (0)

const int screenWidth = 700;
const int screenHeight = 700;

const int _GRID_LINES_WIDTH = 2;
const int _GRID_TEXT_PADDING = 5;
const int _GRID_FONT_SIZE = 16;
const int _NUM_POINTS = 100;

//Global variable for font. Will be initialized on _LoadResources
Font __NUMBERS_FONT;
Image __ICON;

struct Grid{
	Vector2 pos;
	Vector2 step;
	float zoom;
};

/**
 * Load resources: pictures, fonts and other
 */
void _LoadResources();

/**
 *	Draws grid in the window 
 * 	@param [in] grid pointer to grid struct
 */
void _DrawGrid(const Grid* grid);

/**
 *	Draws coordinate axes in the window 
 * 	@param [in] grid pointer to grid struct
 *  @param [in] font font for labels
 */
void _DrawAxes(const Grid* grid);

/**
 * Convert coordinates from screen to grid space
 */
Vector2 _ScreenToGrid(const Grid* g, Vector2 p);

/**
 * Convert coordinates from grid to screen space
 */
Vector2 _GridToScreen(const Grid* g, Vector2 p);

/**
 * Get bounds for drawing [x_begin, x_end] - the range of x-values that fall within the screen area
 * @param [in] grid 
 * @param [in] equation coeffs
 * @returns The range of x-values that fall within the screen area
 * @note In case the graph does not fit on the screen returns {NAN, NAN}!
 */
Vector2 _GetBounds(const Grid* grid, struct EquationCoeffs coeffs);

//--------------------------------------------
void _LoadResources(){
	__NUMBERS_FONT = LoadFontEx("res/SFProText-Medium.ttf", _GRID_FONT_SIZE, 0, 250);
	__ICON = LoadImage("res/x2.png");

	assert(IsFontValid(__NUMBERS_FONT) && "Failed to load font!");
	assert(IsImageValid(__ICON) && "Failed to load font!");
}

Vector2 _ScreenToGrid(const Grid* g, Vector2 p){
	return Vector2{
		.x = (p.x - g->pos.x) / g->zoom,
		.y = (g->pos.y - p.y) / g->zoom
	};
}

Vector2 _GridToScreen(const Grid* g, Vector2 p){
	return Vector2{
		.x = (g->pos.x + p.x * g->zoom),
		.y = (g->pos.y - p.y * g->zoom)
	};
}

void _DrawAxes(const Grid* grid){
	Vector2 topLeft = _ScreenToGrid(grid, {0, 0});
	Vector2 bottomRight = _ScreenToGrid(grid, {screenWidth, screenHeight});
	Vector2 center_scr = _GridToScreen(grid, {0, 0});
	bool x_on_top = false;
	bool x_on_bot = false;
	bool y_on_left = false;
	bool y_on_right = false;

	if (center_scr.x < 0) y_on_left = true;
	if (center_scr.x > screenWidth) y_on_right = true;
	if (center_scr.y < 0) x_on_top = true;
	if (center_scr.y > screenHeight) x_on_bot = true;
	//Draw x --->
	if (!x_on_bot && !x_on_top){
		DrawLineEx({0, 		 	 	 grid->pos.y}, 
			       {screenWidth, 	 grid->pos.y}, 
			       2, 
			       BLACK);
	}
	//Draw x labels
	float x_start = floor(topLeft.x / grid->step.x) * grid->step.x;
	float x_end = floor(bottomRight.x / grid->step.x) * grid->step.x;
	float pos = x_start;

	float x_labels_pos = grid->pos.y + _GRID_TEXT_PADDING;
	if (x_on_top) x_labels_pos = _GRID_TEXT_PADDING;
	if (x_on_bot) x_labels_pos = screenHeight - _GRID_TEXT_PADDING - _GRID_FONT_SIZE;

	while(pos<=x_end){
		if (!(fabs(pos)<1e-9)){
			DrawTextPro(__NUMBERS_FONT, 
					   TextFormat("%g", pos),
					   {grid->pos.x + pos * grid->zoom, 
					   x_labels_pos},
					   {0, 0},
					   90,
					   _GRID_FONT_SIZE,
					   0,
					   !x_on_top && !x_on_bot ? BLACK : DARKBLUE
					   );
		}
		pos += grid->step.x;
	}
	//Draw y |^
	if (!y_on_left && !y_on_right){
		DrawLineEx({grid->pos.x,     0}, 
			       {grid->pos.x, 	 screenHeight}, 
			       2, 
			       BLACK);
	}
	//Draw y labels
	float y_start = floor(bottomRight.y / grid->step.y) * grid->step.y;
	float y_stop = floor(topLeft.y / grid->step.y) * grid->step.y;
	pos = y_start;

	float y_labels_pos = grid->pos.x + _GRID_TEXT_PADDING;
	if (y_on_left) y_labels_pos = _GRID_TEXT_PADDING;
	if (y_on_right) y_labels_pos = screenWidth - _GRID_TEXT_PADDING - _GRID_FONT_SIZE;

	while(pos <= y_stop){
		DrawTextPro(__NUMBERS_FONT, 
					TextFormat("%g", pos),
					{y_labels_pos,
					grid->pos.y - pos * grid->zoom},
					{0, 0},
					0,
					_GRID_FONT_SIZE,
					0,
					!y_on_left && !y_on_right ? BLACK : DARKBLUE
					);
		pos += grid->step.y;
	}

}

void _DrawGrid(const Grid* grid){
	assert(grid != NULL);

	Vector2 topLeft = _ScreenToGrid(grid, {0, 0});
	Vector2 bottomRight = _ScreenToGrid(grid, {screenWidth, screenHeight});
	//DEBUG printf("\ntopLeft: %f %f\n", topLeft.x, topLeft.y);

	//Draw vertical |
	float x_start = floor(topLeft.x / grid->step.x) * grid->step.x;
	float x_end = floor(bottomRight.x / grid->step.x) * grid->step.x;
	float pos = x_start;
	while(pos <= x_end){
		if (!(fabs(pos) < 1e-9)){
			float x_scr = grid->pos.x + pos * grid->zoom;
			DrawLineEx({x_scr, 0}, 
			       	   {x_scr, screenHeight}, 
			       	   _GRID_LINES_WIDTH, 
			       	   LIGHTGRAY);
		}	
		pos += grid->step.x;
	}
	//Draw horizontal -
	float y_start = floor(bottomRight.y / grid->step.y) * grid->step.y;
	float y_stop = floor(topLeft.y / grid->step.y) * grid->step.y;
	pos = y_start;
	while(pos <= y_stop){
		if (!(fabs(pos) < 1e-9)){
			float y_scr = grid->pos.y - pos * grid->zoom;
			DrawLineEx({0, 		 	 y_scr}, 
			       	   {screenWidth, y_scr}, 
			       _GRID_LINES_WIDTH, 
			       LIGHTGRAY);
		}
		pos += grid->step.y;
	}

}

//returns {nan, nan} if graph not in viewport
Vector2 _GetBounds(const Grid* grid, struct EquationCoeffs coeffs){
	assert(grid != NULL);

	Vector2 topLeft = _ScreenToGrid(grid, {0, 0});
	Vector2 bottomRight = _ScreenToGrid(grid, {screenWidth, screenHeight});
	Vector2 bounds = {}; //(left/right)
	if (IsZero(coeffs.coeff_of_sq_x)) return {topLeft.x, bottomRight.x};
	if (coeffs.coeff_of_sq_x > 0){
		coeffs.free_coeff -= topLeft.y;
	}else{
		coeffs.free_coeff -= bottomRight.y;
	}
	EquationSolutions roots;
	SolveEquation(&coeffs, &roots);
	//DEBUG PrintRoots(&roots, 2);
	//DEBUG printf("Левая граница экрана - %g правая - %g\n", topLeft.x, bottomRight.x);
	double mnroot = MIN(roots.root1, roots.root2);
	if (mnroot < topLeft.x) bounds.x = topLeft.x;
	else bounds.x = mnroot;
	double mxroot = MAX(roots.root1, roots.root2);
	if (mxroot > bottomRight.x) bounds.y = bottomRight.x;
	else bounds.y = mxroot;
	//DEBUG printf("Левый - %g правый - %g\n", bounds.x, bounds.y);
	if (Equals(bounds.x, bounds.y) || bounds.x > bounds.y) return {NAN, NAN};
	return bounds;
}

//TODO?
void _DrawFunction(const Grid* grid, 
				   /*double (*GetVal)(const struct EquationCoeffs* coeffs,double val),*/
				   const struct EquationCoeffs* coeffs, Vector2 bounds){
	assert(grid != NULL);
	assert(coeffs!= NULL);
	//assert(GetVal != NULL);

	float pos_x = bounds.x;
	Vector2 prev = {pos_x, (float)GetQuadraticValue(coeffs, pos_x)};
	Vector2 point = prev;
	float step = (bounds.y - bounds.x) / _NUM_POINTS; 
	if (IsZero(step)) return;
	while (pos_x <= bounds.y+1){ //TODO make const
		DrawLineEx(_GridToScreen(grid, prev), _GridToScreen(grid, point), 3, GREEN);
		prev = point;
		pos_x += step;
		point = {pos_x, (float)GetQuadraticValue(coeffs, pos_x)};
	}
}

void _DrawPlot(const struct Grid* g, const struct EquationCoeffs* coeffs, const struct Vector2 bounds){
	_DrawGrid(g);
    _DrawAxes(g);
    if (isfinite(bounds.x) && isfinite(bounds.y)){
    	_DrawFunction(g, /*&GetQuadraticValue,*/ coeffs, bounds);
    }
}


void GraphMode(const struct EquationCoeffs* coeffs){
	Grid g = {{screenWidth/2, screenHeight/2}, {1, 1}, 60.f};

	SetConfigFlags(FLAG_MSAA_4X_HINT);

	InitWindow(screenWidth, screenHeight, "Quadratic");

	_LoadResources();

	SetWindowIcon(__ICON);

	//Font axe_numbers = LoadFontEx("SFProText-Medium.ttf", _GRID_FONT_SIZE, 0, 250);

    SetTargetFPS(60);

    Vector2 bounds = _GetBounds(&g, *coeffs);
    DEBUG printf("Bounds: left = %g right = %g\n", bounds.x, bounds.y);

    while (!WindowShouldClose()){

    	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
           	g.pos = Vector2Add(g.pos, delta);
        }

        //TODO fix zoom and step changes
        if (IsKeyPressed(KEY_DOWN)){
        	g.zoom /= 2.1;
        	g.step.x *= 2; g.step.y *= 2;
        }
        if (IsKeyPressed(KEY_UP)){
        	g.zoom *= 2.1;
        	g.step.x /= 2; g.step.y /=2;
        }

        bounds = _GetBounds(&g, *coeffs);
    	BeginDrawing();
    		ClearBackground(RAYWHITE);
    		_DrawPlot(&g, coeffs, bounds);
    		
    	EndDrawing();
    }
}
